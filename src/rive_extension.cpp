#include "rive_extension.h"

#include <algorithm>

// Godot
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>

// Rive
#include <rive/animation/linear_animation.hpp>
#include <rive/animation/linear_animation_instance.hpp>

// Extension
#include "utils/godot_macros.hpp"
#include "utils/rive.hpp"
#include "utils/types.hpp"

const Image::Format IMAGE_FORMAT = Image::Format::FORMAT_RGBA8;

bool is_editor_hint() {
    return Engine::get_singleton()->is_editor_hint();
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

    ClassDB::bind_method(D_METHOD("get_artboard"), &RiveViewer::get_artboard);
    ClassDB::bind_method(D_METHOD("set_artboard", "value"), &RiveViewer::set_artboard);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::INT, "artboard", PROPERTY_HINT_ENUM, "None:-1", PROPERTY_USAGE_NONE),
        "set_artboard",
        "get_artboard"
    );

    ClassDB::bind_method(D_METHOD("get_scene"), &RiveViewer::get_scene);
    ClassDB::bind_method(D_METHOD("set_scene", "value"), &RiveViewer::set_scene);
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::INT, "scene", PROPERTY_HINT_ENUM, "None:-1", PROPERTY_USAGE_NONE),
        "set_scene",
        "get_scene"
    );
}

void RiveViewer::_gui_input(const Ref<InputEvent> &event) {
    auto mouse_event = dynamic_cast<InputEventMouse *>(event.ptr());
    if (!mouse_event || !controller) return;

    rive::Vec2D pos = rive::Vec2D(mouse_event->get_position().x, mouse_event->get_position().y);

    if (auto mouse_button = dynamic_cast<InputEventMouseButton *>(event.ptr())) {
        if (mouse_button->is_pressed()) controller->pointer_down(pos);
        else if (mouse_button->is_released()) controller->pointer_up(pos);
    }
    if (auto mouse_motion = dynamic_cast<InputEventMouseMotion *>(event.ptr())) {
        controller->pointer_move(pos);
    }
}

void RiveViewer::_draw() {
    if (!is_editor_hint() && texture != nullptr && !texture.is_null() && texture.is_valid()) {
        draw_texture(texture, Vector2(0, 0));
    }
}

void RiveViewer::_process(float delta) {
    if (!is_editor_hint() && controller) {
        if (image == nullptr || !image.is_valid() || image.is_null())
            image = Image::create(width(), height(), false, IMAGE_FORMAT);
        if (texture == nullptr || !texture.is_valid() || texture.is_null())
            texture = ImageTexture::create_from_image(image);

        auto bytes = controller->frame(delta);
        if (bytes.size()) {
            image->load_png_from_buffer(bytes);
            texture->update(image);
            queue_redraw();
        }
    }
}

void RiveViewer::_notification(int what) {
    switch (what) {
        case NOTIFICATION_RESIZED:
            _on_resize();
    }
}

void RiveViewer::_ready() {
    if (!is_editor_hint() && path.length() > 0) {
        _on_path_changed();
        controller->start(artboard, scene);
        _on_resize();
    }
}

rive::Fit RiveViewer::get_rive_fit() {
    switch (fit) {
        case Fit::COVER:
            return rive::Fit::cover;
        case Fit::FILL:
            return rive::Fit::fill;
        case Fit::FIT_HEIGHT:
            return rive::Fit::fitHeight;
        case Fit::FIT_WIDTH:
            return rive::Fit::fitWidth;
        case Fit::NONE:
            return rive::Fit::none;
        case Fit::SCALE_DOWN:
            return rive::Fit::scaleDown;
        case Fit::CONTAIN:
        default:
            return rive::Fit::contain;
    }
}

rive::Alignment RiveViewer::get_rive_alignment() {
    switch (alignment) {
        case Align::BOTTOM_CENTER:
            return rive::Alignment::bottomCenter;
        case Align::BOTTOM_LEFT:
            return rive::Alignment::bottomLeft;
        case Align::BOTTOM_RIGHT:
            return rive::Alignment::bottomRight;
        case Align::CENTER:
            return rive::Alignment::center;
        case Align::CENTER_LEFT:
            return rive::Alignment::centerLeft;
        case Align::CENTER_RIGHT:
            return rive::Alignment::centerRight;
        case Align::TOP_CENTER:
            return rive::Alignment::topCenter;
        case Align::TOP_RIGHT:
            return rive::Alignment::topRight;
        case Align::TOP_LEFT:
        default:
            return rive::Alignment::topLeft;
    }
}

int RiveViewer::width() {
    return std::max(get_size().x, (real_t)1);
}

int RiveViewer::height() {
    return std::max(get_size().y, (real_t)1);
}

void RiveViewer::_on_resize() {
    if (controller) controller->resize(width(), height());
    image = Image::create(width(), height(), false, IMAGE_FORMAT);
    texture = ImageTexture::create_from_image(image);
}

void RiveViewer::_on_path_changed() {
    controller->path = path;
    controller->load();
    controller->realign(get_rive_fit(), get_rive_alignment());
    controller->resize(width(), height());
}

void RiveViewer::_on_path_changed_in_editor() {
    notify_property_list_changed();
    set_artboard(-1);
    set_scene(-1);
}

void RiveViewer::_get_property_list(List<PropertyInfo> *list) const {
    if (controller) {
        String artboard_hint = controller->get_artboard_property_hint();
        list->push_back(PropertyInfo(Variant::INT, "artboard", PROPERTY_HINT_ENUM, artboard_hint));
        if (artboard != -1) {
            String scene_hint = controller->get_scene_property_hint(artboard);
            list->push_back(PropertyInfo(Variant::INT, "scene", PROPERTY_HINT_ENUM, scene_hint));
        }
    }
}

void RiveViewer::set_file_path(String value) {
    path = value;
    _on_path_changed();
    if (is_editor_hint()) _on_path_changed_in_editor();
}

void RiveViewer::set_fit(int value) {
    fit = static_cast<RiveViewer::Fit>(value);
    if (controller) controller->realign(get_rive_fit(), get_rive_alignment());
}

void RiveViewer::set_alignment(int value) {
    alignment = static_cast<RiveViewer::Align>(value);
    if (controller) controller->realign(get_rive_fit(), get_rive_alignment());
}

void RiveViewer::set_artboard(int value) {
    if (artboard != value) set_scene(-1);
    artboard = value;
    if (controller) controller->set_artboard(value);
    notify_property_list_changed();
}

void RiveViewer::set_scene(int value) {
    scene = value;
}