#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <doctest.h>

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

class TestRunner : public godot::Node
{
    GDCLASS(TestRunner, godot::Node);

public:
    static bool g_error_called;
    static bool g_currently_testing_error;
    static bool g_print_test_timing;
    static bool g_print_suite_timing;

private:
    bool tests_ran = false;
    godot::Ref<godot::SceneTreeTimer> test_timer;

    /* TODO: this is currently ugly ah with those globals. THink of something better. */
    bool test_duration_printing = false;
    bool suite_duration_printing = false;
    bool aborting_on_failure = false;
    godot::String filter_pattern = "";
protected:
    void run(const char* gd_filter);

public:
    void run_runtime();
    void run_editor();
    virtual void _ready() override;

    void set_test_duration_printing(bool test_duration_printing);
    _FORCE_INLINE_ bool is_test_duration_printing() const
    {
        return this->test_duration_printing;
    }
    void set_suite_duration_printing(bool test_duration_printing);
    _FORCE_INLINE_ bool is_suite_duration_printing() const
    {
        return this->suite_duration_printing;
    }
    void set_aborting_on_failure(bool aborting_on_failure);
    _FORCE_INLINE_ bool is_aborting_on_failure() const
    {
        return this->aborting_on_failure;
    }
    void set_filter_pattern(godot::String filter_pattern);
    _FORCE_INLINE_ godot::String get_filter_pattern() const
    {
        return this->filter_pattern;
    }

protected:
    static void _bind_methods();

};

#endif // TEST_RUNNER_HPP