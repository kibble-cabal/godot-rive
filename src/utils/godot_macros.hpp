#ifndef _RIVEEXTENSION_MACROS_
#define _RIVEEXTENSION_MACROS_

#include <godot_cpp/variant/utility_functions.hpp>

/* Printing */

#define GDPRINT godot::UtilityFunctions::print
#define GDPRINTS godot::UtilityFunctions::prints
#define GDERR godot::UtilityFunctions::printerr

/* Binding */

#define BIND_GET(cls, prop_name) ClassDB::bind_method(D_METHOD("get_" #prop_name), &cls::get_##prop_name);
#define BIND_SET(cls, prop_name) ClassDB::bind_method(D_METHOD("set_" #prop_name), &cls::set_##prop_name);
#define BIND_SETGET(cls, prop_name) \
    BIND_GET(cls, prop_name);       \
    BIND_SET(cls, prop_name)

#define ADD_PROP_WITH_HINT(cls, variant_type, prop_name, ...) \
    BIND_SETGET(cls, prop_name);                              \
    ClassDB::add_property(                                    \
        get_class_static(),                                   \
        PropertyInfo(variant_type, #prop_name, __VA_ARGS__),  \
        "set_" #prop_name,                                    \
        "get_" #prop_name                                     \
    )

#define ADD_PROP(cls, variant_type, prop_name)  \
    BIND_SETGET(cls, prop_name);                \
    ClassDB::add_property(                      \
        get_class_static(),                     \
        PropertyInfo(variant_type, #prop_name), \
        "set_" #prop_name,                      \
        "get_" #prop_name                       \
    )

#endif