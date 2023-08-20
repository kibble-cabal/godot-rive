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

RiveViewer::RiveViewer() {}

RiveViewer::~RiveViewer() {
    auto output = _err_output.str();
    if (output.length() > 0) GDERR("Errors: ", output.c_str());
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
}

void RiveViewer::_gui_input(const Ref<InputEvent> &event) {
    auto mouse_event = dynamic_cast<InputEventMouse *>(event.ptr());
    if (!mouse_event || !controller) return;

    rive::Vec2D pos = rive::Vec2D(mouse_event->get_position().x, mouse_event->get_position().y);

    if (auto mouse_button = dynamic_cast<InputEventMouseButton *>(event.ptr())) {
        GDPRINT("button!", mouse_button->is_pressed());
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
    if (is_node_ready() && controller) switch (what) {
            case NOTIFICATION_RESIZED:
                _on_resize();
        }
}

void RiveViewer::_ready() {
    if (!is_editor_hint() && path.length() > 0) {
        controller = rivestd::make_unique<RiveController>(path);
        controller->load();
        controller->start();
        _on_resize();
    }
}

String RiveViewer::get_file_path() {
    return path;
}

void RiveViewer::set_file_path(String value) {
    path = value;
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
