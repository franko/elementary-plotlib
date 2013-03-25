#include "plot_axis.h"
#include "plot_style.h"
#include "rect.h"
#include "path.h"
#include "trans.h"
#include "text.h"

namespace graphics {

sg_composite axis::draw_with_labels(label_iterator& labels,
    const agg::trans_affine& m, double& screen_size, const double scale)
{
    sg_composite group;

    const double text_label_size = get_default_font_size(text_axis_labels, scale);
    const double eps = 1.0e-3;
    const double lab_space = 10, mark_len = 5;

    // used to store the bounding box of text labels
    opt_rect<double> bb;
    agg::rect_d r;

    const bool isx = (dir == x_axis);
    const double hj = labels_hjustif(), vj = labels_vjustif();
    const double langle = labels_angle();

    draw::path* marks_obj = new draw::path();
    trans::scaling* marks = new trans::scaling(marks_obj);
    agg::path_storage& marks_path = marks_obj->self();

    // std_line_width(scale, 1.0)
    group.add_stroke(marks, colors::black(), 1.0, ELEMENT_CRISP_RENDER);

    double uq;
    const char* text;
    while (labels.next(uq, text))
    {
        double x = (isx ? uq : 0.0), y = (isx ? 0.0 : uq);
        m.transform(&x, &y);

        double q = (isx ? x : y);

        if (q < -eps || q > 1.0 + eps)
            continue;

        draw::text* label = new draw::text(text, text_label_size, hj, vj);

        label->set_point(isx ? q : -lab_space, isx ? -lab_space : q);
        label->angle(langle);

        agg::bounding_rect_single(*label, 0, &r.x1, &r.y1, &r.x2, &r.y2);
        bb.add<rect_union>(r);

        group.add_fill(label, colors::black());

        marks_path.move_to(isx ? q :  0.0 , isx ?  0.0  : q);
        marks_path.line_to(isx ? q : -mark_len, isx ? -mark_len : q);
    }

    marks_path.move_to(0.0, 0.0);
    marks_path.line_to(isx ? 1.0 : 0.0 , isx ?  0.0 : 1.0);

    if (bb.is_defined())
    {
        const agg::rect_d& br = bb.rect();
        screen_size = (isx ? br.y2 - br.y1 : br.x2 - br.x1);
    }
    else
    {
        screen_size = 0.0;
    }

    screen_size += lab_space;

    return group;
}

static void draw_mark(agg::path_storage& mark, const agg::trans_affine& m, double x, const double y1, const double y2) {
    double y = 0.0;
    m.transform(&x, &y);
    mark.move_to(x, y1);
    mark.line_to(x, y2);
}

sg_composite axis::draw_comp_labels_axis(const agg::trans_affine& m, double& screen_size, const double scale)
{
    sg_composite group;

    const double text_label_size = get_default_font_size(text_axis_labels, scale);
    const double lab_angle = labels_angle();
    const double y_spac_top = 3, y_spac_bot = 3;

    draw::path* marks_obj = new draw::path();
    trans::scaling* marks = new trans::scaling(marks_obj);
    agg::path_storage& marks_path = marks_obj->self();

    // std_line_width(scale, 1.0)
    group.add_stroke(marks, colors::black(), 1.0, ELEMENT_CRISP_RENDER);

    const int layers_number = m_comp_labels->size();
    double p_lab = 0;
    for (int layer = layers_number - 1; layer >= 0; layer--)
    {
        factor_labels* factor = m_comp_labels->at(layer);
        if (factor->labels_number() > 256) continue;

        agg::pod_bvector<draw::text*> tlabels;
        double hmax = 0.0;
        const bool draw_labels = (factor->labels_number() < 32);
        if (draw_labels)
        {
            for (int k = 0; k < factor->labels_number(); k++)
            {
                const char* text = factor->label_text(k);
                draw::text* label = new draw::text(text, text_label_size, 0.5, 0.5);
                label->angle(lab_angle);

                double rx1, ry1, rx2, ry2;
                agg::bounding_rect_single(*label, 0, &rx1, &ry1, &rx2, &ry2);
                double rh = ry2 - ry1;
                if (rh > hmax) {
                    hmax = rh;
                }
                tlabels.add(label);
            }
        }

        double p_lab_inf = p_lab - (y_spac_top + y_spac_bot + hmax);

        for (int k = 0; k < factor->labels_number(); k++)
        {
            double x_lab_a = factor->mark(k);
            double x_lab_b = factor->mark(k+1);

            draw_mark(marks_path, m, x_lab_a, p_lab, p_lab_inf);

            double x_lab = (x_lab_a + x_lab_b) / 2, y_lab = 0.0;
            m.transform(&x_lab, &y_lab);

            if (draw_labels)
            {
                draw::text* label = tlabels[k];
                label->set_point(x_lab, p_lab_inf + y_spac_bot + hmax/2.0);
                group.add_fill(label, colors::black());
            }
        }

        double x_lab = factor->mark(factor->labels_number());
        draw_mark(marks_path, m, x_lab, p_lab, p_lab_inf);

        p_lab = p_lab_inf;
    }

    screen_size = - p_lab;
    return group;
}


}