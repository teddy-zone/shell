#pragma once

#include <sstream>

#include "level.h"
#include "procedural_level_component.h"
#include "noodle_stand_proto.h"
#include "rock_proto.h"
#include "shop_proto.h"
#include "net.h"
#include "blink_dagger.h"
#include "vladmirs_offering.h"

class ProcTestLevel : public Level
{
public:
    ProcTestLevel():
        Level("ProcTestLevel")
    {
        std::cout << "LEVELLLLLLL : " << _level << "\n";
    }

    virtual void level_init() override
    {

        EntityRef proc_level_entity = _interface->add_entity_with_components({ uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompProceduralLevel>),
                    uint32_t(type_id<CompPickupee>)
            });

        proc_level_entity.cmp<CompProceduralLevel>()->generated = false;
        proc_level_entity.cmp<CompProceduralLevel>()->length = 40;
        proc_level_entity.cmp<CompProceduralLevel>()->x_amplitude = 80;
        proc_level_entity.cmp<CompProceduralLevel>()->y_amplitude = 15;
        proc_level_entity.cmp<CompBounds>()->is_static = true;
        proc_level_entity.cmp<CompPosition>()->pos = glm::vec3(1, 1, 1);

    }

    virtual void update(double dt) override
    {
        auto time_comp = get_array<CompTime>()[0];

        auto& player_array = get_array<CompPlayer>();
        auto& proc_levels = get_array<CompProceduralLevel>();


        if (player_array.size() && !proc_levels[0].stuff_placed && proc_levels.size())
        {
            if (proc_levels[0].generated)
            {
                proc_levels[0].stuff_placed = true;
                /*
				LightEntityProto light_proto2(glm::vec3(40, 40, 80));
				auto light_entity2 = _interface->add_entity_from_proto(&light_proto2);
				light_entity2.cmp<CompPointLight>()->light.intensity = 0.3;
				light_entity2.set_name("Sun");
                */

				std::lognormal_distribution<float> rock_size_dist(0.2, 0.75);
				std::lognormal_distribution<float> crystal_size_dist(1.0, 0.4);
				std::normal_distribution<float> rock_placement_dist(0,5);
				std::uniform_real_distribution<float> rock_placement_draw(0, 1.0);
				std::uniform_real_distribution<float> rock_rotation_draw(0, 3.1415926*2);
				std::uniform_int_distribution rock_selection(0, 2);
				std::mt19937 gen(int(_interface->get_current_game_time()));
				const float rock_spawn_chance = 0.30;
				const float crystal_spawn_chance = 0.01;
                const float rock_white_chance = 0.30;
				const float grass_spawn_chance = 0.25;
                auto& player1 = get_array<CompPlayer>()[0];
                player1.sibling<CompPosition>()->pos = proc_levels[0].spawn_point;
                get_array<CompCamera>()[0].set_look_target(player1.sibling<CompPosition>()->pos, true);
                bool noodle_placed = false;
                glm::vec3 noodle_shop_location;
                for (auto& [yi, bottom_loc] : proc_levels[0].y_indexed_path_bottom_edges)
                {
                    auto top_loc = proc_levels[0].y_indexed_path_top_edges[yi];
                    float width = glm::length(top_loc - bottom_loc);
                    if (width > 20 && yi > 100 && !noodle_placed)
                    {
                        auto norm = proc_levels[0].y_indexed_path_bottom_normals[yi];
                        auto spawn_point = proc_levels[0].y_indexed_path_bottom_edges[yi];
                        auto noodle_stand_proto = std::make_shared<NoodleStandProto>(glm::vec3(norm.x, norm.y, norm.z));
                        auto noodle_stand_entity = _interface->add_entity_from_proto(noodle_stand_proto.get());
                        noodle_stand_entity.cmp<CompPosition>()->pos = spawn_point;
                        noodle_stand_entity.cmp<CompPosition>()->pos.z = proc_levels[0].floor_level + 1.0;
                        noodle_placed = true;
                        noodle_shop_location = noodle_stand_entity.cmp<CompPosition>()->pos;
                    }
                }

                for (auto& [yi, bottom_loc] : proc_levels[0].y_indexed_path_bottom_edges)
                {
                    auto top_loc = proc_levels[0].y_indexed_path_top_edges[yi];
                    float width = glm::length(top_loc - bottom_loc);
                    float rock_draw = rock_placement_draw(gen);
                    if (rock_draw < rock_spawn_chance)
                    { 
                        int rock_number = rock_selection(gen);
                        auto location = rock_placement_dist(gen);
                        glm::vec3 new_pos;
                        if (location > 0)
                        {
                            new_pos = bottom_loc + glm::vec3(location, 0, proc_levels[0].floor_level + 1.0);
                        }
                        else if (location < 0)
                        {
                            new_pos = top_loc + glm::vec3(location, 0, proc_levels[0].floor_level + 1.0);
                        }
                        if (glm::length(new_pos - noodle_shop_location) > 20)
                        {
                            float white_draw = rock_placement_draw(gen);
                            auto rock_proto = std::make_shared<RockProto>(rock_number + 1);
                            auto rock_entity = _interface->add_entity_from_proto(rock_proto.get());
                            auto rock_size = rock_size_dist(gen);
                            rock_entity.cmp<CompPosition>()->rot = glm::rotate(float(rock_rotation_draw(gen)), glm::vec3(0.0f, 0.0f, 1.0f));
                            rock_entity.cmp<CompPosition>()->scale = glm::vec3(rock_size);
                            if (white_draw < rock_white_chance)
                            {
                                rock_entity.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec3(0.9,0.9,1.0));
                            }
							if (new_pos.x > 0 && new_pos.y > 0)
							{
								rock_entity.cmp<CompPosition>()->pos = new_pos;
							}
                        }
                    }
                    float crystal_draw = rock_placement_draw(gen);
                    if (crystal_draw < crystal_spawn_chance)
                    { 
                        auto location = rock_placement_dist(gen);
                        glm::vec3 new_pos;
                        if (location > 0)
                        {
                            new_pos = bottom_loc + glm::vec3(location, 0, proc_levels[0].floor_level + 1.0);
                        }
                        else if (location < 0)
                        {
                            new_pos = top_loc + glm::vec3(location, 0, proc_levels[0].floor_level + 1.0);
                        }
                        if (glm::length(new_pos - noodle_shop_location) > 20)
                        {
                            auto crystal_proto = std::make_shared<CrystalProto>();
                            auto crystal_entity = _interface->add_entity_from_proto(crystal_proto.get());
                            auto crystal_size = crystal_size_dist(gen);
                            crystal_entity.cmp<CompPosition>()->rot = glm::rotate(float(rock_rotation_draw(gen)), glm::vec3(0.0f, 0.0f, 1.0f));
                            crystal_entity.cmp<CompPosition>()->scale = glm::vec3(crystal_size);
                            crystal_entity.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec3(1.7,1.7,1.9));
							if (new_pos.x > 0 && new_pos.y > 0)
							{
								crystal_entity.cmp<CompPosition>()->pos = new_pos;
                                crystal_entity.cmp<CompPointLight>()->light.location = glm::vec4(new_pos + glm::vec3(0,0,3), 1.0);
							}
                        }
                    }
                    float grass_draw = rock_placement_draw(gen);
                    if (grass_draw < grass_spawn_chance)
                    { 
                        int grass_number = rock_selection(gen);
                        auto grass_proto = std::make_shared<GrassClumpProto>(grass_number+1);
                        auto rock_entity = _interface->add_entity_from_proto(grass_proto.get());
                        auto rock_size = 0.3*rock_size_dist(gen);
                        auto location = rock_placement_dist(gen);
                        rock_entity.cmp<CompPosition>()->rot = glm::rotate(float(rock_rotation_draw(gen)), glm::vec3(0.0f,0.0f,1.0f));
                        rock_entity.cmp<CompPosition>()->scale = glm::vec3(rock_size);
                        if (location > 0)
                        {
                            auto new_pos = bottom_loc + glm::vec3(location, 0, proc_levels[0].floor_level + 1.0);
                            if (new_pos.x > 0 && new_pos.y > 0)
                            {
                                rock_entity.cmp<CompPosition>()->pos = new_pos;
                            }
                        }
                        else if (location < 0)
                        {
                            auto new_pos = top_loc + glm::vec3(location, 0, proc_levels[0].floor_level + 1.0);
                            if (new_pos.x > 0 && new_pos.y > 0)
                            {
                                rock_entity.cmp<CompPosition>()->pos = new_pos;
                            }
                        }
                    }
				}

                bool path_width_visual_enabled = false;
                if (path_width_visual_enabled)
                {
                    auto nav_mesh_visual = _interface->add_entity_with_components(std::vector<uint32_t>{uint32_t(type_id<CompLineObject>)});
                    std::vector<float> vertices;
                    auto line_mesh = std::make_shared<bgfx::Mesh>();
                    std::vector<float> normals;
                    for (auto& [yi, bottom_edge] : proc_levels[0].y_indexed_path_bottom_edges)
                    {
                        auto top_edge = proc_levels[0].y_indexed_path_top_edges[yi];
                        normals.push_back(0);
                        normals.push_back(0);
                        normals.push_back(-1);
                        vertices.push_back(top_edge.x);
                        vertices.push_back(top_edge.y);
                        vertices.push_back(proc_levels[0].floor_level + 2);
                        vertices.push_back(bottom_edge.x);
                        vertices.push_back(bottom_edge.y);
                        vertices.push_back(proc_levels[0].floor_level + 2);
                    }
                    line_mesh->set_vertices(vertices);
                    line_mesh->_saved_vertices = vertices;
                    line_mesh->set_normals(normals);
                    line_mesh->set_solid_color(glm::vec3(0, 0, 0));
                    auto* mesh = nav_mesh_visual.cmp<CompLineObject>();
                    mesh->mesh.strip = false;
                    mesh->mesh.set_mesh(line_mesh);
                    //mesh->mesh.set_id(100);
                }

				auto start_mod_proto = std::make_shared<AbilityModStationProto>(glm::vec3(0));
				auto start_mod = _interface->add_entity_from_proto(start_mod_proto.get());
                start_mod.cmp<CompPosition>()->pos = proc_levels[0].path[3];
                start_mod.cmp<CompPosition>()->pos.z = proc_levels[0].floor_level + 2;

				auto start_draft_proto = std::make_shared<AbilityDraftStationProto>(glm::vec3(0));
				auto start_draft = _interface->add_entity_from_proto(start_draft_proto.get());
                start_draft.cmp<CompPosition>()->pos = proc_levels[0].path[3] + glm::vec3(7,0,0);
                start_draft.cmp<CompPosition>()->pos.z = proc_levels[0].floor_level + 2;

                {
                    ShopProto shop_proto(glm::vec3(1, 1, 40));
                    auto shopkeeper = _interface->add_entity_from_proto(static_cast<EntityProto*>(&shop_proto));
                    shopkeeper.cmp<CompShopInventory>()->camera = &get_array<CompCamera>()[0].graphics_camera;
                    {
                        BlinkDaggerProto blink_dagger_proto;
                        auto blink_dagger_entity = _interface->add_entity_from_proto(&blink_dagger_proto);
                        InventorySlot blink_dagger_slot;
                        blink_dagger_slot.cost = 2150;
                        blink_dagger_slot.item_entity = blink_dagger_entity;
                        blink_dagger_entity.cmp<CompItem>()->name = "Blink Dagger";
                        shopkeeper.cmp<CompShopInventory>()->slots.push_back(blink_dagger_slot);
                    }
                    {
                        VladmirProto vladmir_proto;
                        auto vladmir_entity = _interface->add_entity_from_proto(&vladmir_proto);
                        InventorySlot vladmir_slot;
                        vladmir_slot.cost = 1500;
                        vladmir_slot.item_entity = vladmir_entity;
                        vladmir_entity.cmp<CompItem>()->name = "Vladmir's Offering";
                        shopkeeper.cmp<CompShopInventory>()->slots.push_back(vladmir_slot);
                    }
                    {
                        ItemNetProto net_proto;
                        auto net_entity = _interface->add_entity_from_proto(&net_proto);
                        InventorySlot net_slot;
                        net_slot.cost = 300;
                        net_slot.item_entity = net_entity;
                        net_entity.cmp<CompItem>()->name = "Bug Net";
                        shopkeeper.cmp<CompShopInventory>()->slots.push_back(net_slot);
                    }
                    shopkeeper.cmp<CompInteractable>()->interaction_callback = [](SystemInterface* sys_interface, EntityRef interactor, EntityRef interactee)
                    {
                        if (auto* inventory = interactee.cmp<CompShopInventory>())
                        {
                            inventory->visible = true;
                        }
                    };
					shopkeeper.cmp<CompPosition>()->pos = proc_levels[0].path[3] + glm::vec3(-7,0,0);
					shopkeeper.cmp<CompPosition>()->pos.z = proc_levels[0].floor_level + 2;
                }

				auto spawner_proto = std::make_shared<SpawnerProto>(glm::vec3(0));
				auto enemy_proto = std::make_shared<EnemyUnitProto>(glm::vec3(0));
				auto enemy_proto2 = std::make_shared<EnemyUnitProto2>(glm::vec3(0));
				auto enemy_proto3 = std::make_shared<HeavyEnemyUnit>(glm::vec3(0));
				auto enemy_spawn_anim_proto = std::make_shared<SpawnAnimationProto>(1.5, enemy_proto);
				auto enemy_spawn_anim_proto2 = std::make_shared<SpawnAnimationProto>(1.5, enemy_proto2);
				auto enemy_spawn_anim_proto3 = std::make_shared<SpawnAnimationProto>(1.5, enemy_proto3);
                std::vector<std::shared_ptr<EntityProto>> enemy_protos = {
                    enemy_proto,
                    enemy_proto2,
                    enemy_proto3
                };

                int light_counter = 0;
                static std::uniform_int_distribution<int> type_selection_distribution(0, 2);
                static std::uniform_int_distribution<int> count_distribution(1,3);
                static std::mt19937 enemy_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
                for (auto& path_element : proc_levels[0].path)
                {
                    if (light_counter % 10 == 0 && light_counter > 10)
                    {
                        auto radial_actuator_proto = std::make_shared<RadialActuatorProto>(glm::vec3(0));
                        auto radial_actuator2 = _interface->add_entity_from_proto(radial_actuator_proto.get());
                        radial_actuator2.cmp<CompPosition>()->pos = path_element + glm::vec3(0,0,proc_levels[0].floor_level);
                        radial_actuator2.cmp<CompRadialSensor>()->radius = proc_levels[0].widths[light_counter];
                        radial_actuator2.cmp<CompRadialSensor>()->component_filter = { uint32_t(type_id<CompPlayer>) };

                        auto spawner2 = _interface->add_entity_from_proto(spawner_proto.get());
                        spawner2.cmp<CompPosition>()->pos = path_element + glm::vec3(0,0,proc_levels[0].floor_level);
                        spawner2.cmp<CompActuatorDetector>()->actuator = radial_actuator2;
                        int num_enemies = count_distribution(enemy_gen);
                        for (int i = 0; i < num_enemies; ++i)
                        {
                            int enemy_selection = type_selection_distribution(gen);
                            spawner2.cmp<CompSpawnProtoList>()->protos.push_back(enemy_protos[enemy_selection]);
                        }
                        spawner2.cmp<CompSpawnProtoList>()->radius = proc_levels[0].widths[light_counter]/2.0;
                    }

                    light_counter++;
                }


            }
        }
    }
};
