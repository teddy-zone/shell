#pragma once

#include "entity_prototype.h"

struct FootstepProto : public ActorProto
{

    FootstepProto(const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompLifetime>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompDecal>()->decal.type = 6;
        entity.cmp<CompDecal>()->decal.radius = 0.175;
        entity.cmp<CompLifetime>()->lifetime = 2.0;
    }
};
