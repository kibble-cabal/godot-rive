#include "rive_viewer.h"

#include <algorithm>

// godot-cpp
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>

// rive-cpp
#include <rive/animation/linear_animation.hpp>
#include <rive/animation/linear_animation_instance.hpp>

// extension
#include "rive_exceptions.hpp"
#include "utils/godot_macros.hpp"
#include "utils/types.hpp"

const Image::Format IMAGE_FORMAT = Image::Format::FORMAT_RGBA8;

bool is_editor_hint() {
    return Engine::get_singleton()->is_editor_hint();
}

RiveViewer::RiveViewer() {
    inst.set_props(&props);
    sk.set_props(&props);
    props.on_artboard_changed([this](int index) { _on_artboard_changed(index); });
    props.on_scene_changed([this](int index) { _on_scene_changed(index); });
    props.on_animation_changed([this](int index) { _on_animation_changed(index); });
    props.on_path_changed([this](String path) { _on_path_changed(path); });
    props.on_size_changed([this](float w, float h) { _on_size_changed(w, h); });
}

void RiveViewer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_file_path"), &RiveViewer::get_file_path);
    ClassDB::bind_method(D_METHOD("set_file_path", "value"), &RiveViewer::set_file_path);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::STRING, "path", PROPERTY_HINT_FILE, "*.riv"),
        "set_file_path",
        "get_file_path"
    );

    ClassDB::bind_method(D_METHOD("get_fit"), &RiveViewer::get_fit);
    ClassDB::bind_method(D_METHOD("set_fit", "value"), &RiveViewer::set_fit);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::INT, "fit", PROPERTY_HINT_ENUM, FitEnumPropertyHint),
        "set_fit",
        "get_fit"
    );

    ClassDB::bind_method(D_METHOD("get_alignment"), &RiveViewer::get_alignment);
    ClassDB::bind_method(D_METHOD("set_alignment", "value"), &RiveViewer::set_alignment);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::INT, "alignment", PROPERTY_HINT_ENUM, AlignEnumPropertyHint),
        "set_alignment",
        "get_alignment"
    );

    ClassDB::bind_method(D_METHOD("get_disable_press"), &RiveViewer::get_disable_press);
    ClassDB::bind_method(D_METHOD("set_disable_press", "value"), &RiveViewer::set_disable_press);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::BOOL, "disable_press"),
        "set_disable_press",
        "get_disable_press"
    );
    ClassDB::bind_method(D_METHOD("get_disable_hover"), &RiveViewer::get_disable_hover);
    ClassDB::bind_method(D_METHOD("set_disable_hover", "value"), &RiveViewer::set_disable_hover);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::BOOL, "disable_hover"),
        "set_disable_hover",
        "get_disable_hover"
    );
    ClassDB::bind_method(D_METHOD("get_paused"), &RiveViewer::get_paused);
    ClassDB::bind_method(D_METHOD("set_paused", "value"), &RiveViewer::set_paused);
    ClassDB::add_property(get_class_static(), PropertyInfo(Variant::BOOL, "paused"), "set_paused", "get_paused");

    /* Signals */
    ADD_SIGNAL(MethodInfo("pressed", PropertyInfo(Variant::VECTOR2, "position")));
    ADD_SIGNAL(MethodInfo("released", PropertyInfo(Variant::VECTOR2, "position")));
    ADD_SIGNAL(MethodInfo(
        "scene_property_changed",
        PropertyInfo(Variant::OBJECT, "scene"),
        PropertyInfo(Variant::STRING, "property"),
        PropertyInfo(Variant::VARIANT_MAX, "new_value"),
        PropertyInfo(Variant::VARIANT_MAX, "old_value")
    ));

    /* API methods */
    ClassDB::bind_method(D_METHOD("get_elapsed_time"), &RiveViewer::get_elapsed_time);
    ClassDB::bind_method(D_METHOD("get_file"), &RiveViewer::get_file);
    ClassDB::bind_method(D_METHOD("get_artboard"), &RiveViewer::get_artboard);
    ClassDB::bind_method(D_METHOD("get_scene"), &RiveViewer::get_scene);
    ClassDB::bind_method(D_METHOD("get_animation"), &RiveViewer::get_animation);
    ClassDB::bind_method(D_METHOD("go_to_artboard", "artboard"), &RiveViewer::go_to_artboard);
    ClassDB::bind_method(D_METHOD("go_to_scene", "scene"), &RiveViewer::go_to_scene);
    ClassDB::bind_method(D_METHOD("go_to_animation", "animation"), &RiveViewer::go_to_animation);
    ClassDB::bind_method(D_METHOD("press_mouse", "position"), &RiveViewer::press_mouse);
    ClassDB::bind_method(D_METHOD("release_mouse", "position"), &RiveViewer::release_mouse);
    ClassDB::bind_method(D_METHOD("move_mouse", "position"), &RiveViewer::move_mouse);
}

void RiveViewer::_gui_input(const Ref<InputEvent> &event) {
    auto mouse_event = dynamic_cast<InputEventMouse *>(event.ptr());
    if (!mouse_event || is_editor_hint()) return;

    Vector2 pos = mouse_event->get_position();

    if (auto mouse_button = dynamic_cast<InputEventMouseButton *>(event.ptr())) {
        if (!props.disable_press() && mouse_button->is_pressed()) {
            inst.press_mouse(pos);
            emit_signal("pressed", mouse_event->get_position());
        } else if (!props.disable_press() && mouse_button->is_released()) {
            inst.release_mouse(pos);
            emit_signal("released", mouse_event->get_position());
        }
    }
    if (auto mouse_motion = dynamic_cast<InputEventMouseMotion *>(event.ptr())) {
        if (!props.disable_hover()) inst.move_mouse(pos);
    }
}

void RiveViewer::_draw() {
    if (!is_null(texture)) draw_texture_rect(texture, Rect2(0, 0, width(), height()), false);
}

void RiveViewer::_process(float delta) {
    if (is_node_ready() && !props.paused()) {
        if (is_null(image)) image = Image::create(width(), height(), false, IMAGE_FORMAT);
        if (is_null(texture)) texture = ImageTexture::create_from_image(image);
        PackedByteArray bytes = frame(delta);
        if (bytes.size()) {
            image->set_data(width(), height(), false, IMAGE_FORMAT, bytes);
            texture->update(image);
            queue_redraw();
        }
        check_scene_property_changed();
    }
}

void RiveViewer::_notification(int what) {
    switch (what) {
        case NOTIFICATION_RESIZED:
            props.size(width(), height());
    }
}

void RiveViewer::_ready() {
    elapsed = 0.0;
    props.size(width(), height());
}

void RiveViewer::check_scene_property_changed() {
    if (props.disable_hover() && props.disable_press()) return;  // Don't bother checking if input is disabled
    auto scene = inst.scene();
    if (exists(scene))
        scene->inputs.for_each([this, scene](Ref<RiveInput> input, int _) {
            String prop = input->get_name();
            Variant old_value = cached_scene_property_values.get(prop, input->get_default());
            Variant new_value = input->get_value();
            if (old_value != new_value) emit_signal("scene_property_changed", scene, prop, new_value, old_value);
            cached_scene_property_values[prop] = new_value;
        });
}

int RiveViewer::width() {
    return std::max(get_size().x, (real_t)1);
}

int RiveViewer::height() {
    return std::max(get_size().y, (real_t)1);
}

void RiveViewer::_on_path_changed(String path) {
    try {
        inst.file = RiveFile::Load(path, sk.factory.get());
        GDPRINT("Successfully imported <", path, ">!");
    } catch (RiveException error) {
        error.report();
    }
    if (exists(inst.file)) {
        _on_size_changed(props.width(), props.height());
        if (is_editor_hint()) notify_property_list_changed();
    }
}

void RiveViewer::_get_property_list(List<PropertyInfo> *list) const {
    if (is_node_ready()) {
        inst.instantiate();
        if (exists(inst.file)) {
            String artboard_hint = inst.file->_get_artboard_property_hint();
            list->push_back(PropertyInfo(Variant::INT, "artboard", PROPERTY_HINT_ENUM, artboard_hint));
        }
        auto artboard = inst.artboard();
        if (exists(artboard)) {
            String scene_hint = artboard->_get_scene_property_hint();
            list->push_back(PropertyInfo(Variant::INT, "scene", PROPERTY_HINT_ENUM, scene_hint));
            String anim_hint = artboard->_get_animation_property_hint();
            list->push_back(PropertyInfo(Variant::INT, "animation", PROPERTY_HINT_ENUM, anim_hint));
        }
        auto scene = inst.scene();
        if (exists(scene)) {
            list->push_back(PropertyInfo(Variant::NIL, "Scene", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_CATEGORY));
            scene->_get_input_property_list(list);
        }
    }
}

void RiveViewer::_on_artboard_changed(int _index) {
    notify_property_list_changed();
}

void RiveViewer::_on_scene_changed(int _index) {
    cached_scene_property_values.clear();
    notify_property_list_changed();
}

void RiveViewer::_on_animation_changed(int _index) {
    try {
        if (props.scene() != -1)
            throw RiveException("Animation will not play because a scene is selected.")
                .from(this, "set_animation")
                .warning();
    } catch (RiveException error) {
        error.report();
    }
}

bool RiveViewer::_set(const StringName &prop, const Variant &value) {
    String name = prop;
    if (name == "artboard") {
        props.artboard((int)value);
        return true;
    }
    if (name == "scene") {
        props.scene((int)value);
        return true;
    }
    if (name == "animation") {
        props.animation((int)value);
        return true;
    }
    if (exists(inst.scene()) && inst.scene()->get_input_names().has(name)) {
        props.scene_property(name, value);
        return true;
    }
    return false;
}

bool RiveViewer::_get(const StringName &prop, Variant &return_value) const {
    String name = prop;
    if (name == "artboard") {
        return_value = props.artboard();
        return true;
    }
    if (name == "scene") {
        return_value = props.scene();
        return true;
    }
    if (name == "animation") {
        return_value = props.animation();
        return true;
    }
    if (props.has_scene_property(name)) {
        return_value = props.scene_property(name);
        return true;
    }
    return false;
}

void RiveViewer::_on_size_changed(float w, float h) {
    if (sk.renderer) sk.renderer->transform(inst.get_transform());
    if (!is_null(image)) unref(image);
    if (!is_null(texture)) unref(texture);
    image = Image::create(width(), height(), false, IMAGE_FORMAT);
    texture = ImageTexture::create_from_image(image);
}

bool RiveViewer::advance(float delta) {
    elapsed += delta;
    return inst.advance(delta);
}

PackedByteArray RiveViewer::redraw() {
    auto artboard = inst.artboard();
    if (sk.surface && sk.renderer && exists(artboard)) {
        sk.clear();
        inst.draw(sk.renderer.get());
        return sk.bytes();
    }
    return PackedByteArray();
}

PackedByteArray RiveViewer::frame(float delta) {
    if (!exists(inst.file) || !exists(inst.artboard()) || !sk.renderer || !sk.surface) return PackedByteArray();
    if (advance(delta) && is_visible()) return redraw();
    return PackedByteArray();
}

float RiveViewer::get_elapsed_time() const {
    return elapsed;
}

Ref<RiveFile> RiveViewer::get_file() const {
    return inst.file;
}

Ref<RiveArtboard> RiveViewer::get_artboard() const {
    return inst.artboard();
}

Ref<RiveScene> RiveViewer::get_scene() const {
    return inst.scene();
}

Ref<RiveAnimation> RiveViewer::get_animation() const {
    return inst.animation();
}

void RiveViewer::go_to_artboard(Ref<RiveArtboard> artboard_value) {
    try {
        if (is_null(artboard_value))
            throw RiveException("Attempted to go to null artboard").from(this, "go_to_artboard").warning();
        props.artboard(artboard_value->get_index());
    } catch (RiveException error) {
        error.report();
    }
}

void RiveViewer::go_to_scene(Ref<RiveScene> scene_value) {
    try {
        if (is_null(scene_value))
            throw RiveException("Attempted to go to null scene").from(this, "go_to_scene").warning();
        props.scene(scene_value->get_index());
    } catch (RiveException error) {
        error.report();
    }
}

void RiveViewer::go_to_animation(Ref<RiveAnimation> animation_value) {
    try {
        if (is_null(animation_value))
            throw RiveException("Attempted to go to null animation").from(this, "go_to_animation").warning();
        props.animation(animation_value->get_index());
        if (props.scene() != -1)
            throw RiveException("Went to animation, but it won't play because a scene is currently playing.")
                .from(this, "go_to_animation")
                .warning();
    } catch (RiveException error) {
        error.report();
    }
}

void RiveViewer::press_mouse(Vector2 position) {
    inst.press_mouse(position);
}

void RiveViewer::release_mouse(Vector2 position) {
    inst.release_mouse(position);
}

void RiveViewer::move_mouse(Vector2 position) {
    inst.move_mouse(position);
}