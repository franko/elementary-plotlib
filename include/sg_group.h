#pragma once

#include "agg_trans_affine.h"

#include "sg_drawable.h"
#include "sg_element.h"

/* A reusable collection of independently styled drawing operations. Children
   have positions and geometric dimensions in the group's local coordinate
   system; the group composes its local-to-parent affine with the incoming
   parent-to-screen matrix at every draw.

   Leaves keep their existing rendering semantics, so stroke widths, dash
   lengths, glyph sizes, and marker symbols stay screen-sized while positions
   and path geometry follow the complete effective affine. */
class sg_group : public sg_drawable {
public:
    sg_group() = default;

    explicit sg_group(const agg::trans_affine& local_to_parent) :
        m_local_to_parent(local_to_parent) {
        check_finite_affine(local_to_parent);
    }

    sg_group(const sg_group&) = default;
    sg_group(sg_group&&) noexcept = default;
    sg_group& operator=(const sg_group&) = default;
    sg_group& operator=(sg_group&&) noexcept = default;

    const agg::trans_affine& local_to_parent() const {
        return m_local_to_parent;
    }

    size_t size() const {
        return m_children.size();
    }

    /* Compose an insertion placement onto the group's own transform. The
       existing transform applies first and the placement second, so inserting
       a group that already carries a transform does not lose it. */
    void place(const agg::trans_affine& placement) {
        check_finite_affine(placement);
        m_local_to_parent.multiply(placement);
    }

    void add(std::unique_ptr<sg_drawable> child) {
        m_children.add(std::move(child));
    }

    void add(sg_element child) {
        add(std::make_unique<sg_element>(std::move(child)));
    }

    void add(elem_object* object, agg::rgba8 stroke_color, float stroke_width,
             agg::rgba8 fill_color,
             unsigned flags = graphics::property::fill|graphics::property::stroke) {
        add(sg_element{object, stroke_color, stroke_width, fill_color, flags});
    }

    /* Insert a subgroup, applying its placement once to the inserted root and
       not again to each of its children. */
    void add_group(sg_group child, const agg::trans_affine& placement = agg::trans_affine()) {
        child.place(placement);
        add(std::make_unique<sg_group>(std::move(child)));
    }

    std::unique_ptr<sg_drawable> clone() const override {
        return std::make_unique<sg_group>(*this);
    }

    opt_rect<double> data_bounds() const override {
        opt_rect<double> result;
        for (const auto& child : m_children) {
            const opt_rect<double> local = child->data_bounds();
            if (local.is_defined()) {
                result.add<rect_union>(transform_bounds(local.rect(), m_local_to_parent));
            }
        }
        return result;
    }

    void draw(virtual_canvas& canvas, const agg::trans_affine& parent_to_screen,
              opt_rect<double>* painted_bounds = nullptr) override
    {
        // AGG multiplication applies the left transform first.
        agg::trans_affine m = m_local_to_parent;
        m.multiply(parent_to_screen);
        if (painted_bounds) {
            painted_bounds->clear();
        }
        opt_rect<double> child_bounds;
        for (auto& child : m_children) {
            child->draw(canvas, m, painted_bounds ? &child_bounds : nullptr);
            if (painted_bounds) {
                // Child bounds are already in screen coordinates.
                painted_bounds->add<rect_union>(child_bounds);
            }
        }
    }

private:
    agg::trans_affine m_local_to_parent;
    sg_drawable_list m_children;
};

/* Root-only placement wrapper for a screen-sized annotation anchored at a data
   coordinate: screen_position = P(anchor) + T(local). Only the anchor passes
   through the plot's data transform, so changing the limits or the window size
   moves the annotation without rescaling its layout.

   Insert it as a plot root, where the incoming matrix is known to map data to
   logical screen coordinates. */
class sg_annotation : public sg_drawable {
public:
    sg_annotation(sg_group group, double anchor_x, double anchor_y) :
        m_group(std::move(group)), m_anchor_x(anchor_x), m_anchor_y(anchor_y) {
        if (!std::isfinite(anchor_x) || !std::isfinite(anchor_y)) {
            throw std::invalid_argument("annotation anchor must be finite");
        }
    }

    std::unique_ptr<sg_drawable> clone() const override {
        return std::make_unique<sg_annotation>(*this);
    }

    /* An annotation contributes neither its screen rectangle nor its anchor to
       automatic limits: adding a label must not change the plotted view. */
    opt_rect<double> data_bounds() const override {
        return opt_rect<double>();
    }

    void draw(virtual_canvas& canvas, const agg::trans_affine& parent_to_screen,
              opt_rect<double>* painted_bounds = nullptr) override
    {
        if (painted_bounds) {
            painted_bounds->clear();
        }
        // Resolve the anchor on every draw rather than caching it at insertion.
        double x = m_anchor_x, y = m_anchor_y;
        parent_to_screen.transform(&x, &y);
        if (!std::isfinite(x) || !std::isfinite(y)) {
            return;
        }
        // A pure screen translation: the plot's linear scaling deliberately
        // does not reach the group's own layout.
        m_group.draw(canvas, agg::trans_affine_translation(x, y), painted_bounds);
    }

private:
    sg_group m_group;
    double m_anchor_x, m_anchor_y;
};
