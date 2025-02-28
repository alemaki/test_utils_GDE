#include "cpp_test_editor_plugin.hpp"

void CppTestEditorPlugin::_enter_tree()
{
    this->dock = memnew(godot::VBoxContainer);
    this->dock->set_name("GDE C++ Tests");
    this->add_control_to_dock(DOCK_SLOT_RIGHT_UL, this->dock);

    godot::Button *runtime_btn = memnew(godot::Button);
    runtime_btn->set_text("Run Runtime Tests");
    runtime_btn->connect("pressed", callable_mp(this, &CppTestEditorPlugin::run_runtime_tests));
    this->dock->add_child(runtime_btn);

    godot::Button *editor_btn = memnew(godot::Button);
    editor_btn->set_text("Run Editor Tests");
    editor_btn->connect("pressed", callable_mp(this, &CppTestEditorPlugin::run_editor_tests));
    this->dock->add_child(editor_btn);

    this->duration_printing_checkbox = memnew(godot::CheckBox);
    this->aborting_on_failure_checkbox = memnew(godot::CheckBox);

    this->duration_printing_checkbox->set_text("Print test durations.");
    this->aborting_on_failure_checkbox->set_text("Abort on first failure.");

    this->dock->add_child(this->duration_printing_checkbox);
    this->dock->add_child(this->aborting_on_failure_checkbox);

    this->tests_filter_line_edit = memnew(godot::LineEdit);
    this->tests_filter_line_edit->set_tooltip_text("Tests filter pattern");
    this->dock->add_child(this->tests_filter_line_edit);
}

void CppTestEditorPlugin::_exit_tree()
{
    this->remove_control_from_docks(this->dock);
    this->dock->queue_free();
    this->dock = nullptr;
}

TestRunner *CppTestEditorPlugin::create_runner()
{
    TestRunner *runner = memnew(TestRunner);
    if (this->duration_printing_checkbox)
    {
        runner->set_duration_printing(this->duration_printing_checkbox->is_pressed());
    }
    if (this->aborting_on_failure_checkbox)
    {
        runner->set_aborting_on_failure(this->aborting_on_failure_checkbox->is_pressed());
    }
    if (this->tests_filter_line_edit)
    {
        runner->set_filter_pattern(this->tests_filter_line_edit->get_text().strip_edges());
    }
    return runner;
}

void CppTestEditorPlugin::run_runtime_tests()
{
    godot::Ref<godot::PackedScene> ps;
    TestRunner *runner = this->create_runner();
    Node *scene_root = runner;

    ps.instantiate();
    ps->pack(scene_root);

    godot::String temp_scene_path = "user://_temp_cpp_runtime_test_scene.tscn";
    godot::ResourceSaver* rs = godot::ResourceSaver::get_singleton();
    rs->save(ps, temp_scene_path);

    godot::EditorInterface *ei = get_editor_interface();
    ei->play_custom_scene(temp_scene_path);

    memdelete(runner);
}

void CppTestEditorPlugin::run_editor_tests()
{
    godot::Ref<godot::PackedScene> ps;
    TestRunner *runner = this->create_runner();
    runner->run_editor();
    memdelete(runner);
}
