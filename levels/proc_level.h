#pragma once

#include <sstream>

#include "level.h"
#include "procedural_level_component.h"
#include "noodle_stand_proto.h"
#include "rock_proto.h"

class ProcTestLevel : public Level
{
public:
    ProcTestLevel():
        Level("ProcTestLevel")
    {
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


        static bool player_placed = false;
        if (player_array.size() && !player_placed && proc_levels.size())
        {
            if (proc_levels[0].generated)
            {
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
                player_placed = true;
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

                int light_counter = 0;
                for (auto& path_element : proc_levels[0].path)
                {

                    light_counter++;
                }


            }
        }
    }
};
