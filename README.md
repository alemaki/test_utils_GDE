# GDExtension C++ Testing Framework for Godot

A powerful testing framework for Godot's GDExtension in C++, bringing the convenience of Doctest with additional custom utilities tailored for Godot.

## Features

- **Doctest Integration**: Leverage almost all Doctest features while working with GDExtension.
- **Custom Signal Watcher**: Track Godot signals effortlessly:
  - Check if a signal was emitted.
  - Count how many times it was triggered.
  - Inspect the arguments of the last emitted signal.
- **Error Condition Macros**: Detect `ERR_FAIL_**` failures in functions without cluttering output with error messages.
- **`simulate()` Function**: Invoke `_process` and `_physics_process` of nodes for seamless simulation in tests.
- **Scene Root Injection**: Add nodes to the root scene using `get_scene_root()`, allowing tests to run in a real scene environment.
- **Editor/Runtime**: Can handle both editor and runtime tests.

## Limitations

- **Exception Requirement**:  Doctest's `REQUIRE` demands GDExtension to be built with exceptions enabled. If exceptions are disabled, any `REQUIRE` will behave as `CHECK` instead.
- **Segfault Risks**: If a test causes a segmentation fault during editor tests, it will crash the entire engine. ¯\\\_(ツ)_/¯. During runtime, only the scene/current process will crash.

## Example Usage

- To initiate tests, create a `TestRunner` node within a scene (preferably outside gameplay-related scenes). When this node loads in the editor, it runs editor-related tests. If the scene containing the `TestRunner` is executed, it will run runtime tests.


- **Note**: The following examples are taken from another project.

```cpp
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS /* Since exceptions are disabled normally */
#include "tests_utils_GDE/test_macros.hpp" /*Contains Doctest, signal watcher, and other Godot-specific macros. */
#include "somewhere/health_component.hpp"

struct HealthComponentFixture
{
    HealthComponent* health_component;

    HealthComponentFixture() : health_component(memnew(HealthComponent)){}
    ~HealthComponentFixture()
    {
        memdelete(health_component);
    }
};

TEST_SUITE("HealthComponentTests") /* Runs at runtime since it is not specified as an editor test. */
{
    TEST_CASE_FIXTURE(HealthComponentFixture, "Test health component basics")
    {
        CHECK_EQ(health_component->get_max_hp(), 0);
        CHECK_EQ(health_component->get_current_hp(), 0);

        health_component->set_max_hp(100);
        CHECK_EQ(health_component->get_max_hp(), 100);

        health_component->set_max_hp(-10);
        CHECK_EQ(health_component->get_max_hp(), 0);

        health_component->set_max_hp(10);
        health_component->set_current_hp(5);
        CHECK_EQ(health_component->get_current_hp(), 5);
    }

    TEST_CASE_FIXTURE(HealthComponentFixture, "Test health component health_depleted signal")
    {
        health_component->set_max_hp(10);
        health_component->set_current_hp(2);

        REQUIRE(health_component->has_signal("health_depleted"));

        SignalWatcher::watch_signals(health_component);

        health_component->take_damage(1);
        CHECK_FALSE(SignalWatcher::signal_emitted(health_component, godot::String("health_depleted")));

        health_component->take_damage(1);
        CHECK(SignalWatcher::signal_emitted(health_component, godot::String("health_depleted")));
        CHECK_EQ(SignalWatcher::get_signal_emitted_count(health_component, godot::String("health_depleted")), 1);
    }
}
```

### Errors
- These tests will detect `ERR_FAIL_**` calls without producing error prints within `CHECK_GODOT_ERROR` or `REQUIRE_GODOT_ERROR` bounds.
- **NOTE**: They currently do not support `EDMSG` prints—this is planned for future updates.


```cpp
/*  Example function: */
void BTGraphView::create_task_node(const godot::StringName& task_name, const godot::StringName& task_class_name)
{
    ERR_FAIL_COND(task_name.is_empty());
    ERR_FAIL_COND(task_class_name.is_empty());
    ERR_FAIL_COND_MSG(this->has_task_name(task_name), "BTGraphView already has task_name named: " + task_name + ".");
    ...
}

/* Testing for expected errors.*/
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "tests_utils_GDE/test_macros.hpp"
#include "somewhere/graph_view.hpp"

TEST_SUITE("[editor]" "BTGraphView") /* Runs in the editor; useful for testing plugins and UI elements. */
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to create task node")
    {
        SUBCASE("Non-existent nodes")
        {
            CHECK_GODOT_ERROR(graph_view->create_task_node("", "TaskClass")); /* Won't produce an error print.*/
            CHECK_GODOT_ERROR(graph_view->create_task_node("task_1", ""));
        }

        SUBCASE("Fail to Create Duplicate Task Nodes")
        {
            graph_view->create_task_node("task_1"); /* If this call produces an error, it will print as expected. */
            CHECK_GODOT_ERROR(graph_view->create_task_node("task_1"));
        }
    }
}
```

## Installation

1. Clone the framework in your GDExtension project.
2. Ensure your build supports exceptions if using `REQUIRE`. (Optional)
3. Run tests normally within the Godot engine with the `TestRunner` node.


## Future of the project

- Transform this framework into a full-fledged Godot plugin for easier testing.
- Explore solutions to prevent engine crashes due to segmentation faults during testing.

## Contributing

Contributions are welcome always! Feel free to submit PRs or open issues.

## License

MIT License. Do as thou wilt!
