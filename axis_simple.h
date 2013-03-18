namespace graphics {

extern sg_composite draw_axis_simple(const plot::axis& ax, plot::axis_e dir,
    label_iterator& labels, const agg::trans_affine& user_mtx, double& screen_size,
    const double scale);

}
