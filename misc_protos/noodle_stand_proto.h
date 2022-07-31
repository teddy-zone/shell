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

struct NoodleStandProto : public ActorProto
{
    glm::vec3 facing_direction;

    NoodleStandProto(const glm::vec3& in_facing_direction, const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types),
        facing_direction(in_facing_direction)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompInteractable>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompSpotlight>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompPickupee>),
                    uint32_t(type_id<CompLifetime>)
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto facing_angle = atan2(facing_direction.x, -facing_direction.y);
        auto noodle_stand_mesh = std::make_shared<bgfx::Mesh>();
        noodle_stand_mesh->load_obj("noodle_shop.obj", false);
        auto* noodle_mesh = entity.cmp<CompStaticMesh>();
        noodle_mesh->mesh.set_mesh(noodle_stand_mesh);
        noodle_mesh->mesh.set_id(-1);
        entity.set_name("NoodleStand");// +std::to_string(noodle_stand.get_id()));
        entity.cmp<CompPosition>()->pos = glm::vec3(100, 100, 17.5);
        std::cout << "Noodle stand facing angle " << facing_angle << "\n";
        std::cout << "Facing dir " << glm::to_string(facing_direction) << "\n";
        entity.cmp<CompPosition>()->rot = glm::rotate(float(facing_angle), glm::vec3(0.0f,0.0f,1.0f));
        entity.cmp<CompSpotlight>()->light.direction = glm::normalize(glm::vec4(0, -1, -0.1, 0.0));
        entity.cmp<CompSpotlight>()->light.location = glm::vec4(-2, 0, 5, 1.0);
        entity.cmp<CompSpotlight>()->light.intensity = 0.02;
        entity.cmp<CompSpotlight>()->light.type = 0;
        entity.cmp<CompSpotlight>()->light.size = glm::vec4(3.1415926/4, 3.14159/8, 0, 0);
        entity.cmp<CompSpotlight>()->light.color = glm::vec4(1.5, 0.9, 0.2, 1.0);
        //entity.cmp<CompAnimation>()->spin_enabled = true;
        //entity.cmp<CompAnimation>()->spin_axis = glm::vec3(0,0,1);
        //entity.cmp<CompAnimation>()->spin_speed = 5;
        //auto noodle_tri_oct_comp = octree::vector_to_octree(noodle_mesh->mesh.get_mesh()->_octree_vertices, noodle_mesh->mesh.get_mesh()->_bmin, noodle_mesh->mesh.get_mesh()->_bmax, noodle_stand.cmp<CompPosition>()->rot);
        //noodle_mesh->tri_octree = noodle_tri_oct_comp;
        auto temp_bounds = noodle_mesh->mesh.get_mesh()->_bmax - noodle_mesh->mesh.get_mesh()->_bmin;

        entity.cmp<CompBounds>()->bounds = temp_bounds;
        entity.cmp<CompBounds>()->is_static = true;
    }
};