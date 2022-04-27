
#pragma once

#include "component.h"
#include "ability_set_component.h"

struct CompCaster : public Component
{
    int ability_index;
    AbilityState state;
    float state_time;
    std::optional<EntityRef> unit_target;
    std::optional<glm::vec3> ground_target;
    void activate_ability(int index)
    {
        ability_index = index;
        state = AbilityState::CastPoint;
        state_time = 0.0;
    }
    CompAbility* get_ability(std::optional<int> in_index=std::nullopt)
    {
        int to_get;
        if (!in_index)
        {
            to_get = ability_index;
        }
        else 
        {
            to_get = in_index.value();
        }
        if (to_get < 4)
        {
            auto* ability_set = sibling<CompAbilitySet>();
            return ability_set->get_ability_component_by_index(to_get);
        }
        else if (to_get > 3 && to_get < 10)
        {
            auto* inventory = sibling<CompInventory>();
            auto* out_ability = inventory->items[to_get - 4].cmp<CompAbility>();
            if (out_ability)
            {
                return out_ability;
            }
            else
            {
                throw std::runtime_error("Trying to activate non-active ability");
            }
        }
        else
        {
            throw std::runtime_error("Bad ability index");
        }
    }
};