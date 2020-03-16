#pragma once

#include <fx.h>

#include "elem_window_factory.h"

class fox_elem_window_factory : public elem_window_factory {
public:
    fox_elem_window_factory(FXGUISignal *start_signal):
        m_start_signal(start_signal) {
    }
    elem_window *create() override;
    elem_window *create_with_layout(const char *layout) override;
private:
    FXGUISignal *m_start_signal; // Not owned.
};
