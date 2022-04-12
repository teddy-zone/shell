#pragma once

#include <functional>

#include "component.h"

struct CompOnCast : public Component
{
    std::vector<std::function<void(EntityRef)>> on_cast_callbacks;
};