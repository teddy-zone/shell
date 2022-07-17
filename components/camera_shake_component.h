#pragma once

#include "component.h"

struct CompCameraShake : public Component
{
	float shake_amount;
	float shake_frequency;
	bool fade_out = true;
};