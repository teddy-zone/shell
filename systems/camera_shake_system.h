#pragma once

#include <glm/glm.hpp>

#include "camera_shake_component.h"

class SysCameraShake : public System
{
	std::mt19937 shake_generator;
	std::normal_distribution<float> shake_distribution;

public:

	SysCameraShake() :
		shake_distribution(0.0, 1.0)
	{}

	virtual void update(double dt) override
	{
		auto& camera_shake_components = get_array<CompCameraShake>();
		auto& camera = get_array<CompCamera>()[0];
		float t = _interface->get_current_game_time();
		float x_deviation = 0;
		float y_deviation = 0;
		for (auto& shake_comp : camera_shake_components)
		{
			float one_over_shake_distance_squared = 1.0;
			if (auto* pos_comp = shake_comp.sibling<CompPosition>())
			{
				one_over_shake_distance_squared = glm::length(pos_comp->pos - (camera.graphics_camera.get_position() + camera.graphics_camera.get_unoffset_look() * 10));
				one_over_shake_distance_squared = 1.0/(one_over_shake_distance_squared * one_over_shake_distance_squared);
			}
			x_deviation += shake_comp.shake_amount * sin(t * shake_comp.shake_frequency + shake_distribution(shake_generator))* one_over_shake_distance_squared;
			y_deviation += shake_comp.shake_amount*cos(t * shake_comp.shake_frequency + shake_distribution(shake_generator))* one_over_shake_distance_squared;
			if (shake_comp.fade_out)
			{
				shake_comp.shake_amount *= 0.99;
			}
		}

		auto view_x_vector = glm::cross(camera.graphics_camera.get_look(), glm::vec3(0, 0, 1));
		auto view_y_vector = glm::cross(view_x_vector, camera.graphics_camera.get_look());

		camera.graphics_camera.set_look_offset(view_x_vector * x_deviation + view_y_vector * y_deviation);
	}
};