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
                    uint32_t(type_id<CompHasOwner>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompOnCast>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompProjectile>()->speed = 200;
        entity.cmp<CompProjectile>()->constant_height = 2.0;
        entity.cmp<CompLifetime>()->lifetime = 1.2;
        entity.cmp<CompPhysics>()->has_collision = false;

        auto dash_mesh = std::make_shared<bgfx::Mesh>();
        dash_mesh->load_obj("dash.obj");
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(dash_mesh);
        entity.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec4(1, 1, 1, 1));

        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [](SystemInterface* iface,
                EntityRef caster,
                std::optional<glm::vec3> ground_target,
                std::optional<EntityRef> unit_target,
                std::optional<EntityRef> instance_entity)
            {
                if (instance_entity)
                {
                    
                }
            });
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
                    uint32_t(type_id<CompAbilityInstance>),
                    uint32_t(type_id<CompTeam>),
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
        entity.cmp<CompAbility>()->radius = 5;
        entity.cmp<CompAbility>()->animation = "idle";
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [status_entity](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
            {
                if (unit_target)
                {
                    if (auto* status_manager = unit_target->cmp<CompStatusManager>())
                    {
                        auto* status_comp = status_entity.cmp<CompStatus>();
                        status_manager->apply_status(status_comp);
                    }
                }
                if (instance_entity)
                {
                    instance_entity.value().cmp<CompAttachment>()->attached_entities.push_back(caster);

                    auto* instance_pos = instance_entity.value().cmp<CompPosition>();
                    auto move_dir = glm::normalize(ground_target.value() - caster.cmp<CompPosition>()->pos);
                    auto move_angle = atan2(move_dir.y, move_dir.x);
                    instance_pos->rot = glm::rotate(float(move_angle + 3.14159f), glm::vec3(0.0f, 0.0f, 1.0f));
                }
            }
        );
    }
};
