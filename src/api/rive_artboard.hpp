#ifndef _RIVEEXTENSION_API_ARTBOARD_HPP_
#define _RIVEEXTENSION_API_ARTBOARD_HPP_

// godot-cpp
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/transform2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/vector2.hpp>

// rive-cpp
#include <rive/animation/state_machine_instance.hpp>
#include <rive/artboard.hpp>
#include <rive/math/transform_components.hpp>

// extension
#include "api/rive_animation.hpp"
#include "api/rive_scene.hpp"

using namespace godot;

class RiveArtboard : public Resource {
    GDCLASS(RiveArtboard, Resource);

   private:
    rive::File *file;
    rive::ArtboardInstance *artboard;
    TypedArray<RiveScene> scenes;
    TypedArray<RiveAnimation> animations;
    rive::Mat2D inverse_transform;
    int index = -1;

    friend class RiveFile;
    friend class RiveController;

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveArtboard::exists);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveArtboard::get_index);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveArtboard::get_name);
        ClassDB::bind_method(D_METHOD("get_scenes"), &RiveArtboard::get_scenes);
        ClassDB::bind_method(D_METHOD("get_scene_count"), &RiveArtboard::get_scene_count);
        ClassDB::bind_method(D_METHOD("get_animations"), &RiveArtboard::get_animations);
        ClassDB::bind_method(D_METHOD("get_animation_count"), &RiveArtboard::get_animation_count);
        ClassDB::bind_method(D_METHOD("get_bounds"), &RiveArtboard::get_bounds);
        ClassDB::bind_method(D_METHOD("get_scene", "index"), &RiveArtboard::get_scene);
        ClassDB::bind_method(D_METHOD("find_scene", "name"), &RiveArtboard::find_scene);
        ClassDB::bind_method(D_METHOD("get_animation", "index"), &RiveArtboard::get_animation);
        ClassDB::bind_method(D_METHOD("get_world_transform"), &RiveArtboard::get_world_transform);
    }

    void set_inverse_transform(rive::Mat2D value) {
        inverse_transform = value;
        for (int i = 0; i < get_scene_count(); i++) {
            Ref<RiveScene> scene = scenes[i];
            scene->inverse_transform = value;
        }
    }

    void cache_scenes() {
        scenes.clear();
        if (artboard) {
            int size = artboard->stateMachineCount();
            scenes.resize(size);
            for (int i = 0; i < size; i++)
                scenes[i] = RiveScene::MakeRef(artboard, artboard->stateMachineAt(i).get(), i);
        }
        set_inverse_transform(inverse_transform);
    }

    void cache_animations() {
        animations.clear();
        if (artboard) {
            int size = artboard->animationCount();
            animations.resize(size);
            for (int i = 0; i < size; i++)
                animations[i] = RiveAnimation::MakeRef(artboard, artboard->animationAt(i).get(), i);
        }
    }

   public:
    static Ref<RiveArtboard> MakeRef(
        rive::File *file_value, rive::ArtboardInstance *artboard_value, int index_value = -1
    ) {
        if (!file_value || !artboard_value) return nullptr;
        Ref<RiveArtboard> obj = memnew(RiveArtboard);
        obj->file = file_value;
        obj->artboard = artboard_value;
        obj->index = index_value;
        obj->cache_scenes();
        obj->cache_animations();
        return obj;
    }

    RiveArtboard() {}

    bool exists() const {
        return artboard != nullptr;
    }

    int get_index() const {
        return index;
    }

    String get_name() const {
        return artboard ? artboard->name().c_str() : String();
    }

    int get_scene_count() const {
        return scenes.size();
    }

    int get_animation_count() const {
        return animations.size();
    }

    TypedArray<RiveScene> get_scenes() const {
        return scenes;
    }

    TypedArray<RiveAnimation> get_animations() const {
        return animations;
    }

    Rect2 get_bounds() const {
        auto aabb = artboard ? artboard->bounds() : rive::AABB();
        return Rect2(aabb.left(), aabb.top(), aabb.width(), aabb.height());
    }

    Transform2D get_world_transform() const {
        if (artboard) {
            auto trans = artboard->worldTransform().decompose();
            return Transform2D(
                trans.rotation(),
                Vector2(trans.scaleX(), trans.scaleY()),
                trans.skew(),
                Vector2(trans.x(), trans.y())
            );
        }
        return Transform2D();
    }

    Ref<RiveScene> get_scene(int index) const {
        if (index >= 0 && index < get_scene_count()) return scenes[index];
        return nullptr;
    }

    Ref<RiveScene> find_scene(String name) const {
        for (int i = 0; i < get_scene_count(); i++) {
            Ref<RiveScene> scene = scenes[i];
            if (scene->get_name() == name) return scene;
        }
        return nullptr;
    }

    Ref<RiveAnimation> get_animation(int index) const {
        if (index >= 0 && index < get_animation_count()) return animations[index];
        return nullptr;
    }

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["name"] = get_name();
        return String("{cls}({name})").format(format_args);
    }

    bool operator==(const RiveArtboard &other) const {
        return other.artboard == artboard;
    }

    bool operator!=(const RiveArtboard &other) const {
        return other.artboard != artboard;
    }
};

#endif