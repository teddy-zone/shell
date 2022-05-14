#pragma once 

#include "system.h"
#include "status_manager.h"

class SysMainPlayerHud : public GuiSystem
{

public:

    const int widget_margin = 10;
    const int ability_widget_height = 150;

    virtual void update_gui(double dt) override
    {
        auto& selected_entities = get_array<CompSelectedObjects>();
        if (selected_entities.size())
        {
            int my_team = 0;
            if (selected_entities[0].selected_objects.size())
            {
                auto selected_entity = selected_entities[0].selected_objects[0];
                if (auto* team_comp = selected_entity.cmp<CompTeam>())
                {
                    my_team = team_comp->team;
                }
                update_statuses(selected_entity);
                update_abilities_and_health(selected_entity);
                update_inventory(selected_entity);
            }
            update_health_bars(my_team);
        }
        update_fps_display();
    }

    void update_fps_display()
    {
            static int count = 0;
            static float fps = 0;
            static int fps_count = 0;
            count += 1;
            if (count % 15 == 0)
            {
                fps = fps_count*1.0/count;
                count = 0;
                fps_count = 0;
            }
            else
            {
                fps_count += _interface->get_current_fps();
            }
            fps = _interface->get_current_fps();
            ImGui::SetNextWindowPos(ImVec2(2,2));
            ImGui::Begin("FPS");
            ImGui::Text(std::to_string(fps).c_str());
            ImGui::End();
    }

    void update_statuses(EntityRef entity)
    {
        const auto* status_manager = entity.cmp<CompStatusManager>();
        if (status_manager)
        {
            const int status_count = status_manager->statuses.size();
            bool active;
            const int status_width = 20;
            const int widget_height = status_width + 30;
            const int widget_width = status_width * status_count + 4*20;
            ImGui::SetNextWindowPos(ImVec2(widget_margin, CompWidget::window_height - (2*widget_margin + widget_height + ability_widget_height)));
            ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
            ImGui::Begin("Statuses", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            for (auto& [application_id, application] : status_manager->statuses)
            {
                auto& [entity_id, stack_id] = application_id;
                auto* status_comp = EntityRef(entity_id).cmp<CompStatus>();
                ImGui::ImageButton(0, ImVec2(status_width, status_width));
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip(status_comp->get_name());
                }
            }

            ImGui::End();
        }
    }

    void update_health_bars(int my_team)
    {
        int index = 0;
        auto& cameras = get_array<CompCamera>();
        if (!cameras.size())
        {
            return;
        }
        auto* camera = &cameras[0].graphics_camera;
        auto& health_components = get_array<CompHealth>();
        for (auto& health_component : health_components)
        {
            auto* comp_static_mesh = health_component.sibling<CompStaticMesh>();
            if (comp_static_mesh->visible == false)
            {
                continue;
            }
            int other_team = 0;
            if (auto* team_comp = health_component.sibling<CompTeam>())
            {
                other_team = team_comp->team;
            }
            constexpr int unit_health_bar_width = 80;
            constexpr int unit_health_bar_height = 10;
            auto entity_pos = health_component.sibling<CompPosition>()->pos;
            bool active = true;
            // ABOVE UNIT HEALTH BAR
            glm::vec4 screen_space = camera->world_to_screen_space(entity_pos);
            ImVec2 p = ImVec2(screen_space.x - 40, CompWidget::window_height - screen_space.y - 50);
            if (!(p.x > 0 && p.x < CompWidget::window_width &&
                p.y > 0 && p.y < CompWidget::window_height &&
                screen_space.z < 1.0))
            { continue; }
            ImGui::SetNextWindowPos(ImVec2(p.x - 4, p.y-2));
            ImGui::SetNextWindowSize(ImVec2(unit_health_bar_width + 20, unit_health_bar_height + 20));
            ImGui::Begin(("HealthBar" + std::to_string(index)).c_str(), &active, 
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize); 
            {
                int filter_health_width = unit_health_bar_width*health_component.filtered_health_percentage/100.0;
                int current_health_width = unit_health_bar_width*health_component.health_percentage/100.0;
                std::string health_text("Health");
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRectFilled(p, ImVec2(p.x + unit_health_bar_width, p.y + unit_health_bar_height), ImColor(0,0,0), 2.0);
                int border_size = 1;
                int w_border_size = 1;
                draw_list->AddRectFilled(ImVec2(p.x+border_size, p.y+border_size), ImVec2(p.x + filter_health_width - 2*border_size, p.y + unit_health_bar_height - 2*border_size), ImColor(255,255,255), 2.0);
                ImColor health_bar_color(255,0,0);
                if (my_team == other_team)
                {
                    health_bar_color = ImColor(0,255,0);
                }
                draw_list->AddRectFilled(ImVec2(p.x+w_border_size, p.y+w_border_size), ImVec2(p.x + current_health_width - 2*w_border_size, p.y + unit_health_bar_height - 2*w_border_size), health_bar_color, 2.0);
            }
            ImGui::End();
            // END ABOVE UNIT HEALTH BAR
            index++;
        }
    }

    void update_abilities_and_health(EntityRef entity)
    {
        if (entity.is_valid())
        {
            auto entity_pos = entity.cmp<CompPosition>()->pos;

            if (auto* ab_set = entity.cmp<CompAbilitySet>())
            {
                int ability_count = 0;
                for (auto& ability : ab_set->abilities)
                {
                    if (ability.is_valid())
                    {
                        ability_count++;
                    }
                }

                bool active = true;

                const int ability_width = 100;
                const int widget_height = ability_widget_height;
                const int widget_width = ability_width * ability_count;;
                ImGui::SetNextWindowPos(ImVec2(widget_margin, CompWidget::window_height - (widget_margin + widget_height)));
                ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
                ImGui::Begin("Abilities", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                // HEALTH BAR DRAWING
                if (auto* health_comp = ab_set->sibling<CompHealth>())
                {
                    constexpr int health_bar_width = 200;
                    constexpr int health_bar_height = 20;
                    int current_health_width = health_bar_width*health_comp->health_percentage/100.0;
                    int max_disp_health = 100;
                    int cur_disp_health = health_comp->health_percentage;
                    if (auto* stat_comp = health_comp->sibling<CompStat>())
                    {
                        max_disp_health = int(stat_comp->get_abs_stat(Stat::MaxHealth));
                        cur_disp_health = max_disp_health*health_comp->health_percentage/100.0;
                    }
                    std::string health_text(std::to_string(cur_disp_health) + "/" + std::to_string(max_disp_health));
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    const ImVec2 p = ImGui::GetCursorScreenPos();
                    draw_list->AddRectFilled(p, ImVec2(p.x + health_bar_width, p.y + health_bar_height), ImColor(0,0,0));
                    draw_list->AddRectFilled(ImVec2(p.x+1, p.y+1), ImVec2(p.x + current_health_width - 2, p.y + health_bar_height - 2), ImColor(255,0,0));
                    draw_list->AddText(ImGui::GetIO().FontDefault, 14, ImVec2(p.x + 5, p.y), ImColor(255,255,255), health_text.c_str(), health_text.c_str() + health_text.size(), 200, nullptr);
                    ImGui::Dummy(ImVec2(health_bar_width + 10, health_bar_height + 10));
                }
                // END HEALTH BAR
                
                ImGui::Columns(ability_count); 
                for (auto& ability : ab_set->abilities)
                {
                    if (ability.is_valid())
                    {
                        CompAbility* ab = ability.cmp<CompAbility>();
                        ImGui::ImageButton(0, ImVec2(widget_height - 100, widget_height - 100));
                        ImGui::Text((std::string("Level: ") + std::to_string(ab->level)).c_str());
                        if (ab->current_cooldown)
                        {
                            ImGui::Text((std::string("CD: ") + std::to_string(ab->current_cooldown.value())).c_str());
                        }
                        ImGui::NextColumn();
                    }
                }
                ImGui::Columns();
                ImGui::End();
            }
        }
    }

    void update_inventory(EntityRef entity)
    {
        int index = 0;
        bool active = true;
        {
            auto entity_pos = entity.cmp<CompPosition>()->pos;
            if (auto* inventory = entity.cmp<CompInventory>())
            {
                bool active = true;
                int widget_height = 180;
                int widget_width = 250;
                ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width - (10 + widget_width), CompWidget::window_height - (10 + widget_height)));
                //ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width - (10 + widget_width), 10));
                ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
                ImGui::Begin("Inventory", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
                if (auto* wallet_comp = entity.cmp<CompWallet>())
                {
                    ImGui::Text(std::string("GOLD: " + std::to_string(wallet_comp->balance)).c_str());
                }
                else
                {
                    ImGui::Text(std::string(" ").c_str());
                }
                int item_count = 6;
                int column_count = 3;
                int row_count = 2;
                ImGui::Columns(column_count, "inventory_columns");
                int index = 0;
                for (auto& item : inventory->items)
                {
                    index++;
                    ImGui::ImageButton(0, ImVec2(widget_height/2 - 30, widget_height/2 - 30));
                    {
                        //ImGui::Text((std::string("CD: ") + std::to_string(ab->current_cooldown.value())).c_str());
                    }
                    if (item.is_valid())
                    {
                        ImGui::Text(item.cmp<CompItem>()->name.c_str());
                    }
                    else
                    {
                        ImGui::Text(" ");
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