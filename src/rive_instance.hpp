#ifndef _RIVEEXTENSION_RIVE_INSTANCE_HPP_
#define _RIVEEXTENSION_RIVE_INSTANCE_HPP_

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

struct RiveInstance {
    friend class RiveViewer;

    ViewerProps *props;
    Ref<RiveFile> file;
    rive::Mat2D current_transform;

    void set_props(ViewerProps *props_value) {
        props = props_value;
        if (props) {
            props->on_path_changed([this](godot::String path) { on_path_changed(path); });
            props->on_artboard_changed([this](int index) { on_artboard_changed(index); });
            props->on_scene_changed([this](int index) { on_scene_changed(index); });
            props->on_animation_changed([this](int index) { on_animation_changed(index); });
            props->on_scene_properties_changed([this]() { on_scene_properties_changed(); });
            props->on_transform_changed([this]() { on_transform_changed(); });
        }
    }

    rive::Mat2D get_transform() const {
        auto ab = artboard();
        if (props && exists(ab))
            return rive::computeAlignment(
                props->rive_fit(),
                props->rive_alignment(),
                rive::AABB(0, 0, props->width(), props->height()),
                ab->artboard->bounds()
            );
        return rive::Mat2D();
    }

    rive::Mat2D get_inverse_transform() const {
        return get_transform().invertOrIdentity();
    }

    void reset() {
        auto ab = artboard();
        if (exists(file) && props->artboard() != -1) file->reset_artboard(props->artboard());
        if (exists(ab) && props->scene() != -1) ab->reset_scene(props->scene());
        if (exists(ab) && props->animation() != -1) ab->reset_animation(props->animation());
    }

    bool advance(float delta) {
        auto sm = scene();
        auto anim = animation();
        auto ab = artboard();
        if (exists(sm)) return sm->scene->advanceAndApply(delta);
        else if (exists(anim)) return anim->animation->advanceAndApply(delta);
        else if (exists(ab)) return ab->artboard->advance(delta);
        else return false;
    }

    void press_mouse(godot::Vector2 position) {
        auto sm = scene();
        if (exists(sm)) sm->press_mouse(current_transform.invertOrIdentity(), position);
    }

    void release_mouse(godot::Vector2 position) {
        auto sm = scene();
        if (exists(sm)) sm->release_mouse(current_transform.invertOrIdentity(), position);
    }

    void move_mouse(godot::Vector2 position) {
        auto sm = scene();
        if (exists(sm)) sm->move_mouse(current_transform.invertOrIdentity(), position);
    }

    void draw(rive::Renderer *renderer) {
        auto ab = artboard();
        if (exists(ab)) ab->artboard->draw(renderer);
    }

   protected:
    void instantiate() const {
        try {
            if (exists(file)) file->_instantiate_artboards();
            auto ab = artboard();
            if (exists(ab)) {
                ab->_instantiate_scenes();
                ab->_instantiate_animations();
            }
            auto sm = scene();
            if (exists(sm)) sm->_instantiate_inputs();
        } catch (RiveException error) {
            error.report();
        }
    }

    Ref<RiveArtboard> artboard() const {
        if (exists(file) && props->artboard() != -1) return file->get_artboard(props->artboard());
        return nullptr;
    }

    Ref<RiveScene> scene() const {
        if (exists(artboard()) && props->scene() != -1) return artboard()->get_scene(props->scene());
        return nullptr;
    }

    Ref<RiveAnimation> animation() const {
        if (exists(artboard()) && props->animation() != -1) return artboard()->get_animation(props->animation());
        return nullptr;
    }

   private:
    void on_path_changed(godot::String path) {
        if (exists(file)) unref(file);
        on_artboard_changed(-1);
        on_animation_changed(-1);
        on_scene_changed(-1);
    }

    void on_artboard_changed(int index) {
        if (exists(file)) file->get_artboard(index);
    }

    void on_scene_changed(int index) {
        if (exists(artboard())) artboard()->get_scene(index);
    }

    void on_animation_changed(int index) {
        if (exists(artboard())) artboard()->get_animation(index);
    }

    void on_scene_properties_changed() {
        auto sm = scene();
        if (exists(sm)) {
            auto scene_props = props->scene_properties();
            for (int i = 0; i < sm->get_input_count(); i++) {
                auto input = sm->get_input(i);
                auto input_name = input->get_name();
                if (scene_props.has(input_name)) input->set_value(scene_props[input_name]);
            }
        }
    }

    void on_transform_changed() {
        current_transform = get_transform();
        if (exists(artboard())) artboard()->queue_redraw();
    }
};

#endif