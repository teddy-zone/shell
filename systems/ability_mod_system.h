#pragma once

#include <unordered_map>
#include <random>

#include "system.h"
#include "ability_mod.h"
#include "ability_set_component.h"

struct AbilityModEntry
{

};

struct AbilityModDatabase
{
    std::mt19937 gen;
    std::map<std::string, std::vector<AbilityMod>> abilities;

    AbilityMod get_random_mod(const std::vector<std::string>& ability_set, const std::string& ability_name="")
    {
        size_t num_abilities = ability_set.size();
        std::uniform_int_distribution<int> distribution(0,num_abilities);
        auto rand_ability_num = distribution(gen); 
        //auto ability_mod_entry = *(std::advance(heroes.at(hero_name).abilities.begin(), rand_ability_num)).first;
        return get_random_ability_mod(ability_set[rand_ability_num]);
    }

    AbilityMod get_random_ability_mod(const std::string& ability_name)
    {
        size_t num_mods = abilities.at(ability_name).size();
        std::uniform_int_distribution<int> distribution(0,num_mods);
        auto rand_ability_num = distribution(gen); 
        auto begin = abilities.begin();
        return abilities[ability_name][rand_ability_num];
    }
};

class SysAbilityMod : public GuiSystem
{
    AbilityModDatabase _mods;
    const int num_mods_choice = 3;
public:

    virtual void init_update() override
    {
        return;
        _mods.abilities["Ice Shards"] = std::vector<AbilityMod>();
        _mods.abilities["Ice Shards"].push_back(AbilityMod());
        _mods.abilities["Ice Shards"].back().mod_name = "Add damage";
        _mods.abilities["Ice Shards"].back().mod_function = [](EntityRef entity)
            {
                if (auto* radius_application = entity.cmp<CompRadiusApplication>())    
                {
                    if (radius_application->damage)
                    {
                        radius_application->damage.value().damage += 100;
                    }
                }
            };

    }

    virtual void update_gui(double dt) override
    {
        auto& selected_entities = get_array<CompSelectedObjects>();
        if (selected_entities.size())
        {
            if (selected_entities[0].selected_objects.size())
            {
                auto selected_entity = selected_entities[0].selected_objects[0];
                if (auto* ability_mod_comp = selected_entity.cmp<CompAbilityMod>())
                {
                    if (ability_mod_comp->mods_available > 0)
                    {
                        update_mod_window(ability_mod_comp);
                    }
                }
            }
        }
    }

    virtual void update_mod_window(CompAbilityMod* ability_mod_comp)
    {
        int choice_height = 100;
        int widget_width = 400;
        int widget_height = 100 + num_mods_choice*choice_height;
        bool active = true;
        ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width/2 - widget_width/2, CompWidget::window_height - widget_height/2));
        ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
        ImGui::Begin("AbilityMods", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        for (auto& mod : ability_mod_comp->currently_available_mods)
        {
            if (ImGui::Button(mod.mod_name.c_str()))
            {
                if (auto* ability_set = ability_mod_comp->sibling<CompAbilitySet>())
                {
                    for (auto& ability_entity : ability_set->abilities)
                    {
                        if (mod.ability_name == ability_entity.cmp<CompAbility>()->ability_name)
                        {
                            mod.mod_function(ability_entity);
                            ability_mod_comp->mods_available -= 1;
                        }
                    }
                }
            }
        }
        ImGui::End();
    }

    virtual void update(double dt) override
    {
        return;
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
                        auto ability_mod = _mods.get_random_mod(ability_set->get_ability_names());
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
            }
        }
    }
};
