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
#include "utils/types.hpp"

const Image::Format IMAGE_FORMAT = Image::Format::FORMAT_RGBA8;

bool is_editor_hint() {
    return Engine::get_singleton()->is_editor_hint();
}

SkImageInfo get_image_info(RiveViewer *viewer) {
    return SkImageInfo::Make(
        viewer->width(),
        viewer->height(),
        SkColorType::kRGBA_8888_SkColorType,
        SkAlphaType::kUnpremul_SkAlphaType
    );
}

PackedByteArray get_byte_array(RiveViewer *viewer) {
    sk_sp<SkData> data = viewer->surface->makeImageSnapshot()->encodeToData();
    const uint8_t *bytes = data->bytes();
    PackedByteArray arr = PackedByteArray();
    arr.resize(data->size());
    for (int i = 0; i < data->size(); i++) {
        arr[i] = bytes[i];
    }
    return arr;
}

void handle_resize(RiveViewer *viewer) {
    int width = viewer->width(), height = viewer->height();
    viewer->surface = SkSurface::MakeRaster(get_image_info(viewer));
    viewer->renderer
        = rivestd::make_unique<SkiaRenderer>(viewer->surface->getCanvas());
    viewer->image = Image::create(width, height, false, IMAGE_FORMAT);
    viewer->texture = ImageTexture::create_from_image(viewer->image);
    if (viewer->file)
        viewer->renderer->align(
            rive::Fit::contain,
            rive::Alignment::topLeft,
            rive::AABB(0, 0, width, height),
            viewer->file->artboard()->bounds()
        );
}

RiveViewer::RiveViewer() {}

RiveViewer::~RiveViewer() {
    auto output = _err_output.str();
    if (output.length() > 0) GDERR("Errors: ", output.c_str());
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
    if (!is_editor_hint() && is_node_ready()) {
        Ptr<ArtboardInstance> artboard = file->artboardDefault();
        if (!artboard) {
            GDERR("Artboard is null.");
            return;
        }
        // if (state_machine) {
        //     GDPRINT("State machine found!");
        //     state_machine->advanceAndApply(delta);
        // } else
        if (animation) {
            animation->advance(_last_delta);
            animation->animation()->apply(artboard.get(), elapsed);
            if (animation->didLoop()) {
                elapsed = 0;
            }
            artboard->advance(_last_delta);
        } else {
            bool did_update = artboard->advance(_last_delta);
            if (!did_update) {
                GDERR("Did not update artboard.");
                return;
            }
        }
        if (renderer) artboard->draw(renderer.get());
        if (image->get_size() != texture->get_size()) {
            GDPRINT("Image and texture size do not match.");
            return;
        }
        image->load_png_from_buffer(get_byte_array(this));
        texture->update(image);
        draw_texture(texture, Vector2(0, 0));
        surface->getCanvas()->clear(SkColors::kTransparent);
    }
}

void RiveViewer::_process(float delta) {
    if (path.length() > 0 && file && is_node_ready() && !is_editor_hint()) {
        _last_delta = delta;
        elapsed += delta;
        queue_redraw();
    }
}

void RiveViewer::_notification(int what) {
    if (is_node_ready()) switch (what) {
            case NOTIFICATION_RESIZED:
                handle_resize(this);
        }
}

void RiveViewer::test() {
    if (file->artboardCount() > 0) {
        handle_resize(this);
        Ptr<ArtboardInstance> artboard = file->artboardDefault();
        if (artboard) {
            if (artboard->animationCount() > 0)
                animation = artboard->animationAt(0);
            if (artboard->stateMachineCount() > 0)
                state_machine = artboard->defaultStateMachine();
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
    return std::max(get_size().x, (real_t)1);
}

int RiveViewer::height() {
    return std::max(get_size().y, (real_t)1);
}

void RiveViewer::_load() {
    if (!is_editor_hint()) {
        factory = rivestd::make_unique<SkiaFactory>();
        file = read_rive_file(path, factory.get());
        if (file == nullptr) GDERR("Failed to import <", path, ">.");
        else {
            handle_resize(this);
            GDPRINT("Successfully imported <", path, ">!");
            auto artboard = file->artboardDefault();
            if (artboard) {
                GDPRINTS(
                    "Width:",
                    artboard->width(),
                    "Height:",
                    artboard->height()
                );
            }
        }
    }
}