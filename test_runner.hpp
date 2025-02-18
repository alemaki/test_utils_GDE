#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

class TestRunner : public godot::Node
{
    GDCLASS(TestRunner, godot::Node);

public:
    static bool g_error_called;
    static bool currently_testing_error;

private:
    bool tests_ran = false;
    godot::Ref<godot::SceneTreeTimer> test_timer;

    bool duration_printing = false;
    bool aborting_on_failure = false;
    godot::String filter_pattern = "";
protected:
    void run(const char* gd_filter);

public:
    void run_runtime();
    void run_editor();
    virtual void _ready() override;

    void set_duration_printing(bool duration_printing);
    _FORCE_INLINE_ bool is_duration_printing() const
    {
        return this->duration_printing;
    }
    void set_aborting_on_failure(bool aborting_on_failure);
    _FORCE_INLINE_ bool is_aborting_on_failure() const
    {
        return this->duration_printing;
    }
    void set_filter_pattern(godot::String filter_pattern);
    _FORCE_INLINE_ godot::String is_filter_pattern() const
    {
        return this->filter_pattern;
    }

protected:
    static void _bind_methods();

};

#endif // TEST_RUNNER_HPP