#pragma once

#include "entity_prototype.h"
#include "attachment_component.h"

struct CoinProto : public ActorProto
{

    CoinProto(const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompAttachment>),
                    uint32_t(type_id<CompAnimation>)
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompLifetime>()->lifetime = 1.5;
        std::shared_ptr<bgfx::Mesh> coin_mesh = std::make_shared<bgfx::Mesh>();
        coin_mesh->load_obj("coin.obj" , true);
        coin_mesh->set_solid_color(glm::vec3(0.9, 0.7, 0.4)*1.5);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(coin_mesh);
        entity.cmp<CompPosition>()->scale = glm::vec3(2.0);
        entity.cmp<CompAnimation>()->spin_enabled = true;
        entity.cmp<CompAnimation>()->spin_axis = glm::vec3(0, 0, 1);
        entity.cmp<CompAnimation>()->spin_speed = 12.0;
        entity.cmp<CompAnimation>()->start_loc_offset = glm::vec3(0);
        entity.cmp<CompAnimation>()->end_loc_offset = glm::vec3(0, 0, 2.0);
        entity.cmp<CompAnimation>()->offset_enabled = true;
        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.5;
    }
};
