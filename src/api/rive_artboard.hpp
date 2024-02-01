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

    friend class RiveFile;
    friend class RiveInstance;
    friend class RiveViewerBase;

   private:
    rive::File *file;
    Ptr<rive::ArtboardInstance> artboard;
    String name = "";
    int index = -1;

    Instances<RiveScene> scenes = Instances<RiveScene>([this](int index) -> Ref<RiveScene> {
        if (!exists() || index < 0 || index >= artboard->stateMachineCount()) return nullptr;
        return RiveScene::MakeRef(
            artboard.get(),
            artboard->stateMachineAt(index),
            index,
            artboard->stateMachineNameAt(index).c_str()
        );
    });

    Instances<RiveAnimation> animations = Instances<RiveAnimation>([this](int index) -> Ref<RiveAnimation> {
        if (!exists() || index < 0 || index >= artboard->animationCount()) return nullptr;
        return RiveAnimation::MakeRef(
            artboard.get(),
            artboard->animationAt(index),
            index,
            artboard->animationNameAt(index).c_str()
        );
    });

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveArtboard::exists);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveArtboard::get_index);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveArtboard::get_name);
        ClassDB::bind_method(D_METHOD("get_scenes"), &RiveArtboard::get_scenes);
        ClassDB::bind_method(D_METHOD("get_scene_count"), &RiveArtboard::get_scene_count);
        ClassDB::bind_method(D_METHOD("get_scene_names"), &RiveArtboard::get_scene_names);
        ClassDB::bind_method(D_METHOD("get_animations"), &RiveArtboard::get_animations);
        ClassDB::bind_method(D_METHOD("get_animation_count"), &RiveArtboard::get_animation_count);
        ClassDB::bind_method(D_METHOD("get_animation_names"), &RiveArtboard::get_animation_names);
        ClassDB::bind_method(D_METHOD("get_bounds"), &RiveArtboard::get_bounds);
        ClassDB::bind_method(D_METHOD("get_scene", "index"), &RiveArtboard::get_scene);
        ClassDB::bind_method(D_METHOD("find_scene", "name"), &RiveArtboard::find_scene);
        ClassDB::bind_method(D_METHOD("get_animation", "index"), &RiveArtboard::get_animation);
        ClassDB::bind_method(D_METHOD("find_animation", "name"), &RiveArtboard::find_animation);
        ClassDB::bind_method(D_METHOD("reset_animation", "index"), &RiveArtboard::reset_animation);
        ClassDB::bind_method(D_METHOD("get_world_transform"), &RiveArtboard::get_world_transform);
        ClassDB::bind_method(D_METHOD("queue_redraw"), &RiveArtboard::queue_redraw);
    }

    void _instantiate_scenes() {
        if (exists())
            for (int i = 0; i < artboard->stateMachineCount(); i++) {
                auto scene = get_scene(i);
                if (scene.is_null() || !scene->exists()) throw RiveException("Failed to instantiate scene.");
            }
    }

    void _instantiate_animations() {
        if (exists())
            for (int i = 0; i < artboard->animationCount(); i++) {
                auto animation = get_animation(i);
                if (animation.is_null() || !animation->exists())
                    throw RiveException("Failed to instantiate animation.");
            }
    }

    String _get_scene_property_hint() const {
        PackedStringArray hints;
        hints.append("None:-1");
        scenes.for_each([&hints](Ref<RiveScene> scene, int index) {
            hints.append(scene->get_name() + ":" + std::to_string(index).c_str());
        });
        return String(",").join(hints);
    }

    String _get_animation_property_hint() const {
        PackedStringArray hints;
        hints.append("None:-1");
        animations.for_each([&hints](Ref<RiveAnimation> animation, int index) {
            hints.append(animation->get_name() + ":" + std::to_string(index).c_str());
        });
        return String(",").join(hints);
    }

   public:
    static Ref<RiveArtboard> MakeRef(
        rive::File *file_value, Ptr<rive::ArtboardInstance> artboard_value, int index_value, String name_value
    ) {
        if (!file_value || !artboard_value) return nullptr;
        Ref<RiveArtboard> obj = memnew(RiveArtboard);
        obj->file = file_value;
        obj->artboard = std::move(artboard_value);
        obj->index = index_value;
        obj->name = name_value;
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
        return name;
    }

    int get_scene_count() const {
        return scenes.get_size();
    }

    int get_animation_count() const {
        return animations.get_size();
    }

    TypedArray<RiveScene> get_scenes() const {
        return scenes.get_list();
    }

    TypedArray<RiveAnimation> get_animations() const {
        return animations.get_list();
    }

    PackedStringArray get_scene_names() const {
        PackedStringArray names;
        scenes.for_each([&names](Ref<RiveScene> scene, int _) { names.append(scene->get_name()); });
        return names;
    }

    PackedStringArray get_animation_names() const {
        PackedStringArray names;
        animations.for_each([&names](Ref<RiveAnimation> animation, int _) { names.append(animation->get_name()); });
        return names;
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

    Ref<RiveScene> get_scene(int index) {
        return scenes.get(index);
    }

    Ref<RiveScene> find_scene(String name) const {
        return scenes.find([name](Ref<RiveScene> scene, int i) { return scene->get_name() == name; });
    }

    Ref<RiveScene> reset_scene(int index) {
        return scenes.reinstantiate(index);
    }

    Ref<RiveAnimation> get_animation(int index) {
        return animations.get(index);
    }

    Ref<RiveAnimation> find_animation(String name) const {
        return animations.find([name](Ref<RiveAnimation> animation, int i) { return animation->get_name() == name; });
    }

    Ref<RiveAnimation> reset_animation(int index) {
        return animations.reinstantiate(index);
    }

    void queue_redraw() {
        if (artboard && !artboard->hasDirt(rive::ComponentDirt::Components))
            artboard->addDirt(rive::ComponentDirt::Components, false);
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