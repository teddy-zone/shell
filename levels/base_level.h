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
#include "character_type_component.h"
#include "skeletal_mesh_component.h"
#include "hud_control_component.h"

class BaseLevel : public Level
{
public:
    BaseLevel():
        Level("BaseLevel")
    {
    }

    virtual void level_init() override
    {

        auto& hud_control = get_array<CompHudControl>()[0];
        hud_control.hud_enabled = true;

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
        auto& char_type_components = get_array<CompCharacterType>();
        if (char_type_components.size())
        {
            auto player = c->add_entity_from_proto(char_type_components[0].type_proto.get());
            player.cmp<CompTeam>()->team = 1;
            c->add_component(CompPlayer(), player.get_id());
            c->add_component(CompRespawn(), player.get_id());
            player.cmp<CompRespawn>()->default_respawn_time = 5; 
            auto& selected_objects = get_array<CompSelectedObjects>();
            for (auto& comp_selected_objects : selected_objects)
            {
                comp_selected_objects.selected_objects.resize(1);
                comp_selected_objects.selected_objects[0] = player;
            }

            //auto skeleton_proto = std::make_shared<SkeletonProto>();
            //auto skeleton_entity = c->add_entity_from_proto(skeleton_proto.get());
            //skeleton_entity.cmp<CompAttachment>()->attached_entities.push_back(player);
        }
        auto& camera = get_array<CompCamera>()[0];
        camera.graphics_camera.set_position(glm::vec3(50.0f, 50, 50));
        camera.graphics_camera.set_look_target(glm::vec3(38,38,25));

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
