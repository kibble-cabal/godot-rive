#include "rive_controller.h"

// rive-cpp
#include <rive/animation/state_machine_bool.hpp>
#include <rive/animation/state_machine_input_instance.hpp>
#include <rive/animation/state_machine_number.hpp>

#include "utils/read_rive_file.hpp"

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
    if (file != nullptr) {
        file_wrapper = RiveFile::MakeRef(file.get(), path);
        resize(size.x, size.y);
        GDPRINT("Successfully imported <", path, ">!");
    }
}

void RiveController::start(
    int artboard_index, int scene_index, int animation_index, const godot::Dictionary &scene_properties
) {
    try {
        elapsed = 0;
        set_artboard(artboard_index);
        set_scene(scene_index);
        set_animation(animation_index);
        set_scene_properties(scene_properties);
        if (artboard_index == -1) throw RiveException("Started rive animation, but no artboard selected.").warning();
    } catch (RiveException error) {
        error.report();
    }
}

void RiveController::resize(unsigned int width, unsigned int height) {
    size = rive::Vec2D{ (float)width, (float)height };
    surface = SkSurface::MakeRaster(make_image_info());
    renderer = rivestd::make_unique<SkiaRenderer>(surface->getCanvas());
    if (!is_null(artboard_wrapper)) artboard_wrapper->queue_redraw();
    realign();
}

void RiveController::realign() {
    if (file && artboard && renderer) {
        auto transform = rive::computeAlignment(fit, alignment, rive::AABB(0, 0, size.x, size.y), artboard->bounds());
        renderer->transform(transform);
        inverse_transform = transform.invertOrIdentity();
        if (!is_null(artboard_wrapper)) artboard_wrapper->set_inverse_transform(inverse_transform);
    }
}

void RiveController::realign(rive::Fit fit_value, rive::Alignment align_value) {
    fit = fit_value;
    alignment = align_value;
    realign();
}

godot::PackedByteArray RiveController::frame(float delta) {
    if (!file || !renderer || !surface || !artboard) {
        return godot::PackedByteArray();
    }
    elapsed += delta;
    bool did_advance = false;
    if (scene) did_advance = scene->advanceAndApply(delta);
    else if (animation) did_advance = animation->advanceAndApply(delta);
    else did_advance = artboard->advance(delta);
    if (did_advance) {
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

void RiveController::set_artboard(int index) {
    // Delete old scene reference
    if (!is_null(scene_wrapper)) unref(scene_wrapper);
    if (scene) scene.release();

    // Delete old animation reference
    if (!is_null(animation_wrapper)) unref(animation_wrapper);
    if (animation) animation.release();

    // Delete old artboard reference
    if (!is_null(artboard_wrapper)) unref(artboard_wrapper);
    if (artboard) artboard.release();

    // Make new artboard reference
    if (index > -1 && file && file->artboardCount() > index) artboard = file->artboardAt(index);
    if (artboard && !is_null(file_wrapper)) {
        artboard_wrapper = RiveArtboard::MakeRef(file_wrapper->file, artboard.get(), index);
        if (!is_null(artboard_wrapper)) artboard_wrapper->set_inverse_transform(inverse_transform);
    }
}

void RiveController::set_scene(int index) {
    // Delete old scene reference
    if (!is_null(scene_wrapper)) unref(scene_wrapper);
    if (scene) scene.release();

    // Make new scene reference
    if (index > -1 && artboard && artboard->stateMachineCount() > index) scene = artboard->stateMachineAt(index);
    if (scene && !is_null(artboard_wrapper)) {
        scene_wrapper = RiveScene::MakeRef(artboard_wrapper->artboard, scene.get(), index);
        if (!is_null(scene_wrapper)) scene_wrapper->inverse_transform = inverse_transform;
    }
}

void RiveController::set_animation(int index) {
    // Delete old animation reference
    if (!is_null(animation_wrapper)) unref(animation_wrapper);
    if (animation) animation.release();

    // Make new animation reference
    if (index > -1 && artboard && artboard->animationCount() > index) animation = artboard->animationAt(index);
    if (animation && !is_null(artboard_wrapper))
        animation_wrapper = RiveAnimation::MakeRef(artboard_wrapper->artboard, animation.get(), index);
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

godot::String RiveController::get_artboard_property_hint() {
    godot::PackedStringArray hints;
    hints.append("None:-1");
    if (file) {
        for (int i = 0; i < file->artboardCount(); i++) {
            auto ab = file->artboardAt(i);
            hints.append(
                godot::String(ab->name().c_str()) + godot::String(":") + godot::String(std::to_string(i).c_str())
            );
        }
    }
    return godot::String(",").join(hints);
}

godot::String RiveController::get_scene_property_hint() {
    godot::PackedStringArray hints;
    hints.append("None:-1");
    if (file && artboard) {
        for (int i = 0; i < artboard->stateMachineCount(); i++) {
            auto state_machine = artboard->stateMachineAt(i);
            hints.append(
                godot::String(state_machine->name().c_str()) + godot::String(":")
                + godot::String(std::to_string(i).c_str())
            );
        }
    }
    return godot::String(",").join(hints);
}

godot::String RiveController::get_animation_property_hint() {
    godot::PackedStringArray hints;
    hints.append("None:-1");
    if (file && artboard) {
        for (int i = 0; i < artboard->animationCount(); i++) {
            auto animation = artboard->animationAt(i);
            hints.append(
                godot::String(animation->name().c_str()) + godot::String(":") + godot::String(std::to_string(i).c_str())
            );
        }
    }
    return godot::String(",").join(hints);
}

godot::PackedStringArray RiveController::get_scene_property_names() {
    godot::PackedStringArray names;
    if (scene)
        for (int i = 0; i < scene->inputCount(); i++) {
            auto input = scene->input(i);
            names.append(input->name().c_str());
        }
    return names;
}

void RiveController::get_scene_property_list(godot::List<godot::PropertyInfo> *list) {
    if (scene)
        for (int i = 0; i < scene->inputCount(); i++) {
            auto input = scene->input(i);
            godot::Variant::Type type;
            if (input->input()->is<StateMachineBool>()) type = godot::Variant::BOOL;
            if (input->input()->is<StateMachineNumber>()) type = godot::Variant::FLOAT;
            list->push_back(godot::PropertyInfo(type, input->name().c_str()));
        }
}

void RiveController::set_scene_properties(const godot::Dictionary &props) {
    if (scene)
        for (int i = 0; i < scene->inputCount(); i++) {
            auto input = scene->input(i);
            godot::String input_name = input->name().c_str();
            if (props.has(input_name)) {
                if (input->input()->is<StateMachineBool>()) static_cast<SMIBool *>(input)->value(props[input_name]);
                if (input->input()->is<StateMachineNumber>()) static_cast<SMINumber *>(input)->value(props[input_name]);
            }
        }
}