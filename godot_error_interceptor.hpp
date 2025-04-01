#ifndef GODOT_ERROR_INTERCEPTOR_HPP
#define GODOT_ERROR_INTERCEPTOR_HPP

#include <godot_cpp/variant/utility_functions.hpp>

class GodotErrorInterceptor
{
private:
    static GDExtensionInterfacePrintError original_gdextension_interface_print_error;
    static GDExtensionInterfacePrintErrorWithMessage original_gdextension_interface_print_error_with_message;
public:
    GodotErrorInterceptor()
    {
        GodotErrorInterceptor::original_gdextension_interface_print_error = godot::internal::gdextension_interface_print_error;
        GodotErrorInterceptor::original_gdextension_interface_print_error_with_message = godot::internal::gdextension_interface_print_error_with_message;
        godot::internal::gdextension_interface_print_error = GodotErrorInterceptor::gdextension_interface_print_error_intercept;
        godot::internal::gdextension_interface_print_error_with_message = GodotErrorInterceptor::gdextension_interface_print_error_with_message_intercept;
    }

    ~GodotErrorInterceptor()
    {
        godot::internal::gdextension_interface_print_error = GodotErrorInterceptor::original_gdextension_interface_print_error;
        godot::internal::gdextension_interface_print_error_with_message = GodotErrorInterceptor::original_gdextension_interface_print_error_with_message;
    }

    static bool g_error_called;
    static bool g_currently_testing_error;
    static thread_local const char* g_current_test_name;

private:
    static void gdextension_interface_print_error_intercept(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify);

    static void gdextension_interface_print_error_with_message_intercept(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify);
};

#endif // GODOT_ERROR_INTERCEPTOR_HPP