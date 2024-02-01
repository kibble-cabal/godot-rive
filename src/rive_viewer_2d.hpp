#ifndef RIVEEXTENSION_VIEWER_2D_H
#define RIVEEXTENSION_VIEWER_2D_H

// godot-cpp
#include <godot_cpp/classes/node2d.hpp>

// extension
#include "rive_viewer_base.h"

using namespace godot;

class RiveViewer2D : public Node2D {
    GDCLASS(RiveViewer2D, Node2D);
    RIVE_VIEWER_WRAPPER(RiveViewer2D);

   protected:
    static void _bind_methods() {
        RIVE_VIEWER_BIND(RiveViewer2D);
        ADD_PROP(RiveViewer2D, Variant::VECTOR2, size);
    }

   public:
    void _notification(int what) {
        switch (what) {
            case NOTIFICATION_INTERNAL_PROCESS:
            case NOTIFICATION_PROCESS:
                base.on_process(get_process_delta_time());
                break;
        }
    }

    void _input(const Ref<InputEvent> &event) override {
        base.on_input_event(event);
    }

    RIVE_VIEWER_SETGET(Vector2, size)
};

#endif