#include "inventory.h"

#include "stat_component.h"

StatPart CompInventory::get_stat(Stat stat) 
{
    StatPart out_part;
    for (auto& item_eid : items)
    {
        if (item_eid >= 0)
        {
            auto single_item_stat_comp = (CompStat*)get_component(type_id<CompStat>, item_eid);
            out_part = out_part.join(single_item_stat_comp->get_stat(stat));
        }
    }
    return out_part;
}