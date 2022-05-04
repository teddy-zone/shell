#pragma once

#include "component.h"

struct CompBasicEnemyAI : public Component
{

    float vision_range;
    float last_radius_check = -1;
    std::optional<EntityRef> target;
};