#pragma once
#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"

struct CompAbilitySet : public Component, public StatInterface
{

    std::array<EntityId, 6> abilities;

    virtual StatPart get_stat(Stat stat) override
    {
        StatPart out_part;
        for (auto& ability_eid : abilities)
        {
            if (ability_eid >= 0)
            {
                auto single_ability_stat_comp = (CompStat*)get_component(type_id<CompStat>, ability_eid);
                out_part = out_part.join(single_ability_stat_comp->get_stat(stat));
            }
        }
        return out_part;
    }

};