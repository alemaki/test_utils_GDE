#include "test_runner.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>

#include "test_utils.hpp"

static const char* godot_reporter_name = "GDE C++ Tests";
static thread_local const char* g_current_test_name = nullptr;

/* Custom reporter to track test start/end */
struct GodotReporter : public doctest::IReporter
{
    using IReporter::IReporter;

    //DOCTEST_DECLARE_INTERFACE(GodotReporter);
    GodotReporter(const doctest::ContextOptions& in) : IReporter() {}

    void test_run_start() override {}

    void test_run_end(const doctest::TestRunStats& in) override
    {
        godot::UtilityFunctions::print("---- Test Results ----");
        godot::UtilityFunctions::print(
            godot::vformat("[%s] test cases: %d | %d passed | %d failed | %d skipped", 
                godot_reporter_name, 
                in.numTestCases, 
                in.numTestCasesPassingFilters - in.numTestCasesFailed, 
                in.numTestCasesFailed, 
                in.numTestCases - in.numTestCasesPassingFilters
            )
        );
        
        godot::UtilityFunctions::print(
            godot::vformat("[%s] assertions: %d | %d passed | %d failed | ", 
                godot_reporter_name, 
                in.numAsserts, 
                in.numAsserts - in.numAssertsFailed, 
                in.numAssertsFailed
            )
        );
        
        godot::UtilityFunctions::print(
            godot::vformat("[%s] Status: %s!",
                godot_reporter_name,
                in.numTestCasesFailed == 0 ? "SUCCESS" : "FAILURE"
            )
        );
    }

    void test_case_start(const doctest::TestCaseData& in) override
    {
        g_current_test_name = in.m_name;
    }

    // (due to subcases)
    void test_case_reenter(const doctest::TestCaseData&) override {}

    void test_case_end(const doctest::CurrentTestCaseStats&) override
    {
        g_current_test_name = nullptr;
    }

    void test_case_exception(const doctest::TestCaseException&) override {}

    void subcase_start(const doctest::SubcaseSignature&) override {}
    void subcase_end() override {}

    // Called for asserts and messages
    void log_assert(const doctest::AssertData&) override {}
    void log_message(const doctest::MessageData&) override {}

    void test_case_skipped(const doctest::TestCaseData&) override {}

    // Query reporting (for listing tests, etc.)
    void report_query(const doctest::QueryData&) override {}
};


/* Sideline listener: Listener always run in background, no affected
 * by the --reporters flag. Reports per-suite timing every run, and
 * per-test-case timing when TestRunner::g_print_timing is enabled */
struct TimingListener : public doctest::IReporter
{
    using IReporter::IReporter;

    TimingListener(const doctest::ContextOptions& in) : IReporter() {}

    std::unordered_map<std::string, double> suite_totals;
    const char* current_name = nullptr;
    const char* current_suite = nullptr;

    void test_run_start() override
    {
        this->suite_totals.clear();
        /* TODO: this doesn't feel appropriate here but it works for now: */
        if (TestRunner::g_print_test_timing)
        {
            godot::UtilityFunctions::print("---- Test timings ----");
        }
    }

    void test_run_end(const doctest::TestRunStats&) override
    {
        if (!TestRunner::g_print_suite_timing)
        {
            return;
        }

        std::vector<std::pair<std::string, double>> totals(this->suite_totals.begin(), this->suite_totals.end());
        std::sort(totals.begin(), totals.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

        godot::UtilityFunctions::print("---- Suite timings ----");
        for (const auto& entry : totals)
        {
            godot::UtilityFunctions::print(godot::vformat(
                "[%s] [%ss] %s",
                godot_reporter_name, 
                godot::String::num(entry.second, 3),
                entry.first.empty() ? "(no suite)" : entry.first.c_str()
            ));
        }
    }

    void test_case_start(const doctest::TestCaseData& in) override
    {
        this->current_name = in.m_name;
        this->current_suite = in.m_test_suite;
    }

    void test_case_reenter(const doctest::TestCaseData&) override {}

    void test_case_end(const doctest::CurrentTestCaseStats& in) override
    {
        this->suite_totals[this->current_suite ? this->current_suite : "?"] += in.seconds;

        if (TestRunner::g_print_test_timing)
        {
            godot::UtilityFunctions::print(godot::vformat(
                "[%s] [%ss] %s - %s",
                godot_reporter_name,
                godot::String::num(in.seconds, 4),
                in.testCaseSuccess ? "PASS" : "FAIL",
                this->current_name ? this->current_name : "?"
            ));
        }
    }

    void test_case_exception(const doctest::TestCaseException&) override {}

    void subcase_start(const doctest::SubcaseSignature&) override {}
    void subcase_end() override {}

    void log_assert(const doctest::AssertData&) override {}
    void log_message(const doctest::MessageData&) override {}

    void test_case_skipped(const doctest::TestCaseData&) override {}

    void report_query(const doctest::QueryData&) override {}
};


REGISTER_REPORTER("godot", 1, GodotReporter);
REGISTER_LISTENER("timing", 1, TimingListener);

GDExtensionInterfacePrintError original_gdextension_interface_print_error = nullptr;
GDExtensionInterfacePrintErrorWithMessage original_gdextension_interface_print_error_with_message = nullptr;

bool TestRunner::g_error_called = false;
bool TestRunner::g_currently_testing_error = false;
bool TestRunner::g_print_test_timing = false;
bool TestRunner::g_print_suite_timing = false;

void custom_gdextension_interface_print_error(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    TestRunner::g_error_called = true;
    if (!(TestRunner::g_currently_testing_error))
    {
        original_gdextension_interface_print_error(p_description, p_function, p_file, p_line, p_editor_notify);
        godot::UtilityFunctions::print("Error in test: ", g_current_test_name);
    }
}

void custom_gdextension_interface_print_error_with_message(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    TestRunner::g_error_called = true;
    if (!(TestRunner::g_currently_testing_error))
    {
        original_gdextension_interface_print_error_with_message(p_description, p_message, p_function, p_file, p_line, p_editor_notify);
        godot::UtilityFunctions::print("Error in test: ", g_current_test_name);
    }
}

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

    /* Save for later */
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

void TestRunner::set_test_duration_printing(bool test_duration_printing)
{
    this->test_duration_printing = test_duration_printing;
    TestRunner::g_print_test_timing = test_duration_printing;
}

void TestRunner::set_suite_duration_printing(bool suite_duration_printing)
{
    this->suite_duration_printing = suite_duration_printing;
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