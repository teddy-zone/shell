#include "stat_component.h"

#include "inventory.h"
#include "ability_set_component.h"
#include "status_manager.h"

StatPart CompStat::get_stat(Stat stat) 
{
    StatPart out_part = _parts.at(stat);
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