#pragma once

#include "component.h"

enum class AttachmentType
{
    Attacher,
    Attachee
};

struct CompAttachment : public Component
{
    std::vector<EntityRef> attached_entities;
    glm::vec3 position_offset;
    AttachmentType type = AttachmentType::Attachee;
};