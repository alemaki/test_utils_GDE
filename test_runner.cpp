#include "test_runner.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <doctest.h>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>

#include "test_utils.hpp"
#include "godot_error_interceptor.hpp"

bool TestRunner::g_print_test_timing = false;
bool TestRunner::g_print_suite_timing = false;

void TestRunner::run(const char* gd_filter)
{
    godot::String tests_filter;
    godot::String suite_filter;
    if (!(this->filter_pattern.is_empty()))
    {
        tests_filter += "--test-case=" + this->filter_pattern;
        suite_filter += "--test-suite=" + this->filter_pattern;
        godot::UtilityFunctions::print("Applying filter" + this->filter_pattern);
    }

    const char* argv[] = {
        "",
        "--test-suite-exclude=*[deprecated]*",
        "--test-case-exclude=*[deprecated]*",
        gd_filter,
        tests_filter.utf8().ptr(),
        suite_filter.utf8().ptr(),
        this->aborting_on_failure ? "--abort-after=1" : "",
        "--reporters=godot",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    doctest::Context context(argc, argv);
    std::stringstream output_stream;

    context.setCout(&output_stream);

    /* Defining it is enough */
    GodotErrorInterceptor error_interceptor;

    int res = context.run();
    godot::UtilityFunctions::print(output_stream.str().c_str());

    if (context.shouldExit())
    {
        /* Nothing to do. */
    }
}

void TestRunner::run_runtime()
{
    godot::UtilityFunctions::print("\n\nRuntime tests running.\n\n");
    this->run("--test-suite-exclude=*[editor]*");
}

void TestRunner::run_editor()
{
    godot::UtilityFunctions::print("\n\nEditor tests running.\n\n");
    this->run("--test-suite=*[editor]*");
}

void TestRunner::_ready()
{
    godot::Node* current_scene_root = ::get_scene_root();
    /* Ensure editor tests do not run in scene that is played */
    if (current_scene_root != nullptr)
    {
        /* The reason for this is to give the engine some time to pass a few frames, so the physics delta may be different than 0. */
        /* Approximate 1 frame at 60 FPS */
        this->test_timer = ::get_scene_tree()->create_timer(1.0 / 60.0);
        this->test_timer->connect("timeout", callable_mp(this, &TestRunner::run_runtime), CONNECT_ONE_SHOT);
    }
}

void TestRunner::set_test_duration_printing(bool test_duration_printing)
{
    TestRunner::g_print_test_timing = test_duration_printing;
}

void TestRunner::set_suite_duration_printing(bool suite_duration_printing)
{
    TestRunner::g_print_suite_timing = suite_duration_printing;
}

void TestRunner::set_aborting_on_failure(bool aborting_on_failure)
{
    this->aborting_on_failure = aborting_on_failure;
}

void TestRunner::set_filter_pattern(godot::String filter_pattern)
{
    this->filter_pattern = filter_pattern;
}

void TestRunner::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(godot::D_METHOD("run_runtime"), &TestRunner::run_runtime);
    ClassDB::bind_method(godot::D_METHOD("run_editor"), &TestRunner::run_editor);

    ClassDB::bind_method(godot::D_METHOD("set_test_duration_printing", "test_duration_printing"), &TestRunner::set_test_duration_printing);
    ClassDB::bind_method(godot::D_METHOD("is_test_duration_printing"), &TestRunner::is_test_duration_printing);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "test_duration_printing", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_test_duration_printing", "is_test_duration_printing");

    ClassDB::bind_method(godot::D_METHOD("set_suite_duration_printing", "suite_duration_printing"), &TestRunner::set_suite_duration_printing);
    ClassDB::bind_method(godot::D_METHOD("is_suite_duration_printing"), &TestRunner::is_suite_duration_printing);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "suite_duration_printing", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_suite_duration_printing", "is_suite_duration_printing");

    ClassDB::bind_method(godot::D_METHOD("set_aborting_on_failure", "aborting_on_failure"), &TestRunner::set_aborting_on_failure);
    ClassDB::bind_method(godot::D_METHOD("is_aborting_on_failure"), &TestRunner::is_aborting_on_failure);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "aborting_on_failure", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_aborting_on_failure", "is_aborting_on_failure");

    ClassDB::bind_method(godot::D_METHOD("set_filter_pattern", "filter_pattern"), &TestRunner::set_filter_pattern);
    ClassDB::bind_method(godot::D_METHOD("get_filter_pattern"), &TestRunner::get_filter_pattern);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "filter_pattern", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_filter_pattern", "get_filter_pattern");
}
