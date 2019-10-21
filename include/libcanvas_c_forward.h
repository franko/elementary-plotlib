#pragma once

class canvas_object;
class canvas_path;
class canvas_curve;
class canvas_dashed;

namespace graphics {
    class plot;
    class plot_agent;
}

struct canvas_plot {
    graphics::plot *plot;
    graphics::plot_agent *plot_agent;
};
