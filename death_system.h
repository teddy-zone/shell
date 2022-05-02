#pragma once

#include "system.h"
#include "bounty_component.h"
#include "respawn_component.h"

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
                        }
                        if (auto* exp = health_comp.killer.value().cmp<CompExperience>())
                        {
                            exp->experience += bounty_comp->exp_bounty;
                        }
                    }
                }
                if (auto* respawn_comp = health_comp.sibling<CompRespawn>())
                {
                    respawn_comp->current_respawn_time = respawn_comp->default_respawn_time;
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
                    }
                }
            }
        }
    }

};