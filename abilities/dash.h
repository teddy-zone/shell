#pragma once

#include "ability_proto.h"
#include "attachment_component.h"

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

struct DashProjectileProto : public EntityProto 
{
    DashProjectileProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompProjectile>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAttachment>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompProjectile>()->speed = 200;
        entity.cmp<CompProjectile>()->constant_height = 1.0;
        entity.cmp<CompLifetime>()->lifetime = 0.2;
        entity.cmp<CompPhysics>()->has_collision = false;
        printf("INitialized proje!\n");
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
                    uint32_t(type_id<CompAbilityInstance>)
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        DashStatusProto status_proto;
        auto status_entity = iface->add_entity_from_proto(&status_proto);
        auto projectile_proto = std::make_shared<DashProjectileProto>();
        entity.cmp<CompAbilityInstance>()->proto = projectile_proto;
        entity.cmp<CompAbility>()->cooldown = 5;
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->self_targeted = false;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->ability_name = "Dash";
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [status_entity](EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
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
                if (instance_entity)
                {
                    instance_entity.value().cmp<CompAttachment>()->attached_entities.push_back(caster);
                }
            }
        );
    }
};