#pragma once

#include "component.h"
#include "stat_interface.h"
#include "widget_component.h"
#include "physics_component.h"
#include "camera.h"

struct CompShopInventory : public Component
{

    std::array<EntityRef, 6> items;
    bgfx::Camera* camera;
    bool visible = false;

};