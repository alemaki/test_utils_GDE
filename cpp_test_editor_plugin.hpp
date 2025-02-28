#ifndef CPP_TEST_EDITOR_PLUGIN
#define CPP_TEST_EDITOR_PLUGIN
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/check_box.hpp>
#include <godot_cpp/classes/line_edit.hpp>

#include "test_runner.hpp"

class CppTestEditorPlugin : public godot::EditorPlugin
{
    GDCLASS(CppTestEditorPlugin, godot::EditorPlugin);

private:
    godot::Control* dock = nullptr;
    godot::CheckBox* duration_printing_checkbox = nullptr;
    godot::CheckBox* aborting_on_failure_checkbox = nullptr;
    godot::LineEdit* tests_filter_line_edit = nullptr;
public:

    void _enter_tree() override;
    void _exit_tree() override;

private:
    TestRunner* create_runner();
    void run_runtime_tests();
    void run_editor_tests();

protected:
    static void _bind_methods() {}

};

#endif /* CPP_TEST_EDITOR_PLUGIN */
