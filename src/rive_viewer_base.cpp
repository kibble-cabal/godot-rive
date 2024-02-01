#include "rive_viewer_base.h"

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

RiveViewerBase::RiveViewerBase(CanvasItem *owner) {
    this->owner = owner;
    inst.set_props(&props);
    sk.set_props(&props);
    props.on_artboard_changed([this](int index) { _on_artboard_changed(index); });
    props.on_scene_changed([this](int index) { _on_scene_changed(index); });
    props.on_animation_changed([this](int index) { _on_animation_changed(index); });
    props.on_path_changed([this](String path) { _on_path_changed(path); });
    props.on_size_changed([this](float w, float h) { _on_size_changed(w, h); });
    props.on_transform_changed([this]() { _on_transform_changed(); });
}

void RiveViewerBase::on_input_event(const Ref<InputEvent> &event) {
    auto mouse_event = dynamic_cast<InputEventMouse *>(event.ptr());
    if (!mouse_event || is_editor_hint()) return;

    Vector2 pos = mouse_event->get_position();

    if (auto mouse_button = dynamic_cast<InputEventMouseButton *>(event.ptr())) {
        if (!props.disable_press() && mouse_button->is_pressed()) {
            inst.press_mouse(pos);
            owner->emit_signal("pressed", mouse_event->get_position());
        } else if (!props.disable_press() && mouse_button->is_released()) {
            inst.release_mouse(pos);
            owner->emit_signal("released", mouse_event->get_position());
        }
    }
    if (auto mouse_motion = dynamic_cast<InputEventMouseMotion *>(event.ptr())) {
        if (!props.disable_hover()) inst.move_mouse(pos);
    }
}

void RiveViewerBase::on_draw() {
    if (!is_null(texture)) owner->draw_texture_rect(texture, Rect2(0, 0, width(), height()), false);
}

void RiveViewerBase::on_process(float delta) {
    if (owner->is_node_ready() && !props.paused()) {
        if (is_null(image)) image = Image::create(width(), height(), false, IMAGE_FORMAT);
        if (is_null(texture)) texture = ImageTexture::create_from_image(image);
        PackedByteArray bytes = frame(delta);
        if (bytes.size()) {
            image->set_data(width(), height(), false, IMAGE_FORMAT, bytes);
            texture->update(image);
            owner->queue_redraw();
        }
        check_scene_property_changed();
    }
}

void RiveViewerBase::on_ready() {
    elapsed = 0.0;
    props.size(width(), height());
}

void RiveViewerBase::check_scene_property_changed() {
    if (props.disable_hover() && props.disable_press()) return;  // Don't bother checking if input is disabled
    auto scene = inst.scene();
    if (exists(scene))
        scene->inputs.for_each([this, scene](Ref<RiveInput> input, int _) {
            String prop = input->get_name();
            Variant old_value = cached_scene_property_values.get(prop, input->get_default());
            Variant new_value = input->get_value();
            if (old_value != new_value) owner->emit_signal("scene_property_changed", scene, prop, new_value, old_value);
            cached_scene_property_values[prop] = new_value;
        });
}

int RiveViewerBase::width() const {
    return std::max(get_size().x, (real_t)1);
}

int RiveViewerBase::height() const {
    return std::max(get_size().y, (real_t)1);
}

void RiveViewerBase::_on_path_changed(String path) {
    try {
        inst.file = RiveFile::Load(path, sk.factory.get());
        GDPRINT("Successfully imported <", path, ">!");
    } catch (RiveException error) {
        error.report();
    }
    if (exists(inst.file)) {
        _on_size_changed(props.width(), props.height());
        if (is_editor_hint()) owner->notify_property_list_changed();
    }
}

void RiveViewerBase::get_property_list(List<PropertyInfo> *list) const {
    if (owner->is_node_ready()) {
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

void RiveViewerBase::_on_artboard_changed(int _index) {
    owner->notify_property_list_changed();
}

void RiveViewerBase::_on_scene_changed(int _index) {
    cached_scene_property_values.clear();
    owner->notify_property_list_changed();
}

void RiveViewerBase::_on_animation_changed(int _index) {
    try {
        if (props.scene() != -1)
            throw RiveException("Animation will not play because a scene is selected.")
                .from(owner, "set_animation")
                .warning();
    } catch (RiveException error) {
        error.report();
    }
}

bool RiveViewerBase::on_set(const StringName &prop, const Variant &value) {
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
    inst.instantiate();
    if (exists(inst.scene()) && inst.scene()->get_input_names().has(name)) {
        props.scene_property(name, value);
        return true;
    }
    return false;
}

bool RiveViewerBase::on_get(const StringName &prop, Variant &return_value) const {
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

void RiveViewerBase::_on_size_changed(float w, float h) {
    if (!is_null(image)) unref(image);
    if (!is_null(texture)) unref(texture);
    image = Image::create(width(), height(), false, IMAGE_FORMAT);
    texture = ImageTexture::create_from_image(image);
}

void RiveViewerBase::_on_transform_changed() {
    if (sk.renderer) sk.renderer->transform(inst.current_transform);
    PackedByteArray bytes = frame(0.0);
    if (bytes.size()) {
        image->set_data(width(), height(), false, IMAGE_FORMAT, bytes);
        texture->update(image);
        owner->queue_redraw();
    }
}

bool RiveViewerBase::advance(float delta) {
    elapsed += delta;
    return inst.advance(delta);
}

PackedByteArray RiveViewerBase::redraw() {
    auto artboard = inst.artboard();
    if (sk.surface && sk.renderer && exists(artboard)) {
        sk.clear();
        inst.draw(sk.renderer.get());
        return sk.bytes();
    }
    return PackedByteArray();
}

PackedByteArray RiveViewerBase::frame(float delta) {
    if (!exists(inst.file) || !exists(inst.artboard()) || !sk.renderer || !sk.surface) return PackedByteArray();
    if (advance(delta) && owner->is_visible()) return redraw();
    return PackedByteArray();
}

float RiveViewerBase::get_elapsed_time() const {
    return elapsed;
}

Ref<RiveFile> RiveViewerBase::get_file() const {
    return inst.file;
}

Ref<RiveArtboard> RiveViewerBase::get_artboard() const {
    return inst.artboard();
}

Ref<RiveScene> RiveViewerBase::get_scene() const {
    return inst.scene();
}

Ref<RiveAnimation> RiveViewerBase::get_animation() const {
    return inst.animation();
}

void RiveViewerBase::go_to_artboard(Ref<RiveArtboard> artboard_value) {
    try {
        if (is_null(artboard_value))
            throw RiveException("Attempted to go to null artboard").from(owner, "go_to_artboard").warning();
        props.artboard(artboard_value->get_index());
    } catch (RiveException error) {
        error.report();
    }
}

void RiveViewerBase::go_to_scene(Ref<RiveScene> scene_value) {
    try {
        if (is_null(scene_value))
            throw RiveException("Attempted to go to null scene").from(owner, "go_to_scene").warning();
        props.scene(scene_value->get_index());
    } catch (RiveException error) {
        error.report();
    }
}

void RiveViewerBase::go_to_animation(Ref<RiveAnimation> animation_value) {
    try {
        if (is_null(animation_value))
            throw RiveException("Attempted to go to null animation").from(owner, "go_to_animation").warning();
        props.animation(animation_value->get_index());
        if (props.scene() != -1)
            throw RiveException("Went to animation, but it won't play because a scene is currently playing.")
                .from(owner, "go_to_animation")
                .warning();
    } catch (RiveException error) {
        error.report();
    }
}

void RiveViewerBase::press_mouse(Vector2 position) {
    inst.press_mouse(position);
}

void RiveViewerBase::release_mouse(Vector2 position) {
    inst.release_mouse(position);
}

void RiveViewerBase::move_mouse(Vector2 position) {
    inst.move_mouse(position);
}