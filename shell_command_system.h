#pragma once

#include <variant>

#include "system.h"
#include "command_component.h"
#include "nav_component.h"
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
                if (auto cmd = std::dynamic_pointer_cast<AttackCommand>(next_command)) 
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
            // if within cast range or not unit targeted and not ground targeted
            caster_comp->activate_ability(cmd->ability_index);
            // else move to target
        }
        return true;
    }

};