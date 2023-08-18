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
#include "utils/read_rive_file.hpp"
#include "utils/rive.hpp"
#include "utils/tvg.hpp"
#include "utils/types.hpp"

const Image::Format IMAGE_FORMAT = Image::Format::FORMAT_RGBA8;

bool is_editor_hint() {
    return Engine::get_singleton()->is_editor_hint();
}

PackedByteArray Img::get_byte_array() {
    PackedByteArray arr = PackedByteArray();
    arr.resize(bytes.size() * 4);
    for (int i = 0; i < bytes.size(); i++) {
        arr.encode_u32(i * 4, bytes[i]);
    }
    return arr;
}

/**
 * Clears current draw state.
 */
void Rive::reset() {
    canvas = tvg::SwCanvas::gen();
    renderer = TvgRenderer(canvas.get());
    factory = rivestd::make_unique<TvgFactory>();
}

/**
 * Clears current image/buffer state.
 */
void Img::reset() {
    bytes.clear();
    image = Image::create(1, 1, false, IMAGE_FORMAT);
}

RiveViewer::RiveViewer() {
    if (!is_editor_hint()) {
        riv.reset();
        img.reset();
        tvg::Result result = tvg::Initializer::init(tvg::CanvasEngine::Sw, 0);
        if (result != tvg::Result::Success) {
            GDERR("Error initializing ThorVG: ", as_string(result).c_str());
            return;
        }
    }
}

RiveViewer::~RiveViewer() {
    tvg::Initializer::term(tvg::CanvasEngine::Sw);
    auto output = _err_output.str();
    if (output.length() > 0) GDERR("Errors: ", output.c_str());
}

void RiveViewer::_update_image_size() {
    if (img.bytes.size() < width() * height()) {
        img.bytes.resize(width() * height());
    }
}

void RiveViewer::_update_target() {
    if (!is_editor_hint()) {
        _update_image_size();
        GDPRINTS(
            "Width:",
            riv.file->artboard()->bounds().width(),
            "Height:",
            riv.file->artboard()->bounds().height()
        );
        riv.renderer.align(
            rive::Fit::contain,
            rive::Alignment::topLeft,
            rive::AABB(0, 0, width(), height()),
            riv.file->artboard()->bounds()
        );
        tvg::Result result = riv.canvas->target(
            img.bytes.data(),
            width(),
            width(),
            height(),
            tvg::SwCanvas::ABGR8888
        );
        if (result != tvg::Result::Success) {
            GDERR("Error updating canvas target: ", as_string(result).c_str());
            return;
        }
        img.image->set_data(
            width(),
            height(),
            false,
            IMAGE_FORMAT,
            img.get_byte_array()
        );
    }
}

void RiveViewer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("test"), &RiveViewer::test);
    ClassDB::bind_method(D_METHOD("get_file_path"), &RiveViewer::get_file_path);
    ClassDB::bind_method(
        D_METHOD("set_file_path", "value"),
        &RiveViewer::set_file_path
    );
    ClassDB::add_property(
        get_class_static(),
        PropertyInfo(Variant::STRING, "path", PROPERTY_HINT_FILE),
        "set_file_path",
        "get_file_path"
    );
}

void RiveViewer::_draw() {
    if (img.texture != nullptr) {
        draw_texture(img.texture, Vector2(0, 0));
    }
}

void RiveViewer::_process(float delta) {
    if (path.length() > 0 && is_node_ready() && !is_editor_hint()) {
        _render(delta);
    }
}

void RiveViewer::_render(float delta) {
    Ptr<ArtboardInstance> artboard = riv.file->artboardDefault();
    if (!artboard) {
        GDERR("Artboard is null.");
        return;
    }
    elapsed += delta;

    // if (riv.state_machine) {
    //     GDPRINT("State machine found!");
    //     riv.state_machine->advanceAndApply(delta);
    // } else
    if (riv.animation) {
        riv.animation->advance(delta);
        riv.animation->animation()->apply(artboard.get(), elapsed);
        if (riv.animation->didLoop()) {
            elapsed = 0;
        }
        artboard->advance(delta);
    } else {
        bool did_update = artboard->advance(delta);
        if (!did_update) {
            GDERR("Did not update artboard.");
            return;
        }
    }

    artboard->draw(&riv.renderer);

    // Sync canvas
    tvg::Result result = riv.canvas->draw();
    if (result != tvg::Result::Success) {
        GDERR("Error drawing: ", as_string(result).c_str());
        return;
    }
    riv.canvas->sync();

    // Update texture
    img.image->set_data(
        width(),
        height(),
        false,
        IMAGE_FORMAT,
        img.get_byte_array()
    );
    img.texture = ImageTexture::create_from_image(img.image);
    queue_redraw();

    riv.canvas->clear();
}

void RiveViewer::test() {
    _update_target();

    if (riv.file->artboardCount() > 0) {
        Ptr<ArtboardInstance> artboard = riv.file->artboardDefault();
        if (artboard) {
            if (artboard->animationCount() > 0)
                riv.animation = artboard->animationAt(0);
            if (artboard->stateMachineCount() > 0)
                riv.state_machine = artboard->defaultStateMachine();
        }
    }
}

String RiveViewer::get_file_path() {
    return path;
}

void RiveViewer::set_file_path(String value) {
    path = value;
    _load();
}

int RiveViewer::width() {
    return get_size().x;
}

int RiveViewer::height() {
    return get_size().y;
}

rive::Factory *RiveViewer::factory() {
    return riv.factory.get();
}

void RiveViewer::_reset_target() {
    riv.reset();
    img.reset();
}

void RiveViewer::_load() {
    _reset_target();
    if (!is_editor_hint()) {
        riv.file = read_rive_file(path, factory());
        if (riv.file == nullptr) GDERR("Failed to import <", path, ">.");
        else GDPRINT("Successfully imported <", path, ">!");
    }
}