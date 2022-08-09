#pragma once

#include "entity_prototype.h"
#include "ability.h"
#include "animation_component.h"
#include "lifetime_system.h"
#include "health_component.h"
#include "has_owner_component.h"
#include "interactable_component.h"
#include "shell_level_component.h"
#include "team_component.h"
#include "caster_component.h"
#include "shop_inventory.h"
#include "point_light_component.h"

struct RockProto : public ActorProto
{
    int rock_number;

    RockProto(int in_rock_number, const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types),
        rock_number(in_rock_number)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto noodle_stand_mesh = std::make_shared<bgfx::Mesh>();
        noodle_stand_mesh->load_obj("rock" + std::to_string(rock_number) + ".obj", true);
        auto* noodle_mesh = entity.cmp<CompStaticMesh>();
        noodle_mesh->mesh.set_mesh(noodle_stand_mesh);
        noodle_mesh->mesh.set_id(-1);
        //noodle_stand_mesh->set_solid_color(glm::vec3(0.5, 0.45, 0.51));
        entity.set_name("Rock" + std::to_string(rock_number));// +std::to_string(noodle_stand.get_id()));
        auto temp_bounds = noodle_mesh->mesh.get_mesh()->_bmax - noodle_mesh->mesh.get_mesh()->_bmin;

        entity.cmp<CompBounds>()->bounds = temp_bounds;
        entity.cmp<CompBounds>()->is_static = true;
    }
};

struct CrystalProto : public ActorProto
{
    CrystalProto(const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompPointLight>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto noodle_stand_mesh = std::make_shared<bgfx::Mesh>();
        noodle_stand_mesh->load_obj("crystal.obj", true);
        auto* noodle_mesh = entity.cmp<CompStaticMesh>();
        noodle_mesh->mesh.set_mesh(noodle_stand_mesh);
        noodle_mesh->mesh.set_id(-1);
        //noodle_stand_mesh->set_solid_color(glm::vec3(0.5, 0.45, 0.51));
        entity.set_name("Crystal");// +std::to_string(noodle_stand.get_id()));
        auto temp_bounds = noodle_mesh->mesh.get_mesh()->_bmax - noodle_mesh->mesh.get_mesh()->_bmin;

        entity.cmp<CompBounds>()->bounds = temp_bounds;
        entity.cmp<CompBounds>()->is_static = true;
        entity.cmp<CompPointLight>()->light.intensity = 0.002;
        entity.cmp<CompPointLight>()->light.color = glm::vec4(0.8,0.8,1.0,1.0);
    }
};

struct GrassClumpProto : public ActorProto
{
    int grass_number;

    GrassClumpProto(int in_grass_number, const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types),
        grass_number(in_grass_number)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto noodle_stand_mesh = std::make_shared<bgfx::Mesh>();
        noodle_stand_mesh->load_obj("grass_clump.obj", true);
        auto* noodle_mesh = entity.cmp<CompStaticMesh>();
        noodle_mesh->mesh.set_mesh(noodle_stand_mesh);
        noodle_mesh->mesh.set_id(-1);
        //noodle_stand_mesh->set_solid_color(glm::vec3(0.5, 0.45, 0.51));
        entity.set_name("Grass" + std::to_string(grass_number));// +std::to_string(noodle_stand.get_id()));
        auto temp_bounds = noodle_mesh->mesh.get_mesh()->_bmax - noodle_mesh->mesh.get_mesh()->_bmin;

        entity.cmp<CompBounds>()->bounds = temp_bounds;
        entity.cmp<CompBounds>()->is_static = true;
    }
};