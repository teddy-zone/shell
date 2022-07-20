#pragma once

#include <sstream>

#include "level.h"
#include "procedural_level_component.h"

class ProcTestLevel : public Level
{
public:
    ProcTestLevel():
        Level("ProcTestLevel")
    {
    }

    virtual void level_init() override
    {

        EntityRef proc_level_entity = _interface->add_entity_with_components({ uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompProceduralLevel>),
                    uint32_t(type_id<CompPickupee>)
            });

        proc_level_entity.cmp<CompProceduralLevel>()->generated = false;
        proc_level_entity.cmp<CompProceduralLevel>()->length = 40;
        proc_level_entity.cmp<CompProceduralLevel>()->x_amplitude = 80;
        proc_level_entity.cmp<CompProceduralLevel>()->y_amplitude = 15;
        proc_level_entity.cmp<CompBounds>()->is_static = true;
        proc_level_entity.cmp<CompPosition>()->pos = glm::vec3(1, 1, 1);

    }

    virtual void update(double dt) override
    {
        auto time_comp = get_array<CompTime>()[0];

        auto& player_array = get_array<CompPlayer>();
        auto& proc_levels = get_array<CompProceduralLevel>();

        static bool player_placed = false;
        if (player_array.size() && !player_placed && proc_levels.size())
        {
            if (proc_levels[0].generated)
            {
                auto& player1 = get_array<CompPlayer>()[0];
                player1.sibling<CompPosition>()->pos = proc_levels[0].spawn_point;
                get_array<CompCamera>()[0].set_look_target(player1.sibling<CompPosition>()->pos, true);
                player_placed = true;

                int light_counter = 0;
                for (auto& path_element : proc_levels[0].path)
                {
                    glm::vec3 color1(1, 0, 0);
                    glm::vec3 color2(0, 1, 0);
                    LightEntityProto light_proto3(glm::vec3(160, 160, 40));
                    auto light_entity3 = _interface->add_entity_from_proto(&light_proto3);
                    light_entity3.cmp<CompPointLight>()->light.intensity = 0.03;
                    if (light_counter % 2)
                    {
                        light_entity3.cmp<CompPointLight>()->light.color = glm::vec4(color1, 1.0);// glm::vec4(0.9, 0.4, 0.3, 1.0);
                    }
                    else
                    {
                        light_entity3.cmp<CompPointLight>()->light.color = glm::vec4(color2, 1.0);// glm::vec4(0.9, 0.4, 0.3, 1.0);
                    }
                    light_entity3.cmp<CompPointLight>()->light.location = glm::vec4(path_element + glm::vec3(0, 0, 40), 0);
                    
                    light_entity3.set_name("PathLight" + std::to_string(light_counter));
                    light_counter++;
                }
            }
        }
    }
};
