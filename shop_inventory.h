#pragma once

#include "component.h"
#include "stat_interface.h"
#include "widget_component.h"
#include "physics_component.h"
#include "camera.h"

struct InventorySlot
{
    EntityRef item_entity;
    std::optional<int> count; // Number of items in inventory. Unlimited if not specified
    int cost;
};

struct CompShopInventory : public Component
{
    std::vector<InventorySlot> slots;
    bgfx::Camera* camera;
    bool visible = false;

};
