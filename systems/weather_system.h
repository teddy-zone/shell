#pragma once

#include "system.h"
#include "weather_component.h"


class SysWeather : public System
{
	int countdown = 10;

public:
	virtual void init_update() override
	{
	}

	virtual void update(double dt) override
	{
		if (countdown == 0)
		{
			_interface->add_entity_from_proto(std::make_shared<WeatherEntityProto>().get());
			countdown--;
		}
		else if (countdown > 0)
		{
			countdown--;
		}

		auto& weather_components = get_array<CompWeather>();
		auto& camera = get_array<CompCamera>()[0].graphics_camera;
		for (auto& comp_weather : weather_components)
		{
			if (auto* weather_pos = comp_weather.sibling<CompPosition>())
			{
				weather_pos->pos = camera.get_position() + glm::vec3(-5,-8,-5);
			}
			if (comp_weather.state != comp_weather.prev_state)
			{
				if (comp_weather.state == WeatherState::Rain)
				{
					for (auto& sound : comp_weather.sibling<CompVoice>()->sounds)
					{
						if (sound.first == "rain")
						{
							sound.second.trigger = true;
						}
						else
						{
							sound.second.trigger = false;
						}
					}
				}
				if (comp_weather.state == WeatherState::Snow)
				{
					for (auto& sound : comp_weather.sibling<CompVoice>()->sounds)
					{
						if (sound.first == "wind")
						{
							sound.second.trigger = true;
						}
						else
						{
							sound.second.trigger = false;
						}
					}
				}
			}
			comp_weather.prev_state = comp_weather.state;
		}


		
	}
};