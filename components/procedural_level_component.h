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
};