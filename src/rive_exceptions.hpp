#ifndef _RIVEEXTENSION_EXCEPTIONS_HPP_
#define _RIVEEXTENSION_EXCEPTIONS_HPP_

// stdlib
#include <exception>
#include <string>
#include <vector>

// godot-cpp
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace std;

class RiveException : public exception {
   public:
    enum Severity {
        WARNING,
        ERROR,
        NONE,
    };

   private:
    const godot::String prefix = godot::String("[Rive] ");
    godot::String message = "";
    godot::Dictionary format_args;
    Severity severity = Severity::ERROR;

   public:
    RiveException(godot::String message_value) {
        message = message_value;
    }

    godot::String get_caller() const _NOEXCEPT {
        if (format_args.has("calling_class") && format_args.has("calling_method"))
            return godot::String(" (in {calling_class}.{calling_method})").format(format_args);
        if (format_args.has("calling_class")) return godot::String(" (in {calling_class})").format(format_args);
        if (format_args.has("calling_method")) return godot::String(" (in {calling_method})").format(format_args);
        return "";
    }

    godot::String get_string() const _NOEXCEPT {
        return prefix + message + get_caller();
    }

    RiveException from(string calling_class_value, string calling_method_value) {
        format_args["calling_class"] = calling_class_value.c_str();
        format_args["calling_method"] = calling_method_value.c_str();
        return *this;
    }

    template <class GodotObject>
    RiveException from(GodotObject *calling_class_value, string calling_method_value) {
        format_args["calling_class"] = calling_class_value->get_class_static();
        format_args["calling_method"] = calling_method_value.c_str();
        return *this;
    }

    RiveException error() {
        severity = Severity::ERROR;
        return *this;
    }

    RiveException warning() {
        severity = Severity::WARNING;
        return *this;
    }

    RiveException no_report() {
        severity = Severity::NONE;
        return *this;
    }

    void report() {
        auto str = get_string();
        switch (severity) {
            case Severity::ERROR:
                godot::UtilityFunctions::push_error(str);
                godot::UtilityFunctions::printerr(str);
                break;
            case Severity::WARNING:
                godot::UtilityFunctions::push_warning(str);
                break;
            case Severity::NONE:
                break;
        }
    }
};

#endif