#pragma once

#include "component.h"
#include "ability_proto.h"

struct AttackInstanceProto : public ActorProto
{
    EntityRef _owner;
    AttackInstanceProto(EntityRef owner, const glm::vec3& in_pos=glm::vec3(0), const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types),
        _owner(owner)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompProjectile>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompOnHit>),
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
        entity.cmp<CompPosition>()->scale = glm::vec3(0.2);

        entity.cmp<CompProjectile>()->speed = 40;
        entity.cmp<CompProjectile>()->die_on_hit = true;

        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;

        DamageInstance attack_damage;
        attack_damage.is_attack = true;
        attack_damage.type = DamageType::Physical;
        attack_damage.damage = 10;
        attack_damage.applier = _owner;
        entity.cmp<CompOnHit>()->damage = attack_damage;

        auto sphere_mesh = std::make_shared<bgfx::Mesh>();
        sphere_mesh->load_obj("sphere.obj");
        sphere_mesh->set_solid_color(glm::vec4(0,0,0,1.0));
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(sphere_mesh);
        entity.cmp<CompPosition>()->scale = glm::vec3(0.1);
    }
};

struct AttackAbilityProto : public AbilityProto
{
    EntityRef _owner;
    AttackAbilityProto(EntityRef owner, const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types),
        _owner(owner)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbilityInstance>),
                    uint32_t(type_id<CompOnCast>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompAbilityInstance>()->proto = std::make_shared<AttackInstanceProto>(_owner);
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->cast_point = 0.1;
        entity.cmp<CompAbility>()->backswing = 0.1;
        entity.cmp<CompAbility>()->cooldown = 1;
        entity.cmp<CompAbility>()->unit_targeted = true;
    }
};

struct AttackSlashProto : public ActorProto
{
    EntityRef _owner;
    AttackSlashProto(const std::vector<CompType>& extension_types={}):
       ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompLifetime>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto sphere_mesh = std::make_shared<bgfx::Mesh>();
        sphere_mesh->load_obj("slash.obj");
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(sphere_mesh);
        entity.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec4(1,0,0,1));
        entity.cmp<CompPosition>()->scale = glm::vec3(0.2,0.2,0.2);
        entity.cmp<CompLifetime>()->lifetime = 0.3;
    }
};

struct MeleeAttackAbilityProto : public AbilityProto
{
    EntityRef _owner;
    MeleeAttackAbilityProto(EntityRef owner, const std::vector<CompType>& extension_types={}):
       AbilityProto(TargetDecalType::None, extension_types),
        _owner(owner)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompOnCast>),
                    uint32_t(type_id<CompAbilityInstance>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto slash_proto = std::make_shared<AttackSlashProto>(); 
        entity.cmp<CompAbilityInstance>()->proto = slash_proto;

        entity.cmp<CompAbility>()->cast_range = 3.0;
        entity.cmp<CompAbility>()->cast_point = 0.1;
        entity.cmp<CompAbility>()->backswing = 0.1;
        entity.cmp<CompAbility>()->cooldown = 1;
        entity.cmp<CompAbility>()->unit_targeted = true;

        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
                                 [](SystemInterface* iface, 
                                    EntityRef caster, 
                                    std::optional<glm::vec3> ground_target, 
                                    std::optional<EntityRef> unit_target, 
                                    std::optional<EntityRef> instance_entity)
                                    {
                                        DamageInstance attack_damage;
                                        attack_damage.is_attack = true;
                                        attack_damage.type = DamageType::Physical;
                                        attack_damage.damage = 10;
                                        attack_damage.applier = caster;
                                        if (auto* health_comp = unit_target.value().cmp<CompHealth>())
                                        {
                                            health_comp->apply_damage(attack_damage);
                                        }
                                        if (instance_entity)
                                        {
                                            auto* instance_pos = instance_entity.value().cmp<CompPosition>();
                                            auto move_dir = glm::normalize(unit_target.value().cmp<CompPosition>()->pos - caster.cmp<CompPosition>()->pos);
                                            auto move_angle = atan2(move_dir.y, move_dir.x);
                                            instance_pos->pos = caster.cmp<CompPosition>()->pos + move_dir*2.0;
                                            instance_pos->rot = glm::rotate(float(move_angle + 3.14159f/2 + 3.14159f), glm::vec3(0.0f,0.0f,1.0f));
                                        }
                                    });

    }
};

struct CompAttacker : public Component
{
    EntityRef attack_ability;
    bool attack_targeting = false;

    CompAbility* get_ability()
    {
        if (auto* out_ab = attack_ability.cmp<CompAbility>())
        {
            return out_ab;
        }
        else
        {
            throw std::runtime_error("Attack ability without ability!");
        }
    }
};
