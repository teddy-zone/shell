#pragma once

#include <sstream>

#include "level.h"
#include "procedural_level_component.h"
#include "noodle_stand_proto.h"

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
                auto& player1 = get_array<CompPlayer>()[0];
                player1.sibling<CompPosition>()->pos = proc_levels[0].spawn_point;
                get_array<CompCamera>()[0].set_look_target(player1.sibling<CompPosition>()->pos, true);
                player_placed = true;
                bool noodle_placed = false;

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
						noodle_stand_entity.cmp<CompPosition>()->pos.z = proc_levels[0].floor_level+1.0;
						noodle_placed = true;
					}
				}

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
				line_mesh->set_solid_color(glm::vec3(0,0,0));
				auto* mesh = nav_mesh_visual.cmp<CompLineObject>();
				mesh->mesh.strip = false;
				mesh->mesh.set_mesh(line_mesh);
				//mesh->mesh.set_id(100);

                int light_counter = 0;
                for (auto& path_element : proc_levels[0].path)
                {
                    // if (is christmas)
                    // glm::vec3 color1(1, 0, 0);
                    // glm::vec3 color2(0, 1, 0);
                    // LightEntityProto light_proto3(glm::vec3(160, 160, 40));
                    // auto light_entity3 = _interface->add_entity_from_proto(&light_proto3);
                    // light_entity3.cmp<CompPointLight>()->light.intensity = 0.03;
                    // if (light_counter % 2)
                    // {
                    //     light_entity3.cmp<CompPointLight>()->light.color = glm::vec4(color1, 1.0);// glm::vec4(0.9, 0.4, 0.3, 1.0);
                    // }
                    // else
                    // {
                    //     light_entity3.cmp<CompPointLight>()->light.color = glm::vec4(color2, 1.0);// glm::vec4(0.9, 0.4, 0.3, 1.0);
                    // }
                    // light_entity3.cmp<CompPointLight>()->light.location = glm::vec4(path_element + glm::vec3(0, 0, 40), 0);
                    // 
                    // light_entity3.set_name("PathLight" + std::to_string(light_counter));

                    // If we are a good ways into the level but not the last path point and we haven't placed the noodle stand already
                    glm::vec3 norm;
                    glm::vec3 spawn_point;
                    int path_point;
                    if (light_counter > 15 && light_counter != proc_levels[0].path.size() - 1 && !noodle_placed)
                    {
                        // if we are at a wide enough point
                        if (proc_levels[0].widths[light_counter] > 15)
                        {
                            /*
                            auto path_dir = glm::normalize(proc_levels[0].path[light_counter + 1] - proc_levels[0].path[light_counter]);
                            path_dir += glm::normalize(proc_levels[0].path[light_counter] - proc_levels[0].path[light_counter - 1]);
                            path_dir += glm::normalize(proc_levels[0].path[light_counter + 2] - proc_levels[0].path[light_counter + 1]);
                            path_dir = glm::normalize(path_dir);
                            const auto normal = glm::normalize(glm::cross(glm::vec3(0, 0, 1), path_dir));
                            */
                            int average_num = 3;
                            glm::vec3 dir_vec;
                            for (int i = light_counter - average_num; i < light_counter + average_num; ++i)
                            {
                                dir_vec += (proc_levels[0].path[i + 1] - proc_levels[0].path[i]);
                            }
                        }
                    }


                    light_counter++;
                }


            }
        }
    }
};
