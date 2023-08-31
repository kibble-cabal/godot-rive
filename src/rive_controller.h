#ifndef _RIVEEXTENSION_CONTROLLER_H_
#define _RIVEEXTENSION_CONTROLLER_H_

// Stdlib
#include <vector>

// Godot
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/templates/list.hpp>
#include <godot_cpp/variant/dictionary.hpp>
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
#include "api/rive_file.hpp"
#include "utils/memory.hpp"
#include "viewer_props.hpp"

class RiveController {
   public:
    float elapsed = 0;
    bool is_visible = true;

    friend class RiveViewer;

   private:
    Ref<RiveFile> file_wrapper;
    Ptr<rive::File> file = nullptr;

    Ref<RiveArtboard> artboard_wrapper;
    Ptr<rive::ArtboardInstance> artboard = nullptr;

    Ref<RiveScene> scene_wrapper;
    Ptr<rive::StateMachineInstance> scene = nullptr;

    Ref<RiveAnimation> animation_wrapper;
    Ptr<rive::LinearAnimationInstance> animation = nullptr;

    sk_sp<SkSurface> surface = nullptr;
    Ptr<rive::SkiaRenderer> renderer = nullptr;
    Ptr<rive::SkiaFactory> factory = nullptr;
    rive::Mat2D inverse_transform;

    ViewerProps *props = nullptr;

   public:
    RiveController();
    RiveController(ViewerProps *props_value);

    void load();
    void resize();
    void pointer_down(rive::Vec2D position);
    void pointer_up(rive::Vec2D position);
    void pointer_move(rive::Vec2D position);
    void realign();
    void on_scene_properties_changed();
    void on_artboard_changed(int index);
    void on_scene_changed(int index);
    void on_animation_changed(int index);
    godot::String get_artboard_property_hint();
    godot::String get_scene_property_hint();
    godot::String get_animation_property_hint();
    godot::PackedStringArray get_scene_property_names();
    void get_scene_property_list(godot::List<godot::PropertyInfo> *list);
    void set_scene_properties(const godot::Dictionary &props);
    void add_listeners();
    godot::PackedByteArray frame(float delta);
    godot::PackedByteArray editor_frame(float delta);

   private:
    godot::PackedByteArray redraw();
    godot::PackedByteArray byte_array();
    SkImageInfo make_image_info();
    bool advance(float delta);
    void ensure_has_instances();
};

#endif