#include "rive_extension.h"

#include <algorithm>

// Godot
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>

// Rive
#include <rive/animation/linear_animation.hpp>
#include <rive/animation/linear_animation_instance.hpp>

// Extension
#include "rive_exceptions.hpp"
#include "utils/godot_macros.hpp"
#include "utils/types.hpp"

const Image::Format IMAGE_FORMAT = Image::Format::FORMAT_RGBA8;

bool is_editor_hint() {
    return Engine::get_singleton()->is_editor_hint();
}

void RiveViewer::_init() {
    props.on_artboard_changed([this](int index) -> void { _on_artboard_changed(index); });
    props.on_scene_changed([this](int index) -> void { _on_scene_changed(index); });
    props.on_animation_changed([this](int index) -> void { _on_animation_changed(index); });
    props.on_path_changed([this](String path) -> void { _on_path_changed(path); });
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
}

void RiveViewer::_gui_input(const Ref<InputEvent> &event) {
    auto mouse_event = dynamic_cast<InputEventMouse *>(event.ptr());
    if (!mouse_event || !controller || is_editor_hint()) return;

    rive::Vec2D pos = rive::Vec2D(mouse_event->get_position().x, mouse_event->get_position().y);

    if (auto mouse_button = dynamic_cast<InputEventMouseButton *>(event.ptr())) {
        if (!props.disable_press() && mouse_button->is_pressed()) {
            controller->pointer_down(pos);
            emit_signal("pressed", mouse_event->get_position());
        } else if (!props.disable_press() && mouse_button->is_released()) {
            controller->pointer_up(pos);
            emit_signal("released", mouse_event->get_position());
        }
    }
    if (auto mouse_motion = dynamic_cast<InputEventMouseMotion *>(event.ptr())) {
        if (!props.disable_hover()) controller->pointer_move(pos);
    }
}

void RiveViewer::_draw() {
    if (!is_null(texture)) {
        draw_texture_rect(texture, Rect2(0, 0, width(), height()), false);
    }
}

void RiveViewer::_process(float delta) {
    if (is_node_ready() && controller) {
        if (is_null(image)) image = Image::create(width(), height(), false, IMAGE_FORMAT);
        if (is_null(texture)) texture = ImageTexture::create_from_image(image);
        controller->is_visible = is_visible();
        PackedByteArray bytes;
        if (is_editor_hint()) bytes = controller->editor_frame(delta);
        else bytes = controller->frame(delta);
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
            _on_resize();
    }
}

void RiveViewer::_ready() {
    _on_resize();
}

void RiveViewer::check_scene_property_changed() {
    if (props.disable_hover() && props.disable_press()) return;  // Don't bother checking if input is disabled
    if (controller && !is_null(controller->scene_wrapper)) {
        auto inputs = controller->scene_wrapper->get_inputs();
        for (int i = 0; i < inputs.size(); i++) {
            Ref<RiveInput> input = inputs[i];
            String prop = input->get_name();
            Variant old_value = cached_scene_property_values.get(prop, input->get_default());
            Variant new_value = input->get_value();
            if (old_value != new_value)
                emit_signal("scene_property_changed", controller->scene_wrapper, prop, new_value, old_value);
            cached_scene_property_values[prop] = new_value;
        }
    }
}

int RiveViewer::width() {
    return std::max(get_size().x, (real_t)1);
}

int RiveViewer::height() {
    return std::max(get_size().y, (real_t)1);
}

void RiveViewer::_on_resize() {
    props.size(width(), height());
    if (!is_null(image)) unref(image);
    if (!is_null(texture)) unref(texture);
    image = Image::create(width(), height(), false, IMAGE_FORMAT);
    texture = ImageTexture::create_from_image(image);
}

void RiveViewer::_on_path_changed(String path) {
    if (is_editor_hint()) notify_property_list_changed();
}

void RiveViewer::_get_property_list(List<PropertyInfo> *list) const {
    if (controller) {
        String artboard_hint = controller->get_artboard_property_hint();
        list->push_back(PropertyInfo(Variant::INT, "artboard", PROPERTY_HINT_ENUM, artboard_hint));
        if (props.artboard() != -1) {
            String scene_hint = controller->get_scene_property_hint();
            list->push_back(PropertyInfo(Variant::INT, "scene", PROPERTY_HINT_ENUM, scene_hint));
            list->push_back(PropertyInfo(
                Variant::INT,
                "animation",
                PROPERTY_HINT_ENUM,
                controller->get_animation_property_hint(),
                (props.scene() != -1) ? (PROPERTY_USAGE_DEFAULT & PROPERTY_USAGE_READ_ONLY) : PROPERTY_USAGE_DEFAULT
            ));
        }
        if (props.scene() != -1) {
            list->push_back(PropertyInfo(Variant::NIL, "Scene", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_CATEGORY));
            controller->get_scene_property_list(list);
        }
    }
}

void RiveViewer::set_file_path(String value) {
    props.path(value);
}

void RiveViewer::set_fit(int value) {
    props.fit((Fit)value);
}

void RiveViewer::set_alignment(int value) {
    props.alignment((Align)value);
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

void RiveViewer::set_disable_press(bool value) {
    props.disable_press(value);
}

void RiveViewer::set_disable_hover(bool value) {
    props.disable_hover(value);
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
    if (controller && controller->get_scene_property_names().has(name)) {
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

float RiveViewer::get_elapsed_time() const {
    if (controller) return controller->elapsed;
    return 0.0;
}

Ref<RiveFile> RiveViewer::get_file() const {
    if (controller) return controller->file_wrapper;
    return nullptr;
}

Ref<RiveArtboard> RiveViewer::get_artboard() const {
    if (controller) return controller->artboard_wrapper;
    return nullptr;
}

Ref<RiveScene> RiveViewer::get_scene() const {
    if (controller) return controller->scene_wrapper;
    return nullptr;
}

Ref<RiveAnimation> RiveViewer::get_animation() const {
    if (controller) return controller->animation_wrapper;
    return nullptr;
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