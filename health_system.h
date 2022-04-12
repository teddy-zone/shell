#pragma once

#include "system.h"
#include "health_component.h"

class SysHealth : public System
{
public:
    constexpr float filter_velocity = 0.2;
    virtual void update(double dt) override
    {
        auto& health_comps = get_array<CompHealth>();
        for (auto& health_comp : health_comps)
        {
            health_comp.filtered_health_percentage = 
                std::max(health_comp.filtered_health_percentage - filter_velocity*float(dt), 
                         health_comp.health_percentage);
        }
    }
};