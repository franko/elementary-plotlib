
/* elp_object.h
 *
 * Copyright (C) 2009, 2010 Francesco Abbate
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include <cstring>

#include "agg_trans_affine.h"
#include "agg_bounding_rect.h"
#include "agg_conv_transform.h"
#include "agg_rendering_buffer.h"
#include "agg_scanline_u.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_ellipse.h"

#include "resource-manager.h"
#include "pixel_fmt.h"
#include "draw_svg.h"
#include "utils.h"
#include "strpp.h"

struct vertex_source {
    virtual void rewind(unsigned path_id) = 0;
    virtual unsigned vertex(double* x, double* y) = 0;
    virtual ~vertex_source() { }
};

/* Meant to copy a path_base<VertexStorage> instance as declared in
   agg_path_storage.h. */
template <typename PathStorage>
void path_base_copy(PathStorage& dest, const PathStorage& source) {
    const auto& source_vertices = source.vertices();
    auto& dest_vertices = dest.vertices();
    dest_vertices = source_vertices;
}

template <typename VertexSource>
void vertex_source_copy(VertexSource& dest, const VertexSource& source) {
    path_base_copy(dest, source);
}

/* As agg:ellipse is used as a VertexSource somewhere so we need to provide a
   function specialization. The agg::ellipse cannot be copied using public
   methods or constructors but since it is a POD object we can use memcpy. */
template <>
inline void vertex_source_copy<agg::ellipse>(agg::ellipse& dest, const agg::ellipse& source) {
    memcpy(&dest, &source, sizeof(agg::ellipse));
}

// Scalable Graphics Object
struct elp_object : public vertex_source {

    virtual void apply_transform(const agg::trans_affine& m, double as) = 0;
    virtual void bounding_box(double *x1, double *y1, double *x2, double *y2) = 0;

    virtual bool affine_compose(agg::trans_affine& m) {
        return false;
    }

    virtual str write_svg(int id, agg::rgba8 c, double h) {
        str path;
        svg_property_list* ls = this->svg_path(path, h);
        str s = svg_fill_path(path, id, c, ls);
        svg_property_list::free(ls);
        return s;
    }

    virtual svg_property_list* svg_path(str& s, double h) {
        svg_coords_from_vs(this, s, h);
        return 0;
    }

    virtual elp_object *copy() const = 0;
    virtual ~elp_object() { }
};

struct approx_scale {
    template <class T> static void approximation_scale(T& obj, double as)
    {
        obj.approximation_scale(as);
    }
};

struct no_approx_scale {
    template <class T> static void approximation_scale(T& obj, double as) { }
};

template <class VertexSource, class ApproxManager=no_approx_scale>
class elp_object_gen : public elp_object {
protected:
    VertexSource m_base;

public:
    elp_object_gen(): m_base() {}

    template <class InitType> elp_object_gen(InitType& i) : m_base(i) { }

    template <class InitType1, class InitType2>
    elp_object_gen(InitType1& i1, InitType2& i2) : m_base(i1, i2) { }

    void rewind(unsigned path_id) override {
        m_base.rewind(path_id);
    }
    unsigned vertex(double* x, double* y) override {
        return m_base.vertex(x, y);
    }

    void apply_transform(const agg::trans_affine& m, double as) override {
        ApproxManager::approximation_scale(m_base, as);
    }

    void bounding_box(double *x1, double *y1, double *x2, double *y2) override {
        agg::bounding_rect_single(m_base, 0, x1, y1, x2, y2);
    }

    void copy_content(elp_object_gen& dest) const {
        vertex_source_copy(dest.m_base, this->m_base);
    }

    elp_object *copy() const override {
        elp_object_gen *new_object = new elp_object_gen();
        copy_content(*new_object);
        return new_object;
    }

    const VertexSource& self() const {
        return m_base;
    };
    VertexSource& self()       {
        return m_base;
    };
};

/* this class does create an elp_object obtained combining an an AGG
   transformation like conv_stroke, conv_dash or any other transform
   with a elp_object source. This adapter implements therefore the
   virtual methods from the elp_object abstract class */
template <class ConvType, class ApproxManager>
class sg_adapter : public elp_object {
protected:
    ConvType m_output;
    elp_object* m_source;

public:
    sg_adapter(elp_object* src): m_output(*src), m_source(src) { }

    template <class InitType>
    sg_adapter(elp_object* src, InitType& val): m_output(*src, val), m_source(src)
    { }

    virtual ~sg_adapter() { }

    void rewind(unsigned path_id) override {
        m_output.rewind(path_id);
    }
    unsigned vertex(double* x, double* y) override {
        return m_output.vertex(x, y);
    }

    void apply_transform(const agg::trans_affine& m, double as) override {
        ApproxManager::approximation_scale(m_output, as);
        this->m_source->apply_transform(m, as);
    }

    void bounding_box(double *x1, double *y1, double *x2, double *y2) override {
        this->m_source->bounding_box(x1, y1, x2, y2);
    }

    const ConvType& self() const {
        return m_output;
    };

    elp_object *copy() const override {
        // TODO: should be removed as not used.
        return nullptr;
    }

    ConvType& self()       {
        return m_output;
    };
};

/* This class add a scaling transformation to an object. The scaling
   transfomation is an affine transform that adapt to the size of the
   canvas where the object is drawn. */
template <class ResourceManager = manage_owner>
class elp_object_scaling : public elp_object
{
    elp_object* m_source;
    agg::conv_transform<elp_object> m_trans;
    agg::trans_affine m_mtx;

public:
    elp_object_scaling(elp_object* src):
        m_source(src), m_trans(*m_source, m_mtx)
    {
        ResourceManager::acquire(m_source);
    }

    ~elp_object_scaling() override {
        ResourceManager::dispose(m_source);
    }

    void rewind(unsigned path_id) override {
        m_trans.rewind(path_id);
    }
    unsigned vertex(double* x, double* y) override {
        return m_trans.vertex(x, y);
    }

    void apply_transform(const agg::trans_affine& m, double as) override {
        m_mtx = m;
        m_source->apply_transform (m, as * m.scale());
    }

    void bounding_box(double *x1, double *y1, double *x2, double *y2) override {
        agg::bounding_rect_single (*m_source, 0, x1, y1, x2, y2);
    }

    elp_object *copy() const override {
        elp_object *new_source = m_source->copy();
        elp_object_scaling *new_object = new elp_object_scaling(new_source);
        new_object->m_mtx = m_mtx;
        return new_object;
    }
};
