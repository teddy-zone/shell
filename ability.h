#pragma once

#include <sstream>

#include "component.h"
#include "animation_system.h"
#include "lifetime_system.h"
#include "health_component.h"
#include "has_owner_component.h"
#include "on_cast_component.h"
#include "on_hit_component.h"
#include "projectile_component.h"
#include "decal_component.h"
#include "team_component.h"
#include "mesh.h"
#include "material.h"
#include "static_mesh_component.h"
#include "voice_component.h"

enum class AbilityState
{
    None,
    GroundTargeting,
    UnitTargeting,
    CastPoint,
    Channeling,
    Backswing
};

enum class TargetDecalType
{
    None,
    Circle,
    Cone,
    Strip
};

struct CompAbility : public Component
{
    float cooldown;
    std::optional<float> current_cooldown;
    float cast_point;
    float cast_range;
    float backswing;
    bool ground_targeted;
    bool unit_targeted;
    bool channeled;
    float channel_time;
    int level;
    int max_level;
    std::string ability_name;
    TargetDecalType target_decal_type;
    float radius;
    float angle;
};

struct CompAbilityLevel : public Component 
{
    int level;
};

struct CompAbilityInstance : public Component
{
    std::shared_ptr<EntityProto> proto;
    std::vector<EntityRef> instances;
};

struct CompRadiusApplication : public Component
{
    bool apply_to_same_team = false;
    bool apply_to_other_teams = true;
    bool apply_once;
    float radius;
    float tick_time;
    float last_tick_time = -1000;
    std::set<EntityId> applied_already;
    std::optional<DamageInstance> damage;
}; 

struct AbilityProto : public EntityProto
{
    TargetDecalType decal_type;
    AbilityProto(TargetDecalType in_decal_type, const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types),
        decal_type(in_decal_type)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbility>),
                    uint32_t(type_id<CompHasOwner>),
                    uint32_t(type_id<CompTeam>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
        entity.cmp<CompAbility>()->target_decal_type = decal_type;
    }
};

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

struct CrystalNovaInstanceProto : public ActorProto
{

    CrystalNovaInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompRadiusApplication>),
                    uint32_t(type_id<CompDecal>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("crystal_nova.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
        auto box_mat = std::make_shared<bgfx::Material>();

        std::ifstream t("C:\\Users\\tjwal\\projects\\ECS\\materials\\box_mat\\VertexShader.glsl");
        std::stringstream buffer;
        buffer << t.rdbuf();
        auto vshader = std::make_shared<Shader>(Shader::Type::Vertex, buffer.str(), true);
        std::ifstream t2("C:\\Users\\tjwal\\projects\\ECS\\materials\\box_mat\\FragmentShader.glsl");
        std::stringstream buffer2;
        buffer2 << t2.rdbuf();
        auto fshader = std::make_shared<Shader>(Shader::Type::Fragment, buffer2.str(), true);
        box_mat->set_vertex_shader(vshader);
        box_mat->set_fragment_shader(fshader);
        box_mat->link();
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompLifetime>()->lifetime = 0.5;

        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.2);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(10.0);

        entity.cmp<CompRadiusApplication>()->radius = 5;
        entity.cmp<CompRadiusApplication>()->tick_time = 100;
        entity.cmp<CompRadiusApplication>()->damage = {DamageType::Magical, 100, false};

        entity.cmp<CompDecal>()->decal.location = glm::vec4(pos, 1);
        entity.cmp<CompDecal>()->decal.color = glm::vec4(0.0,0.0,1.0,1.0);
        entity.cmp<CompDecal>()->decal.t = 0;
        entity.cmp<CompDecal>()->decal.radius = 5;
        entity.cmp<CompDecal>()->decal.type = 1;
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

    virtual void init(EntityRef entity) 
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
        entity.cmp<CompRadiusApplication>()->damage = {DamageType::Magical, 100, false};
    }
};

struct TargetingProto : public ActorProto
{
    TargetDecalType decal_type;
    TargetingProto(const glm::vec3& in_pos, TargetDecalType in_decal_type, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types),
        decal_type(in_decal_type)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompDecal>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
        entity.cmp<CompDecal>()->decal.location = glm::vec4(pos, 1);
        entity.cmp<CompDecal>()->decal.color = glm::vec4(0.0,0.0,1.0,1.0);
        entity.cmp<CompDecal>()->decal.t = 0;
        entity.cmp<CompDecal>()->decal.radius = 5;
        switch (decal_type)
        {
        case TargetDecalType::None:
            entity.cmp<CompDecal>()->decal.type = 0;
            break;
        case TargetDecalType::Circle:
            entity.cmp<CompDecal>()->decal.type = 3;
            break;
        case TargetDecalType::Cone:
            entity.cmp<CompDecal>()->decal.type = 4;
            break;
        case TargetDecalType::Strip:
            entity.cmp<CompDecal>()->decal.type = 5;
            break;
        default:
            entity.cmp<CompDecal>()->decal.type = 3;
        }
    }
};