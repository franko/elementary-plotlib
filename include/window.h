#pragma once

#include "window_surface.h"

class elem_plot;

class elem_window {
public:
    virtual void set_layout(const char *fmt) = 0;
    virtual int attach(elem_plot* p, const char* slot_str) = 0;
    virtual void slot_refresh(unsigned index) = 0;
    virtual void start(unsigned width, unsigned height, unsigned flags) = 0;
    virtual void wait() = 0;
    virtual void close() = 0;
    virtual graphics::window_surface *get_window_surface() = 0;
    virtual ~elem_window() { }

    void retain() {
        m_ref_count++;
    }

    void release() {
        m_ref_count--;
        gc_context gc;
        if (!has_references(gc)) {
            gc.collect_visited();
        }
    }

    bool has_references(gc_context& gc) {
        if (gc.visited(this)) {
            return false;
        }
        gc.add_visited(this);
        if (m_ref_count > 0) {
            return true;
        } 
        auto linked_plots_list = get_window_surface()->linked_plots();
        for (unsigned i = 0; i < linked_plots_list.size(); i++) {
            elem_plot *linked_plot = linked_plots_list[i];
            if (linked_plot->has_references(gc)) {
                return true;
            }
        }
        return false;
    }

private:
    int m_ref_count = 1;
};
