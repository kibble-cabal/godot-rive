#ifndef RIVEEXTENSION_VIEWER_H
#define RIVEEXTENSION_VIEWER_H

// godot-cpp
#include <godot_cpp/classes/control.hpp>

// extension
#include "rive_viewer_base.h"

using namespace godot;

class RiveViewer : public Control {
    GDCLASS(RiveViewer, Control);
    RIVE_VIEWER_WRAPPER(RiveViewer);

   protected:
    static void _bind_methods() {
        RIVE_VIEWER_BIND(RiveViewer);
    }

   public:
    void _notification(int what) {
        switch (what) {
            case NOTIFICATION_INTERNAL_PROCESS:
            case NOTIFICATION_PROCESS:
                base.on_process(get_process_delta_time());
                break;
            case NOTIFICATION_RESIZED:
                base.set_size(get_size());
        }
    }

    void _gui_input(const Ref<InputEvent> &event) override {
        base.on_input_event(event);
    }
};

#endif