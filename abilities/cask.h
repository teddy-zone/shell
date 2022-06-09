#pragma once

#include "component.h"
#include "ability_proto.h"

struct CaskStatusProto : public EntityProto 
{
    CaskStatusProto(const std::vector<CompType>& extension_types={}):
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
        entity.cmp<CompStat>()->set_status_state(StatusState::Stunned, true);
        entity.cmp<CompStatus>()->duration = 5;
    }
};

struct CaskInstanceProto : public ActorProto
{
    CaskInstanceProto(const glm::vec3& in_pos=glm::vec3(0), const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompProjectile>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompOnHit>),
                    uint32_t(type_id<CompOnDestruction>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompStaticMesh>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override 
    {
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompPosition>()->scale = glm::vec3(0.8);

        CaskStatusProto status_proto;
        auto status_entity = iface->add_entity_from_proto(&status_proto);
        entity.cmp<CompProjectile>()->speed = 18;
        entity.cmp<CompProjectile>()->die_on_hit = true;

        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;

        auto sphere_mesh = std::make_shared<bgfx::Mesh>();
        sphere_mesh->load_obj("sphere.obj");
        sphere_mesh->set_solid_color(glm::vec4(0.3,0.6,0.2,1.0));
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(sphere_mesh);
        entity.cmp<CompPosition>()->scale = glm::vec3(0.1);
        entity.cmp<CompOnHit>()->on_hit_callbacks.push_back(
            [status_entity](SystemInterface* iface, EntityRef projectile, EntityRef hittee)
            {
                if (hittee.is_valid())
                {
                    if (auto* status_manager = hittee.cmp<CompStatusManager>())
                    {
                        auto* status_comp = status_entity.cmp<CompStatus>();
                        status_manager->apply_status(status_comp);
                    }
                }
            });
        entity.cmp<CompOnDestruction>()->on_destruction_callbacks.push_back(
            [status_entity](SystemInterface* iface, EntityRef me)
            {
                if (me.is_valid())
                {
                    if (auto* my_on_destruct = me.cmp<CompOnDestruction>())
                    {
                        auto cast_candidates = iface->data_within_sphere_selective(me.cmp<CompPosition>()->pos, 10, {uint32_t(type_id<CompStat>)});
                        for (auto& candidate : cast_candidates)
                        {
                            if (candidate.cmp<CompTeam>()->team == me.cmp<CompTeam>()->team || 
                                candidate == me.cmp<CompProjectile>()->homing_target.value())
                            {
                                continue;
                            }
                            if (my_on_destruct->generation < 4)
                            {
                                auto new_entity = iface->duplicate_entity(me);
                                new_entity.cmp<CompProjectile>()->homing_target = candidate;
                                if (auto* on_destruct = new_entity.cmp<CompOnDestruction>())
                                {
                                    on_destruct->generation += 1;
                                }
                            }
                            break;
                        }
                    }
                }
            });
    }
};

struct CaskAbilityProto : public AbilityProto
{
    CaskAbilityProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbilityInstance>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompAbilityInstance>()->proto = std::make_shared<CaskInstanceProto>();
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->cast_point = 0.1;
        entity.cmp<CompAbility>()->backswing = 0.1;
        entity.cmp<CompAbility>()->cooldown = 4;
        entity.cmp<CompAbility>()->max_level = 4;
        entity.cmp<CompAbility>()->ability_name = "Cask";
        entity.cmp<CompAbility>()->unit_targeted = true;
    }
};