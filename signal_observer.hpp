#ifndef SIGNAL_WATCHER_HPP
#define SIGNAL_WATCHER_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

/**
 * @brief A utility class to monitor Godot signals and is responsible for watching objects's signals. Used for testing purposes.
 *
 */
class SignalObserver
{
private:
    static std::unordered_map<std::string, int> signal_count;
    static std::unordered_map<std::string, godot::Array> signal_last_arguments;
    static std::unordered_set<void*> watched_objects;

private:
    /* In Godot, signals can have a maximum of 5 parameters.
    This limitation is built into the Godot engine.*/
    static void on_signal_emitted_0_params(const godot::String& signal_key);
    static void on_signal_emitted_1_params(godot::Variant param1,
                                           const godot::String& signal_key);
    static void on_signal_emitted_2_params(godot::Variant param1,
                                           godot::Variant param2,
                                           const godot::String& signal_key);
    static void on_signal_emitted_3_params(godot::Variant param1,
                                           godot::Variant param2,
                                           godot::Variant param3,
                                           const godot::String& signal_key);
    static void on_signal_emitted_4_params(godot::Variant param1,
                                           godot::Variant param2,
                                           godot::Variant param3,
                                           godot::Variant param4,
                                           const godot::String& signal_key);
    static void on_signal_emitted_5_params(godot::Variant param1,
                                           godot::Variant param2,
                                           godot::Variant param3,
                                           godot::Variant param4,
                                           godot::Variant param5,
                                           const godot::String& signal_key);

    static void connect_target_signal(godot::Object* target, const godot::Dictionary& signal_info);

public:
    /**
     * @brief Registers a target for the signal observer to observe.
     *
     * @param target The object to observe
     */
    static void watch_signals(godot::Object* target);
    /**
     * @brief Cheks if the target has emitted a signal with name signal_name.
     * The target must be registered with watch_signals() before the signal occurs.
     *
     * @param target The object to check
     * @param signal_name The name of the signal to check
     * @return true when the object has emmitted the signal after it has been registered,
     * @return false otherwise
     */
    static bool signal_emitted(godot::Object* target, const godot::String& signal_name);
     /**
     * @brief Returns how many times a given signal was emitted by a target object.
     *
     * @param target The observed object.
     * @param signal_name The name of the signal to count.
     * @return The number of times the signal was emitted since the object was registered.
     */
    static int get_signal_emitted_count(godot::Object* target, const godot::String& signal_name);
    /**
     * @brief Retrieves the arguments from the most recent emission of a given signal.
     * The returned array contains the parameters passed during tnat signal.
     *
     * @param target The observed object.
     * @param signal_name The name of the signal whose arguments to fetch.
     * @return A `godot::Array` containing the signal’s arguments, or an empty array if the signal was not emitted.
     */
    static godot::Array get_signal_emitted_arguments(godot::Object* target, const godot::String& signal_name);
    /**
     * @brief Clears all tracked signal data for all observed objects.
     */
    static void reset();
    /**
     * @brief Clears tracked signal data for a specific object. The object remains registered.
     *
     * @param target The object whose tracked data should be cleared.
     */
    static void reset_object(godot::Object* target);
};

#endif /* SIGNAL_WATCHER_HPP */
