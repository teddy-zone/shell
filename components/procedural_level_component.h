#pragma once

#include "component.h"

struct CompProceduralLevel : public Component
{
	bool generated;
	int length;
	float x_amplitude;
	float y_amplitude;
	float radius_mean;
	float radius_std_dev;
	glm::vec3 spawn_point;
	std::vector<glm::vec3> path;
	std::vector<float> widths;
	float floor_level;
	float cliff_level;
};