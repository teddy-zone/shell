#pragma once

#include "system.h"
#include "ability.h"

class SysAbility : public System
{

public:
    virtual void update(double dt)
    {
        auto& caster_components = get_array<CompCaster>();
        for (auto& caster_component : caster_components)
        {
            switch (caster_component.state)
            {
                case AbilityState::CastPoint:
                    {
                        // if state_time >= cast point of ability
                        // Activate ability!
                        // set state to backswing, state time to zero
                    }
                    break;
                case AbilityState::Backswing:
                    {
                        // if state_time >= backswing of ability
                        // set state to None
                    }
                    break;
                case AbilityState::None:
                    {
                    }
                    break;
            }
        }
    }

};