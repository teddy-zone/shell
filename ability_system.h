#pragma once

#include "system.h"
#include "ability.h"
#include "ability_set_component.h"
#include "selected_objects_component.h"

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
        auto& selected_object_component = get_array<CompSelectedObjects>();
        if (selected_object_component.size())
        {
            if (selected_object_component[0].selected_objects.size())
            {
                EntityId cur_obj = selected_object_component[0].selected_objects[0];
                auto* ability_set = (CompAbilitySet*)_interface->get_component(type_id<CompAbilitySet>, cur_obj);
                if (ability_set)
                {
                    draw_ability_widget(ability_set);
                }
            }
        }
    }

    void draw_ability_widget(CompAbilitySet* ability_set)
    {

    }

};