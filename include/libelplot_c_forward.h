#pragma once

class elp_object;
class elp_path;
class elp_curve;
class elp_dashed;
class elp_window;

namespace graphics {
    class plot;
    class plot_agent;
}

struct elp_plot {
    graphics::plot *plot;
    graphics::plot_agent *plot_agent;
};
