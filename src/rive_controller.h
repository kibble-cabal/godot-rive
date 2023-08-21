#ifndef _RIVEEXTENSION_CONTROLLER_H_
#define _RIVEEXTENSION_CONTROLLER_H_

// Stdlib
#include <vector>

// Godot
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

// Rive
#include <rive/file.hpp>
#include <rive/scene.hpp>

// Skia
#include <include/core/SkCanvas.h>
#include <include/core/SkImageInfo.h>
#include <include/core/SkSurface.h>

#include <include/skia_factory.hpp>
#include <include/skia_renderer.hpp>

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
    rive::Mat2D inverse_transform;

   public:
    RiveController();
    RiveController(godot::String path_value);
    void load();
    void start();
    void resize(unsigned int width, unsigned int height);
    void pointer_down(rive::Vec2D position);
    void pointer_up(rive::Vec2D position);
    void pointer_move(rive::Vec2D position);
    void realign();
    void realign(rive::Fit fit, rive::Alignment align);

    godot::PackedByteArray frame(float delta);

   private:
    godot::PackedByteArray byte_array();
    SkImageInfo make_image_info();
};

#endif