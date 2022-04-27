#pragma once

#include "entity_prototype.h"


struct ItemProto : public EntityProto
{
    ItemProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompHasOwner>),
                    uint32_t(type_id<CompItem>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
    }
};
