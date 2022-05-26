#pragma once

#include <functional>

#include "component.h"

struct CompOnCast : public Component
{
    bool any_ability = false;
    bool any_item = false;
    std::vector<std::function<void(EntityRef, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)>> on_cast_callbacks;
};