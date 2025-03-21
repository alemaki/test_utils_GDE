#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

godot::SceneTree* get_scene_tree();
godot::Node* get_scene_root();

void simulate(godot::Node* node, int frames = 1);

double get_node_delta(godot::Node* node);
double get_current_engine_delta();

#endif /* BT_TEST_UTILS_HPP */