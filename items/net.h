#pragma once

#include "item_proto.h"
#include "insect_system.h"
#include "notification_component.h"

struct NetInstanceProto : public ActorProto
{

    NetInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompProjectile>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompHasOwner>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompPosition>()->pos = pos;

        entity.cmp<CompAnimation>()->start_time = 0;
        entity.cmp<CompAnimation>()->end_time = 0.3;
        entity.cmp<CompAnimation>()->spin_enabled = true;
        entity.cmp<CompAnimation>()->spin_speed = 5.0;

        entity.cmp<CompProjectile>()->speed = 40;
        entity.cmp<CompProjectile>()->max_range = 20;
        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;

        Sound new_sound;
        new_sound.path = "sounds/beam.wav";
        new_sound.trigger = true;
        entity.cmp<CompVoice>()->sounds["beam"] = new_sound;

        auto net_mesh = std::make_shared<bgfx::Mesh>();
        net_mesh->load_obj("net.obj" );
        net_mesh->set_solid_color(glm::vec3(0.5,0.5,0.5));
        entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(net_mesh);
    }
};


struct ItemNetProto : ItemProto 
{
    ItemNetProto (const std::vector<CompType>& extension_types={}):
        ItemProto(extension_types)
    { 
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompCastPointInstance>),
                    uint32_t(type_id<CompOnCast>),
                    uint32_t(type_id<CompAbility>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        entity.cmp<CompAbility>()->cast_point = 0.5;
        entity.cmp<CompAbility>()->backswing = 0.5;
        entity.cmp<CompAbility>()->ground_targeted = false;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->self_targeted = false;
        entity.cmp<CompAbility>()->cooldown = 0.2;
        entity.cmp<CompAbility>()->cast_range = 10;
        entity.cmp<CompAbility>()->radius = 1;
        entity.cmp<CompAbility>()->max_level = 4;
        entity.cmp<CompAbility>()->level = 1;

        entity.cmp<CompAbility>()->ability_name = "Bug Net";
        entity.cmp<CompAbility>()->target_decal_type = TargetDecalType::Cone;

        //entity.cmp<CompAbility>()->damages = {{entity, DamageType::Magical, 40, false}};
        //entity.cmp<CompHasOwner>()->owner = entity;
        auto net_instance_proto = std::make_shared<NetInstanceProto>(glm::vec3(0));
        auto* cp_inst = entity.cmp<CompCastPointInstance>();
        cp_inst->proto = net_instance_proto;
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
            {
                auto facing_direction = caster.cmp<CompPosition>()->facing_direction;
                auto caster_pos = caster.cmp<CompPosition>()->pos;
                std::cout << "FACE: " << glm::to_string(facing_direction) << "\n";
                auto found_data = iface->data_within_cylinder_selective(caster_pos + facing_direction * 7.0 + glm::vec3(0, 0, 0), 3.2, { uint32_t(type_id<InsectType>) });
                for (auto& found_insect : found_data)
                {
                    iface->delete_entity(found_insect.get_id());
                    if (auto* inventory_comp = caster.cmp<CompInventory>())
                    {
                        if (inventory_comp->has_open_slot())
                        {
                            if (found_insect.cmp<InsectType>()->item_proto)
                            {
                                auto item_entity = iface->add_entity_from_proto(found_insect.cmp<InsectType>()->item_proto.get());
                                inventory_comp = caster.cmp<CompInventory>();
                                inventory_comp->insert_item(item_entity);
                                if (auto* notification_comp = inventory_comp->sibling<CompNotification>())
                                {
                                    notification_comp->add_notification("Caught a " + found_insect.cmp<InsectType>()->name, iface->get_current_game_time());
                                }
                            }
                        }
                    }
                }
            }
        );
    }
};