#pragma once

#include "gui_system.h"

class SysShop : public GuiSystem
{

    virtual void update_gui(double dt) override
    {
        auto& shop_inventories = get_array<CompShopInventory>();
        for (auto& shop_inventory : shop_inventories)
        {
            if (shop_inventory.visible)
            {
                update_widget(shop_inventory.get_entity());
            }
        }
    }

    void update_widget(EntityRef entity)
    {
        int index = 0;
        auto& cameras = get_array<CompCamera>();
        if (!cameras.size())
        {
            return;
        }
        auto* camera = &cameras[0].graphics_camera;
        bool active = true;
        {
            auto entity_pos = entity.cmp<CompPosition>()->pos;
            {
                bool active = true;
                int widget_height = 150;
                int widget_width = 250;

                glm::vec4 screen_space = camera->world_to_screen_space(entity_pos);
                ImVec2 p = ImVec2(screen_space.x - 40, CompWidget::window_height - screen_space.y - 50);
                ImGui::SetNextWindowPos(ImVec2(p.x - 4, p.y-2));

                //ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width - (10 + widget_width), 10));
                ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
                ImGui::Begin("ShopInventory", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                int item_count = 6;
                int column_count = 3;
                int row_count = 2;
                ImGui::Columns(column_count, "inventory_columns");
                int index = 0;
                for (auto& item : entity.cmp<CompShopInventory>()->items)
                {
                    index++;
                    //if (item.is_valid())
                    {
                        ImGui::ImageButton(0, ImVec2(widget_height/2 - 30, widget_height/2 - 30));
                        {
                            //ImGui::Text((std::string("CD: ") + std::to_string(ab->current_cooldown.value())).c_str());
                        }
                    }
                    if (index % row_count == 0)
                    {
                        ImGui::NextColumn();
                    }
                }
                ImGui::Columns();
                ImGui::End();
            }
        }
    }
};