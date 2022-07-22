#pragma once

#include <sstream>

#include "entity.h"
#include "system.h"
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
#include "vladmirs_offering.h"
#include "basic_enemy_ai_component.h"
#include "level.h"
#include "unit_prototypes.h"
#include "teleport_proto.h"
#include "materials/line_mat/FragmentShader.glsl.h"
#include "materials/line_mat/VertexShader.glsl.h"
#include "spawner_proto.h"
#include "ability_mod_station_proto.h"
#include "ability_draft_station_proto.h"
#include "spotlight_component.h"

class TestLevel : public Level
{
public:
    TestLevel():
        Level("TestLevel")
    {
    }

    virtual void level_init() override
    {
        Sound music_sound;
        music_sound.path = "sounds\\main_theme.wav";
        music_sound.loop = true;
        music_sound.trigger = true;
        music_sound.range = 100;

        auto& player_array = get_array<CompPlayer>();

        if (player_array.size())
        {
            auto& player1 = get_array<CompPlayer>()[0];
            player1.sibling<CompPosition>()->pos = glm::vec3(30, 135, 20);
            get_array<CompCamera>()[0].set_look_target(player1.sibling<CompPosition>()->pos, true);
        }

        EntityRef noodle_stand = _interface->add_entity_with_components({ uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompSpotlight>),
                    uint32_t(type_id<CompPickupee>)
            });
        auto noodle_stand_mesh = std::make_shared<bgfx::Mesh>();
        noodle_stand_mesh->load_obj("noodle_shop.obj", false);
        auto* noodle_mesh = noodle_stand.cmp<CompStaticMesh>();
        noodle_mesh->mesh.set_mesh(noodle_stand_mesh);
        noodle_mesh->mesh.set_id(-1);
        noodle_stand.cmp<CompVoice>()->sounds["music"] = music_sound;
        noodle_stand.set_name("NoodleStand");// +std::to_string(noodle_stand.get_id()));
        noodle_stand.cmp<CompPosition>()->pos = glm::vec3(100, 100, 17.5);
        noodle_stand.cmp<CompPosition>()->rot = glm::rotate(float(3.14159f/3), glm::vec3(0.0f,0.0f,1.0f));
        noodle_stand.cmp<CompSpotlight>()->light.direction = glm::normalize(glm::vec4(0, -1, -0.1, 0.0));
        noodle_stand.cmp<CompSpotlight>()->light.location = glm::vec4(-2, 0, 5, 1.0);
        noodle_stand.cmp<CompSpotlight>()->light.intensity = 0.02;
        noodle_stand.cmp<CompSpotlight>()->light.type = 0;
        noodle_stand.cmp<CompSpotlight>()->light.size = glm::vec4(3.1415926/4, 3.14159/8, 0, 0);
        noodle_stand.cmp<CompSpotlight>()->light.color = glm::vec4(1.0, 0.8, 0.2, 1.0);
        //auto noodle_tri_oct_comp = octree::vector_to_octree(noodle_mesh->mesh.get_mesh()->_octree_vertices, noodle_mesh->mesh.get_mesh()->_bmin, noodle_mesh->mesh.get_mesh()->_bmax, noodle_stand.cmp<CompPosition>()->rot);
        //noodle_mesh->tri_octree = noodle_tri_oct_comp;
        auto temp_bounds = noodle_mesh->mesh.get_mesh()->_bmax - noodle_mesh->mesh.get_mesh()->_bmin;

        noodle_stand.cmp<CompBounds>()->bounds = temp_bounds;
        noodle_stand.cmp<CompBounds>()->is_static = true;
        UnitProto unit_proto(glm::vec3(1.0));

        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("suzanne.obj" );
        auto box_mat = std::make_shared<bgfx::Material>();
        auto c = _interface;

        LightEntityProto light_proto(glm::vec3(100,100,60));
        auto light_entity = c->add_entity_from_proto(&light_proto);
        light_entity.cmp<CompPointLight>()->light.intensity = 0.05;
        light_entity.set_name("Light1");

        LightEntityProto light_proto2(glm::vec3(40, 40, 40));
        auto light_entity2 = c->add_entity_from_proto(&light_proto2);
        light_entity2.cmp<CompPointLight>()->light.intensity = 0.03;
        light_entity2.set_name("Light2");

        LightEntityProto light_proto3(glm::vec3(160, 160, 40));
        auto light_entity3 = c->add_entity_from_proto(&light_proto3);
        light_entity3.cmp<CompPointLight>()->light.intensity = 0.03;
        light_entity3.cmp<CompPointLight>()->light.color = glm::vec4(0.9,0.4,0.3,1.0);
        light_entity3.set_name("Light3");

        LightEntityProto light_proto4(glm::vec3(106, 125, 25));
        auto light_entity4 = c->add_entity_from_proto(&light_proto4);
        light_entity4.cmp<CompPointLight>()->light.intensity = 0.01;
        light_entity4.cmp<CompPointLight>()->light.color = glm::vec4(0.2,0.8,0.0,1.0);
        light_entity4.set_name("Light4");

        /*
        SpotlightEntityProto light_proto5(glm::vec3(100, 100, 50));
        auto light_entity5 = c->add_entity_from_proto(&light_proto5);3
        light_entity5.cmp<CompSpotlight>()->light.color = glm::vec4(1.0, 0.9, 0.7, 1.0);
        light_entity5.cmp<CompSpotlight>()->light.visible = 1;
        light_entity5.cmp<CompSpotlight>()->light.intensity = 1.0;
        */

        //LightEntityProto light_proto2(glm::vec3(150, 10, 50));
        //auto light_entity2 = c->add_entity_from_proto(&light_proto2);

        ShopProto shop_proto(glm::vec3(1,1,40));
        auto shopkeeper = c->add_entity_from_proto(static_cast<EntityProto*>(&shop_proto));
        shopkeeper.cmp<CompShopInventory>()->camera = &get_array<CompCamera>()[0].graphics_camera;
        {
            BlinkDaggerProto blink_dagger_proto;
            auto blink_dagger_entity = c->add_entity_from_proto(&blink_dagger_proto);
            InventorySlot blink_dagger_slot;
            blink_dagger_slot.cost = 2150;
            blink_dagger_slot.item_entity = blink_dagger_entity;
            blink_dagger_entity.cmp<CompItem>()->name = "Blink Dagger";
            shopkeeper.cmp<CompShopInventory>()->slots.push_back(blink_dagger_slot);
        }
        {
            VladmirProto vladmir_proto;
            auto vladmir_entity = c->add_entity_from_proto(&vladmir_proto);
            InventorySlot vladmir_slot;
            vladmir_slot.cost = 1500;
            vladmir_slot.item_entity = vladmir_entity;
            vladmir_entity.cmp<CompItem>()->name = "Vladmir's Offering";
            shopkeeper.cmp<CompShopInventory>()->slots.push_back(vladmir_slot);
        }
        shopkeeper.cmp<CompInteractable>()->interaction_callback = [](SystemInterface* sys_interface, EntityRef interactor, EntityRef interactee)
            {
                if (auto* inventory = interactee.cmp<CompShopInventory>())
                {
                    inventory->visible = true;
                }
            };
        shopkeeper.cmp<CompPosition>()->pos = glm::vec3(40, 125, 40);

        EntityRef player2 = c->add_entity_with_components({ uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompPickupee>)
            });
        auto* bounds = player2.cmp<CompBounds>();
        auto* pos = player2.cmp<CompPosition>();

        auto landscape_mesh = std::make_shared<bgfx::Mesh>();
        landscape_mesh->load_obj("pool.obj", true);
        //landscape_mesh->set_solid_color_by_hex(0x46835D);
        
        //landscape_mesh->set_solid_color_by_hex(0xD00000);
        //landscape_mesh->set_solid_color(glm::vec3(0,1,0));
        auto* lmesh = player2.cmp<CompStaticMesh>();
        lmesh->mesh.set_mesh(landscape_mesh);
        lmesh->mesh.set_id(-1);
        player2.cmp<CompVoice>()->sounds["music"] = music_sound;
        lmesh->mesh.set_material(box_mat);
        player2.set_name("LevelMesh" + std::to_string(player2.get_id()));


        
        //lmesh->mesh.set_scale(glm::vec3(5, 5, 1.0));
        auto tri_oct_comp = octree::vector_to_octree(lmesh->mesh.get_mesh()->_octree_vertices, lmesh->mesh.get_mesh()->_bmin, lmesh->mesh.get_mesh()->_bmax, glm::mat3(1));
        lmesh->tri_octree = tri_oct_comp;

        bounds->is_static = true;

        bounds->set_bounds(lmesh->mesh.get_mesh()->_bmax - lmesh->mesh.get_mesh()->_bmin);
        bounds->insert_size = 5.0;
        pos->pos = glm::vec3(1,1,1);

        /*
        EntityRef ground = c->add_entity_with_components({ uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
            });
        bounds = ground.cmp<CompBounds>();
        pos = ground.cmp<CompPosition>();

        auto ground_mesh = std::make_shared<bgfx::Mesh>();
        ground_mesh->load_obj("ground.obj", true);
        ground_mesh->set_solid_color_by_hex(0x664900);
        //landscape_mesh->set_solid_color_by_hex(0xD00000);
        //landscape_mesh->set_solid_color(glm::vec3(0,1,0));
        lmesh = ground.cmp<CompStaticMesh>();
        lmesh->mesh.set_mesh(ground_mesh);
        lmesh->mesh.set_id(-1);
        ground.set_name("Ground" + std::to_string(ground.get_id()));
        auto ground_tri_oct_comp = octree::vector_to_octree(lmesh->mesh.get_mesh()->_octree_vertices, lmesh->mesh.get_mesh()->_bmin, lmesh->mesh.get_mesh()->_bmax);
        lmesh->tri_octree = ground_tri_oct_comp;
        bounds->is_static = true;
        bounds->set_bounds(lmesh->mesh.get_mesh()->_bmax - lmesh->mesh.get_mesh()->_bmin);
        bounds->insert_size = 5.0;
        pos->pos = glm::vec3(1,1,8);
        */

        
        for (int i = 0; i < 0; ++i)
        {
            auto enemy_playerr = c->add_entity_from_proto(static_cast<EntityProto*>(&unit_proto));
            enemy_playerr.cmp<CompPosition>()->pos = glm::vec3(zerotoone_level()* 100.0f, zerotoone_level()* 100.0f, (zerotoone_level()*0.5 + 0.5)*100.0f) + glm::vec3(2.0, 2.0,0);
            enemy_playerr.cmp<CompTeam>()->team = 2;
            CompBasicEnemyAI ai_comp;
            ai_comp.vision_range = 20;
            c->add_component(ai_comp, enemy_playerr.get_id());
            //AttackAbilityProto attack_ability_proto(enemy_playerr);
            //enemy_playerr.cmp<CompAttacker>()->attack_ability = c->add_entity_from_proto(static_cast<EntityProto*>(&attack_ability_proto));
            enemy_playerr.cmp<CompAttacker>()->attack_ability.cmp<CompAbility>()->cast_range = 15;
        }

/*
        TeleportProto teleproto(glm::vec3(1.0));
        auto teleport_entity = c->add_entity_from_proto(&teleproto);
        teleport_entity.cmp<CompPosition>()->pos = glm::vec3(75,75,20);
        */

        if (1)
        {
            EntityRef test_ent = c->add_entity_with_components({
                uint32_t(type_id<CompPosition>),
                uint32_t(type_id<CompBounds>),
                uint32_t(type_id<CompLineObject>),
                #ifdef ENABLE_GRAPHICS
                #endif
                });
    #ifdef ENABLE_GRAPHICS 
            auto line_mat = std::make_shared<bgfx::Material>(line_vertex_shader,
                line_fragment_shader,
                true);
    #endif

            auto line_mesh = std::make_shared<bgfx::Mesh>();
            line_mesh->set_vertices(std::vector<float>{0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 10.0, 10.0});
            line_mesh->_saved_vertices = std::vector<float>{0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 10.0, 10.0};
            line_mesh->set_normals(std::vector<float>{0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 10.0, 10.0});
            line_mesh->set_solid_color(glm::vec3(0,1,0));
            auto* mesh = test_ent.cmp<CompLineObject>();
            mesh->mesh.set_mesh(line_mesh);
            //mesh->mesh.set_id(test_ent);
    #ifdef ENABLE_GRAPHICS
            mesh->mesh.set_material(line_mat);
    #endif
            test_ent.cmp<CompPosition>()->pos = glm::vec3(0);
        }
        auto radial_actuator_proto = std::make_shared<RadialActuatorProto>(glm::vec3(0));
        auto radial_actuator = c->add_entity_from_proto(radial_actuator_proto.get());
        radial_actuator.cmp<CompPosition>()->pos = glm::vec3(125,125,7);
        radial_actuator.cmp<CompRadialSensor>()->radius = 10;
        radial_actuator.cmp<CompRadialSensor>()->component_filter = {uint32_t(type_id<CompPlayer>)};

        auto radial_actuator2 = c->add_entity_from_proto(radial_actuator_proto.get());
        radial_actuator2.cmp<CompPosition>()->pos = glm::vec3(50,50,7);
        radial_actuator2.cmp<CompRadialSensor>()->radius = 40;
        radial_actuator2.cmp<CompRadialSensor>()->component_filter = {uint32_t(type_id<CompPlayer>)};

        auto spawner_proto = std::make_shared<SpawnerProto>(glm::vec3(0));
        auto enemy_proto = std::make_shared<EnemyUnitProto>(glm::vec3(0));
        auto enemy_proto2 = std::make_shared<EnemyUnitProto2>(glm::vec3(0));
        auto enemy_proto3 = std::make_shared<HeavyEnemyUnit>(glm::vec3(0));
        auto enemy_spawn_anim_proto = std::make_shared<SpawnAnimationProto>(1.5, enemy_proto);
        auto enemy_spawn_anim_proto2 = std::make_shared<SpawnAnimationProto>(1.5, enemy_proto2);
        auto enemy_spawn_anim_proto3 = std::make_shared<SpawnAnimationProto>(1.5, enemy_proto3);

        {
            
            auto spawner = c->add_entity_from_proto(spawner_proto.get());
            spawner.cmp<CompPosition>()->pos = glm::vec3(125, 125, 15);
            spawner.cmp<CompActuatorDetector>()->actuator = radial_actuator;
            
            //spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);

            auto mod_proto = std::make_shared<AbilityModStationProto>(glm::vec3(0));
            auto mod_spawner_proto = std::make_shared<SpawnerProto>(glm::vec3(0));
            auto mod_spawner = c->add_entity_from_proto(mod_spawner_proto.get());
            mod_spawner.cmp<CompPosition>()->pos = glm::vec3(80, 80, 7);
            mod_spawner.cmp<CompActuatorDetector>()->actuator = spawner;
            mod_spawner.cmp<CompSpawnProtoList>()->protos.push_back(mod_proto);
        }

        // Random Spawner
        {
            auto timer_actuator_proto = std::make_shared<TimerActuatorProto>(glm::vec3(0));
            auto timer_actuator = c->add_entity_from_proto(timer_actuator_proto.get());
            timer_actuator.cmp<CompTimer>()->time_amount = 10;
            timer_actuator.cmp<CompTimer>()->current_time = 10;

            auto spawner_proto = std::make_shared<SpawnerProto>(glm::vec3(0));
            auto spawner = c->add_entity_from_proto(spawner_proto.get());
            spawner.cmp<CompPosition>()->pos = glm::vec3(100, 100, 15);
            spawner.cmp<CompActuatorDetector>()->actuator = timer_actuator;
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto3);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto3);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto3);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto3);
            spawner.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);
            spawner.cmp<CompSpawnProtoList>()->one_at_a_time = true;
            spawner.cmp<CompSpawnProtoList>()->radius = 80;
        }

        auto spawner2 = c->add_entity_from_proto(spawner_proto.get());
        spawner2.cmp<CompPosition>()->pos = glm::vec3(50,50,15);
        spawner2.cmp<CompActuatorDetector>()->actuator = radial_actuator2;
        spawner2.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
        spawner2.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto);
        spawner2.cmp<CompSpawnProtoList>()->protos.push_back(enemy_spawn_anim_proto2);

        

        auto teleport_proto = std::make_shared<TeleportProto>(glm::vec3(1.0));
        auto teleport_spawner_proto = std::make_shared<SpawnerProto>(glm::vec3(0));
        auto teleport_spawner = c->add_entity_from_proto(teleport_spawner_proto.get());
        teleport_spawner.cmp<CompPosition>()->pos = glm::vec3(50,50,7);
        teleport_spawner.cmp<CompActuatorDetector>()->actuator = spawner2;
        teleport_spawner.cmp<CompSpawnProtoList>()->protos.push_back(teleport_proto);

        auto start_mod_proto = std::make_shared<AbilityModStationProto>(glm::vec3(0));
        auto start_mod = c->add_entity_from_proto(start_mod_proto.get());
        start_mod.cmp<CompPosition>()->pos = glm::vec3(45, 125, 40);

        auto start_draft_proto = std::make_shared<AbilityDraftStationProto>(glm::vec3(0));
        auto start_draft = c->add_entity_from_proto(start_draft_proto.get());
        start_draft.cmp<CompPosition>()->pos = glm::vec3(50, 125, 40);
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
