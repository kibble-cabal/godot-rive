#ifndef _RIVEEXTENSION_API_SCENE_HPP_
#define _RIVEEXTENSION_API_SCENE_HPP_

// godot-cpp
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/vector2.hpp>

// rive-cpp
#include <rive/animation/state_machine_bool.hpp>
#include <rive/animation/state_machine_input_instance.hpp>
#include <rive/animation/state_machine_instance.hpp>
#include <rive/animation/state_machine_number.hpp>
#include <rive/scene.hpp>

// extension
#include "api/rive_input.hpp"

using namespace godot;

class RiveScene : public Resource {
    GDCLASS(RiveScene, Resource);

   private:
    rive::ArtboardInstance *artboard;
    rive::StateMachineInstance *scene;
    TypedArray<RiveInput> inputs;
    int index = -1;

    friend class RiveArtboard;

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveScene::exists);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveScene::get_index);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveScene::get_name);
        ClassDB::bind_method(D_METHOD("get_input_count"), &RiveScene::get_input_count);
        ClassDB::bind_method(D_METHOD("get_bounds"), &RiveScene::get_bounds);
        ClassDB::bind_method(D_METHOD("get_duration"), &RiveScene::get_duration);
        ClassDB::bind_method(D_METHOD("is_opaque"), &RiveScene::is_opaque);
        ClassDB::bind_method(D_METHOD("get_input_by_name", "name"), &RiveScene::get_input_by_name);
        ClassDB::bind_method(D_METHOD("get_input_by_index", "index"), &RiveScene::get_input_by_index);
    }

    void cache_inputs() {
        inputs.clear();
        if (scene) {
            int size = scene->inputCount();
            inputs.resize(size);
            for (int i = 0; i < size; i++) inputs[i] = RiveInput::MakeRef(scene->input(i), i);
        }
    }

   public:
    static Ref<RiveScene> MakeRef(
        rive::ArtboardInstance *artboard_value, rive::StateMachineInstance *scene_value, int index_value = -1
    ) {
        if (!artboard_value || !scene_value) return nullptr;
        Ref<RiveScene> obj = memnew(RiveScene);
        obj->artboard = artboard_value;
        obj->scene = scene_value;
        obj->index = index_value;
        obj->cache_inputs();
        return obj;
    }

    RiveScene() {}

    bool exists() const {
        return scene != nullptr;
    }

    int get_index() const {
        return index;
    }

    String get_name() const {
        return scene ? scene->name().c_str() : "-";
    }

    int get_input_count() const {
        return inputs.size();
    }

    Rect2 get_bounds() const {
        auto aabb = scene ? scene->bounds() : rive::AABB();
        return Rect2(aabb.left(), aabb.top(), aabb.width(), aabb.height());
    }

    float get_duration() const {
        return scene ? scene->durationSeconds() : -1;
    }

    bool is_opaque() const {
        return scene ? !scene->isTranslucent() : true;
    }

    Ref<RiveInput> get_input_by_name(String name) const {
        for (int i = 0; i < get_input_count(); i++) {
            Ref<RiveInput> input = inputs[i];
            if (input->get_name() == name) return input;
        }
        return nullptr;
    }

    Ref<RiveInput> get_input_by_index(int index) const {
        if (index >= 0 && index < get_input_count()) return inputs[index];
        return nullptr;
    }

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["name"] = get_name();
        return String("{cls}({name})").format(format_args);
    }

    bool operator==(const RiveScene &other) const {
        return other.scene == scene;
    }

    bool operator!=(const RiveScene &other) const {
        return other.scene != scene;
    }
};

#endif