#pragma once

#include "entity_prototype.h"
#include "ability.h"
#include "animation_component.h"
#include "lifetime_system.h"
#include "health_component.h"
#include "has_owner_component.h"

struct UnitProto : public ActorProto
{

    UnitProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPhysics>), 
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompPickupper>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompNav>),
                    uint32_t(type_id<CompCommand>),
                    uint32_t(type_id<CompAbilitySet>),
                    uint32_t(type_id<CompStatusManager>),
                    uint32_t(type_id<CompInventory>),
                    uint32_t(type_id<CompCaster>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompHealth>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("suzanne.obj" );
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
        auto box_mat = std::make_shared<bgfx::Material>();

        std::ifstream t("C:\\Users\\tjwal\\projects\\ECS\\materials\\box_mat\\VertexShader.glsl");
        std::stringstream buffer;
        buffer << t.rdbuf();
        auto vshader = std::make_shared<Shader>(Shader::Type::Vertex, buffer.str(), true);
        std::ifstream t2("C:\\Users\\tjwal\\projects\\ECS\\materials\\box_mat\\FragmentShader.glsl");
        std::stringstream buffer2;
        buffer2 << t2.rdbuf();
        auto fshader = std::make_shared<Shader>(Shader::Type::Fragment, buffer2.str(), true);
        box_mat->set_vertex_shader(vshader);
        box_mat->set_fragment_shader(fshader);
        box_mat->link();
        entity.cmp<CompStaticMesh>()->mesh.set_material(box_mat);
        entity.cmp<CompStaticMesh>()->mesh.set_id(entity.get_id());
        entity.cmp<CompPosition>()->pos = glm::vec3(20,20,45);
        entity.cmp<CompHealth>()->health_percentage = 100;
        /*
        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 50;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(1.0);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(5.0);
        */
    }
};

struct AbilityProto : public EntityProto
{

    AbilityProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompAbility>),
                    uint32_t(type_id<CompHasOwner>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
    }
};

struct CommandIndicatorProto : public ActorProto
{

    CommandIndicatorProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("command_indicator.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
        auto box_mat = std::make_shared<bgfx::Material>();

        std::ifstream t("C:\\Users\\tjwal\\projects\\ECS\\materials\\box_mat\\VertexShader.glsl");
        std::stringstream buffer;
        buffer << t.rdbuf();
        auto vshader = std::make_shared<Shader>(Shader::Type::Vertex, buffer.str(), true);
        std::ifstream t2("C:\\Users\\tjwal\\projects\\ECS\\materials\\box_mat\\FragmentShader.glsl");
        std::stringstream buffer2;
        buffer2 << t2.rdbuf();
        auto fshader = std::make_shared<Shader>(Shader::Type::Fragment, buffer2.str(), true);
        box_mat->set_vertex_shader(vshader);
        box_mat->set_fragment_shader(fshader);
        box_mat->link();
        entity.cmp<CompStaticMesh>()->mesh.set_material(box_mat);
        entity.cmp<CompStaticMesh>()->mesh.set_id(entity.get_id());
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompLifetime>()->lifetime = 0.5;

        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.7);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(0.0);
    }
};
