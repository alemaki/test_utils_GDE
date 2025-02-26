#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

godot::SceneTree* get_scene_tree();
godot::Node* get_scene_root();

void simulate(godot::Node* node, int frames = 1);

double get_node_delta(godot::Node* node);
double get_current_engine_delta();

template<typename T>
godot::TypedArray<T> get_all_children_of_type(godot::Node* node)
{
    static_assert(std::is_base_of<godot::Node, T>::value, "T must inherit from godot::Node");
    ERR_FAIL_NULL_V(node, {});
    godot::TypedArray<godot::Node> children = node->get_children();
    godot::TypedArray<T> result;
    for (int i = 0, size = children.size(); i < size; i++)
    {
        godot::Node* child = godot::Object::cast_to<godot::Node>(children[i]);
        if (child->get_class() == T::get_class_static())
        {
            result.push_back(child);
        }
        result.append_array(get_all_children_of_type<T>(child));
    }
    return result;
}

#endif /* BT_TEST_UTILS_HPP */