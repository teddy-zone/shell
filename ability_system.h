#pragma once

#include "system.h"
#include "ability.h"
#include "ability_set_component.h"
#include "selected_objects_component.h"
#include "health_component.h"
#include "camera_component.h"
#include "on_cast_component.h"
#include "team_component.h"

struct CompAbilityWidget : public CompWidget
{

    EntityRef entity;
    bgfx::Camera* camera;

    CompAbilityWidget()
    {
    }

    virtual void tick() 
    {
        if (entity.is_valid())
        {
            auto entity_pos = entity.cmp<CompPosition>()->pos;

            if (auto* ab_set = entity.cmp<CompAbilitySet>())
            {
                bool active = true;
                // ABOVE UNIT HEALTH BAR
                glm::vec4 screen_space = camera->world_to_screen_space(entity_pos);
                ImVec2 p = ImVec2(screen_space.x - 40, CompWidget::window_height - screen_space.y - 50);
                ImGui::SetNextWindowPos(p);
                ImGui::Begin("HealthBar", &active, 
                    ImGuiWindowFlags_NoBackground |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize); 
                if (auto* health_comp = ab_set->sibling<CompHealth>())
                {
                    constexpr int unit_health_bar_width = 80;
                    constexpr int unit_health_bar_height = 10;
                    int filter_health_width = unit_health_bar_width*health_comp->filtered_health_percentage/100.0;
                    int current_health_width = unit_health_bar_width*health_comp->health_percentage/100.0;
                    std::string health_text("Health");
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    draw_list->AddRectFilled(p, ImVec2(p.x + unit_health_bar_width, p.y + unit_health_bar_height), ImColor(0,0,0));
                    draw_list->AddRectFilled(ImVec2(p.x+1, p.y+1), ImVec2(p.x + filter_health_width - 2, p.y + unit_health_bar_height - 2), ImColor(255,255,255));
                    draw_list->AddRectFilled(ImVec2(p.x+1, p.y+1), ImVec2(p.x + current_health_width - 2, p.y + unit_health_bar_height - 2), ImColor(255,0,0));
                }
                ImGui::End();
                // END ABOVE UNIT HEALTH BAR

                int widget_height = 150;
                int widget_width = 800;
                ImGui::SetNextWindowPos(ImVec2(10, CompWidget::window_height - (10 + widget_height)));
                ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
                ImGui::Begin("Abilities", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                // HEALTH BAR DRAWING
                if (auto* health_comp = ab_set->sibling<CompHealth>())
                {
                    constexpr int health_bar_width = 200;
                    constexpr int health_bar_height = 20;
                    int current_health_width = health_bar_width*health_comp->health_percentage/100.0;
                    std::string health_text("Health");
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    const ImVec2 p = ImGui::GetCursorScreenPos();
                    draw_list->AddRectFilled(p, ImVec2(p.x + health_bar_width, p.y + health_bar_height), ImColor(0,0,0));
                    draw_list->AddRectFilled(ImVec2(p.x+1, p.y+1), ImVec2(p.x + current_health_width - 2, p.y + health_bar_height - 2), ImColor(255,0,0));
                    draw_list->AddText(ImGui::GetIO().FontDefault, 14, ImVec2(p.x + 5, p.y), ImColor(255,255,255), health_text.c_str(), health_text.c_str() + health_text.size(), 200, nullptr);
                    ImGui::Dummy(ImVec2(health_bar_width + 10, health_bar_height + 10));
                }
                // END HEALTH BAR

                int ability_count = 0;
                for (auto& ability : ab_set->abilities)
                {
                    if (ability.is_valid())
                    {
                        ability_count++;
                    }
                }
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
    };
};

class SysAbility : public System
{

public:
    virtual void update(double dt)
    {
        auto& caster_components = get_array<CompCaster>();
        bgfx::Camera* cam = &get_array<CompCamera>()[0].graphics_camera;
        for (auto& caster_component : caster_components)
        {
            auto ability_set_comp = caster_component.sibling<CompAbilitySet>();
            for (auto& ab_ref : ability_set_comp->abilities)
            {
                if (ab_ref.is_valid())
                {
                    if (auto* ab = ab_ref.cmp<CompAbility>())
                    {
                        if (ab->current_cooldown)
                        {
                            ab->current_cooldown.value() -= dt;
                            if (ab->current_cooldown <= 0)
                            {
                                ab->current_cooldown = std::nullopt;
                            }
                        }
                    }
                }
            }
            if (caster_component.ability_index >= 0)
            {
                CompAbility* ability = nullptr;
                if (caster_component.ability_index < 6)
                {
                    if (ability_set_comp->abilities[caster_component.ability_index].is_valid())
                    {
                        ability = ability_set_comp->abilities[caster_component.ability_index].cmp<CompAbility>();
                    }
                }
                if (!ability)
                {
                    continue;
                }
                switch (caster_component.state)
                {
                    case AbilityState::CastPoint:
                        {
                            // if state_time >= cast point of ability
                            // Activate ability!
                            // set state to backswing, state time to zero
                            if (caster_component.state_time >= ability->cast_point)
                            {
                                caster_component.state = AbilityState::Backswing;
                                caster_component.state_time = 0.0;
                                cast_ability(&caster_component);
                            }
                            else
                            {
                                caster_component.state_time += dt;
                            }
                        }
                        break;
                    case AbilityState::Backswing:
                        {
                            // if state_time >= backswing of ability
                            // set state to None
                            if (caster_component.state_time >= ability->backswing)
                            {
                                caster_component.state = AbilityState::None;
                                caster_component.state_time = 0.0;
                            }
                            else
                            {
                                caster_component.state_time += dt;
                            }
                        }
                        break;
                    case AbilityState::None:
                        {
                        }
                        break;
                }
            }
        }
        auto& selected_object_component = get_array<CompSelectedObjects>();
        if (selected_object_component.size())
        {
            if (selected_object_component[0].selected_objects.size())
            {
                EntityRef cur_obj = selected_object_component[0].selected_objects[0];
                auto& ability_widgets = get_array<CompAbilityWidget>();
                if (ability_widgets.size())
                {
                    ability_widgets[0].entity = cur_obj;
                    ability_widgets[0].camera = cam;
                }
            }
        }
    }

    void cast_ability(CompCaster* caster)
    {
        auto* ability_set = caster->sibling<CompAbilitySet>();
        auto* ab = ability_set->abilities[caster->ability_index].cmp<CompAbility>();
        ab->current_cooldown = 
            ab->cooldown;
        if (auto* instance_comp = ab->sibling<CompAbilityInstance>())
        {
            auto ability_instance = _interface->add_entity_from_proto(instance_comp->proto.get());
            if (auto* on_cast_comp = ab->sibling<CompOnCast>())
            {
                for (auto& on_cast_func : on_cast_comp->on_cast_callbacks)
                {
                    on_cast_func(ability_instance);
                }
            }
            if (caster->unit_target)
            {
                ability_instance.cmp<CompPosition>()->pos = caster->unit_target.value().cmp<CompPosition>()->pos;
            }
            else if (caster->ground_target)
            {
                ability_instance.cmp<CompPosition>()->pos = caster->ground_target.value();
            }
        }
    }

    void update_applicators(std::vector<CompRadiusApplication>& applicator_array)
    {
        for (auto& applicator : applicator_array)
        {
            Team my_team = 0;
            if (auto* my_team_comp = applicator.sibling<CompTeam>())
            {
                my_team = my_team_comp->team;
            }
            if (_interface->get_current_game_time() - applicator.last_tick_time > applicator.tick_time)
            {
                if (auto* pos_comp = applicator.sibling<CompPosition>())
                {
                    auto pos = pos_comp->pos;
                    auto inside_entities = _interface->data_within_sphere_selective(pos, applicator.radius, {{uint32_t(type_id<CompHealth>)}});
                    for (auto& inside_entity : inside_entities)
                    {
                        Team other_team = 0;
                        if (auto* team_comp = inside_entity.cmp<CompTeam>())
                        {
                            other_team = team_comp->team;
                        }
                        if (applicator.damage)
                        {
                            if (my_team == other_team)
                            {
                                if (applicator.apply_to_same_team)
                                {
                                    auto* other_health = inside_entity.cmp<CompHealth>();
                                    other_health->apply_damage(applicator.damage.value());
                                }
                            }
                            if (my_team != other_team)
                            {
                                if (applicator.apply_to_other_teams)
                                {
                                    auto* other_health = inside_entity.cmp<CompHealth>();
                                    other_health->apply_damage(applicator.damage.value());
                                }
                            }
                        }
                    }
                }
                applicator.last_tick_time = _interface->get_current_game_time();
            }
        }
    }
};