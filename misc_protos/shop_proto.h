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
#include "materials/box_mat/VertexShader.glsl.h"
#include "materials/box_mat/FragmentShader.glsl.h"

struct ShopProto : public ActorProto
{

    ShopProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPhysics>), 
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompShopInventory>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompInteractable>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 5);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(cube_mesh);
        auto box_mat = std::make_shared<bgfx::Material>();
        auto vshader = std::make_shared<Shader>(Shader::Type::Vertex, box_vertex_shader, true);
        auto fshader = std::make_shared<Shader>(Shader::Type::Fragment, box_fragment_shader, true);
        box_mat->set_vertex_shader(vshader);
        box_mat->set_fragment_shader(fshader);
        box_mat->link();
        entity.cmp<CompStaticMesh>()->mesh.set_material(box_mat);
        entity.cmp<CompStaticMesh>()->mesh.set_id(entity.get_id());
        entity.cmp<CompPosition>()->pos = glm::vec3(50,50,30);
        entity.cmp<CompShopInventory>()->visible = false;
        entity.cmp<CompInteractable>()->interact_range = 10;

        Sound new_sound;
        new_sound.path = "sounds/purchase.wav";
        new_sound.trigger = false;
        entity.cmp<CompVoice>()->sounds["purchase"] = new_sound;
        /*
        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 50;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(1.0);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(5.0);
        */
    }
};