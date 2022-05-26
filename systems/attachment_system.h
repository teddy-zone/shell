#pragma once

#include "system.h"
#include "attachment_component.h"

class SysAttachment : public System
{
    void update(double dt) override
    {
        auto& attachment_components = get_array<CompAttachment>();
        for (auto& attachment_component : attachment_components)
        {
            if (auto* my_pos_comp = attachment_component.sibling<CompPosition>())
            {
                for (auto& attached_entity : attachment_component.attached_entities)
                {
                    if (auto* other_pos_comp = attached_entity.cmp<CompPosition>())
                    {
                        other_pos_comp->pos = my_pos_comp->pos + attachment_component.position_offset;
                    }
                }
            }
        }
    }
};