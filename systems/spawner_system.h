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
                                for (auto& proto : proto_list_comp->protos)
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
                                    proto_list_comp = spawn_sensor.sibling<CompSpawnProtoList>();
                                    proto_list_comp->instantiated_entities.insert(new_entity);
                                    {
                                        if (auto* new_pos_comp = new_entity.cmp<CompPosition>())
                                        {
                                            float radius = proto_list_comp->radius*dist(generator);
                                            float az = 2*3.1415926*dist(generator);
                                            glm::vec3 offset(radius*cos(az), radius*sin(az), 0);
                                            new_pos_comp->pos = my_pos + offset;
                                            auto& nav_mesh = get_array<CompNavMesh>()[0];
                                            new_pos_comp->pos = nav_mesh.nav_mesh->get_closest_point_on_navmesh(new_pos_comp->pos) + glm::vec3(0,0,10);
                                            std::cout << glm::to_string(new_pos_comp->pos) << "\n";
                                        }
                                    }
                                    
                                }
                                proto_list_comp->has_spawned = true;
                            }
                        }
                    }
                }
            }
        }
    }
};
