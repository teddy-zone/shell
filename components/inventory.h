#pragma once

#include "component.h"
#include "stat_interface.h"
#include "widget_component.h"
#include "physics_component.h"
#include "camera.h"

struct CompInventory : public StatInterface, public CompWidget
{

    std::array<EntityRef, 6> items;

    virtual StatPart get_stat(Stat stat) override;

    virtual void tick() 
    {
    };

};