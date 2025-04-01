#include "godot_error_interceptor.hpp"

#include "test_runner.hpp"

bool GodotErrorInterceptor::g_error_called = false;
bool GodotErrorInterceptor::g_currently_testing_error = false;
thread_local const char* GodotErrorInterceptor::g_current_test_name = nullptr;
GDExtensionInterfacePrintError GodotErrorInterceptor::original_gdextension_interface_print_error = nullptr;
GDExtensionInterfacePrintErrorWithMessage GodotErrorInterceptor::original_gdextension_interface_print_error_with_message = nullptr;

void GodotErrorInterceptor::gdextension_interface_print_error_intercept(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    GodotErrorInterceptor::g_error_called = true;
    if (!(GodotErrorInterceptor::g_currently_testing_error))
    {
        original_gdextension_interface_print_error(p_description, p_function, p_file, p_line, p_editor_notify);
        godot::UtilityFunctions::print("Error in test: ", GodotErrorInterceptor::g_current_test_name);
    }
}

void GodotErrorInterceptor::gdextension_interface_print_error_with_message_intercept(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    GodotErrorInterceptor::g_error_called = true;
    if (!(GodotErrorInterceptor::g_currently_testing_error))
    {
        original_gdextension_interface_print_error_with_message(p_description, p_message, p_function, p_file, p_line, p_editor_notify);
        godot::UtilityFunctions::print("Error in test: ", GodotErrorInterceptor::g_current_test_name);
    }
}
