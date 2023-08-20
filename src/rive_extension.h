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

class RiveViewer : public Control {
    GDCLASS(RiveViewer, Control);

   public:
    Ptr<RiveController> controller = nullptr;
    // Ptr<rive::File> file = nullptr;
    // Ptr<rive::SkiaFactory> factory = nullptr;
    // sk_sp<SkSurface> surface = nullptr;
    // Ptr<rive::SkiaRenderer> renderer = nullptr;
    // Ptr<rive::LinearAnimationInstance> animation = nullptr;
    // Ptr<rive::StateMachineInstance> state_machine = nullptr;
    Ref<Image> image = nullptr;
    Ref<ImageTexture> texture = nullptr;

   private:
    String path;
    // float elapsed = 0;
    // float _last_delta = 0;
    CerrRedirect _err_output;

   protected:
    static void _bind_methods();
    void _on_resize();

   public:
    RiveViewer();
    ~RiveViewer();

    void _draw() override;
    void _process(float delta);
    void _notification(int what);
    void _gui_input(const Ref<InputEvent> &event) override;
    void _ready() override;

    String get_file_path();
    void set_file_path(String value);
    int width();
    int height();
};

#endif