#pragma once

#include "component.h"
#include "animation_system.h"
#include "lifetime_system.h"

enum class AbilityState
{
    None,
    GroundTargeting,
    UnitTargeting,
    CastPoint,
    Backswing
};

struct CompCaster : public Component
{
    int ability_index;
    AbilityState state;
    float state_time;
    void activate_ability(int index)
    {
        ability_index = index;
        state = AbilityState::CastPoint;
    }
};

struct CompAbility : public Component
{
    float cooldown;
    std::optional<float> current_cooldown;
    float cast_point;
    float cast_range;
    float backswing;
    bool ground_targeted;
    bool unit_targeted;
    bool channeled;
    float channel_time;
    int level;
    int max_level;
};

struct CompAbilityInstance : public Component
{
    std::shared_ptr<EntityProto> proto;
};

struct CompRadiusApplication : public Component
{
    float radius;
    float tick_time;
}; 

struct CrystalNovaInstanceProto : public ActorProto
{

    CrystalNovaInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompRadiusApplication>),
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
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.2);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(2.0);

        entity.cmp<CompRadiusApplication>()->radius = 5;
        entity.cmp<CompRadiusApplication>()->tick_time = 100;
    }
};
