#pragma once

#include "gc_context.h"
#include "plot.h"
#include "plot_agent.h"

class elem_plot : public graphics::plot {
public:
    elem_plot(unsigned flags):
        graphics::plot{flags} {
    }
    elem_plot(const elem_plot& other):
        graphics::plot{other},
        m_plot_agent{} {
    }
    elem_plot(elem_plot&& other):
        graphics::plot{std::move(other)},
        m_plot_agent{std::move(other.m_plot_agent)} {
    }

    void update_windows() {
        m_plot_agent.update_windows();
    }

    void clear_windows_pending_flags() {
        m_plot_agent.clear_pending_flags();
    }

    void add_window_link(graphics::window_surface *surface, int slot_index) {
        m_plot_agent.add_window(surface, slot_index);
    }

    void clear_windows_links() {
        m_plot_agent.clear();
    }

    bool release();
    // Returns true if object has some remaining references.
    bool has_references(gc_context& gc);
private:
    graphics::plot_agent m_plot_agent;
    int m_ref_count = 1;
};
