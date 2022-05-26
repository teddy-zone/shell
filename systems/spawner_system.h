#pragma once

#include "system.h"
#include "actuator_sensor_component.h"
#include "spawner_proto.h"

class SysSpawner : public System
{
public:
    void update(double dt) override
    {
        auto& spawn_sensors = get_array<CompActuatorSensor>();
        for (auto& spawn_sensor : spawn_sensors)
        {
            CompPosition* my_pos_comp = spawn_sensor.sibling<CompPosition>();
            if (spawn_sensor.actuator.is_valid())
            {
                if (auto* comp_actuator = spawn_sensor.actuator.cmp<CompActuator>())
                {
                    if (comp_actuator->is_triggered)
                    {
                        if (auto* proto_list_comp = spawn_sensor.sibling<CompSpawnProtoList>())
                        {
                            for (auto& proto : proto_list_comp->protos)
                            {
                                auto new_entity = _interface->add_entity_from_proto(proto.get());
                                if (my_pos_comp)
                                {
                                    if (auto* new_pos_comp = new_entity.cmp<CompPosition>())
                                    {
                                        new_pos_comp->pos = my_pos_comp->pos;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};
