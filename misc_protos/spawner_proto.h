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

struct SpawnAnimationProto : public ActorProto 
{
    std::shared_ptr<EntityProto> to_spawn;
    float duration;
    SpawnAnimationProto(float in_duration, std::shared_ptr<EntityProto> in_proto, const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types),
        to_spawn(in_proto),
        duration(in_duration)
    {
        std::vector<CompType> unit_components = {{
              uint32_t(type_id<CompStaticMesh>),
              uint32_t(type_id<CompLifetime>),
              uint32_t(type_id<CompAnimation>),
              uint32_t(type_id<CompOnDestruction>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompOnDestruction>()->on_destruction_callbacks.push_back(
            [&](SystemInterface* iface, EntityRef entity)
            {
                auto new_entity = iface->add_entity_from_proto(to_spawn.get(), entity);
                new_entity.cmp<CompPosition>()->pos = entity.cmp<CompPosition>()->pos;
            });
            entity.cmp<CompLifetime>()->lifetime = duration;
            auto monkey_mesh = std::make_shared<bgfx::Mesh>();
            monkey_mesh->load_obj("cube.obj" );
            monkey_mesh->set_solid_color(glm::vec4(0xFF*1.0f/0xFF, 0xBA*1.0f/0xFF, 0x08*1.0f/0xFF, 0.75));
            entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
            entity.cmp<CompAnimation>()->end_time = duration;
            entity.cmp<CompAnimation>()->scale_enabled = true;
            entity.cmp<CompAnimation>()->start_scale = glm::vec3(1, 1, 20);
            entity.cmp<CompAnimation>()->end_scale = glm::vec3(1,1,0);
    }
};

struct CompSpawnAnimation : public Component 
{
    std::shared_ptr<SpawnAnimationProto> animation_proto;
    float duration;
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