#pragma once

#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"

struct CompHealth : public Component
{

    float health_percentage;
    float get_current_health() const
    {
        auto stat_comp = sibling<CompStat>():
        if (stat_comp)
        {
            auto max_health = stat_comp->get_stat(Stat::MaxHealth);
            return health_percentage*max_health;
        }
        else
        {
            return health_percentage;
        }
    }

};