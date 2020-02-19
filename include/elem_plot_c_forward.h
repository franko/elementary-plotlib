#pragma once

class elem_object;
class elem_path;
class elem_curve;
class elem_dashed;
class elem_window;

namespace graphics {
    class plot;
    class plot_agent;
}

struct elem_plot {
    graphics::plot *plot;
    graphics::plot_agent *plot_agent;
};
