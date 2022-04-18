
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
    CompAbility* get_ability()
    {
        auto* ability_set = sibling<CompAbilitySet>();
        return ability_set->get_ability_component_by_index(ability_index);
    }
};