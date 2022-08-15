#include "stat_component.h"

#include "inventory.h"
#include "ability_set_component.h"
#include "status_manager.h"

StatPart CompStat::get_stat(Stat stat) 
{

    StatPart out_part;
    if (_parts.count(stat))
    {
        int num_charges = 1;
        if (auto* item_comp = sibling<CompItem>())
        {
            if (item_comp->has_charges)
            {
                num_charges = item_comp->num_charges;
            }
        }
        out_part = _parts.at(stat);
        out_part.addition = out_part.addition * num_charges;
        out_part.multiplication = std::pow(out_part.multiplication,num_charges);
    }
    auto ability_set_comp = sibling<CompAbilitySet>();
    if (ability_set_comp)
    {
        out_part = out_part.join(ability_set_comp->get_stat(stat));
    }
    auto inventory_comp = sibling<CompInventory>();
    if (inventory_comp)
    {
        out_part = out_part.join(inventory_comp->get_stat(stat));
    }
    auto status_manager_comp = sibling<CompStatusManager>();
    if (status_manager_comp)
    {
        out_part = out_part.join(status_manager_comp->get_stat(stat));
    }
    return out_part;
}

bool CompStat::get_status_state(StatusState state) 
{
    if (_states.count(state))
    {
        auto out_part = _states.at(state);
        if (out_part)
        {
            return true;
        }
    }
    auto ability_set_comp = sibling<CompAbilitySet>();
    if (ability_set_comp)
    {
        if (ability_set_comp->get_status_state(state))
        {
            return true;
        }
    }
    auto inventory_comp = sibling<CompInventory>();
    if (inventory_comp)
    {
        if (inventory_comp->get_status_state(state))
        {
            return true;
        }
    }
    auto status_manager_comp = sibling<CompStatusManager>();
    if (status_manager_comp)
    {
        if (status_manager_comp->get_status_state(state))
        {
            return true;
        }
    }
    return false;
}