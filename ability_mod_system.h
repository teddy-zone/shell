#pragma once

#include <unordered_map>
#include <random>

#include "system.h"
#include "ability_mod.h"
#include "ability_set_component.h"

struct AbilityModEntry
{
    std::mt19937 gen;
    std::unordered_map<std::string, std::vector<AbilityMod>> abilities;

    AbilityMod get_random_mod(const std::string& ability_name)
    {
        size_t num_mods = abilities.at(ability_name).size();
        std::uniform_int_distribution<int> distribution(0,num_mods);
        auto rand_ability_num = distribution(gen); 
        return abilities.at(rand_ability_num);
    }
};

struct AbilityModDatabase
{
    std::mt19937 gen;
    std::unordered_map<std::string, AbilityModEntry> heroes;

    AbilityMod get_random_mod(const std::string& hero_name, const std::string& ability_name="")
    {
        size_t num_abilities = heroes.at(hero_name).abilities.size();
        std::uniform_int_distribution<int> distribution(0,num_abilities);
        auto rand_ability_num = distribution(gen); 
        auto ability_mod_entry = (heroes.at(hero_name).abilities.begin() + rand_ability_num).first;
        return heroes[hero_name].get_random_mod(ability_mod_entry);
    }
};

class SysAbilityMod : public System
{
    AbilityModDatabase _mods;
    const int num_mods_choice = 3;
public:
    virtual void update(double dt)
    {
        auto& ability_mod_comps = get_array<CompAbilityMod>();
        for (auto& ability_mod_comp : ability_mod_comps)
        {
            // I think this should be for the selected unit not every component?
            if (auto* ability_set = ability_mod_comp.sibling<CompAbilitySet>())
            {
                if (ability_mod_comp.mods_available > 0 && ability_mod_comp.currently_available_mods.empty())
                {
                    for (int i = 0; i < num_mods_choice; ++i)
                    {
                        auto ability_mod = _mods.get_random_mod(ability_mod_comp.hero_name);
                        EntityRef ability_ref = ability_set->get_ability_by_name(ability_mod.ability_name);
                        if (ability_ref.is_valid())
                        {
                            // if ImGui::Button(mod_name)
                            ability_mod_comp.currently_available_mods.push_back(ability_mod);
                        }
                        else
                        {

                        }
                    }
                }
                else if (ability_mod_comp.mods_available > 0 && !ability_mod_comp.currently_available_mods.empty())
                {
                    // Start ImGui window
                    for (auto& ability_mod : ability_mod_comp.currently_available_mods)
                    {
                        EntityRef ability_ref = ability_set->get_ability_by_name(ability_mod.ability_name);
                        if (ability_ref.is_valid())
                        {
                            // if ImGui::Button(mod_name)
                            //    ability_mod.mod_function(ability_ref);
                        }
                    }
                    // End ImGui window
                }
            }
        }
    }
};