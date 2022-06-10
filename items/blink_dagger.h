#pragma once

#include "item_proto.h"
#include "on_cast_component.h"


struct BlinkDaggerProto : public ItemProto
{
    BlinkDaggerProto(const std::vector<CompType>& extension_types={}):
        ItemProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbility>),
                    uint32_t(type_id<CompOnCast>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompAbility>()->cast_point = 0.5;
        entity.cmp<CompAbility>()->backswing = 0.5;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->cooldown = 12.0;
        entity.cmp<CompAbility>()->cast_range = 100;
        entity.cmp<CompAbility>()->radius = 10;
        entity.cmp<CompAbility>()->level = 1;
        entity.cmp<CompAbility>()->max_level = 1;
        entity.cmp<CompAbility>()->target_decal_type = TargetDecalType::Circle;
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back([] 
            (SystemInterface* iface, EntityRef casting_entity, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
            {
                if (auto* caster_pos = casting_entity.cmp<CompPosition>())
                {
                    caster_pos->pos = ground_target.value() + glm::vec3(0,0,2);
                }
            });
    }
};
