#ifndef GRAPHICS_PLOT_AXIS_H
#define GRAPHICS_PLOT_AXIS_H

#include "units.h"
#include "strpp.h"
#include "categories.h"
#include "factor_labels.h"
#include "sg_element.h"

namespace graphics {

enum axis_e { x_axis, y_axis };

class axis {
public:
    units u;
    str title;
    axis_e dir;
    bool active;
    bool use_categories;
    bool pad_units;
    category_map categories;
    units::format_e format_tag;

    axis(axis_e _dir, bool enable, const char* _title = 0):
        title(_title ? _title : ""), dir(_dir),
        active(enable), use_categories(false), pad_units(false),
        format_tag(units::format_invalid),
        m_labels_angle(0.0),
        m_labels_hjustif(_dir == x_axis ? 0.5 : 1.0),
        m_labels_vjustif(_dir == x_axis ? 1.0 : 0.5),
        m_comp_labels(0)
    { }

    ~axis() {
        delete m_comp_labels;
    }

    const char* label_format() const
    {
        return (format_tag == units::format_invalid ? 0 : m_label_format);
    }

    void set_label_format(units::format_e tag, const char* fmt)
    {
        format_tag = tag;
        memcpy(m_label_format, fmt, strlen(fmt) + 1);
    }

    void clear_label_format() {
        format_tag = units::format_invalid;
    }

    void set_labels_angle(double angle)
    {
        double a = (dir == x_axis ? -angle + M_PI/2 : -angle);
        double c = cos(a), s = sin(a);
        m_labels_hjustif = round(c + 1.0) / 2.0;
        m_labels_vjustif = round(s + 1.0) / 2.0;
        m_labels_angle = angle;
    }

    void set_comp_labels(ptr_list<factor_labels>* labels) {
        delete m_comp_labels;
        m_comp_labels = labels;
    }

    double labels_angle()   const {
        return m_labels_angle;
    }
    double labels_hjustif() const {
        return m_labels_hjustif;
    }
    double labels_vjustif() const {
        return m_labels_vjustif;
    }

    void get_limits(const double xin1, const double xin2, double& x1, double& x2) const {
        if (active && pad_units) {
            int ixi, ixs;
            double xd;
            u.limits(ixi, ixs, xd);
            x1 = ixi * xd;
            x2 = ixs * xd;
        } else {
            x1 = xin1;
            x2 = xin2;
        }
    }

    sg_composite draw(const agg::trans_affine& m, double& label_size, const double scale)
    {
        if (!active) {
            label_size = 0.0;
            return sg_composite();
        }
        if (use_categories) {
            category_map::iterator clabels(categories);
            return draw_with_labels(clabels, m, label_size, scale);
        } else {
            units_iterator ulabels(u, format_tag, label_format());
            return draw_with_labels(ulabels, m, label_size, scale);
        }
    }

protected:

    sg_composite draw_with_labels(label_iterator& labels,
        const agg::trans_affine& m, double& screen_size, const double scale);

private:
    double m_labels_angle;
    double m_labels_hjustif, m_labels_vjustif;
    char m_label_format[units::label_format_max_size];
    ptr_list<factor_labels>* m_comp_labels;
};

}
#endif
