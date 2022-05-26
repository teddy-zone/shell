#pragma once

#include "component.h"

struct CompAttachment : public Component
{
    std::vector<EntityRef> attached_entities;
    glm::vec3 position_offset;
};