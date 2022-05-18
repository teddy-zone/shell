#pragma once

#include <sstream>

#include "entity.h"
#include "system.h"
#include "level.h"
#include "physics_component.h"
#include "player_components.h"
#include "pickup_system.h"
#include "static_mesh_component.h"
#include "nav_component.h"
#include "nav_system.h"
#include "command_component.h"
#include "ability_set_component.h"
#include "status_manager.h"
#include "inventory.h"
#include "unit_prototypes.h"
#include "point_light_component.h"
#include "shop_proto.h"
#include "camera_component.h"
#include "blink_dagger.h"
#include "basic_enemy_ai_component.h"
#include "crystal_nova.h"
#include "ice_shards.h"

class BaseLevel : public Level
{
public:
    BaseLevel():
        Level("BaseLevel")
    {
    }

    virtual void level_init() override
    {


        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("suzanne.obj" );
        auto box_mat = std::make_shared<bgfx::Material>();
        auto c = _interface;

/*
        SpeedBoostAbilityProto speed_boost_proto;
        auto test_ability = c->add_entity_from_proto(&speed_boost_proto);
        test_ability.cmp<CompAbility>()->cast_point = 0.5;
        test_ability.cmp<CompAbility>()->backswing = 1.5;

        AbilityProto test_ability_proto2(TargetDecalType::Circle);
        auto test_ability2 = c->add_entity_from_proto(static_cast<EntityProto*>(&test_ability_proto2));
        test_ability2.cmp<CompAbility>()->cast_point = 1.5;
        test_ability2.cmp<CompAbility>()->backswing = 0.5;
        test_ability2.cmp<CompAbility>()->ground_targeted = true;
        test_ability2.cmp<CompAbility>()->unit_targeted = false;
        test_ability2.cmp<CompAbility>()->cooldown = 3.5;
        test_ability2.cmp<CompAbility>()->cast_range = 100;
        test_ability2.cmp<CompAbility>()->radius = 10;
        test_ability2.cmp<CompAbility>()->target_decal_type = TargetDecalType::Circle;
        test_ability2.cmp<CompTeam>()->team = 1;
        auto crystal_nova_proto = std::make_shared<CrystalNovaInstanceProto>(glm::vec3(0,0,0));
        c->add_component(CompAbilityInstance(), test_ability2.get_id());
        auto ab_inst = test_ability2.cmp<CompAbilityInstance>();
        auto cn_proto = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto);
        ab_inst->proto = cn_proto;

        AbilityProto test_ability_proto3(TargetDecalType::Cone);
        auto test_ability3 = c->add_entity_from_proto(static_cast<EntityProto*>(&test_ability_proto3));
        test_ability3.cmp<CompAbility>()->cast_point = 0.5;
        test_ability3.cmp<CompAbility>()->backswing = 0.5;
        test_ability3.cmp<CompAbility>()->ground_targeted = true;
        test_ability3.cmp<CompAbility>()->unit_targeted = false;
        test_ability3.cmp<CompAbility>()->cooldown = 3.5;
        test_ability3.cmp<CompAbility>()->cast_range = 100;
        test_ability3.cmp<CompAbility>()->radius = 2;
        test_ability3.cmp<CompAbility>()->target_decal_type = TargetDecalType::Cone;
        test_ability3.cmp<CompTeam>()->team = 1;
        auto crystal_nova_proto2 = std::make_shared<IceShardsInstanceProto>(glm::vec3(0,0,0));
        c->add_component(CompAbilityInstance(), test_ability3.get_id());
        auto ab_inst2 = test_ability3.cmp<CompAbilityInstance>();
        auto cn_proto2 = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto2);
        ab_inst2->proto = cn_proto2;
        */

        UnitProto unit_proto(glm::vec3(1.0));
        auto player = c->add_entity_from_proto(static_cast<EntityProto*>(&unit_proto));
        /*
        player.cmp<CompAbilitySet>()->abilities[0] = test_ability;
        player.cmp<CompAbilitySet>()->abilities[2] = test_ability2;
        player.cmp<CompAbilitySet>()->abilities[1] = test_ability3;
        AttackAbilityProto attack_ability_proto(player);
        player.cmp<CompAttacker>()->attack_ability = c->add_entity_from_proto(static_cast<EntityProto*>(&attack_ability_proto));
        */
        player.cmp<CompTeam>()->team = 1;
        c->add_component(CompPlayer(), player.get_id());
        c->add_component(CompRespawn(), player.get_id());
        player.cmp<CompRespawn>()->default_respawn_time = 5; 
    }

    virtual void update(double dt) override
    {
        auto time_comp = get_array<CompTime>()[0];
        if (time_comp.current_time > 20.0)
        {
            //_interface->unload_level("TestLevel");
        }
    }
};
