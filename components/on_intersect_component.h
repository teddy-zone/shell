#pragma once

#include <functional>

#include "component.h"

struct CompOnIntersect : public Component
{
    std::vector<std::function<void(EntityRef, EntityRef)>> on_intersect_callbacks;
};