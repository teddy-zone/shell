#pragma once

#include <unordered_set>

#include "entity_prototype.h"
#include "component.h"
#include "actuator_component.h"
#include "radial_sensor_component.h"
#include "actuator_detector_component.h"

struct CompSpawnProtoList : public Component
{
    std::vector<std::shared_ptr<EntityProto>> protos;
    std::unordered_set<EntityRef> instantiated_entities;
    float radius = 10;
    bool has_spawned = false;
};

struct SpawnerProto : public ActorProto
{
    std::string _level_to_load;

    SpawnerProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
              uint32_t(type_id<CompSpawnProtoList>),
              uint32_t(type_id<CompActuatorDetector>),
              uint32_t(type_id<CompActuator>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        
    }
};

struct RadialActuatorProto : public ActorProto
{
    std::string _level_to_load;

    RadialActuatorProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
              uint32_t(type_id<CompActuator>),
              uint32_t(type_id<CompRadialSensor>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        
    }
};