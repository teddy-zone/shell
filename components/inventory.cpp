#include "inventory.h"

#include "stat_component.h"

StatPart CompInventory::get_stat(Stat stat) 
{
    StatPart out_part;
    for (auto& item_ref : items)
    {
        if (item_ref.is_valid())
        {
            if (auto* single_item_stat_comp = item_ref.cmp<CompStat>())
            {
                out_part = out_part.join(single_item_stat_comp->get_stat(stat));
            }
        }
    }
    return out_part;
}

bool CompInventory::get_status_state(StatusState state) 
{
    for (auto& item_ref : items)
    {
        if (item_ref.is_valid())
        {
            if (auto* single_item_stat_comp = item_ref.cmp<CompStat>())
            {
                if (single_item_stat_comp->get_status_state(state))
                {
                    return true;
                }
            }
        }
    }
    return false;
}