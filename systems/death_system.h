#pragma once

#include "system.h"
#include "bounty_component.h"
#include "respawn_component.h"
#include "coin_proto.h"

class SysDeath : public System
{

public:
    virtual void update(double dt) override
    {
        auto& health_comps = get_array<CompHealth>();
        for (auto& health_comp : health_comps)
        {
            if (health_comp.is_dead)
            {
                if (auto* bounty_comp = health_comp.sibling<CompBounty>())
                {
                    if (health_comp.killer)
                    {
                        if (auto* wallet = health_comp.killer.value().cmp<CompWallet>())
                        {
                            wallet->balance += bounty_comp->money_bounty;
                            auto coin_proto = std::make_shared<CoinProto>();
                            auto coin_entity = _interface->add_entity_from_proto(coin_proto.get());
                            EntityRef death_location_entity = _interface->add_entity_with_components({ uint32_t(type_id<CompPosition>), uint32_t(type_id<CompLifetime>)});
                            death_location_entity.cmp<CompPosition>()->pos = health_comp.sibling<CompPosition>()->pos;
                            death_location_entity.cmp<CompLifetime>()->lifetime = 2;
                            std::cout << "DEATH: " << glm::to_string(death_location_entity.cmp<CompPosition>()->pos) << "\n";

                            coin_entity.cmp<CompAttachment>()->attached_entities.push_back(death_location_entity);
                            coin_entity.cmp<CompAttachment>()->type = AttachmentType::Attacher;
                            coin_entity.cmp<CompAttachment>()->position_offset = glm::vec3(0, 0, 3);
                        }
                        if (auto* exp = health_comp.killer.value().cmp<CompExperience>())
                        {
                            exp->experience += bounty_comp->exp_bounty;
                        }
                    }
                }
                if (auto* respawn_comp = health_comp.sibling<CompRespawn>())
                {
                    _interface->unload_level("BaseLevel");
                    _interface->unload_level("TestLevel");
                    _interface->load_level("MainMenuLevel");
                    // If current respawn time not set, death hasn't been initialized
                    if (!respawn_comp->current_respawn_time)
                    {
                        if (auto* pos_comp = health_comp.sibling<CompPosition>())
                        {
                            respawn_comp->death_location = pos_comp->pos;
                            pos_comp->pos = glm::vec3(64,64,64);
                            pos_comp->sibling<CompPhysics>()->has_gravity = false;
                        }
                        respawn_comp->current_respawn_time = respawn_comp->default_respawn_time;
                    }
                }
                else
                {
                    _interface->delete_entity(health_comp.get_id());
                }
            }
        }
        auto& respawn_comps = get_array<CompRespawn>();
        for (auto& respawn_comp : respawn_comps)
        {
            if (respawn_comp.current_respawn_time)
            {
                respawn_comp.current_respawn_time.value() -= dt;
                if (respawn_comp.current_respawn_time.value() < 0)
                {
                    respawn_comp.current_respawn_time = std::nullopt;
                    if (auto* health_comp = respawn_comp.sibling<CompHealth>())
                    {
                        health_comp->is_dead = false;
                        health_comp->health_percentage = 100.0;
                        if (auto* pos_comp = health_comp->sibling<CompPosition>())
                        {
                            pos_comp->pos = respawn_comp.death_location;
                            pos_comp->sibling<CompPhysics>()->has_gravity = true;
                        }
                    }
                }
            }
        }
    }

};