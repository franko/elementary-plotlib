#include "plot.h"
#include "plot_style.h"
#include "units.h"
#include "rect.h"
#include "path.h"

namespace graphics {

sg_composite draw_axis_simple(const plot::axis& ax, plot::axis_e dir,
    label_iterator& labels, const agg::trans_affine& user_mtx, double& screen_size,
    const double scale)
{
    sg_composite group;

    const double text_label_size = get_default_font_size(text_axis_labels, scale);
    const double eps = 1.0e-3;
    const double lab_space = 10, mark_len = 5;

    // used to store the bounding box of text labels
    opt_rect<double> bb;
    agg::rect_d r;

    const bool isx = (dir == plot::x_axis);
    const double hj = ax.labels_hjustif(), vj = ax.labels_vjustif();
    const double langle = ax.labels_angle();

    draw::path* marks_obj = new draw::path();
    trans::scaling* marks = new trans::scaling(marks_obj);
    agg::path_storage& marks_path = marks_obj->self();

    group.add_stroke(marks, colors::black(), std_line_width(scale, 1.0));

    double uq;
    const char* text;
    while (labels.next(uq, text))
    {
        double x = (isx ? uq : 0.0), y = (isx ? 0.0 : uq);
        user_mtx.transform(&x, &y);

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

}
