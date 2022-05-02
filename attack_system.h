#pragma once

#include "system.h"
#include "ability.h"
#include "ability_set_component.h"
#include "selected_objects_component.h"
#include "health_component.h"
#include "camera_component.h"
#include "on_cast_component.h"
#include "team_component.h"
class SysAttack : public System
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
            if (caster_component.ability_index >= 0)
            {
                CompAbility* ability = nullptr;
                if (caster_component.ability_index < 10)
                {
                    ability = caster_component.get_ability(caster_component.ability_index);
                }
                if (!ability)
                {
                    continue;
                }
                switch (caster_component.state)
                {
                    case AbilityState::CastPoint:
                        {
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

        auto& radius_applicators = get_array<CompRadiusApplication>();
        update_applicators(radius_applicators);
    }

    void cast_ability(CompCaster* caster)
    {
        auto* ability_set = caster->sibling<CompAbilitySet>();
        int caster_team = 0;
        if (auto* caster_team_comp = caster->sibling<CompTeam>())
        {
            caster_team = caster_team_comp->team;
        }
        //auto* ab = ability_set->abilities[caster->ability_index].cmp<CompAbility>();
        auto* ab = caster->get_ability();
        ab->current_cooldown = 
            ab->cooldown;
        if (auto* on_cast_comp = ab->sibling<CompOnCast>())
        {
            for (auto& on_cast_func : on_cast_comp->on_cast_callbacks)
            {
                on_cast_func(caster->get_entity(), caster->ground_target, caster->unit_target);
            }
        }
        if (auto* instance_comp = ab->sibling<CompAbilityInstance>())
        {
            auto ability_instance = _interface->add_entity_from_proto(instance_comp->proto.get());
            if (auto* team_comp = ability_instance.cmp<CompTeam>())
            {
                team_comp->team = caster_team;
            }
            if (auto* rad_app = ability_instance.cmp<CompRadiusApplication>())
            {
                rad_app->radius = ab->radius;
            }
            if (auto* decal = ability_instance.cmp<CompDecal>())
            {
                decal->decal.radius = ab->radius;
            }
            if (caster->unit_target)
            {
                ability_instance.cmp<CompPosition>()->pos = caster->unit_target.value().cmp<CompPosition>()->pos;
            }
            else if (caster->ground_target)
            {
                if (auto* projectile_comp = ability_instance.cmp<CompProjectile>())
                {
                    ability_instance.cmp<CompPosition>()->pos = caster->sibling<CompPosition>()->pos;
                    auto dir = glm::normalize(caster->ground_target.value() - caster->sibling<CompPosition>()->pos);
                    projectile_comp->direction = dir;
                }
                else
                {
                    ability_instance.cmp<CompPosition>()->pos = caster->ground_target.value();
                }
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
                    }
                }
                applicator.last_tick_time = _interface->get_current_game_time();
            }
        }
    }
};
