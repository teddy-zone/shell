#pragma once

#include "component.h"

struct CompVolumeSettings : public Component
{
	float master_volume = 0.5;
	float sound_fx_volume = 0.5;
	float music_volume = 0.5;
	bool do_update = true;
};