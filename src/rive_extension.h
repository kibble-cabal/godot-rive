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

// skia
#include <skia/dependencies/skia/include/core/SkBitmap.h>
#include <skia/dependencies/skia/include/core/SkCanvas.h>
#include <skia/dependencies/skia/include/core/SkSurface.h>

#include <skia/renderer/include/skia_factory.hpp>
#include <skia/renderer/include/skia_renderer.hpp>

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

    Ptr<RiveController> controller = rivestd::make_unique<RiveController>();
    Ref<Image> image = nullptr;
    Ref<ImageTexture> texture = nullptr;

   private:
    String path;
    Fit fit = Fit::CONTAIN;
    Align alignment = Align::TOP_LEFT;
    int artboard = -1;
    int state_machine = -1;
    int animation = -1;

    CerrRedirect _err_output;

   protected:
    static void _bind_methods();
    void _on_resize();
    void _on_path_changed();
    void _on_path_changed_in_editor();

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
    void _get_property_list(List<PropertyInfo> *p_list) const;

    int width();
    int height();

    void set_file_path(String value);
    void set_fit(int value);
    void set_alignment(int value);
    void set_artboard(int value);
    void set_state_machine(int value);
    void set_animation(int value);

    String get_file_path() {
        return path;
    }

    int get_fit() {
        return fit;
    }

    int get_alignment() {
        return alignment;
    }

    int get_artboard() {
        return artboard;
    }

    int get_state_machine() {
        return state_machine;
    }

    int get_animation() {
        return animation;
    }
};

#endif