#pragma once

#include "entity_prototype.h"
#include "tooltip_component.h"

struct AbilityModStationProto : public ActorProto
{
    std::string _level_to_load;

    AbilityModStationProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompInteractable>),
                    uint32_t(type_id<CompPhysics>), 
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompTooltip>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("cube.obj" );
        monkey_mesh->set_solid_color(glm::vec4(0.4,0.9,1.0,0.75));
        entity.cmp<CompPosition>()->scale = glm::vec3(2, 2, 10);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(monkey_mesh);
        auto box_mat = std::make_shared<bgfx::Material>();

        entity.cmp<CompStaticMesh>()->mesh.set_id(entity.get_id());
        entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompTooltip>()->text = "Ability Upgrade Station";
/*
        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.7);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(0.0);
        */
        std::string level_to_load = _level_to_load;
        entity.cmp<CompInteractable>()->interact_range = 20;
        entity.cmp<CompInteractable>()->interaction_callback = 
            [] (SystemInterface* _interface, EntityRef interactor, EntityRef interactee) 
            {
                if (auto* ability_mod_comp = interactor.cmp<CompAbilityMod>())
                {
                    ability_mod_comp->mods_available += 1;
                    printf("Ability up!\n");
                }
            };
    }
};
