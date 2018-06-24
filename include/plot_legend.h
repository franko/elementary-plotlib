#pragma once

#include <agg_array.h>

#include "sg_element.h"
#include "text.h"
#include "unique_ptr_container.h"

struct legend_element {
    legend_element(const char *_text): label(new graphics::text(_text)) { }

    ~legend_element() {
        delete label;
    }

    sg_composite symbol;
    graphics::text *label;
};

class plot_legend {
public:
    plot_legend() { }

    unsigned add_new_element(const char *text) {
        auto element = new legend_element{text};
        m_elements.push_back(element);
        return m_elements.size() - 1;
    }

    template <class Canvas>
    void draw(Canvas& canvas, const agg::trans_affine& m) {
        for (unsigned i = 0; i < m_elements.size(); i++) {
            m_elements[i]->symbol.draw(canvas, m);
        }
    }

    double get_labels_screen_width();
    double get_labels_screen_height(double label_space);

private:
    unique_ptr_container<agg::pod_bvector, legend_element> m_elements;
};
