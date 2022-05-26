#pragma once

#include "entity_prototype.h"
#include "component.h"

struct CompSpawnProtoList : public Component
{
    std::vector<std::shared_ptr<EntityProto>> protos;
};

struct SpawnerProto : public ActorProto
{
    std::string _level_to_load;

    SpawnerProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
              uint32_t(type_id<CompSpawnProtoList>),
              uint32_t(type_id<CompActuatorSensor>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        
    }
};
