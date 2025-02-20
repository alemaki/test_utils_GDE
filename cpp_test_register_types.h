#ifndef CPP_TEST_REGISTER_TYPES_H
#define CPP_TEST_REGISTER_TYPES_H

#include "godot_cpp/core/class_db.hpp"

#include "test_runner.hpp"
#include "cpp_test_editor_plugin.hpp"

using namespace godot;

void initialize_cpp_test_module(ModuleInitializationLevel p_level);
void uninitialize_cpp_test_module(ModuleInitializationLevel p_level);

#endif /* CPP_TEST_REGISTER_TYPES_H */
