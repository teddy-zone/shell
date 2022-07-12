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
				if (comp_weather.state == WeatherState::Dusk)
				{
					for (auto& sound : comp_weather.sibling<CompVoice>()->sounds)
					{
						sound.second.trigger = false;
					}
					if (auto* point_light = comp_weather.sibling<CompPointLight>())
					{
						point_light->light.visible = 1;
						point_light->light.location = glm::vec4(2000, 2000, 2000, 1.0);
						//point_light->light.location = glm::vec4(100, -500, 500,1.0);
						point_light->light.color = glm::vec4(0xFD * 1.0 / 255, 0xFB * 1.0 / 255, 0xD3 * 1.0 / 255, 1.0);
						point_light->light.intensity = 0.1;
					}
				}
				else if (comp_weather.state == WeatherState::PartlyCloudy)
				{
					for (auto& sound : comp_weather.sibling<CompVoice>()->sounds)
					{
						if (sound.first == "cicada")
						{
							sound.second.trigger = true;
						}
						else
						{
							sound.second.trigger = false;
						}
					}
					if (auto* point_light = comp_weather.sibling<CompPointLight>())
					{
						point_light->light.visible = 1;
						point_light->light.location = glm::vec4(200, -50, 300, 1.0);
						point_light->light.color = glm::vec4(1.1,1,1.05,1.0);
						point_light->light.intensity = 20.0;
					}
				}
				else
				{
					if (auto* point_light = comp_weather.sibling<CompPointLight>())
					{
						point_light->light.visible = false;
					}
				}
			}
			comp_weather.prev_state = comp_weather.state;
		}


		
	}
};