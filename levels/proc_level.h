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
        auto& player_array = get_array<CompPlayer>();

        if (player_array.size())
        {
            auto& player1 = get_array<CompPlayer>()[0];
            player1.sibling<CompPosition>()->pos = glm::vec3(30, 135, 20);
            get_array<CompCamera>()[0].set_look_target(player1.sibling<CompPosition>()->pos, true);
        }

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
    }
};
