#ifndef TEST_REPORTERS_HPP
#define TEST_REPORTERS_HPP

#include <doctest.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>

#include "godot_error_interceptor.hpp"
#include "test_runner.hpp"

static const char* godot_reporter_name = "GDE C++ Tests";

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
        GodotErrorInterceptor::g_current_test_name = in.m_name;
    }

    // (due to subcases)
    void test_case_reenter(const doctest::TestCaseData&) override {}

    void test_case_end(const doctest::CurrentTestCaseStats&) override
    {
        GodotErrorInterceptor::g_current_test_name = nullptr;
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
                "[%s] [%ss] - %s",
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

#endif // TEST_REPORTERS_HPP
