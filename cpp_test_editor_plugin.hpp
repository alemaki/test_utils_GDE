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

#include "test_runner.hpp"
using namespace godot;

class CppTestEditorPlugin : public EditorPlugin
{
    GDCLASS(CppTestEditorPlugin, EditorPlugin);

private:
    Control* dock = nullptr;
public:

    void _enter_tree() override
    {
        this->dock = memnew(VBoxContainer);
        this->dock->set_name("GDE C++ Tests");
        this->add_control_to_dock(DOCK_SLOT_RIGHT_UL, this->dock);

        Button *runtime_btn = memnew(Button);
        runtime_btn->set_text("Run Runtime Tests");
        runtime_btn->connect("pressed", callable_mp(this, &CppTestEditorPlugin::run_runtime_tests));
        this->dock->add_child(runtime_btn);

        
        Button *editor_btn = memnew(Button);
        editor_btn->set_text("Run Editor Tests");
        editor_btn->connect("pressed", callable_mp(this, &CppTestEditorPlugin::run_editor_tests));
        this->dock->add_child(editor_btn);
    }

    void _exit_tree() override
    {
        this->remove_control_from_docks(this->dock);
        this->dock->queue_free();
        this->dock = nullptr;
    }

    void run_runtime_tests() 
    {
        Ref<PackedScene> ps;
        TestRunner *runner = memnew(TestRunner);
        Node *scene_root = runner;

        ps.instantiate();
        ps->pack(scene_root);

        String temp_scene_path = "user://_temp_cpp_runtime_test_scene.tscn";
        ResourceSaver* rs = ResourceSaver::get_singleton();
        rs->save(ps, temp_scene_path);

        EditorInterface *ei = get_editor_interface();
        ei->play_custom_scene(temp_scene_path);

        memdelete(runner);
    }

    void run_editor_tests() 
    {
        Ref<PackedScene> ps;
        TestRunner *runner = memnew(TestRunner);
        runner->run_editor();
        memdelete(runner);
    }

protected:
    static void _bind_methods() {}

};