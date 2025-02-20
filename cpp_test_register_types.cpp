#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "cpp_test_register_types.h"

using namespace godot;

void initialize_cpp_test_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<TestRunner>();
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		ClassDB::register_internal_class<CppTestEditorPlugin>();

		EditorPlugins::add_by_type<CppTestEditorPlugin>();
	}
}

void uninitialize_cpp_test_module(ModuleInitializationLevel p_level)
{
	/* nothing to do */
}
