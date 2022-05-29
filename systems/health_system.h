#pragma once

#include "system.h"
#include "health_component.h"

class SysHealth : public System
{
public:
    const float filter_velocity = 40;
    virtual void update(double dt) override
    {
        auto& health_comps = get_array<CompHealth>();
        for (auto& health_comp : health_comps)
        {
            health_comp.filtered_health_percentage = 
                std::max(health_comp.filtered_health_percentage - filter_velocity*float(dt), 
                         health_comp.health_percentage);
            if (auto* stat_comp = health_comp.sibling<CompStat>())
            {
                auto health_regen = stat_comp->get_abs_stat(Stat::HealthRegen);
                health_comp.add_health(health_regen*dt);
            }
        }
    }
};