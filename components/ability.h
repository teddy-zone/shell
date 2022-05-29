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
#include "item_component.h"
#include "on_intersect_component.h"
#include "on_hit_component.h"
#include "on_destruction_component.h"

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
    bool self_targeted;
    bool channeled;
    float channel_time;
    int level;
    int max_level;
    int mana_cost;
    std::string ability_name;
    TargetDecalType target_decal_type;
    float radius;
    float angle;
    std::vector<DamageInstance> damages;
};

struct CompAbilityLevel : public Component 
{
    int level;
};


struct OwnerDamageSpec
{
    int damage_index;
};

struct CompRadiusApplication : public Component
{
    bool apply_to_same_team = false;
    bool apply_to_other_teams = true;
    bool apply_once;
    float radius;
    float tick_time = 0;
    float last_tick_time = -1000;
    std::set<EntityId> applied_already;
    std::optional<DamageInstance> damage;
    std::optional<OwnerDamageSpec> owner_damage;
}; 

struct CompAbilityInstance : public Component
{
    std::shared_ptr<EntityProto> proto;
    std::vector<EntityRef> instances;
    std::optional<CompOnIntersect> intersect_callback;
    std::optional<CompOnHit> hit_callback;
    std::optional<CompRadiusApplication> radial_application;
    std::optional<CompOnDestruction> destruction_callback;
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

    virtual void init(EntityRef entity, SystemInterface* iface) override
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
