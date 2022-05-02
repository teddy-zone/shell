#pragma once

#include "component.h"

struct CompBasicEnemyAI : public Component
{

    float vision_range;
    std::optional<EntityRef> target;
};