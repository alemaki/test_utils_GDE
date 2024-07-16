#include "test_utils.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "tests/test_utils/test_runner.hpp"

void simulate_frame(godot::Node* node, float delta)
{
	godot::Array children = node->get_children();
	int size = children.size();
	for (int i = 0; i < size; i++)
	{
		simulate_frame(godot::Object::cast_to<godot::Node>(children[i]), delta);
	}
	node->_process(delta);
	node->_physics_process(delta);

}

void simulate(godot::Node* node, int frames)
{
	double delta = get_node_delta(node);
	//ERR_FAIL_COND(delta == 0);
	for (int i = 0; i < frames; ++i)
    {
		simulate_frame(node, delta);
	}
}

double get_current_engine_delta()
{
	return  godot::Engine::get_singleton()->is_in_physics_frame() ? ::get_scene_root()->get_physics_process_delta_time() : ::get_scene_root()->get_process_delta_time();
}

double get_node_delta(godot::Node* node)
{
	return  godot::Engine::get_singleton()->is_in_physics_frame() ? node->get_physics_process_delta_time() : node->get_process_delta_time();
}