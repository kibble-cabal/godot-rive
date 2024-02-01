#ifndef RIVEEXTENSION_VIEWER_BASE_H
#define RIVEEXTENSION_VIEWER_BASE_H

// stdlib
#include <vector>

// godot-cpp
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/engine.hpp>
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

static bool is_editor_hint() {
    return Engine::get_singleton()->is_editor_hint();
}

class RiveViewerBase {
   private:
    CanvasItem *owner;
    ViewerProps props;
    RiveInstance inst;
    SkiaInstance sk;
    float elapsed = 0;
    Dictionary cached_scene_property_values;
    Ref<Image> image;
    Ref<ImageTexture> texture;

   protected:
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
    RiveViewerBase(CanvasItem *owner);

    void on_ready();
    void on_draw();
    void on_process(float delta);
    void on_input_event(const Ref<InputEvent> &event);
    void get_property_list(List<PropertyInfo> *p_list) const;
    bool on_set(const StringName &prop, const Variant &value);
    bool on_get(const StringName &prop, Variant &return_value) const;

    int width() const;
    int height() const;

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

    void set_size(Vector2 value) {
        props.size(value.x, value.y);
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

    Vector2 get_size() const {
        return props.size();
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

#define RIVE_VIEWER_GET(type, prop_name) \
    type get_##prop_name() const {       \
        return base.get_##prop_name();   \
    }

#define RIVE_VIEWER_SET(type, prop_name) \
    void set_##prop_name(type value) {   \
        base.set_##prop_name(value);     \
    }

#define RIVE_VIEWER_SETGET(type, prop_name) \
    RIVE_VIEWER_GET(type, prop_name)        \
    RIVE_VIEWER_SET(type, prop_name)

#define RIVE_VIEWER_BIND(cls)                                                                    \
    ADD_PROP_WITH_HINT(cls, Variant::STRING, file_path, PROPERTY_HINT_FILE, "*.riv");            \
    ADD_PROP_WITH_HINT(cls, Variant::INT, fit, PROPERTY_HINT_ENUM, FitEnumPropertyHint);         \
    ADD_PROP_WITH_HINT(cls, Variant::INT, alignment, PROPERTY_HINT_ENUM, AlignEnumPropertyHint); \
    ADD_PROP(cls, Variant::BOOL, disable_press);                                                 \
    ADD_PROP(cls, Variant::BOOL, disable_hover);                                                 \
    ADD_PROP(cls, Variant::BOOL, paused);                                                        \
    ADD_SIGNAL(MethodInfo("pressed", PropertyInfo(Variant::VECTOR2, "position")));               \
    ADD_SIGNAL(MethodInfo("released", PropertyInfo(Variant::VECTOR2, "position")));              \
    ADD_SIGNAL(MethodInfo(                                                                       \
        "scene_property_changed",                                                                \
        PropertyInfo(Variant::OBJECT, "scene"),                                                  \
        PropertyInfo(Variant::STRING, "property"),                                               \
        PropertyInfo(Variant::VARIANT_MAX, "new_value"),                                         \
        PropertyInfo(Variant::VARIANT_MAX, "old_value")                                          \
    ));                                                                                          \
    BIND_GET(cls, elapsed_time);                                                                 \
    BIND_GET(cls, file);                                                                         \
    BIND_GET(cls, artboard);                                                                     \
    BIND_GET(cls, scene);                                                                        \
    BIND_GET(cls, animation);                                                                    \
    ClassDB::bind_method(D_METHOD("go_to_artboard", "artboard"), &cls::go_to_artboard);          \
    ClassDB::bind_method(D_METHOD("go_to_scene", "scene"), &cls::go_to_scene);                   \
    ClassDB::bind_method(D_METHOD("go_to_animation", "animation"), &cls::go_to_animation);       \
    ClassDB::bind_method(D_METHOD("press_mouse", "position"), &cls::press_mouse);                \
    ClassDB::bind_method(D_METHOD("release_mouse", "position"), &cls::release_mouse);            \
    ClassDB::bind_method(D_METHOD("move_mouse", "position"), &cls::move_mouse)

#define RIVE_VIEWER_WRAPPER(cls)                                             \
   private:                                                                  \
    RiveViewerBase base = RiveViewerBase(Object::cast_to<CanvasItem>(this)); \
                                                                             \
   public:                                                                   \
    void _draw() override {                                                  \
        base.on_draw();                                                      \
    }                                                                        \
    void _process(float delta) {                                             \
        base.on_process(delta);                                              \
    }                                                                        \
    void _ready() override {                                                 \
        base.on_ready();                                                     \
    }                                                                        \
    void _get_property_list(List<PropertyInfo> *list) const {                \
        base.get_property_list(list);                                        \
    }                                                                        \
    bool _set(const StringName &prop, const Variant &value) {                \
        return base.on_set(prop, value);                                     \
    }                                                                        \
    bool _get(const StringName &prop, Variant &return_value) const {         \
        return base.on_get(prop, return_value);                              \
    }                                                                        \
    RIVE_VIEWER_SETGET(String, file_path)                                    \
    RIVE_VIEWER_SETGET(int, fit)                                             \
    RIVE_VIEWER_SETGET(int, alignment)                                       \
    RIVE_VIEWER_SETGET(bool, disable_press)                                  \
    RIVE_VIEWER_SETGET(bool, disable_hover)                                  \
    RIVE_VIEWER_SETGET(bool, paused)                                         \
    RIVE_VIEWER_GET(float, elapsed_time)                                     \
    RIVE_VIEWER_GET(Ref<RiveFile>, file)                                     \
    RIVE_VIEWER_GET(Ref<RiveArtboard>, artboard)                             \
    RIVE_VIEWER_GET(Ref<RiveScene>, scene)                                   \
    RIVE_VIEWER_GET(Ref<RiveAnimation>, animation)                           \
    void go_to_artboard(Ref<RiveArtboard> artboard) {                        \
        base.go_to_artboard(artboard);                                       \
    }                                                                        \
    void go_to_scene(Ref<RiveScene> scene) {                                 \
        base.go_to_scene(scene);                                             \
    }                                                                        \
    void go_to_animation(Ref<RiveAnimation> animation) {                     \
        base.go_to_animation(animation);                                     \
    }                                                                        \
    void press_mouse(Vector2 position) {                                     \
        base.press_mouse(position);                                          \
    }                                                                        \
    void release_mouse(Vector2 position) {                                   \
        base.release_mouse(position);                                        \
    }                                                                        \
    void move_mouse(Vector2 position) {                                      \
        base.move_mouse(position);                                           \
    }

#endif