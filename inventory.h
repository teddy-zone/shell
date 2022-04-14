#pragma once

#include "component.h"
#include "stat_interface.h"

struct CompInventory : public Component, public StatInterface
{

    std::array<EntityRef, 6> items;

    virtual StatPart get_stat(Stat stat) override;

};