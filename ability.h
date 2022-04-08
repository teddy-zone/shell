#pragma once

#include "component.h"

enum class AbilityState
{
    None,
    CastPoint,
    Backswing
};

struct CompCaster : public Component
{
    int ability_index;
    AbilityState state;
    float state_time;
    void activate_ability(int index)
    {
        ability_index = index;
        state = AbilityState::CastPoint;
    }
};

struct CompAbility : public Component
{
    float cooldown;
    float cast_point;
    float cast_range;
    float backswing;
    bool ground_targeted;
    bool unit_targeted;
};