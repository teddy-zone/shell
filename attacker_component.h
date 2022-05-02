#pragma once

#include "component.h"

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

    virtual void init(EntityRef entity) 
    {
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompPosition>()->scale = glm::vec3(0.2);

        entity.cmp<CompProjectile>()->speed = 40;
        entity.cmp<CompProjectile>()->die_on_hit = true;

        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;

        DamageInstance attack_damage;
        attack_damage.is_attack = true;
        attack_damage.is_attack = true;
        attack_damage.type = DamageType::Physical;
        attack_damage.damage = 50;
        attack_damage.applier = _owner;
        entity.cmp<CompOnHit>()->damage = attack_damage;

        auto sphere_mesh = std::make_shared<bgfx::Mesh>();
        sphere_mesh->load_obj("sphere.obj");
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(sphere_mesh);
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

    virtual void init(EntityRef entity) 
    {
        entity.cmp<CompAbilityInstance>()->proto = std::make_shared<AttackInstanceProto>(_owner);
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->cast_point = 0.1;
        entity.cmp<CompAbility>()->backswing = 0.1;
        entity.cmp<CompAbility>()->cooldown = 1;
        entity.cmp<CompAbility>()->unit_targeted = true;
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
