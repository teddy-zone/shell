
#pragma once

#include "component.h"
#include "ability_set_component.h"
#include "attacker_component.h"

struct CompCaster : public Component
{
    int ability_index;
    AbilityState state;
    float state_time;
    std::optional<EntityRef> unit_target;
    std::optional<glm::vec3> ground_target;
    bool ability_level_mode = false;
    bool activated = false;
    void activate_ability(int index)
    {
        ability_index = index;
        state = AbilityState::CastPoint;
        state_time = 0.0;
    }

    void stop()
    {
        ability_index = -1;
        activated = false;
        ground_target = std::nullopt;
        unit_target = std::nullopt;
        state_time = 0;
        state = AbilityState::None;
    }

    bool get_has_extra_levels(int current_level)
    {
        if (auto* ability_set = sibling<CompAbilitySet>())
        {
            int total_levels = 0;
            for (auto& ability : ability_set->abilities)
            {
                if (ability.is_valid())
                {
                    auto* comp_ability_it = ability.cmp<CompAbility>();
                    total_levels += comp_ability_it->level;
                }
            }
            if (current_level > total_levels)
            {
                return true;
            }
        }
        return false;
    }

    bool get_is_levelable(int ability_num, int current_level)
    {
        if (auto* ability_set = sibling<CompAbilitySet>())
        {
            int total_levels = 0;
            for (auto& ability : ability_set->abilities)
            {
                if (ability.is_valid())
                {
                    auto* comp_ability_it = ability.cmp<CompAbility>();
                    total_levels += comp_ability_it->level;
                }
            }
            if (current_level > total_levels)
            {
                auto* comp_ability = ability_set->abilities[ability_num].cmp<CompAbility>();
                const int current_ability_level = comp_ability->level;
                if (ability_num < 3)
                {
                    if (current_level > 2*current_ability_level)
                    {
                        if (comp_ability->max_level > current_level)
                        {
                            return true;
                        }
                    }
                }
                else if (ability_num == 3)
                {
                    if (current_level >= 6 && current_ability_level < 1 ||
                        current_level >= 12 && current_ability_level < 2 ||
                        current_level >= 18 && current_ability_level < 3)
                    {
                        if (comp_ability->max_level > current_level)
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    std::vector<CompOnCast*> get_on_cast_components()
    {
        std::vector<CompOnCast*> out_components;
        if (auto* ability_set = sibling<CompAbilitySet>())
        {
            for (auto& ability : ability_set->abilities)
            {
                if (auto* on_cast_comp = ability.cmp<CompOnCast>())
                {
                    if (on_cast_comp->any_ability)
                    {
                        out_components.push_back(on_cast_comp);
                    }
                }
            }
        }

        if (auto* inventory = sibling<CompInventory>())
        {
            for (auto& item : inventory->items)
            {
                if (auto* on_cast_comp = item.cmp<CompOnCast>())
                {
                    if (on_cast_comp->any_item)
                    {
                        out_components.push_back(on_cast_comp);
                    }
                }
            }
        }

        if (auto* status_manager = sibling<CompStatusManager>())
        {
            for (auto& [application_id, application] : status_manager->statuses)
            {
                EntityRef status = EntityRef(std::get<0>(application_id));
                if (auto* on_cast_comp = status.cmp<CompOnCast>())
                {
                    out_components.push_back(on_cast_comp);
                }
            }
        }
        return out_components;
    }

    CompAbility* get_ability(std::optional<int> in_index=std::nullopt)
    {
        int to_get;
        if (!in_index)
        {
            to_get = ability_index;
        }
        else 
        {
            to_get = in_index.value();
        }
        if (to_get < 4)
        {
            auto* ability_set = sibling<CompAbilitySet>();
            return ability_set->get_ability_component_by_index(to_get);
        }
        else if (to_get > 3 && to_get < 10)
        {
            auto* inventory = sibling<CompInventory>();
            auto* out_ability = inventory->items[to_get - 4].cmp<CompAbility>();
            if (out_ability)
            {
                return out_ability;
            }
            else
            {
                throw std::runtime_error("Trying to activate non-active ability");
            }
        }
        else if (to_get == 10)
        {
            auto* comp_attacker = sibling<CompAttacker>();
            if (comp_attacker)
            {
                return comp_attacker->get_ability();
            }
        }
        else
        {
            throw std::runtime_error("Bad ability index");
        }
        return nullptr;
    }
};