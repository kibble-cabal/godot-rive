#include "rive_controller.h"

#include "utils/read_rive_file.hpp"
#include "utils/types.hpp"

RiveController::RiveController() {}

RiveController::RiveController(godot::String path_value) {
    path = path_value;
}

SkImageInfo RiveController::make_image_info() {
    return SkImageInfo::Make(size.x, size.y, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
}

void RiveController::load() {
    factory = rivestd::make_unique<rive::SkiaFactory>();
    file = read_rive_file(path, factory.get());
    if (file == nullptr) GDERR("Failed to import <", path, ">.");
    else {
        resize(size.x, size.y);
        GDPRINT("Successfully imported <", path, ">!");
        artboard = file->artboardDefault();
        if (artboard) GDPRINTS("Width:", artboard->width(), "Height:", artboard->height());
    }
}

void RiveController::start() {
    if (file && artboard) {
        scene = artboard->defaultScene();
    }
}

void RiveController::resize(unsigned int width, unsigned int height) {
    size = rive::Vec2D{ (float)width, (float)height };
    surface = SkSurface::MakeRaster(make_image_info());
    renderer = rivestd::make_unique<SkiaRenderer>(surface->getCanvas());
    realign();
}

void RiveController::realign() {
    if (file && renderer) {
        auto transform
            = rive::computeAlignment(fit, alignment, rive::AABB(0, 0, size.x, size.y), file->artboard()->bounds());
        // renderer->transform(transform);
        inverse_transform = transform.invertOrIdentity();
    }
}

void RiveController::realign(rive::Fit fit_value, rive::Alignment align_value) {
    fit = fit_value;
    alignment = align_value;
    realign();
}

godot::PackedByteArray RiveController::frame(float delta) {
    if (!file || !renderer || !surface || !artboard) {
        GDPRINT("Unable to render frame.");
        return godot::PackedByteArray();
    }
    elapsed += delta;
    if (scene ? scene->advanceAndApply(delta) : artboard->advance(delta)) {
        surface->getCanvas()->clear(SkColors::kTransparent);
        artboard->draw(renderer.get());
        return byte_array();
    }
    return godot::PackedByteArray();
}

void RiveController::pointer_down(rive::Vec2D position) {
    if (scene) scene->pointerDown(inverse_transform * position);
}

void RiveController::pointer_up(rive::Vec2D position) {
    if (scene) scene->pointerUp(inverse_transform * position);
}

void RiveController::pointer_move(rive::Vec2D position) {
    if (scene) scene->pointerMove(inverse_transform * position);
}

godot::PackedByteArray RiveController::byte_array() {
    if (!surface) return godot::PackedByteArray();
    sk_sp<SkData> data = surface->makeImageSnapshot()->encodeToData();
    const uint8_t *bytes = data->bytes();
    godot::PackedByteArray arr = godot::PackedByteArray();
    arr.resize(data->size());
    for (int i = 0; i < data->size(); i++) {
        arr[i] = bytes[i];
    }
    return arr;
}