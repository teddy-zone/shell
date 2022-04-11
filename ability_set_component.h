#pragma once
#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"

struct CompAbilitySet : public Component, public StatInterface
{

    std::array<EntityRef, 6> abilities;

    virtual StatPart get_stat(Stat stat) override
    {
        StatPart out_part;
        for (auto& ability_ref : abilities)
        {
            auto single_ability_stat_comp = ability_ref.cmp<CompStat>();
            out_part = out_part.join(single_ability_stat_comp->get_stat(stat));
        }
        return out_part;
    }

};