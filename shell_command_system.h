#pragma once

#include <variant>

#include "system.h"
#include "command_component.h"
#include "nav_component.h"
#include "interactable_component.h"
//#include "ability.h"

class SysCommandShell : public System
{

public:
    virtual void update(double dt) override
    {
        auto& command_components = get_array<CompCommand>();
        for (auto& command_component : command_components)
        {
            if (!command_component.command_queue.empty())
            {
                auto next_command = command_component.command_queue.back(); 
                if (auto cmd = std::dynamic_pointer_cast<StopCommand>(next_command)) 
                {
                    auto* nav_comp = command_component.sibling<CompNav>();
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
                    process_attack_command(cmd, nav_comp, command_component.new_command);
                } 
                else if (auto cmd = std::dynamic_pointer_cast<AbilityCommand>(next_command)) 
                {
                    auto* nav_comp = command_component.sibling<CompNav>();
                    if (process_ability_command(cmd, nav_comp, command_component.new_command))
                    {
                        command_component.command_queue.pop_back();
                        command_component.new_command = true;
                    }
                } 
                else if (auto cmd = std::dynamic_pointer_cast<InteractCommand>(next_command)) 
                {
                    auto* interact_comp = command_component.sibling<CompInteractable>();
                    if (process_interact_command(cmd, interact_comp, command_component.new_command))
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
            auto* ability_comp = caster_comp->sibling<CompAbilitySet>()->get_ability_component_by_index(cmd->ability_index);
            // if within cast range or not unit targeted and not ground targeted
            if (cmd->entity_target)
            {
                auto target_loc = cmd->entity_target.value().cmp<CompPosition>()->pos;
                auto range = glm::length(target_loc - my_loc);
                if (range < ability_comp->cast_range)
                {
                    caster_comp->activate_ability(cmd->ability_index);
                    caster_comp->unit_target = cmd->entity_target;
                }
                else
                {
                    // else move to target
                }
            }
            else if (cmd->ground_target)
            {
                auto range = glm::length(my_loc - cmd->ground_target.value());
                if (range < ability_comp->cast_range)
                {
                    caster_comp->activate_ability(cmd->ability_index);
                    caster_comp->ground_target = cmd->ground_target;
                }
                else
                {
                    // else move to target
                }
            }
        }
        return true;
    }

    bool process_interact_command(std::shared_ptr<InteractCommand> cmd, CompInteractable* interact_component, bool is_new)
    {
        return true;
    }

};
