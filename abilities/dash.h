#pragma once

#include "ability_proto.h"

struct DashStatusProto : public EntityProto 
{
    DashStatusProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStat>),
                    uint32_t(type_id<CompStatus>),
                    uint32_t(type_id<CompHasOwner>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompStat>()->set_stat_mult(Stat::Movespeed, 2);
        entity.cmp<CompStatus>()->duration = 0.5;
    }
};

class DashAbilityProto : public AbilityProto 
{
public:
    DashAbilityProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompOnCast>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        DashStatusProto status_proto;
        auto status_entity = iface->add_entity_from_proto(&status_proto);
        entity.cmp<CompAbility>()->cooldown = 5;
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->self_targeted = false;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->ability_name = "Dash";
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [status_entity](EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target)
            {
                if (unit_target)
                {
                    if (auto* status_manager = unit_target->cmp<CompStatusManager>())
                    {
                        auto* status_comp = status_entity.cmp<CompStatus>();
                        printf("Applying status\n");
                        status_manager->apply_status(status_comp);
                    }
                }
            }
        );
    }
};