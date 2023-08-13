#ifndef RIVEEXTENSION_REGISTER_TYPES_H
#define RIVEEXTENSION_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_rive_module(ModuleInitializationLevel p_level);
void uninitialize_rive_module(ModuleInitializationLevel p_level);

#endif // RIVEEXTENSION_REGISTER_TYPES_H