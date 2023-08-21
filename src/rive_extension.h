#ifndef RIVEEXTENSION_H
#define RIVEEXTENSION_H

// Stdlib
#include <vector>

// godot-cpp
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_mouse.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>

// rive-cpp
#include <rive/animation/state_machine_instance.hpp>
#include <rive/file.hpp>
#include <rive/layout.hpp>

// extension
#include "rive_controller.h"
#include "utils/out_redirect.hpp"
#include "utils/types.hpp"

using namespace godot;

static const char *FitEnumPropertyHint = "Fill:1,Contain:2,Cover:3,FitWidth:4,FitHeight:5,None:6,ScaleDown:7";

static const char *AlignEnumPropertyHint
    = "TopLeft:1,TopCenter:2,TopRight:3,CenterLeft:4,Center:5,CenterRight:6,BottomLeft:7,BottomCenter:8,BottomRight:9";

class RiveViewer : public Control {
    GDCLASS(RiveViewer, Control);

   public:
    enum Fit { FILL = 1, CONTAIN = 2, COVER = 3, FIT_WIDTH = 4, FIT_HEIGHT = 5, NONE = 6, SCALE_DOWN = 9 };

    enum Align {
        TOP_LEFT = 1,
        TOP_CENTER = 2,
        TOP_RIGHT = 3,
        CENTER_LEFT = 4,
        CENTER = 5,
        CENTER_RIGHT = 6,
        BOTTOM_LEFT = 7,
        BOTTOM_CENTER = 8,
        BOTTOM_RIGHT = 9
    };

    Ptr<RiveController> controller = nullptr;
    Ref<Image> image = nullptr;
    Ref<ImageTexture> texture = nullptr;

   private:
    String path;
    Fit fit = Fit::CONTAIN;
    Align alignment = Align::TOP_LEFT;
    CerrRedirect _err_output;

   protected:
    static void _bind_methods();
    void _on_resize();

    rive::Fit get_rive_fit();
    rive::Alignment get_rive_alignment();

   public:
    RiveViewer();
    ~RiveViewer();

    void _draw() override;
    void _process(float delta);
    void _notification(int what);
    void _gui_input(const Ref<InputEvent> &event) override;
    void _ready() override;

    int width();
    int height();

    String get_file_path();
    void set_file_path(String value);

    int get_fit() {
        return fit;
    }

    void set_fit(int value);

    int get_alignment() {
        return alignment;
    }

    void set_alignment(int value);
};

#endif