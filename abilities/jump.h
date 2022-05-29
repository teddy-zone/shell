#pragma once

#include "ability_proto.h"
#include "physics_component.h"

class JumpAbilityProto : public AbilityProto 
{
public:
    JumpAbilityProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompOnCast>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        entity.cmp<CompAbility>()->cooldown = 5;
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->self_targeted = false;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->ability_name = "Jump";
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [iface](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
            {
                if (ground_target)
                {
                    auto* caster_pos = caster.cmp<CompPosition>();
                    glm::vec3 dir = glm::normalize(ground_target.value() - caster_pos->pos);
                    if (auto* phys_comp = caster.cmp<CompPhysics>())
                    {
                        if (phys_comp->on_ground)
                        {
                            phys_comp->vel += 20*glm::normalize((dir + glm::vec3(0,0,1)));
                        }
                    }
                }
            }
        );
    }
};