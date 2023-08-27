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
#include "api/rive_listener.hpp"

using namespace godot;

class RiveScene : public Resource {
    GDCLASS(RiveScene, Resource);

   private:
    rive::ArtboardInstance *artboard;
    rive::StateMachineInstance *scene;
    TypedArray<RiveInput> inputs;
    TypedArray<RiveListener> listeners;
    rive::Mat2D inverse_transform;
    int index = -1;

    friend class RiveArtboard;
    friend class RiveController;

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveScene::exists);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveScene::get_index);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveScene::get_name);
        ClassDB::bind_method(D_METHOD("get_input_count"), &RiveScene::get_input_count);
        ClassDB::bind_method(D_METHOD("get_listener_count"), &RiveScene::get_listener_count);
        ClassDB::bind_method(D_METHOD("get_inputs"), &RiveScene::get_inputs);
        ClassDB::bind_method(D_METHOD("get_listeners"), &RiveScene::get_listeners);
        ClassDB::bind_method(D_METHOD("get_bounds"), &RiveScene::get_bounds);
        ClassDB::bind_method(D_METHOD("get_duration"), &RiveScene::get_duration);
        ClassDB::bind_method(D_METHOD("is_opaque"), &RiveScene::is_opaque);
        ClassDB::bind_method(D_METHOD("get_input", "index"), &RiveScene::get_input);
        ClassDB::bind_method(D_METHOD("find_input", "name"), &RiveScene::find_input);
        ClassDB::bind_method(D_METHOD("get_listener", "index"), &RiveScene::get_listener);
        ClassDB::bind_method(D_METHOD("find_listener", "index"), &RiveScene::find_listener);
        ClassDB::bind_method(D_METHOD("is_loop"), &RiveScene::is_loop);
        ClassDB::bind_method(D_METHOD("is_pingpong"), &RiveScene::is_pingpong);
        ClassDB::bind_method(D_METHOD("is_one_shot"), &RiveScene::is_one_shot);
        ClassDB::bind_method(D_METHOD("move_mouse", "position"), &RiveScene::move_mouse);
        ClassDB::bind_method(D_METHOD("press", "position"), &RiveScene::press);
        ClassDB::bind_method(D_METHOD("release", "position"), &RiveScene::release);
    }

    void cache_inputs() {
        inputs.clear();
        if (scene) {
            int size = scene->inputCount();
            inputs.resize(size);
            for (int i = 0; i < size; i++) inputs[i] = RiveInput::MakeRef(scene->input(i), i);
        }
    }

    void cache_listeners() {
        listeners.clear();
        if (scene) {
            auto sm = scene->stateMachine();
            int size = sm->listenerCount();
            listeners.resize(size);
            for (int i = 0; i < size; i++) listeners[i] = RiveListener::MakeRef(sm->listener(i), i);
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
        obj->cache_listeners();
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

    int get_listener_count() const {
        return listeners.size();
    }

    TypedArray<RiveInput> get_inputs() const {
        return inputs;
    }

    TypedArray<RiveListener> get_listeners() const {
        return listeners;
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

    Ref<RiveInput> get_input(int index) const {
        if (index >= 0 && index < get_input_count()) return inputs[index];
        return nullptr;
    }

    Ref<RiveInput> find_input(String name) const {
        for (int i = 0; i < get_input_count(); i++) {
            Ref<RiveInput> input = inputs[i];
            if (input->get_name() == name) return input;
        }
        return nullptr;
    }

    Ref<RiveListener> get_listener(int index) const {
        if (index >= 0 && index < get_listener_count()) return listeners[index];
        return nullptr;
    }

    Ref<RiveListener> find_listener(String name) const {
        for (int i = 0; i < get_listener_count(); i++) {
            Ref<RiveListener> listener = listeners[i];
            if (listener->get_name() == name) return listener;
        }
        return nullptr;
    }

    bool is_loop() const {
        return scene ? scene->loop() == rive::Loop::loop : false;
    }

    bool is_pingpong() const {
        return scene ? scene->loop() == rive::Loop::pingPong : false;
    }

    bool is_one_shot() const {
        return scene ? scene->loop() == rive::Loop::oneShot : false;
    }

    void move_mouse(Vector2 position) {
        if (scene) scene->pointerMove(inverse_transform * rive::Vec2D(position.x, position.y));
    }

    void press(Vector2 position) {
        if (scene) scene->pointerDown(inverse_transform * rive::Vec2D(position.x, position.y));
    }

    void release(Vector2 position) {
        if (scene) scene->pointerUp(inverse_transform * rive::Vec2D(position.x, position.y));
    }

    /* Overrides */

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["name"] = get_name();
        return String("{cls}({name})").format(format_args);
    }

    /* Operators */

    bool operator==(const RiveScene &other) const {
        return other.scene == scene;
    }

    bool operator!=(const RiveScene &other) const {
        return other.scene != scene;
    }
};

#endif