#ifndef _RIVEEXTENSION_API_INPUT_HPP_
#define _RIVEEXTENSION_API_INPUT_HPP_

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

using namespace godot;

class RiveInput : public Resource {
    GDCLASS(RiveInput, Resource);

   private:
    rive::SMIInput *input;
    int index = -1;

    friend class RiveScene;

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveInput::exists);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveInput::get_name);
        ClassDB::bind_method(D_METHOD("get_type"), &RiveInput::get_type);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveInput::get_index);
        ClassDB::bind_method(D_METHOD("get_value"), &RiveInput::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "value"), &RiveInput::set_value);
        ClassDB::bind_method(D_METHOD("get_default"), &RiveInput::get_default);
        ClassDB::bind_method(D_METHOD("is_bool"), &RiveInput::is_bool);
        ClassDB::bind_method(D_METHOD("is_number"), &RiveInput::is_number);
    }

    rive::SMIBool *bool_input() const {
        return dynamic_cast<rive::SMIBool *>(input);
    }

    rive::SMINumber *float_input() const {
        return dynamic_cast<rive::SMINumber *>(input);
    }

   public:
    static Ref<RiveInput> MakeRef(rive::SMIInput *input_value, int index_value) {
        if (!input_value) return nullptr;
        Ref<RiveInput> obj = memnew(RiveInput);
        obj->input = input_value;
        obj->index = index_value;
        return obj;
    }

    RiveInput() {}

    bool exists() const {
        return input != nullptr;
    }

    int get_index() const {
        return index;
    }

    String get_name() const {
        return input ? input->name().c_str() : String();
    }

    Variant::Type get_type() const {
        if (input && input->input()->is<rive::StateMachineBool>()) return Variant::Type::BOOL;
        if (input && input->input()->is<rive::StateMachineNumber>()) return Variant::Type::FLOAT;
        return Variant::Type::NIL;
    }

    Variant get_value() const {
        if (auto i = bool_input()) return i->value();
        if (auto i = float_input()) return i->value();
        return nullptr;
    }

    Variant get_default() const {
        if (is_bool()) return false;
        if (is_number()) return 0.0;
        return nullptr;
    }

    void set_value(Variant value) {
        if (auto i = bool_input()) i->value((bool)value);
        else if (auto i = float_input()) i->value((float)value);
    }

    bool is_bool() const {
        return get_type() == Variant::Type::BOOL;
    }

    bool is_number() const {
        return get_type() == Variant::Type::FLOAT;
    }

    /* Overrides */

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["name"] = get_name();
        format_args["type"] = get_type() == Variant::Type::BOOL ? "bool" : "float";
        return String("{cls}({type} {name})").format(format_args);
    }

    /* Operators */

    bool operator==(const RiveInput &other) const {
        return other.input == input;
    }

    bool operator!=(const RiveInput &other) const {
        return other.input != input;
    }
};

#endif