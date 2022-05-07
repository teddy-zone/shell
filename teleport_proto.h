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

struct TeleportProto : public ActorProto
{
    std::string _level_to_load;

    TeleportProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompInteractable>),
                    uint32_t(type_id<CompPhysics>), 
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompVoice>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity) 
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("cube.obj" );
        monkey_mesh->set_solid_color(glm::vec4(0.9,0.9,1.0,0.75));
        entity.cmp<CompPosition>()->scale = glm::vec3(2, 2, 10);
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
/*
        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.7);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(0.0);
        */
        std::string level_to_load = _level_to_load;
        entity.cmp<CompInteractable>()->interact_range = 20;
        entity.cmp<CompInteractable>()->interaction_callback = 
            [level_to_load] (SystemInterface* _interface, EntityRef interactor, EntityRef interactee) 
            {
                printf("executing level unloan");
                _interface->unload_level("TestLevel");
                _interface->load_level("Test2Level");
                Sound new_sound;
                new_sound.path = "C:\\Users\\tjwal\\OneDrive\\Documents\\REAPER Media\\teleport.wav";
                new_sound.trigger = false;
                interactor.cmp<CompVoice>()->sounds["teleport"] = new_sound;
                interactor.cmp<CompVoice>()->sounds["teleport"].trigger = true;
                /*
                _interface->load_level(level_to_load);
                auto& shell_levels = _interface->get_array<CompShellLevel>();
                std::vector<CompIndex> to_remove;
                CompIndex current_index = 0;
                //for (auto& shell_level : shell_levels)
                for (int i = 0; i < shell_levels.size(); ++i)
                {
                    _interface->unload_level(shell_levels[i].level_name);
                    current_index += 1;
                }
                CompShellLevel new_level;
                new_level.level_name = level_to_load;
                _interface->add_component(new_level);
                */
            };
    }
};
