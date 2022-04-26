#pragma once

#include "gui_system.h"

#include "wallet_component.h"

class SysShop : public GuiSystem
{

    virtual void update_gui(double dt) override
    {
        auto& selected_entities = get_array<CompSelectedObjects>();
        EntityRef selected_entity;
        if (selected_entities.size())
        {
            if (selected_entities[0].selected_objects.size())
            {
                selected_entity = selected_entities[0].selected_objects[0];
            }
        }
        if (selected_entity.is_valid())
        {
            auto& shop_inventories = get_array<CompShopInventory>();
            for (auto& shop_inventory : shop_inventories)
            {
                if (shop_inventory.visible)
                {
                    update_widget(selected_entity, shop_inventory.get_entity());
                }
            }
        }
    }

    void trigger_buy(CompWallet* shopper_wallet, CompShopInventory* shop_inventory, InventorySlot& slot)
    {
        bool has_stock = true;
        if (slot.count)
        {
            if (slot.count.value() <= 0)
            {
                has_stock = false;
            }
        }
        // If shopper has enough money
        if (has_stock && shopper_wallet->balance >= slot.cost)
        {
            // If shopper also has an inventory
            if (auto* shopper_inventory = shopper_wallet->sibling<CompInventory>())
            {
                for (int i = 0; i < shopper_inventory->items.size(); ++i)
                {
                    // If slot is empty, put bought item in it
                    if (!shopper_inventory->items[i].is_valid())
                    {
                        // Can execute buy
                        shopper_inventory->items[i] = _interface->duplicate_entity(slot.item_entity);
                        has_space = true;
                        shopper_wallet->balance -= slot.cost;
                        // If item has limited stock, decrement stock
                        if (slot.count)
                        {
                            slot.count = slot.count.value() - 1;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void update_widget(CompWallet* shopper_wallet, CompShopInventory* shop_inventory)
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
            {
                bool shopper_in_range = true;
                auto entity_pos = shop_inventory->sibling<CompPosition>()->pos;
                auto* shop_interatee = shop_inventory->sibling<CompInteractable>();
                if (auto* shopper_pos = shopper_wallet->sibling<CompPosition>())
                {
                    if (glm::length(shopper_pos->pos - entity_pos) > shop_interactee->interact_range)
                    {
                        shopper_in_range = false;
                        shop_inventory->visible = false;
                        return;
                    }
                }
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
                for (auto& slot : shop_inventory->slots)
                {
                    index++;
                    if (ImGui::ImageButton(0, ImVec2(widget_height/2 - 30, widget_height/2 - 30)))
                    {
                        if (slot.item_entity.is_valid())
                        {
                            bool bought = trigger_buy(shopper_wallet, shop_inventory, slot);
                        }
                        //ImGui::Text((std::string("CD: ") + std::to_string(ab->current_cooldown.value())).c_str());
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
