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
        music_sound.path = "C:\\Users\\tjwal\\Music\\main_theme.wav";
        music_sound.loop = true;
        music_sound.trigger = true;
        music_sound.range = 100;

        auto& player1 = get_array<CompPlayer>()[0];
        player1.sibling<CompPosition>()->pos = glm::vec3(50,50,10);


        UnitProto unit_proto(glm::vec3(1.0));

        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("suzanne.obj" );
        auto box_mat = std::make_shared<bgfx::Material>();
        auto c = _interface;


        LightEntityProto light_proto(glm::vec3(10,200,50));
        auto light_entity = c->add_entity_from_proto(&light_proto);

        LightEntityProto light_proto2(glm::vec3(100, 10, 50));
        auto light_entity2 = c->add_entity_from_proto(&light_proto2);

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
        landscape_mesh->load_obj("basic_level.obj", true);
        landscape_mesh->set_solid_color(glm::vec3(0.2,0.4,0.05)); 
        auto* lmesh = player2.cmp<CompStaticMesh>();
        lmesh->mesh.set_mesh(landscape_mesh);
        lmesh->mesh.set_id(1);
        player2.cmp<CompVoice>()->sounds["music"] = music_sound;
        lmesh->mesh.set_material(box_mat);
        
        //lmesh->mesh.set_scale(glm::vec3(5, 5, 1.0));
        auto tri_oct_comp = octree::vector_to_octree(lmesh->mesh.get_mesh()->_saved_vertices, lmesh->mesh.get_mesh()->_bmin, lmesh->mesh.get_mesh()->_bmax);
        lmesh->tri_octree = tri_oct_comp;

        bounds->is_static = true;

        bounds->set_bounds(lmesh->mesh.get_mesh()->_bmax - lmesh->mesh.get_mesh()->_bmin);
        bounds->insert_size = 5.0;
        pos->pos = glm::vec3(1,1,1);
        
        for (int i = 0; i < 10; ++i)
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

        TeleportProto teleproto(glm::vec3(1.0));
        auto teleport_entity = c->add_entity_from_proto(&teleproto);
        teleport_entity.cmp<CompPosition>()->pos = glm::vec3(75,75,20);

        /*
        if (1)
        {
            EntityRef test_ent = c->add_entity_with_components({
                uint32_t(type_id<CompPosition>),
                uint32_t(type_id<CompBounds>),
                
                uint32_t(type_id<CompStaticMesh>),
                uint32_t(type_id<CompNavPointer>),
                });
    #ifdef ENABLE_GRAPHICS 
            auto sphere_mat = std::make_shared<bgfx::Material>();
            std::ifstream t_sphere("C:\\Users\\tjwal\\projects\\ECS\\materials\\sphere_mat\\VertexShader.glsl");
            std::stringstream buffer_sphere;
            buffer_sphere << t_sphere.rdbuf();
            auto vshader_sphere = std::make_shared<Shader>(Shader::Type::Vertex, buffer_sphere.str(), true);
            std::ifstream t_sphere_f("C:\\Users\\tjwal\\projects\\ECS\\materials\\sphere_mat\\FragmentShader.glsl");
            std::stringstream buffer2_sphere;
            buffer2_sphere << t_sphere_f.rdbuf();
            auto fshader_sphere = std::make_shared<Shader>(Shader::Type::Fragment, buffer2_sphere.str(), true);
            sphere_mat->set_vertex_shader(vshader_sphere);
            sphere_mat->set_fragment_shader(fshader_sphere);
            sphere_mat->link();
    #endif

            auto sphere_mesh = std::make_shared<bgfx::Mesh>();
            sphere_mesh->load_obj("sphere.obj");
            auto* mesh = test_ent.cmp<CompStaticMesh>();
            mesh->mesh.set_mesh(sphere_mesh);
    #ifdef ENABLE_GRAPHICS
            mesh->mesh.set_material(sphere_mat);
    #endif
            mesh->mesh.set_id(test_ent.get_id());
            test_ent.cmp<CompPosition>()->pos = glm::vec3(0,0,0);
        }
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
            auto line_mat = std::make_shared<bgfx::Material>("C:\\Users\\tjwal\\projects\\ECS\\materials\\line_mat\\VertexShader.glsl", 
                                                             "C:\\Users\\tjwal\\projects\\ECS\\materials\\line_mat\\FragmentShader.glsl");
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
