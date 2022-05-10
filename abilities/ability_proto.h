#pragma once

#include "entity_prototype.h"

struct AbilityProto : public EntityProto
{
    TargetDecalType decal_type;
    AbilityProto(TargetDecalType in_decal_type, const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types),
        decal_type(in_decal_type)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbility>),
                    uint32_t(type_id<CompHasOwner>),
                    uint32_t(type_id<CompTeam>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override 
    {
        entity.cmp<CompAbility>()->target_decal_type = decal_type;
    }
};

