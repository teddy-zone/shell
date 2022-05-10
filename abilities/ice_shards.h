#pragma once

#include "ability_proto.h"

struct AbilityIceShardsProto : AbilityProto
{
    AbilityIceShardsProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbilityInstance>),
                    uint32_t(type_id<CompOnCast>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
    }
};
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
        entity.cmp<CompDecal>()->decal.type = 3;
        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;

        Sound new_sound;
        new_sound.path = "C:\\Users\\tjwal\\OneDrive\\Documents\\REAPER Media\\beam.wav";
        new_sound.trigger = true;
        entity.cmp<CompVoice>()->sounds["beam"] = new_sound;


        entity.cmp<CompRadiusApplication>()->radius = 3;
        entity.cmp<CompRadiusApplication>()->tick_time = 0;
        entity.cmp<CompRadiusApplication>()->apply_once = true;
        entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 100, false};
    }
};
