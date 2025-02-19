#include "test_runner.hpp"
#include <sstream>
#include <stdexcept>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>

#include "test_utils.hpp"

GDExtensionInterfacePrintError original_gdextension_interface_print_error = nullptr;
GDExtensionInterfacePrintErrorWithMessage original_gdextension_interface_print_error_with_message = nullptr;

bool TestRunner::g_error_called = false;
bool TestRunner::currently_testing_error = false;

void custom_gdextension_interface_print_error(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    TestRunner::g_error_called = true;
    if (!(TestRunner::currently_testing_error))
    {
        original_gdextension_interface_print_error(p_description, p_function, p_file, p_line, p_editor_notify);
    }
}

void custom_gdextension_interface_print_error_with_message(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    TestRunner::g_error_called = true;
    if (!(TestRunner::currently_testing_error))
    {
        original_gdextension_interface_print_error_with_message(p_description, p_message, p_function, p_file, p_line, p_editor_notify);
    }
}

void TestRunner::run(const char* gd_filter)
{
    godot::String filter;
    if (!(this->filter_pattern.is_empty()))
    {
        filter += "--test-case=" + this->filter_pattern;
        godot::UtilityFunctions::print("Applying filter" + this->filter_pattern);
    }

    const char* argv[] = {
        "", 
        "--test-suite-exclude=*[deprecated]*", 
        "--test-case-exclude=*[deprecated]*", 
        gd_filter,
        filter.utf8().ptr(),
        this->duration_printing ? "--duration" : "",
        this->aborting_on_failure ? "--abort-after=1" : "",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    doctest::Context context(argc, argv);

    std::stringstream output_stream;

    context.setCout(&output_stream);

    /* save for later */
    original_gdextension_interface_print_error = godot::internal::gdextension_interface_print_error;
    original_gdextension_interface_print_error_with_message = godot::internal::gdextension_interface_print_error_with_message;
    godot::internal::gdextension_interface_print_error = custom_gdextension_interface_print_error;
    godot::internal::gdextension_interface_print_error_with_message = custom_gdextension_interface_print_error_with_message;


    int res = context.run();
    godot::UtilityFunctions::print(output_stream.str().c_str());

    /* Restore the original functions */
    godot::internal::gdextension_interface_print_error = original_gdextension_interface_print_error;
    godot::internal::gdextension_interface_print_error_with_message = original_gdextension_interface_print_error_with_message;

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

void TestRunner::set_duration_printing(bool duration_printing)
{
    this->duration_printing = duration_printing;
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
    
    ClassDB::bind_method(godot::D_METHOD("set_duration_printing", "duration_printing"), &TestRunner::set_duration_printing);
    ClassDB::bind_method(godot::D_METHOD("is_duration_printing"), &TestRunner::is_duration_printing);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "duration_printing", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_duration_printing", "is_duration_printing");

    ClassDB::bind_method(godot::D_METHOD("set_aborting_on_failure", "aborting_on_failure"), &TestRunner::set_aborting_on_failure);
    ClassDB::bind_method(godot::D_METHOD("is_aborting_on_failure"), &TestRunner::is_aborting_on_failure);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "aborting_on_failure", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_aborting_on_failure", "is_aborting_on_failure");

    ClassDB::bind_method(godot::D_METHOD("set_filter_pattern", "filter_pattern"), &TestRunner::set_filter_pattern);
    ClassDB::bind_method(godot::D_METHOD("get_filter_pattern"), &TestRunner::get_filter_pattern);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "filter_pattern", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_filter_pattern", "get_filter_pattern");
}