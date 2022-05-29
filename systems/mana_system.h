#pragma once

#include "system.h"
#include "mana_component.h"

class SysMana : public System
{
public:
    virtual void update(double dt) override
    {
        auto& mana_comps = get_array<CompMana>();
        for (auto& mana_comp : mana_comps)
        {
            if (auto* stat_comp = mana_comp.sibling<CompStat>())
            {
                auto mana_regen = stat_comp->get_abs_stat(Stat::ManaRegen);
                mana_comp.add_mana(mana_regen*dt);
            }
        }
    }
};