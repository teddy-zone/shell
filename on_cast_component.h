#pragma once

#include <functional>

#include "component.h"

struct CompOnCast : public Component
{
    std::vector<std::function<void(EntityRef, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target)>> on_cast_callbacks;
};