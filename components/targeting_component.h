#pragma once

#include "component.h"

enum class TargetingMode
{
    Unit,
    Ground,
    Vector,
};

struct CompTargeting : public Component
{
    TargetingMode mode;
    glm::vec3 ground_target;
    glm::vec3 vector_start_target;
    glm::vec3 vector_end_target;
    EntityRef unit_target;
};
