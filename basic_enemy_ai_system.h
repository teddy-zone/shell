#pragma once

#include "system.h"
#include "basic_enemy_ai_component.h"

class SysBasicEnemyAI : public System
{
public:
    virtual void update(double dt) override
    {
        auto& ai_components = get_array<CompBasicEnemyAI>();
        auto current_time = _interface->get_current_game_time();
        for (auto& ai_component : ai_components)
        {
            if (current_time - ai_component.last_radius_check > 0.1)
            {
                if (auto* pos_comp = ai_component.sibling<CompPosition>())
                {
                    int my_team = 0;
                    if (auto* my_team_comp = ai_component.sibling<CompTeam>())
                    {
                        my_team = my_team_comp->team;
                    }
                    auto in_range_entities = _interface->data_within_sphere_selective(pos_comp->pos, ai_component.vision_range,
                        { uint32_t(type_id<CompTeam>), uint32_t(type_id<CompHealth>), uint32_t(type_id<CompPosition>) });
                    EntityRef attack_candidate;
                    float attack_candidate_range = 1e10;
                    for (auto& in_range_entity : in_range_entities)
                    {
                        if (in_range_entity.cmp<CompTeam>()->team != my_team)
                        {
                            auto this_candidate_range = glm::length(in_range_entity.cmp<CompPosition>()->pos - pos_comp->pos);
                            if (this_candidate_range < attack_candidate_range)
                            {
                                attack_candidate_range = this_candidate_range;
                                attack_candidate = in_range_entity;
                            }
                        }
                    }
                    if (attack_candidate.is_valid())
                    {
                        if (!ai_component.target)
                        {
                            if (auto* command_comp = ai_component.sibling<CompCommand>())
                            {
                                AttackCommand attack_command;
                                attack_command.target = attack_candidate;
                                command_comp->set_command(attack_command);
                            }
                            ai_component.target = attack_candidate;
                        }
                    }
                    ai_component.last_radius_check = current_time;
                }
            }
        }
    }
};