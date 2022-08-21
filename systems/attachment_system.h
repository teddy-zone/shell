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
            std::vector<EntityRef> to_remove;
            if (auto* my_pos_comp = attachment_component.sibling<CompPosition>())
            {
                size_t entity_index = 0;
                if (attachment_component.type == AttachmentType::Attachee)
                {
                    for (auto& attached_entity : attachment_component.attached_entities)
                    {
                        if (attached_entity.is_valid())
                        {
                            if (auto* other_pos_comp = attached_entity.cmp<CompPosition>())
                            {
                                other_pos_comp->pos = my_pos_comp->pos + attachment_component.position_offset;
                            }
                        }
                        else
                        {
                            to_remove.push_back(attached_entity);
                        }
                        entity_index++;
                    }
                }
                else
                {
                    for (auto& attached_entity : attachment_component.attached_entities)
                    {
                        if (attached_entity.is_valid())
                        {
                            if (auto* other_pos_comp = attached_entity.cmp<CompPosition>())
                            {
                                //my_pos_comp->pos = other_pos_comp->pos + attachment_component.position_offset;
                                my_pos_comp->pos = other_pos_comp->relative_to_global(attachment_component.position_offset);
                            }
                        }
                        else
                        {
                            to_remove.push_back(attached_entity);
                        }
                        break;
                    }
                }
            }
            for (auto& tor : to_remove)
            {
                for (int i = 0; i < attachment_component.attached_entities.size(); ++i)
                {
                    if (attachment_component.attached_entities[i].get_id() == tor.get_id())
                    {
                        attachment_component.attached_entities[i] = attachment_component.attached_entities.back();
                        attachment_component.attached_entities.resize(attachment_component.attached_entities.size() - 1);
                    }
                }
            }
        }
    }
};