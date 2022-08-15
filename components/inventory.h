#pragma once

#include "component.h"
#include "stat_interface.h"
#include "widget_component.h"
#include "physics_component.h"
#include "camera.h"
#include "item_component.h"

struct CompInventory : public StatInterface, public CompWidget
{

    std::array<EntityRef, 6> items;

    void insert_item(EntityRef& in_item)
    {
        for (int i = 0; i < 6; ++i)
        {
            if (items[i].is_valid())
            {
                if (items[i].cmp<CompItem>()->name == in_item.cmp<CompItem>()->name &&
                    items[i].cmp<CompItem>()->has_charges &&
                    items[i].cmp<CompItem>()->num_charges < items[i].cmp<CompItem>()->max_charges)
                {
                    items[i].cmp<CompItem>()->num_charges++;
                    return;
                }
            }
        }
        for (int i = 0; i < 6; ++i)
        {
            if (!items[i].is_valid())
            {
                items[i] = in_item;
                return;
            }
        }
    }

    bool has_open_slot() const
    {
        for (int i = 0; i < 6; ++i)
        {
            if (!items[i].is_valid())
            {
                return true;
            }
        }
        return false;
    }

    virtual StatPart get_stat(Stat stat) override;

    virtual bool get_status_state(StatusState state) override;

    virtual void tick() 
    {
    };

};