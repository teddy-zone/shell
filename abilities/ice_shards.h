#pragma once

#include "ability_proto.h"

struct IceShardsInstanceProto : public ActorProto
{

    IceShardsInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompProjectile>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompRadiusApplication>),
                    uint32_t(type_id<CompOnHit>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompHasOwner>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompLifetime>()->lifetime = 10;

        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.2);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(2.0);

        entity.cmp<CompProjectile>()->speed = 40;
        entity.cmp<CompProjectile>()->max_range = 20;
        entity.cmp<CompDecal>()->decal.type = 3;
        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;

        Sound new_sound;
        new_sound.path = "sounds/beam.wav";
        new_sound.trigger = true;
        entity.cmp<CompVoice>()->sounds["beam"] = new_sound;


        entity.cmp<CompRadiusApplication>()->radius = 3;
        entity.cmp<CompRadiusApplication>()->tick_time = 0;
        entity.cmp<CompRadiusApplication>()->apply_once = true;
        //entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 200, false};
        entity.cmp<CompRadiusApplication>()->owner_damage = {0};
    }
};

struct IceShardProto : public ActorProto
{

    IceShardProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompHasOwner>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompLifetime>()->lifetime = 10;
        entity.cmp<CompPhysics>()->has_collision = true;
        entity.cmp<CompPhysics>()->has_gravity = false;
        entity.cmp<CompBounds>()->is_static = true;

        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("cube.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 2);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
    }
};

struct AbilityIceShardsProto : AbilityProto
{
    AbilityIceShardsProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    { 
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbilityInstance>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        entity.cmp<CompAbility>()->cast_point = 0.5;
        entity.cmp<CompAbility>()->backswing = 0.5;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->cooldown = 3.5;
        entity.cmp<CompAbility>()->cast_range = 100;
        entity.cmp<CompAbility>()->radius = 3;
        entity.cmp<CompAbility>()->max_level = 4;
        entity.cmp<CompAbility>()->ability_name = "Ice Shards";
        entity.cmp<CompAbility>()->target_decal_type = TargetDecalType::Cone;
        entity.cmp<CompAbility>()->damages = {{entity, DamageType::Magical, 40, false}};
        //entity.cmp<CompHasOwner>()->owner = entity;
        auto crystal_nova_proto2 = std::make_shared<IceShardsInstanceProto>(glm::vec3(0,0,0));
        auto* ab_inst2 = entity.cmp<CompAbilityInstance>();
        auto cn_proto2 = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto2);
        ab_inst2->proto = crystal_nova_proto2;
    }
};