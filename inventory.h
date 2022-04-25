#pragma once

#include "component.h"
#include "stat_interface.h"
#include "widget_component.h"
#include "physics_component.h"

struct CompInventory : public StatInterface, public CompWidget
{

    std::array<EntityRef, 6> items;

    virtual StatPart get_stat(Stat stat) override;

    virtual void tick() 
    {
        int index = 0;
        bool active = true;
        {
            auto entity_pos = sibling<CompPosition>()->pos;

            {
                bool active = true;
                int widget_height = 150;
                int widget_width = 250;
                ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width - (10 + widget_width), CompWidget::window_height - (10 + widget_height)));
                //ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width - (10 + widget_width), 10));
                ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
                ImGui::Begin("Inventory", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                int item_count = 6;
                int column_count = 3;
                int row_count = 2;
                ImGui::Columns(column_count, "inventory_columns");
                int index = 0;
                for (auto& item : items)
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
    };

};