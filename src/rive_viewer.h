#ifndef RIVEEXTENSION_VIEWER_H
#define RIVEEXTENSION_VIEWER_H

// stdlib
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
#include <godot_cpp/variant/builtin_types.hpp>

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
#include "api/rive_file.hpp"
#include "rive_instance.hpp"
#include "skia_instance.hpp"
#include "utils/out_redirect.hpp"
#include "utils/types.hpp"
#include "viewer_props.hpp"

using namespace godot;

class RiveViewer : public Control {
    GDCLASS(RiveViewer, Control);

   private:
    ViewerProps props;
    RiveInstance inst;
    SkiaInstance sk;
    float elapsed = 0;
    Dictionary cached_scene_property_values;
    Ref<Image> image;
    Ref<ImageTexture> texture;

   protected:
    static void _bind_methods();
    void _on_path_changed(String path);
    void _on_artboard_changed(int index);
    void _on_scene_changed(int index);
    void _on_animation_changed(int index);
    void _on_size_changed(float w, float h);
    void _on_transform_changed();
    void check_scene_property_changed();
    bool advance(float delta);
    PackedByteArray frame(float delta);
    PackedByteArray redraw();

   public:
    RiveViewer();
    void _draw() override;
    void _process(float delta);
    void _notification(int what);
    void _gui_input(const Ref<InputEvent> &event) override;
    void _ready() override;
    void _get_property_list(List<PropertyInfo> *p_list) const;
    bool _set(const StringName &prop, const Variant &value);
    bool _get(const StringName &prop, Variant &return_value) const;

    int width();
    int height();

    /* Setters */

    void set_file_path(String value) {
        props.path(value);
    }

    void set_fit(int value) {
        props.fit((FIT)value);
    }

    void set_alignment(int value) {
        props.alignment((ALIGN)value);
    }

    void set_disable_press(bool value) {
        props.disable_press(value);
    }

    void set_disable_hover(bool value) {
        props.disable_hover(value);
    }

    void set_paused(bool value) {
        props.paused(value);
    }

    /* Getters */

    String get_file_path() const {
        return props.path();
    }

    int get_fit() const {
        return props.fit();
    }

    int get_alignment() const {
        return props.alignment();
    }

    bool get_disable_press() const {
        return props.disable_press();
    }

    bool get_disable_hover() const {
        return props.disable_hover();
    }

    bool get_paused() const {
        return props.paused();
    }

    /* Signals */

    void pressed(Vector2 position) const {}

    void released(Vector2 position) const {}

    void scene_property_changed(Ref<RiveScene> scene, String property, Variant new_value, Variant old_value) const {}

    /* API */

    float get_elapsed_time() const;
    Ref<RiveFile> get_file() const;
    Ref<RiveArtboard> get_artboard() const;
    Ref<RiveScene> get_scene() const;
    Ref<RiveAnimation> get_animation() const;

    void go_to_artboard(Ref<RiveArtboard> artboard);
    void go_to_scene(Ref<RiveScene> scene);
    void go_to_animation(Ref<RiveAnimation> animation);

    void press_mouse(Vector2 position);
    void release_mouse(Vector2 position);
    void move_mouse(Vector2 position);
};

#endif