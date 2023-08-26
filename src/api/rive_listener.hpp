#ifndef _RIVEEXTENSION_API_LISTENER_HPP_
#define _RIVEEXTENSION_API_LISTENER_HPP_

// godot-cpp
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/vector2.hpp>

// rive-cpp
#include <rive/animation/state_machine_instance.hpp>
#include <rive/animation/state_machine_listener.hpp>

using namespace godot;

class RiveListener : public Resource {
    GDCLASS(RiveListener, Resource);

   private:
    const rive::StateMachineListener *listener;
    int index = -1;

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveListener::exists);
        ClassDB::bind_method(D_METHOD("get_name"), &RiveListener::get_name);
        ClassDB::bind_method(D_METHOD("get_index"), &RiveListener::get_index);
        ClassDB::bind_integer_constant(
            get_class_static(),
            "RiveListenerType",
            "MOUSE_ENTER",
            (int)rive::ListenerType::enter
        );
        ClassDB::bind_integer_constant(
            get_class_static(),
            "RiveListenerType",
            "MOUSE_EXIT",
            (int)rive::ListenerType::exit
        );
        ClassDB::bind_integer_constant(
            get_class_static(),
            "RiveListenerType",
            "PRESSED",
            (int)rive::ListenerType::down
        );
        ClassDB::bind_integer_constant(get_class_static(), "RiveListenerType", "RELEASED", (int)rive::ListenerType::up);
        ClassDB::bind_integer_constant(
            get_class_static(),
            "RiveListenerType",
            "MOUSE_MOTION",
            (int)rive::ListenerType::move
        );
        ClassDB::bind_method(D_METHOD("get_type"), &RiveListener::get_type);
    }

   public:
    static Ref<RiveListener> MakeRef(const rive::StateMachineListener *listener_value, int index_value) {
        if (!listener_value) return nullptr;
        Ref<RiveListener> obj = memnew(RiveListener);
        obj->listener = listener_value;
        obj->index = index_value;
        return obj;
    }

    RiveListener() {}

    bool exists() const {
        return listener != nullptr;
    }

    int get_index() const {
        return index;
    }

    String get_name() const {
        return listener ? listener->name().c_str() : String();
    }

    int get_type() const {
        return listener ? (int)listener->listenerType() : (int)rive::ListenerType::move;
    }

    String get_type_string() const {
        switch ((rive::ListenerType)get_type()) {
            case rive::ListenerType::down:
                return "RiveListenerType::PRESSED";
            case rive::ListenerType::enter:
                return "RiveListenerType::MOUSE_ENTER";
            case rive::ListenerType::exit:
                return "RiveListenerType::MOUSE_EXIT";
            case rive::ListenerType::up:
                return "RiveListenerType::RELEASED";
            case rive::ListenerType::move:
            default:
                return "RiveListenerType::MOUSE_MOTION";
        }
    }

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["name"] = get_name();
        format_args["type"] = get_type_string();
        return String("{cls}({type} {name})").format(format_args);
    }

    bool operator==(const RiveListener &other) const {
        return other.listener == listener;
    }

    bool operator!=(const RiveListener &other) const {
        return other.listener != listener;
    }
};

#endif