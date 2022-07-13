#pragma once

#include "component.h"

struct CompBasicEnemyAI : public Component
{

    float vision_range;
    float last_radius_check = -1;
    bool wandering = true;
    std::optional<EntityRef> target;
    std::optional<int> ability_index_using;
};