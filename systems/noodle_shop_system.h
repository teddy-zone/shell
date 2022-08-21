#pragma once

#include "gui_system.h"
#include "tooltip_component.h"
#include "keystate_component.h"
#include "camera_component.h"
#include "widget_component.h"
#include "dialog_component.h"
#include "noodle_shop_component.h"

class SysNoodleShop : public GuiSystem
{
public:
    virtual void update_gui(double dt) override
    {
        auto& keystate = get_array<CompKeyState>()[0];
        auto& camera = get_array<CompCamera>()[0];
        auto click_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
            keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
        auto full_ray = ray::New(camera.graphics_camera.get_position(), click_ray);
        auto result = _interface->fire_ray(full_ray, ray::HitType::StaticAndDynamic);

        auto& noodle_shop_components = get_array<CompNoodleShop>();
        for (auto& noodle_shop_component : noodle_shop_components)
        {
            if (noodle_shop_component.enabled)
            {

            }
        }

    }

    virtual void update(double dt) override
    {

    }
};