#pragma once

#include <iostream>

#include "system.h"
#include "ability.h"
#include "ability_set_component.h"
#include "selected_objects_component.h"
#include "health_component.h"
#include "camera_component.h"
#include "on_cast_component.h"
#include "team_component.h"
#include "attackable_component.h"

struct CompAbilityWidget : public CompWidget
{

    EntityRef entity;
    std::vector<CompHealth>* health_components = nullptr;
    bgfx::Camera* camera;

    CompAbilityWidget()
    {
    }

    virtual void tick() 
    {
        if (!health_components) return;
        int index = 0;
        for (auto& health_component : *health_components)
        {
            constexpr int unit_health_bar_width = 80;
            constexpr int unit_health_bar_height = 10;
            auto entity_pos = health_component.sibling<CompPosition>()->pos;
            bool active = true;
            // ABOVE UNIT HEALTH BAR
            glm::vec4 screen_space = camera->world_to_screen_space(entity_pos);
            ImVec2 p = ImVec2(screen_space.x - 40, CompWidget::window_height - screen_space.y - 50);
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
                draw_list->AddRectFilled(ImVec2(p.x+w_border_size, p.y+w_border_size), ImVec2(p.x + current_health_width - 2*w_border_size, p.y + unit_health_bar_height - 2*w_border_size), ImColor(255,0,0), 2.0);
            }
            ImGui::End();
            // END ABOVE UNIT HEALTH BAR
            index++;
        }
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
                const int widget_height = 150;
                const int widget_width = ability_width * ability_count;;
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
            auto* nav_comp = caster_component.sibling<CompNav>();
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
            auto inventory_comp = caster_component.sibling<CompInventory>();
            for (auto& item_ref : inventory_comp->items)
            {
                if (item_ref.is_valid())
                {
                    if (auto* ab = item_ref.cmp<CompAbility>())
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
            if (auto* attacker_comp = caster_component.sibling<CompAttacker>())
            {
                if (attacker_comp->attack_ability.is_valid())
                {
                    if (auto* ab = attacker_comp->attack_ability.cmp<CompAbility>())
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
                if (caster_component.ability_index < 11)
                {
                    ability = caster_component.get_ability(caster_component.ability_index);
                }
                if (!ability)
                {
                    continue;
                }
                const glm::vec3 default_color(0.9);
                switch (caster_component.state)
                {
                    case AbilityState::CastPoint:
                        {
                            const float cast_point_done_fraction = caster_component.state_time/ability->cast_point;
                            if (auto* static_mesh_component = caster_component.sibling<CompStaticMesh>())
                            {
                                static_mesh_component->mesh.get_mesh()->set_solid_color(
                                    glm::vec4(ability->cast_color*cast_point_done_fraction + default_color*(1 - cast_point_done_fraction), 1.0)
                                );
                            }
                            nav_comp->stop();
                            if (auto* mana_comp = caster_component.sibling<CompMana>())
                            {
                                if (ability->mana_cost > mana_comp->get_current_mana())
                                {
                                    caster_component.state = AbilityState::None;
                                    caster_component.state_time = 0.0;
                                }
                            }
                            else
                            {
                                    caster_component.state = AbilityState::None;
                                    caster_component.state_time = 0.0;
                            }
                            if (ability->current_cooldown)
                            {
                                caster_component.state = AbilityState::None;
                                caster_component.state_time = 0.0;
                            }
                            // if state_time >= cast point of ability
                            // Activate ability!
                            // set state to backswing, state time to zero
                            else if (caster_component.state_time >= ability->cast_point)
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
                            const float cast_point_done_fraction = caster_component.state_time/ability->backswing;
                            if (auto* static_mesh_component = caster_component.sibling<CompStaticMesh>())
                            {
                                static_mesh_component->mesh.get_mesh()->set_solid_color(
                                    glm::vec4(ability->cast_color*(1 - cast_point_done_fraction) + default_color*(cast_point_done_fraction), 1.0)
                                );
                            }
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
                            if (auto* static_mesh_component = caster_component.sibling<CompStaticMesh>())
                            {
                                static_mesh_component->mesh.get_mesh()->set_solid_color(
                                    glm::vec4(default_color, 1.0)
                                );
                            }
                        }
                        break;
                }
            }
        }
        auto& selected_object_component = get_array<CompSelectedObjects>();
        auto& ability_widgets = get_array<CompAbilityWidget>();
        if (selected_object_component.size())
        {
            if (selected_object_component[0].selected_objects.size())
            {
                EntityRef cur_obj = selected_object_component[0].selected_objects[0];
                if (ability_widgets.size())
                {
                    ability_widgets[0].entity = cur_obj;
                }
            }
        }
        if (ability_widgets.size())
        {
            ability_widgets[0].camera = cam;
            ability_widgets[0].health_components = &get_array<CompHealth>();
        }

        auto& radius_applicators = get_array<CompRadiusApplication>();
        update_applicators(radius_applicators);
    }

    void cast_ability(CompCaster* caster)
    {
        printf("Cast!\n");
        auto* ab = caster->get_ability();
        if (auto* mana_comp = caster->sibling<CompMana>())
        {
            if (!mana_comp->use_mana(ab->mana_cost))
            {
                return;
            }
        }
        auto* ability_set = caster->sibling<CompAbilitySet>();
        int caster_team = 0;
        if (auto* caster_team_comp = caster->sibling<CompTeam>())
        {
            caster_team = caster_team_comp->team;
        }
        //auto* ab = ability_set->abilities[caster->ability_index].cmp<CompAbility>();
        ab->current_cooldown = 
            ab->cooldown;

        std::optional<EntityRef> instance_entity;
        if (auto* instance_comp = ab->sibling<CompAbilityInstance>())
        {
            auto ability_instance = _interface->add_entity_from_proto(instance_comp->proto.get());
            if (instance_comp->intersect_callback)
            {
                _interface->add_component(instance_comp->intersect_callback.value(), ability_instance.get_id());
            }
            if (instance_comp->hit_callback)
            {
                _interface->add_component(instance_comp->hit_callback.value(), ability_instance.get_id());
            }
            if (instance_comp->radial_application)
            {
                _interface->add_component(instance_comp->radial_application.value(), ability_instance.get_id());
            }
            if (instance_comp->destruction_callback)
            {
                _interface->add_component(instance_comp->destruction_callback.value(), ability_instance.get_id());
            }
            instance_entity = ability_instance;
            if (auto* owner_comp = ability_instance.cmp<CompHasOwner>())
            {
                owner_comp->owner = ab->get_entity();
            }
            if (auto* team_comp = ability_instance.cmp<CompTeam>())
            {
                team_comp->team = caster_team;
            }
            if (auto* rad_app = ability_instance.cmp<CompRadiusApplication>())
            {
                rad_app->radius = ab->radius;
                if (rad_app->damage)
                {
                    rad_app->damage.value().applier = caster->get_entity();
                }
            }
            if (auto* decal = ability_instance.cmp<CompDecal>())
            {
                decal->decal.radius = ab->radius;
            }
            if (caster->unit_target)
            {
                if (auto* projectile_comp = ability_instance.cmp<CompProjectile>())
                {
                    ability_instance.cmp<CompPosition>()->pos = caster->sibling<CompPosition>()->pos;
                    projectile_comp->homing_target = caster->unit_target.value();
                }
                else
                {
                    ability_instance.cmp<CompPosition>()->pos = caster->unit_target.value().cmp<CompPosition>()->pos;
                }
            }
            else if (caster->ground_target)
            {
                if (auto* projectile_comp = ability_instance.cmp<CompProjectile>())
                {
                    ability_instance.cmp<CompPosition>()->pos = caster->sibling<CompPosition>()->pos;
                    float dist = glm::length(caster->ground_target.value() - caster->sibling<CompPosition>()->pos);
                    auto dir = glm::normalize(caster->ground_target.value() - caster->sibling<CompPosition>()->pos);
                    projectile_comp->direction = dir;
                    projectile_comp->origin = ability_instance.cmp<CompPosition>()->pos;
                    projectile_comp->max_range = dist;
                }
                else
                {
                    ability_instance.cmp<CompPosition>()->pos = caster->ground_target.value();
                }
            }
        }
        if (auto* on_cast_comp = ab->sibling<CompOnCast>())
        {
            for (auto& on_cast_func : on_cast_comp->on_cast_callbacks)
            {
                on_cast_func(_interface, caster->get_entity(), caster->ground_target, caster->unit_target, instance_entity);
            }
        }
        caster->unit_target = std::nullopt;
        caster->ground_target = std::nullopt;
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
                        if (applicator.apply_once)
                        {
                            if (applicator.applied_already.find(inside_entity.get_id()) != applicator.applied_already.end())
                            {
                                continue;
                            }
                        }
                        applicator.applied_already.insert(inside_entity.get_id());
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
                        if (applicator.owner_damage)
                        {
                            if (auto* has_owner = applicator.sibling<CompHasOwner>())
                            {
                                if (has_owner->owner.is_valid())
                                {
                                    if (auto* ab_comp = has_owner->owner.cmp<CompAbility>())
                                    {
                                        if (ab_comp->damages.size() > applicator.owner_damage.value().damage_index)
                                        {
                                            if (my_team == other_team)
                                            {
                                                if (applicator.apply_to_same_team)
                                                {
                                                    if (auto* other_health = inside_entity.cmp<CompHealth>())
                                                    {
                                                        auto copied_damage = ab_comp->damages[applicator.owner_damage.value().damage_index];
                                                        copied_damage.applier = has_owner->get_root_owner();
                                                        other_health->apply_damage(copied_damage);
                                                    }
                                                }
                                            }
                                            if (my_team != other_team)
                                            {
                                                if (applicator.apply_to_other_teams)
                                                {
                                                    if (auto* other_health = inside_entity.cmp<CompHealth>())
                                                    {
                                                        auto copied_damage = ab_comp->damages[applicator.owner_damage.value().damage_index];
                                                        copied_damage.applier = has_owner->get_root_owner();
                                                        other_health->apply_damage(copied_damage);
                                                    }
                                                }
                                            }
                                        }
                                    }
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
