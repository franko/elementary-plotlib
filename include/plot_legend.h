#pragma once

#include <agg_array.h>

#include "sg_element.h"
#include "unique_ptr_container.h"

struct legend_element {
    legend_element(const char *_text): text(_text) { }

    sg_composite symbol;
    str text;
};

class plot_legend {
public:
    plot_legend() { }

    unsigned add_new_element(const char *text) {
        auto element = new legend_element{text};
        m_elements.push_back(element);
        return m_elements.size() - 1;
    }

    legend_element& element(unsigned index) { return *(m_elements[index]); }

private:
    unique_ptr_container<agg::pod_bvector, legend_element> m_elements;
};
