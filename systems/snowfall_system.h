#pragma once

#include <random>

#include "system.h"
#include "snowfall_component.h"

class SysSnowfall : public System
{
    const float fall_vel = 1.0;
    const float spawn_chances = 5.0;
    const float spawn_probability = 0.25;
    std::uniform_real_distribution<float> dist_roll;
    std::mt19937 gen;

public:

    SysSnowfall():
        dist_roll(0,1.0)
    {}

    virtual void init_update()
    {

    }

    virtual void update(double dt)
    {
        auto& snowfall_comps = get_array<CompSnowfall>();
        for (auto& snowfall_comp : snowfall_comps)
        {
            for (auto& snow_pos : snowfall_comp.snow_positions)
            {
                snow_pos -= glm::vec3(0,0,1)*dt*fall_vel;
                if (snow_pos.z < 0)
                {
                    snow_pos.z = snowfall_comp.volume.z;
                }
            }
            if (snowfall_comp.snow_positions.empty())
            {
                std::uniform_real_distribution<float> distx(snowfall_comp.volume.x);
                std::uniform_real_distribution<float> disty(snowfall_comp.volume.y);
                std::uniform_real_distribution<float> distz(snowfall_comp.volume.z);
                int num_snowflakes = int(snowfall_comp.density*snowfall_comp.volume.x*snowfall_comp.volume.y*snowfall_comp.volume.z);
                for (int i = 0; i < num_snowflakes; ++i)
                {
                    glm::vec3 new_pos(distx(gen), disty(gen), distz(gen));
                    snowfall_comp.snow_positions.push_back(new_pos);
                }
            }
        }


    }
};