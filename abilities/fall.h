#pragma once

#include "ability_proto.h"

struct FallInstanceProto : public ActorProto
{

    FallInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompRadiusApplication>),
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompTeam>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("crystal_nova.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        entity.cmp<CompLifetime>()->lifetime = 2.0;
        
        entity.cmp<CompRadiusApplication>()->tick_time = 100;
        entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 80, false};

        entity.cmp<CompDecal>()->decal.location = glm::vec4(pos, 1);
        entity.cmp<CompDecal>()->decal.color = glm::vec4(0.0,0.0,1.0,1.0);
        entity.cmp<CompDecal>()->decal.t = 0;
        //entity.cmp<CompDecal>()->decal.radius = 5;
        entity.cmp<CompDecal>()->decal.type = 1;
        entity.set_name("crystal_nova");
    }
};

class FallAbilityProto : public AbilityProto 
{
public:
    FallAbilityProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompOnCast>),
                    uint32_t(type_id<CompAbilityInstance>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        entity.cmp<CompAbility>()->cast_point = 1.0;
        entity.cmp<CompAbility>()->backswing = .5;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->cooldown = 8;
        entity.cmp<CompAbility>()->cast_range = 100;
        entity.cmp<CompAbility>()->radius = 5;
        entity.cmp<CompAbility>()->target_decal_type = TargetDecalType::Cone;
        entity.cmp<CompAbility>()->max_level = 4;
        entity.cmp<CompAbility>()->ability_name = "Crush";
        entity.cmp<CompAbility>()->animation = "fall";
        entity.cmp<CompAbility>()->backswing_animation = "getup";
        entity.cmp<CompAbility>()->dynamic_cast_point = true;
        entity.cmp<CompAbility>()->dynamic_backswing = true;
        auto crystal_nova_proto = std::make_shared<CrystalNovaInstanceProto>(glm::vec3(0,0,0));
        auto ab_inst = entity.cmp<CompAbilityInstance>();
        auto cn_proto = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto);
        ab_inst->proto = cn_proto;
    }
};
