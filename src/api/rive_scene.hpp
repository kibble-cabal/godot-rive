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
#include "api/instances.hpp"
#include "api/rive_input.hpp"
#include "api/rive_listener.hpp"
#include "rive_exceptions.hpp"

using namespace godot;

class RiveScene : public Resource {
    GDCLASS(RiveScene, Resource);

    friend class RiveArtboard;
    friend class RiveController;
    friend class RiveInstance;
    friend class RiveViewer;

   private:
    rive::ArtboardInstance *artboard;
    Ptr<rive::StateMachineInstance> scene;
    int index = -1;
    String name = "";

    Instances<RiveInput> inputs = Instances<RiveInput>([this](int index) -> Ref<RiveInput> {
        if (!exists() || index < 0 || index >= scene->inputCount()) return nullptr;
        return RiveInput::MakeRef(scene->input(index), index);
    });

    Instances<RiveListener> listeners = Instances<RiveListener>([this](int index) -> Ref<RiveListener> {
        if (!exists() || !scene->stateMachine() || index < 0 || index >= scene->stateMachine()->inputCount())
            return nullptr;
        return RiveListener::MakeRef(scene->stateMachine()->listener(index), index);
    });

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveScene::exists);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveScene::get_index);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveScene::get_name);
        ClassDB::bind_method(D_METHOD("get_input_count"), &RiveScene::get_input_count);
        ClassDB::bind_method(D_METHOD("get_listener_count"), &RiveScene::get_listener_count);
        ClassDB::bind_method(D_METHOD("get_inputs"), &RiveScene::get_inputs);
        ClassDB::bind_method(D_METHOD("get_listeners"), &RiveScene::get_listeners);
        ClassDB::bind_method(D_METHOD("get_input_names"), &RiveScene::get_input_names);
        ClassDB::bind_method(D_METHOD("get_bounds"), &RiveScene::get_bounds);
        ClassDB::bind_method(D_METHOD("get_duration"), &RiveScene::get_duration);
        ClassDB::bind_method(D_METHOD("is_opaque"), &RiveScene::is_opaque);
        ClassDB::bind_method(D_METHOD("get_input", "index"), &RiveScene::get_input);
        ClassDB::bind_method(D_METHOD("find_input", "name"), &RiveScene::find_input);
        ClassDB::bind_method(D_METHOD("reset_input", "index"), &RiveScene::reset_input);
        ClassDB::bind_method(D_METHOD("get_listener", "index"), &RiveScene::get_listener);
        ClassDB::bind_method(D_METHOD("find_listener", "index"), &RiveScene::find_listener);
        ClassDB::bind_method(D_METHOD("is_loop"), &RiveScene::is_loop);
        ClassDB::bind_method(D_METHOD("is_pingpong"), &RiveScene::is_pingpong);
        ClassDB::bind_method(D_METHOD("is_one_shot"), &RiveScene::is_one_shot);
    }

    void _instantiate_inputs() {
        if (exists())
            for (int i = 0; i < scene->inputCount(); i++) {
                auto input = get_input(i);
                if (input.is_null() || !input->exists()) throw RiveException("Failed to instantiate input.");
            }
    }

    void _get_input_property_list(List<PropertyInfo> *list) const {
        inputs.for_each([list](Ref<RiveInput> input, int _) {
            list->push_back(PropertyInfo(input->get_type(), input->get_name()));
        });
    }

   public:
    static Ref<RiveScene> MakeRef(
        rive::ArtboardInstance *artboard_value,
        Ptr<rive::StateMachineInstance> scene_value,
        int index_value,
        String name_value
    ) {
        if (!artboard_value || !scene_value) return nullptr;
        Ref<RiveScene> obj = memnew(RiveScene);
        obj->artboard = artboard_value;
        obj->scene = std::move(scene_value);
        obj->index = index_value;
        obj->name = name_value;
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
        return name;
    }

    int get_input_count() const {
        return inputs.get_size();
    }

    int get_listener_count() const {
        return listeners.get_size();
    }

    TypedArray<RiveInput> get_inputs() const {
        return inputs.get_list();
    }

    TypedArray<RiveListener> get_listeners() const {
        return listeners.get_list();
    }

    PackedStringArray get_input_names() const {
        PackedStringArray names;
        inputs.for_each([&names](Ref<RiveInput> input, int _) { names.append(input->get_name()); });
        return names;
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

    Ref<RiveInput> get_input(int index) {
        return inputs.get(index);
    }

    Ref<RiveInput> find_input(String name) const {
        return inputs.find([name](Ref<RiveInput> input, int index) { return input->get_name() == name; });
    }

    Ref<RiveInput> reset_input(int index) {
        return inputs.reinstantiate(index);
    }

    Ref<RiveListener> get_listener(int index) {
        return listeners.get(index);
    }

    Ref<RiveListener> find_listener(String name) const {
        return listeners.find([name](Ref<RiveListener> listener, int index) { return listener->get_name() == name; });
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

    void move_mouse(rive::Mat2D inverse_transform, Vector2 position) {
        if (scene) scene->pointerMove(inverse_transform * rive::Vec2D(position.x, position.y));
    }

    void press_mouse(rive::Mat2D inverse_transform, Vector2 position) {
        if (scene) scene->pointerDown(inverse_transform * rive::Vec2D(position.x, position.y));
    }

    void release_mouse(rive::Mat2D inverse_transform, Vector2 position) {
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