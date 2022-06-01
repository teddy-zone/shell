#pragma once

#include <variant>

#include "system.h"
#include "command_component.h"
#include "nav_component.h"
#include "interactable_component.h"
#include "ability.h"
//#include "ability.h"

class SysCommandShell : public System
{

public:
    virtual void update(double dt) override
    {
        auto& command_components = get_array<CompCommand>();
        for (auto& command_component : command_components)
        {
            auto* nav_comp = command_component.sibling<CompNav>();
            auto* stat_component = command_component.sibling<CompStat>();
            if (stat_component)
            {
                if (stat_component->get_status_state(StatusState::Stunned))
                {
                    if (nav_comp)
                    {
                        nav_comp->stop();
                    }
                    continue;
                }
            }
            if (!command_component.command_queue.empty())
            {
                auto next_command = command_component.command_queue.back(); 
                if (auto cmd = std::dynamic_pointer_cast<StopCommand>(next_command)) 
                {
                    auto* caster_comp = nav_comp->sibling<CompCaster>();
                    if (nav_comp)
                    {
                        nav_comp->stop();
                    }
                    command_component.command_queue.pop_back();
                    command_component.new_command = true;
                    caster_comp->state = AbilityState::None;
                } 
                else if (auto cmd = std::dynamic_pointer_cast<AttackCommand>(next_command)) 
                {
                    auto* nav_comp = command_component.sibling<CompNav>();
                    auto attack_ability_command = std::make_shared<AbilityCommand>();
                    attack_ability_command->entity_target = cmd->target;
                    attack_ability_command->ability_index = 10;
                    if (process_ability_command(attack_ability_command, nav_comp, command_component.new_command))
                    {
                        //command_component.command_queue.pop_back();
                        //command_component.new_command = true;
                    }
                    else
                    {
                        command_component.new_command = false;
                    }
                } 
                else if (auto cmd = std::dynamic_pointer_cast<AttackMoveCommand>(next_command)) 
                {
                    auto* nav_comp = command_component.sibling<CompNav>();
                    if (nav_comp && command_component.new_command)
                    {
                        nav_comp->set_destination(cmd->target);
                    }
                    auto* pos_comp = command_component.sibling<CompPosition>();
                    auto attackable_entities = _interface->data_within_sphere_selective(pos_comp->pos, 50, {uint32_t(type_id<CompAttackable>)});
                    EntityRef closest_entity;
                    float closest_distance = 1000000;
                    for (auto& ent : attackable_entities)
                    {
                        float dist = glm::length(pos_comp->pos - ent.cmp<CompPosition>()->pos);
                        if (ent.get_id() != pos_comp->get_id() && dist < closest_distance)
                        {
                            closest_entity = ent;
                            closest_distance = dist;
                        }
                    }
                    if (closest_entity.is_valid())
                    { 
                        AttackCommand attack_command;
                        attack_command.target = closest_entity;
                        command_component.set_command(attack_command);
                    }
                } 
                else if (auto cmd = std::dynamic_pointer_cast<AbilityCommand>(next_command)) 
                {
                    auto* nav_comp = command_component.sibling<CompNav>();
                    if (process_ability_command(cmd, nav_comp, command_component.new_command))
                    {
                        command_component.command_queue.pop_back();
                        command_component.new_command = true;
                    }
                    else
                    {
                        command_component.new_command = false;
                    }
                } 
                else if (auto cmd = std::dynamic_pointer_cast<InteractCommand>(next_command)) 
                {
                    //auto* interact_comp = command_component.sibling<CompInteractable>();
                    auto* nav_comp = command_component.sibling<CompNav>();
                    if (process_interact_command(cmd, nav_comp, command_component.new_command))
                    {
                        command_component.command_queue.pop_back();
                        command_component.new_command = true;
                    }
                } 
                else 
                { // std::monostate
                // ...
                }
            }
        }
    }

    void process_attack_command(std::shared_ptr<AttackCommand> cmd, CompNav* nav_component, bool is_new)
    {

    }

    bool process_ability_command(std::shared_ptr<AbilityCommand> cmd, CompNav* nav_component, bool is_new)
    {
        auto* caster_comp = nav_component->sibling<CompCaster>();
        if (caster_comp)
        {
            auto my_loc = caster_comp->sibling<CompPosition>()->pos;
            //auto* ability_comp = caster_comp->sibling<CompAbilitySet>()->get_ability_component_by_index(cmd->ability_index);
            auto* ability_comp = caster_comp->get_ability(cmd->ability_index);
            if (ability_comp->level <= 0)
            {
                return false;
            }

            // if within cast range or not unit targeted and not ground targeted
            if (cmd->entity_target)
            {
                if (cmd->entity_target.value().is_valid())
                {
                    auto target_loc = cmd->entity_target.value().cmp<CompPosition>()->pos;
                    auto range = glm::length(target_loc - my_loc);
                    if (range < ability_comp->cast_range )
                    {
                        if (caster_comp->state == AbilityState::None)
                        {
                            caster_comp->activate_ability(cmd->ability_index);
                            caster_comp->unit_target = cmd->entity_target;
                            caster_comp->ground_target = std::nullopt;
                        }
                    }
                    else
                    {
                        // else move to target
                        if (is_new || !nav_component->path_computed)
                        {
                            nav_component->set_destination(cmd->entity_target.value());
                            return false;
                        }
                    }
                }
                else
                {
                    return true;
                }
            }
            else if (cmd->ground_target)
            {
                auto range = glm::length(my_loc - cmd->ground_target.value());
                if (range < ability_comp->cast_range && caster_comp->state == AbilityState::None && !caster_comp->activated)
                {
                    caster_comp->activate_ability(cmd->ability_index);
                    caster_comp->ground_target = cmd->ground_target;
                    caster_comp->unit_target = std::nullopt;
                    caster_comp->activated = true;
                }
                else
                {
                    // else move to target
                }
            }
        }
        if (caster_comp->state == AbilityState::None)
        {
            caster_comp->activated = false;
            return true;
        }
        else if (caster_comp->ability_index != cmd->ability_index)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool process_interact_command(std::shared_ptr<InteractCommand> cmd, CompNav* nav_component, bool is_new)
    {
        {
            auto* interact_comp = cmd->interact_target.cmp<CompInteractable>();
            auto my_loc = nav_component->sibling<CompPosition>()->pos;
            // if within cast range or not unit targeted and not ground targeted
            if (cmd->interact_target.is_valid())
            {
                auto target_loc = cmd->interact_target.cmp<CompPosition>()->pos;
                auto range = glm::length(target_loc - my_loc);
                if (range < interact_comp->interact_range)
                {
                    interact_comp->interaction_callback(_interface, nav_component->get_entity(), cmd->interact_target); 
                }
                else
                {
                    // else move to target
                }
            }
        }
        return true;
    }

};
