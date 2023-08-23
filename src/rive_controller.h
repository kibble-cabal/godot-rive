#ifndef _RIVEEXTENSION_CONTROLLER_H_
#define _RIVEEXTENSION_CONTROLLER_H_

// Stdlib
#include <vector>

// Godot
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

// Rive
#include <rive/animation/state_machine_instance.hpp>
#include <rive/file.hpp>

// Skia
#include <skia/dependencies/skia/include/core/SkBitmap.h>
#include <skia/dependencies/skia/include/core/SkCanvas.h>
#include <skia/dependencies/skia/include/core/SkSurface.h>

#include <skia/renderer/include/skia_factory.hpp>
#include <skia/renderer/include/skia_renderer.hpp>

// Extension
#include "utils/types.hpp"

class RiveController {
   public:
    godot::String path;
    rive::Vec2D size = rive::Vec2D{ 1, 1 };

    rive::Fit fit = rive::Fit::contain;
    rive::Alignment alignment = rive::Alignment::topLeft;

   private:
    float elapsed = 0;
    Ptr<rive::File> file = nullptr;
    sk_sp<SkSurface> surface = nullptr;
    Ptr<rive::SkiaRenderer> renderer = nullptr;
    Ptr<rive::SkiaFactory> factory = nullptr;
    Ptr<rive::ArtboardInstance> artboard = nullptr;
    Ptr<rive::Scene> scene = nullptr;
    Ptr<rive::LinearAnimationInstance> animation = nullptr;
    rive::Mat2D inverse_transform;

   public:
    RiveController();
    RiveController(godot::String path_value);
    void load();
    void start(int artboard_index, int scene_index, int animation_index);
    void resize(unsigned int width, unsigned int height);
    void pointer_down(rive::Vec2D position);
    void pointer_up(rive::Vec2D position);
    void pointer_move(rive::Vec2D position);
    void realign();
    void realign(rive::Fit fit, rive::Alignment align);
    void set_artboard(int artboard_index);
    void set_scene(int scene_index);
    void set_animation(int animation_index);
    godot::String get_artboard_property_hint();
    godot::String get_state_machine_property_hint(int artboard_index);
    godot::String get_animation_property_hint(int artboard_index);
    godot::PackedByteArray frame(float delta);

   private:
    godot::PackedByteArray byte_array();
    SkImageInfo make_image_info();
};

#endif