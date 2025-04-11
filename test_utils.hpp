#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

godot::SceneTree* get_scene_tree();
godot::Node* get_scene_root();
void clear_scene_root();

void simulate(godot::Node* node, int frames = 1);

double get_node_delta(godot::Node* node);
double get_current_engine_delta();

template<typename T>
godot::TypedArray<T> get_all_children_of_type(godot::Node* node, bool recursive = false)
{
    static_assert(std::is_base_of<godot::Node, T>::value, "T must inherit from godot::Node");
    ERR_FAIL_NULL_V(node, {});
    godot::TypedArray<godot::Node> children = node->get_children();
    godot::TypedArray<T> result;
    for (int i = 0, size = children.size(); i < size; i++)
    {
        T* t_child = godot::Object::cast_to<T>(children[i]);
        if (t_child)
        {
            result.push_back(t_child);
        }
        if (recursive)
        {
            godot::Node* child = godot::Object::cast_to<godot::Node>(children[i]);
            result.append_array(get_all_children_of_type<T>(child, true));
        }
    }
    return result;
}

#endif /* BT_TEST_UTILS_HPP */