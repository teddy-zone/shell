#pragma once

#include "ability_proto.h"
#include "attachment_component.h"

struct BladefuryStatusProto : public EntityProto 
{
    BladefuryStatusProto(const std::vector<CompType>& extension_types={}):
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

struct BladefuryInstanceProto : public EntityProto 
{
    BladefuryInstanceProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompHasOwner>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompRadiusApplication>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 90/10.0, false};
        entity.cmp<CompRadiusApplication>()->tick_time = 0.1;
        entity.cmp<CompDecal>()->decal.type = 3;
        entity.cmp<CompLifetime>()->lifetime = 5;
        printf("INitialized proje!\n");
    }
};

class BladefuryAbilityProto : public AbilityProto 
{
public:
    BladefuryAbilityProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompOnCast>),
                    uint32_t(type_id<CompAbilityInstance>),
                    uint32_t(type_id<CompTeam>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        BladefuryStatusProto status_proto;
        auto status_entity = iface->add_entity_from_proto(&status_proto);
        auto projectile_proto = std::make_shared<BladefuryInstanceProto>();
        entity.cmp<CompAbilityInstance>()->proto = projectile_proto;
        entity.cmp<CompAbility>()->cooldown = 10;
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->self_targeted = true;
        entity.cmp<CompAbility>()->ground_targeted = false;
        entity.cmp<CompAbility>()->ability_name = "Bladefury";
        entity.cmp<CompAbility>()->radius = 7;
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [status_entity](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
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
                    caster.cmp<CompAttachment>()->attached_entities.push_back(instance_entity.value());

                }
            }
        );
    }
};