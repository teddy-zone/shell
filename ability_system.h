#pragma once

#include "system.h"
#include "ability.h"
#include "ability_set_component.h"
#include "selected_objects_component.h"
#include "health_component.h"

struct CompAbilityWidget : public CompWidget
{

    EntityRef entity;

    CompAbilityWidget()
    {
    }

    virtual void tick() 
    {
        if (entity.is_valid())
        {
            if (auto* ab_set = entity.cmp<CompAbilitySet>())
            {
                int widget_height = 100;
                int widget_width = 800;
                ImGui::SetNextWindowPos(ImVec2(10, CompWidget::window_height - (10 + widget_height)));
                ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
                bool active = true;
                ImGui::Begin("Abilities", &active, ImGuiWindowFlags_NoTitleBar);

                // HEALTH BAR DRAWING
                if (auto* health_comp = ab_set->sibling<CompHealth>())
                {
                    std::string health_text("Health");
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    const ImVec2 p = ImGui::GetCursorScreenPos();
                    draw_list->AddRectFilled(p, ImVec2(p.x + 200, p.y + 20), ImColor(0,0,0));
                    draw_list->AddRectFilled(ImVec2(p.x+1, p.y+1), ImVec2(p.x + 200 - 2, p.y + 20 - 2), ImColor(255,0,0));
                    draw_list->AddText(ImGui::GetIO().FontDefault, 14, ImVec2(p.x + 5, p.y), ImColor(255,255,255), health_text.c_str(), health_text.c_str() + health_text.size(), 200, nullptr);
                    ImGui::Dummy(ImVec2(200 + 10, 20 + 10));
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
                        ImGui::ImageButton(0, ImVec2(widget_height - 55, widget_height - 55));
                        ImGui::Text((std::string("Level: ") + std::to_string(ab->level)).c_str());
                        if (ab->current_cooldown)
                        {
                            ImGui::Text((std::string("CD: ") + std::to_string(ab->current_cooldown.value())).c_str());
                        }
                    }
                    ImGui::NextColumn();
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
            _interface->add_entity_from_proto(instance_comp->proto);
        }
    }

};