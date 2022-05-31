#pragma once

#include "entity_prototype.h"
#include "ability.h"
#include "animation_component.h"
#include "lifetime_system.h"
#include "health_component.h"
#include "has_owner_component.h"
#include "interactable_component.h"
#include "shell_level_component.h"
#include "team_component.h"
#include "caster_component.h"
#include "wallet_component.h"
#include "experience_component.h"
#include "attacker_component.h"
#include "attackable_component.h"
#include "bounty_component.h"
#include "speed_boost.h"
#include "crystal_nova.h"
#include "ice_shards.h"
#include "eye_component.h"
#include "vision_affected_component.h"
#include "respawn_component.h"
#include "mana_component.h"
#include "jump.h"
#include "dash.h"
#include "ability_mod.h"
#include "bladefury.h"
#include "ice_shards.h"
#include "basic_enemy_ai_component.h"
#include "materials/box_mat/VertexShader.glsl.h"
#include "materials/box_mat/FragmentShader.glsl.h"

struct UnitProto : public ActorProto
{

    UnitProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPhysics>), 
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompPickupper>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompNav>),
                    uint32_t(type_id<CompCommand>),
                    uint32_t(type_id<CompAbilitySet>),
                    uint32_t(type_id<CompStatusManager>),
                    uint32_t(type_id<CompInventory>),
                    uint32_t(type_id<CompCaster>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompHealth>),
                    uint32_t(type_id<CompMana>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompStat>),
                    uint32_t(type_id<CompExperience>),
                    uint32_t(type_id<CompWallet>),
                    uint32_t(type_id<CompAttacker>),
                    uint32_t(type_id<CompAttackable>),
                    uint32_t(type_id<CompBounty>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompEye>),
                    uint32_t(type_id<CompVisionAffected>),
                    uint32_t(type_id<CompAbilityMod>),
                    uint32_t(type_id<CompAttachment>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("suzanne.obj" );
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
        auto box_mat = std::make_shared<bgfx::Material>();

        entity.cmp<CompEye>()->vision_range = 50;

        auto vshader = std::make_shared<Shader>(Shader::Type::Vertex, box_vertex_shader, true);
        auto fshader = std::make_shared<Shader>(Shader::Type::Fragment, box_fragment_shader, true);
        box_mat->set_vertex_shader(vshader);
        box_mat->set_fragment_shader(fshader);
        box_mat->link();
        entity.cmp<CompStaticMesh>()->mesh.set_material(box_mat);
        entity.cmp<CompStaticMesh>()->mesh.set_id(entity.get_id());
        entity.cmp<CompPosition>()->pos = glm::vec3(20,20,45);
        entity.cmp<CompHealth>()->health_percentage = 100;
        entity.cmp<CompMana>()->mana_percentage = 100;
        entity.cmp<CompStat>()->set_stat(Stat::MaxHealth, 200);
        entity.cmp<CompStat>()->set_stat(Stat::MaxMana, 200);
        entity.cmp<CompStat>()->set_stat(Stat::MagicResist, 0.25);
        entity.cmp<CompStat>()->set_stat(Stat::Armor, 2);
        entity.cmp<CompStat>()->set_stat(Stat::Movespeed, 150);
        entity.cmp<CompStat>()->set_stat_mult(Stat::Lifesteal, 1);
        entity.cmp<CompStat>()->set_stat(Stat::Lifesteal, 0);
        entity.cmp<CompInventory>()->visible = true;
        entity.cmp<CompWallet>()->balance = 2200;
        entity.cmp<CompExperience>()->experience = 0;
        entity.cmp<CompBounty>()->money_bounty = 200;
        entity.cmp<CompBounty>()->exp_bounty = 300;

/*
        JumpAbilityProto speed_boost_proto;
        auto test_ability = iface->add_entity_from_proto(&speed_boost_proto);
        test_ability.cmp<CompAbility>()->cast_point = 0.5;
        test_ability.cmp<CompAbility>()->backswing = 1.5;
        test_ability.cmp<CompAbility>()->max_level = 4;

        DashAbilityProto dash_proto;
        auto dash_ability = iface->add_entity_from_proto(&dash_proto);
        dash_ability.cmp<CompAbility>()->cast_point = 0.5;
        dash_ability.cmp<CompAbility>()->backswing = 1.5;
        dash_ability.cmp<CompAbility>()->max_level = 4;

        BladefuryAbilityProto bf_proto;
        auto bf_ability = iface->add_entity_from_proto(&bf_proto);
        bf_ability.cmp<CompAbility>()->cast_point = 0.0;
        bf_ability.cmp<CompAbility>()->backswing = 0.0;
        bf_ability.cmp<CompAbility>()->max_level = 4;

        AbilityProto test_ability_proto2(TargetDecalType::Circle);
        auto test_ability2 = iface->add_entity_from_proto(static_cast<EntityProto*>(&test_ability_proto2));
        test_ability2.cmp<CompAbility>()->cast_point = 1.5;
        test_ability2.cmp<CompAbility>()->backswing = 0.5;
        test_ability2.cmp<CompAbility>()->ground_targeted = true;
        test_ability2.cmp<CompAbility>()->unit_targeted = false;
        test_ability2.cmp<CompAbility>()->cooldown = 3.5;
        test_ability2.cmp<CompAbility>()->cast_range = 100;
        test_ability2.cmp<CompAbility>()->radius = 10;
        test_ability2.cmp<CompAbility>()->target_decal_type = TargetDecalType::Circle;
        test_ability2.cmp<CompAbility>()->max_level = 4;
        test_ability2.cmp<CompTeam>()->team = 1;
        auto crystal_nova_proto = std::make_shared<CrystalNovaInstanceProto>(glm::vec3(0,0,0));
        iface->add_component(CompAbilityInstance(), test_ability2.get_id());
        auto ab_inst = test_ability2.cmp<CompAbilityInstance>();
        auto cn_proto = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto);
        ab_inst->proto = cn_proto;

        AbilityProto test_ability_proto3(TargetDecalType::Cone);
        auto test_ability3 = iface->add_entity_from_proto(static_cast<EntityProto*>(&test_ability_proto3));
        test_ability3.cmp<CompAbility>()->cast_point = 0.5;
        test_ability3.cmp<CompAbility>()->backswing = 0.5;
        test_ability3.cmp<CompAbility>()->ground_targeted = true;
        test_ability3.cmp<CompAbility>()->unit_targeted = false;
        test_ability3.cmp<CompAbility>()->cooldown = 3.5;
        test_ability3.cmp<CompAbility>()->cast_range = 100;
        test_ability3.cmp<CompAbility>()->radius = 2;
        test_ability3.cmp<CompAbility>()->max_level = 4;
        test_ability3.cmp<CompAbility>()->ability_name = "Ice Shards";
        test_ability3.cmp<CompAbility>()->target_decal_type = TargetDecalType::Cone;
        test_ability3.cmp<CompAbility>()->damages = {{test_ability3, DamageType::Magical, 200, false}};
        test_ability3.cmp<CompTeam>()->team = 1;
        test_ability3.cmp<CompHasOwner>()->owner = entity;
        auto crystal_nova_proto2 = std::make_shared<IceShardsInstanceProto>(glm::vec3(0,0,0));
        iface->add_component(CompAbilityInstance(), test_ability3.get_id());
        auto ab_inst2 = test_ability3.cmp<CompAbilityInstance>();
        auto cn_proto2 = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto2);
        ab_inst2->proto = cn_proto2;

        //entity.cmp<CompAbilitySet>()->abilities[0] = bf_ability;
        //entity.cmp<CompAbilitySet>()->abilities[2] = dash_ability;
        //entity.cmp<CompAbilitySet>()->abilities[1] = test_ability3;
        */
        MeleeAttackAbilityProto attack_ability_proto(entity);
        entity.cmp<CompAttacker>()->attack_ability = iface->add_entity_from_proto(static_cast<EntityProto*>(&attack_ability_proto));
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->level = 1;
        
        //entity.cmp<CompRespawn>()->default_respawn_time = 5;
    }
};


struct CommandIndicatorProto : public ActorProto
{

    CommandIndicatorProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("command_indicator.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
        
        auto box_mat = std::make_shared<bgfx::Material>();

        /*
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

        entity.cmp<CompStaticMesh>()->mesh.set_material(box_mat);
        */
        entity.cmp<CompStaticMesh>()->mesh.set_id(entity.get_id());
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompLifetime>()->lifetime = 0.5;

        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.7);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(0.0);
    }
};

struct JuggernautProto : public UnitProto
{

    JuggernautProto(const std::vector<CompType>& extension_types={}):
        UnitProto(glm::vec3(0), extension_types)
    {
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        UnitProto::init(entity, iface);
        entity.cmp<CompTeam>()->team = 2;
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->cast_range = 3.0;
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->level = 1;
        //entity.cmp<CompStat>()->set_stat(Stat::HealthRegen, 10);
        //entity.cmp<CompStat>()->set_stat(Stat::ManaRegen, 10);
        auto bf_proto = std::make_shared<BladefuryAbilityProto>();
        entity.cmp<CompAbilitySet>()->abilities[0] = iface->add_entity_from_proto(bf_proto.get());
        entity.set_name("Juggernaut" + std::to_string(entity.get_id()));
    }
};

struct CrystalMaidenProto : public UnitProto
{

    CrystalMaidenProto(const std::vector<CompType>& extension_types={}):
        UnitProto(glm::vec3(0), extension_types)
    {
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        UnitProto::init(entity, iface);
        entity.cmp<CompTeam>()->team = 2;
        auto ranged_attack_proto = std::make_shared<AttackAbilityProto>(entity);
        entity.cmp<CompAttacker>()->attack_ability = iface->add_entity_from_proto(ranged_attack_proto.get());
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->cast_range = 15;
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->level = 1;
        auto cn_proto = std::make_shared<CrystalNovaAbilityProto>();
        entity.cmp<CompAbilitySet>()->abilities[0] = iface->add_entity_from_proto(cn_proto.get());
        entity.set_name("CrystalMaiden" + std::to_string(entity.get_id()));
    }
};

struct TuskProto : public UnitProto
{

    TuskProto(const std::vector<CompType>& extension_types={}):
        UnitProto(glm::vec3(0), extension_types)
    {
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        UnitProto::init(entity, iface);
        entity.cmp<CompTeam>()->team = 2;
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->cast_range = 3.0;
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->level = 1;
        auto cn_proto = std::make_shared<AbilityIceShardsProto>();
        entity.cmp<CompAbilitySet>()->abilities[0] = iface->add_entity_from_proto(cn_proto.get());
        entity.cmp<CompAbilitySet>()->abilities[0].cmp<CompHasOwner>()->owner = entity;
        entity.set_name("Tusk" + std::to_string(entity.get_id()));
    }
};

struct EnemyUnitProto : public UnitProto
{

    EnemyUnitProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        UnitProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
            uint32_t(type_id<CompBasicEnemyAI>),           
        }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        UnitProto::init(entity, iface);
        entity.cmp<CompTeam>()->team = 2;
        entity.cmp<CompBasicEnemyAI>()->vision_range = 15;
        entity.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->cast_range = 3.0;
        auto cn_proto = std::make_shared<CrystalNovaAbilityProto>();
        entity.cmp<CompAbilitySet>()->abilities[0] = iface->add_entity_from_proto(cn_proto.get());
        entity.cmp<CompAbilitySet>()->abilities[0].cmp<CompAbility>()->level = 1;
        auto is_proto = std::make_shared<AbilityIceShardsProto>();
        entity.cmp<CompAbilitySet>()->abilities[1] = iface->add_entity_from_proto(is_proto.get());
        entity.cmp<CompAbilitySet>()->abilities[1].cmp<CompAbility>()->level = 1;
        entity.set_name("Enemy" + std::to_string(entity.get_id()));
    }
};
