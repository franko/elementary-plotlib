#include "utils.h"
#include "plot_legend.h"

double plot_legend::get_labels_screen_width() {
    double width_accu = 0;
    for (unsigned i = 0; i < m_elements.size(); i++) {
        auto element = m_elements[i];
        const double label_width = element->label->text_width();
        width_accu = max(label_width, width_accu);
    }
    return width_accu;
}

double plot_legend::get_labels_screen_height(double label_space) {
    double height_accu = 0;
    for (unsigned i = 0; i < m_elements.size(); i++) {
        auto element = m_elements[i];
        const double label_height = element->label->text_height();
        height_accu += label_height + label_space;
    }
    return height_accu;
}
