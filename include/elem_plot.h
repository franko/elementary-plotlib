#pragma once

#include "debug_priv.h"
#include "gc_context.h"
#include "plot.h"
#include "plot_agent.h"

struct elem_plot_manager_ref : graphics::plot_manager {
    graphics::plot *copy(const graphics::plot *p) override;
    void dispose(graphics::plot *p) override;
};

class elem_plot : public graphics::plot {
public:
    enum { magic_number = 0x6efda215 }; // guaranteed to be a random number

    elem_plot(unsigned flags):
        graphics::plot{flags, legends_plot_manager} {
    }
    elem_plot(const elem_plot& other):
        graphics::plot{other, legends_plot_manager},
        m_plot_agent{} {
    }
    elem_plot(elem_plot&& other):
        graphics::plot{std::move(other), legends_plot_manager},
        m_plot_agent{std::move(other.m_plot_agent)} {
    }

    ~elem_plot();

    void update_windows() {
        m_plot_agent.update_windows();
    }

    void clear_windows_pending_flags() {
        m_plot_agent.clear_pending_flags();
    }

    void add_window_link(elem_window *window, int slot_index) {
        m_plot_agent.add_window(window, slot_index);
    }

    void clear_windows_links() {
        m_plot_agent.clear();
    }

    void show(unsigned width, unsigned height, unsigned flags);
    void wait_show_window();

    void retain();
    void release();
    // Returns true if object has some remaining references.
    bool has_references(gc_context& gc);

    static elem_plot *assert_cast(graphics::plot *plot) {
        elem_plot *ep = (elem_plot *) plot;
        assert(ep->m_magic == magic_number);
        return ep;
    }

private:
    graphics::plot_agent m_plot_agent;
    elem_window *m_show_window = nullptr;
    int m_ref_count = 1;
    // The value below is for debug/safety reason.
    const unsigned int m_magic = magic_number;

    static elem_plot_manager_ref legends_plot_manager[1];
};
