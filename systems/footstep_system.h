#pragma once

#include <random>

#include "system.h"
#include "skeletal_mesh_component.h"
#include "voice_component.h"

class SysFootsteps : public System
{
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
public:

    SysFootsteps():
        dist(1,5)
    {}

    virtual void update(double dt) override
    {
        auto& skeletons = get_array<CompSkeletalMeshNew>();
        
        std::vector<std::string> feet{"left_foot", "right_foot"};
        for (auto& skeleton : skeletons)
        {
            for (auto& foot : feet)
            {
                if (!skeleton.get_joint(foot).prev_on_ground && skeleton.get_joint(foot).on_ground)
                {
                    if (auto* voice = skeleton.sibling<CompVoice>())
                    {
                        auto rand_footstep = dist(gen);
                        std::string sound = "walk_concrete" + std::to_string(rand_footstep);
                        voice->sounds[sound].trigger = true;
                    }
                }
            }
        }
    }
};