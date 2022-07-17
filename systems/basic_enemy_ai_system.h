#pragma once

#include <random>

#include "system.h"
#include "basic_enemy_ai_component.h"

class SysBasicEnemyAI : public System
{
    std::normal_distribution<float> wandering_normal_distribution;
    std::uniform_real_distribution<float> wandering_uniform_distribution;
    std::mt19937 wandering_generator;

public:

    SysBasicEnemyAI() :
        wandering_normal_distribution(0, 1),
        wandering_uniform_distribution(0, 1)
    {}

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
                                if (auto* ability_set_comp = ai_component.sibling<CompAbilitySet>())
                                {
                                    for (auto& ability : ability_set_comp->abilities)
                                    {
                                        if (auto* comp_ability = ability.cmp<CompAbility>())
                                        {
                                            if (auto* comp_mana = ability.cmp<CompMana>())
                                            {
                                                if (!comp_ability->current_cooldown && comp_ability->mana_cost < comp_mana->get_current_mana())
                                                {
                                                }
                                            }
                                        }
                                    }
                                }
                                AttackCommand attack_command;
                                attack_command.target = attack_candidate;
                                command_comp->set_command(attack_command);
                            }
                            ai_component.target = attack_candidate;
                        }
                        if (ai_component.target)
                        {
                            if (auto* command_comp = ai_component.sibling<CompCommand>())
                            {
                                if (auto* ability_set_comp = ai_component.sibling<CompAbilitySet>())
                                {
                                    int ability_index = 0;
                                    for (auto& ability : ability_set_comp->abilities)
                                    {
                                        if (auto* comp_ability = ability.cmp<CompAbility>())
                                        {
                                            if (auto* comp_mana = command_comp->sibling<CompMana>())
                                            {
                                                if (!comp_ability->current_cooldown && comp_ability->mana_cost < comp_mana->get_current_mana())
                                                {
                                                    bool already_commanded = false;
                                                    if (!command_comp->command_queue.empty())
                                                    {
                                                        if (std::dynamic_pointer_cast<AbilityCommand>(command_comp->command_queue.back()))
                                                        {
                                                            already_commanded = true;
                                                        }
                                                    }
                                                    if (!already_commanded)
                                                    {
                                                        if (comp_ability->unit_targeted || comp_ability->ground_targeted)
                                                        {
                                                            float dist = glm::length(command_comp->sibling<CompPosition>()->pos - ai_component.target.value().cmp<CompPosition>()->pos);
                                                            if (dist <= comp_ability->cast_range)
                                                            {
                                                                AbilityCommand ability_command;
                                                                ability_command.ability_index = ability_index;
                                                                if (comp_ability->unit_targeted)
                                                                {
                                                                    ability_command.entity_target = ai_component.target.value();
                                                                }
                                                                else if (comp_ability->ground_targeted)
                                                                {
                                                                    auto vel = ai_component.target.value().cmp<CompPhysics>()->vel;
                                                                    ability_command.ground_target = ai_component.target.value().cmp<CompPosition>()->pos + vel*0.8;
                                                                }
                                                                command_comp->set_command(StopCommand());
                                                                command_comp->queue_command(ability_command);
                                                                ai_component.ability_index_using = ability_index;
                                                                AttackCommand attack_command;
                                                                attack_command.target = ai_component.target.value();
                                                                command_comp->queue_command(attack_command);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            float dist = glm::length(command_comp->sibling<CompPosition>()->pos - ai_component.target.value().cmp<CompPosition>()->pos);
                                                            if (dist <= comp_ability->cast_range)
                                                            {
                                                                AbilityCommand ability_command;
                                                                ability_command.ability_index = ability_index;
                                                                command_comp->set_command(StopCommand());
                                                                command_comp->queue_command(ability_command);
                                                                ai_component.ability_index_using = ability_index;
                                                                AttackCommand attack_command;
                                                                attack_command.target = ai_component.target.value();
                                                                command_comp->queue_command(attack_command);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        ability_index++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // Wander around if nothings happening
                        if (auto* command_comp = ai_component.sibling<CompCommand>())
                        {
                            if (auto* nav_comp = command_comp->sibling<CompNav>())
                            {
                                if (command_comp->command_queue.empty() && !nav_comp->path_computed)
                                {
                                    const static float max_wander_radius = 25;
                                    const float wander_radius = max_wander_radius * wandering_normal_distribution(wandering_generator) + 20;
                                    const float wander_az = 2 * 3.14159 * wandering_uniform_distribution(wandering_generator);
                                    std::cout << "parameters: " << wander_radius << ", " << wander_az << "\n";
                                    const glm::vec3 wander_offset(cos(wander_az) * wander_radius, sin(wander_az) * wander_radius, 0);
                                    std::cout << "wander position: " << glm::to_string(wander_offset) << "\n";
                                    AttackMoveCommand a_move_command;
                                    a_move_command.target = pos_comp->pos + wander_offset;

                                    command_comp->queue_command(a_move_command);
                                }
                            }
                        }
                    }
                    ai_component.last_radius_check = current_time;
                }
            }
        }
    }
};