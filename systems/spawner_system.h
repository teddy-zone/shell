#pragma once

#include <random>

#include "system.h"
#include "actuator_detector_component.h"
#include "spawner_proto.h"

class SysSpawner : public System
{
    std::uniform_real_distribution<float> dist;
    std::mt19937 generator;
public:

    SysSpawner():
        dist(0,1)
    {

    }

    virtual void update(double dt) override
    {
        auto& spawn_sensors = get_array<CompActuatorDetector>();
        for (auto& spawn_sensor : spawn_sensors)
        {
            if (auto* proto_list_comp = spawn_sensor.sibling<CompSpawnProtoList>())
            {
                std::vector<EntityRef> to_remove;
                for (auto& spawned_entity : proto_list_comp->instantiated_entities)
                {
                    if (!spawned_entity.is_valid())
                    {   
                        to_remove.push_back(spawned_entity);
                    }
                    auto spawned_children = spawned_entity.get_children();
                    for (auto & child : spawned_children)
                    {
                        if (child.is_valid())
                        {
                            proto_list_comp->instantiated_entities.insert(child);
                        }
                    }
                }
                for (auto tor : to_remove)
                {
                    proto_list_comp->instantiated_entities.erase(tor);
                }
                if (proto_list_comp->has_spawned)
                {
                    if (proto_list_comp->instantiated_entities.empty())
                    {
                        if (auto* all_dead_comp = proto_list_comp->sibling<CompActuator>())
                        {
                            all_dead_comp->trigger();
                        }
                    }
                }
            }
            CompPosition* my_pos_comp = spawn_sensor.sibling<CompPosition>();
            auto my_pos = my_pos_comp->pos;
            if (spawn_sensor.actuator.is_valid())
            {
                if (auto* comp_actuator = spawn_sensor.actuator.cmp<CompActuator>())
                {
                    if (comp_actuator->is_triggered())
                    {
                        if (auto* proto_list_comp = spawn_sensor.sibling<CompSpawnProtoList>())
                        {
                            if (!proto_list_comp->has_spawned)
                            {
                                auto* spawn_anim = spawn_sensor.sibling<CompSpawnAnimation>();
                                auto protos = proto_list_comp->protos;
                                std::vector<int> to_remove;
                                std::vector<EntityRef> added;
                                bool one_at_a_time = proto_list_comp->one_at_a_time;
                                float base_radius = proto_list_comp->radius;
                                int proto_index = 0;
                                for (auto& proto : protos)
                                {
                                    EntityRef new_entity;
                                    if (!spawn_anim)
                                    {
                                        new_entity = _interface->add_entity_from_proto(proto.get(), proto_list_comp->get_entity());
                                    }
                                    else
                                    {
                                        spawn_anim->animation_proto->to_spawn = proto;
                                        new_entity = _interface->add_entity_from_proto(spawn_anim->animation_proto.get());
                                        new_entity.cmp<CompLifetime>()->lifetime = spawn_anim->duration;
                                    }
                                    added.push_back(new_entity);
                                    {
                                        if (auto* new_pos_comp = new_entity.cmp<CompPosition>())
                                        {
                                            float radius = base_radius*dist(generator);
                                            float az = 2*3.1415926*dist(generator);
                                            glm::vec3 offset(radius*cos(az), radius*sin(az), 0);
                                            new_pos_comp->pos = my_pos + offset;
                                            auto& nav_mesh = get_array<CompNavMesh>()[0];
                                            new_pos_comp->pos = nav_mesh.nav_mesh->get_closest_point_on_navmesh(new_pos_comp->pos);// +glm::vec3(0, 0, 10);
                                            std::cout << glm::to_string(new_pos_comp->pos) << "\n";
                                        }
                                    }
                                    to_remove.push_back(proto_index);
                                    if (one_at_a_time)
                                    {
                                        break;
                                    }
                                    proto_index++;
                                }
                                proto_list_comp = spawn_sensor.sibling<CompSpawnProtoList>();
                                for (auto& added_entity : added)
                                {
                                    proto_list_comp->instantiated_entities.insert(added_entity);
                                }

                                // A terrible process for removing stuff from the proto list
                                {
                                    for (auto& tor : to_remove)
                                    {
                                        proto_list_comp->protos[tor] = nullptr;
                                    }
                                    for (int i = 0; i < proto_list_comp->protos.size(); ++i)
                                    {
                                        if (!proto_list_comp->protos[i])
                                        {
                                            proto_list_comp->protos[i] = proto_list_comp->protos.back();
                                            proto_list_comp->protos.resize(proto_list_comp->protos.size() - 1);
                                            i = i - 1;
                                        }
                                    }
                                }

                                if (proto_list_comp->protos.size() == 0)
                                {
                                    proto_list_comp->has_spawned = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};
