#pragma once

#include "ability_proto.h"

struct CrystalNovaInstanceProto : public ActorProto
{

    CrystalNovaInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompRadiusApplication>),
                    uint32_t(type_id<CompDecal>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("crystal_nova.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
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
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompLifetime>()->lifetime = 0.5;

        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.2);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(10.0);

        entity.cmp<CompRadiusApplication>()->radius = 5;
        entity.cmp<CompRadiusApplication>()->tick_time = 100;
        entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 100, false};

        entity.cmp<CompDecal>()->decal.location = glm::vec4(pos, 1);
        entity.cmp<CompDecal>()->decal.color = glm::vec4(0.0,0.0,1.0,1.0);
        entity.cmp<CompDecal>()->decal.t = 0;
        entity.cmp<CompDecal>()->decal.radius = 5;
        entity.cmp<CompDecal>()->decal.type = 1;
    }
};
