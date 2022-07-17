#pragma once

#include <unordered_map>
#include <random>

#include "system.h"
#include "ability_mod.h"
#include "ability_set_component.h"
#include "on_intersect_component.h"
#include "crystal_nova.h"
#include "cask.h"
#include "fall.h"

template <typename T>
struct AbilityDraftDatabase
{
    std::mt19937 gen;
    std::vector<T> abilities;
    std::uniform_int_distribution<int> distribution;

    AbilityDraftDatabase():
        distribution(0, 10000)
    {}

    std::optional<T> get_random_ability_mod(const std::set<std::string>& exclude_mods)
    {
        T current;
        int tried = 0;
        while ((exclude_mods.find(current.name) != exclude_mods.end() 
               || current.name == ""))
        {
            size_t num_mods = abilities.size();
            auto rand_ability_num = distribution(gen) % (num_mods); 
            current = abilities[rand_ability_num];
            tried += 1;
            if (tried > abilities.size())
            {
                return std::nullopt;
            }
        }
        return current;
    }
};

template <typename T>
struct DraftDatabase
{
    std::mt19937 gen;
    std::map<std::string, std::vector<T>> abilities;
    std::uniform_int_distribution<int> distribution;

    DraftDatabase():
        distribution(0, 10000)
    {}

    std::optional<T> get_random_mod(const std::vector<std::string>& ability_set, const std::set<std::string>& exclude_mods)
    {
        std::vector<std::string> valid_abilities;
        for (auto& ability_name : ability_set)
        {
            if (abilities.find(ability_name) != abilities.end())
            {
                valid_abilities.push_back(ability_name);
            }
        }
        size_t num_abilities = valid_abilities.size();
        if (num_abilities > 0)
        {
            auto rand_ability_num = distribution(gen) % num_abilities; 
            //auto ability_mod_entry = *(std::advance(heroes.at(hero_name).abilities.begin(), rand_ability_num)).first;
            return get_random_ability_mod(valid_abilities[rand_ability_num], exclude_mods);
        }
        return std::nullopt;
    }

    std::optional<T> get_random_ability_mod(const std::string& ability_name, const std::set<std::string>& exclude_mods)
    {
        T current_mod;
        int mods_tried = 0;
        while ((exclude_mods.find(current_mod.mod_name) != exclude_mods.end() 
               || current_mod.mod_name == ""))
        {
            size_t num_mods = abilities.at(ability_name).size();
            auto rand_ability_num = distribution(gen) % (num_mods); 
            auto begin = abilities.begin();
            current_mod = abilities[ability_name][rand_ability_num];
            mods_tried += 1;
            if (mods_tried > abilities.at(ability_name).size())
            {
                return std::nullopt;
            }
        }
        return current_mod;
    }
};

class SysAbilityMod : public GuiSystem
{
    DraftDatabase<AbilityMod> _mods;
    const int num_mods_choice = 7;
public:

    virtual void init_update() override
    {
        _mods.abilities["Ice Shards"] = std::vector<AbilityMod>();
        _mods.abilities["Ice Shards"].push_back(AbilityMod());
        _mods.abilities["Ice Shards"].back().mod_name = "Add Ice Shards Damage";
        _mods.abilities["Ice Shards"].back().ability_name = "Ice Shards";
        _mods.abilities["Ice Shards"].back().mod_function = [](EntityRef entity)
            {
                if (auto* ability_comp = entity.cmp<CompAbility>())    
                {
                    if (ability_comp->damages.size())
                    {
                        ability_comp->damages[0].damage += 100;
                    }
                }
            };

        _mods.abilities["Ice Shards"] = std::vector<AbilityMod>();
        _mods.abilities["Ice Shards"].push_back(AbilityMod());
        _mods.abilities["Ice Shards"].back().mod_name = "Add Ice Shards Radius";
        _mods.abilities["Ice Shards"].back().ability_name = "Ice Shards";
        _mods.abilities["Ice Shards"].back().mod_function = [](EntityRef entity)
            {
                if (auto* ability_comp = entity.cmp<CompAbility>())    
                {
                    ability_comp->radius += 3;
                }
            };

        _mods.abilities["Ice Shards"].push_back(AbilityMod());
        _mods.abilities["Ice Shards"].back().mod_name = "Ice Shards Barrier";
        _mods.abilities["Ice Shards"].back().ability_name = "Ice Shards";
        _mods.abilities["Ice Shards"].back().mod_function = [](EntityRef entity)
            {
                if (auto* ability_comp = entity.cmp<CompAbility>())
                {
                    if (auto* instance_comp = entity.cmp<CompAbilityInstance>())    
                    {
                        if (!instance_comp->destruction_callback)
                        {
                            instance_comp->destruction_callback = CompOnDestruction();
                        }
                        instance_comp->destruction_callback.value().on_destruction_callbacks.push_back([](SystemInterface* iface, EntityRef me)
                            {
                                if (auto* my_pos_comp = me.cmp<CompPosition>())
                                {
                                    auto* phys_comp = me.cmp<CompPhysics>();
                                    auto vel_dir = glm::normalize(phys_comp->vel);
                                    const int num_shards = 15;
                                    const float radius = 5.0;
                                    const float angle_increment = 2*3.1415926/num_shards;
                                    for (int i = 0; i < num_shards; ++i)
                                    {
                                        auto shard_proto = std::make_shared<IceShardProto>(glm::vec3(0));
                                        auto new_shard = iface->add_entity_from_proto(shard_proto.get());
                                        auto extend_vec = glm::vec3(radius*cos(i*angle_increment), radius*sin(i*angle_increment), 0);
                                        if (glm::dot(glm::normalize(extend_vec), vel_dir) > -0.5)
                                        {
                                            new_shard.cmp<CompPosition>()->pos = my_pos_comp->pos + extend_vec;
                                        }
                                    }
                                }
                            });
                    }
                }
            };

        _mods.abilities["Ice Shards"].push_back(AbilityMod());
        _mods.abilities["Ice Shards"].back().mod_name = "Boomerang Ice Shards";
        _mods.abilities["Ice Shards"].back().ability_name = "Ice Shards";
        _mods.abilities["Ice Shards"].back().mod_function = [](EntityRef entity)
            {
                if (auto* ability_comp = entity.cmp<CompAbility>())
                {
                    if (auto* instance_comp = entity.cmp<CompAbilityInstance>())    
                    {
                        if (!instance_comp->destruction_callback)
                        {
                            instance_comp->destruction_callback = CompOnDestruction();
                        }
                        instance_comp->destruction_callback.value().on_destruction_callbacks.push_back([](SystemInterface* iface, EntityRef me)
                            {
                                if (auto* my_pos_comp = me.cmp<CompPosition>())
                                {
                                    auto* phys_comp = me.cmp<CompPhysics>();
                                    auto vel_dir = glm::normalize(phys_comp->vel);
                                    auto return_shards = iface->duplicate_entity(me);
                                    return_shards.cmp<CompProjectile>()->origin = my_pos_comp->pos;
                                    if (auto* owner_comp = my_pos_comp->sibling<CompHasOwner>())
                                    {
                                        return_shards.cmp<CompProjectile>()->homing_target = owner_comp->get_root_owner();
                                        return_shards.cmp<CompProjectile>()->max_range = std::nullopt;
                                        return_shards.cmp<CompProjectile>()->direction = std::nullopt;
                                        return_shards.cmp<CompProjectile>()->die_on_hit = true;
                                        return_shards.cmp<CompOnDestruction>()->on_destruction_callbacks.clear();
                                    }
                                }
                            });
                    }
                }
            };

        _mods.abilities["Dash"] = std::vector<AbilityMod>();
        _mods.abilities["Dash"].push_back(AbilityMod());
        _mods.abilities["Dash"].back().mod_name = "Dash does damage";
        _mods.abilities["Dash"].back().ability_name = "Dash";
        _mods.abilities["Dash"].back().mod_function = [](EntityRef entity)
            {
                if (auto* ability_comp = entity.cmp<CompAbility>())
                {
                    ability_comp->damages.push_back({entity, DamageType::Magical, 50, false});
                    if (auto* instance_comp = entity.cmp<CompAbilityInstance>())    
                    {
                        instance_comp->radial_application = CompRadiusApplication();
                        instance_comp->radial_application.value().apply_to_same_team = false;
                        instance_comp->radial_application.value().apply_to_other_teams = true;
                        instance_comp->radial_application.value().apply_once = true;
                        instance_comp->radial_application.value().radius = 10;
                        instance_comp->radial_application.value().owner_damage = {int(ability_comp->damages.size()-1)};
                    }
                }
            };

        _mods.abilities["Bladefury"] = std::vector<AbilityMod>();
        _mods.abilities["Bladefury"].push_back(AbilityMod());
        _mods.abilities["Bladefury"].back().mod_name = "Bladefury places second instance";
        _mods.abilities["Bladefury"].back().ability_name = "Bladefury";
        _mods.abilities["Bladefury"].back().mod_function = [](EntityRef entity)
            {
                if (auto* ability_comp = entity.cmp<CompAbility>())
                {
                    if (auto* on_cast = entity.cmp<CompOnCast>())    
                    {
                        on_cast->on_cast_callbacks.push_back([](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
                            {
                                if (instance_entity)
                                {
                                    auto new_entity = iface->duplicate_entity(instance_entity.value());
                                }
                            });
                    }
                }
            };

        _mods.abilities["Crystal Nova"] = std::vector<AbilityMod>();
        _mods.abilities["Crystal Nova"].push_back(AbilityMod());
        _mods.abilities["Crystal Nova"].back().mod_name = "3 Instances of Crystal Nova. 1 every 2 seconds.";
        _mods.abilities["Crystal Nova"].back().ability_name = "Crystal Nova";
        _mods.abilities["Crystal Nova"].back().mod_function = [](EntityRef entity)
            {

                if (auto* ability_comp = entity.cmp<CompAbility>())
                {
                    if (!entity.cmp<CompAbilityInstance>()->destruction_callback)
                    {
                        entity.cmp<CompAbilityInstance>()->destruction_callback = CompOnDestruction();
                    }
                    entity.cmp<CompAbilityInstance>()->destruction_callback.value().on_destruction_callbacks.push_back(
                        [](SystemInterface* iface, EntityRef me)
                        {
                            if (me.is_valid())
                            {
                                if (auto* my_on_destruct = me.cmp<CompOnDestruction>())
                                {
                                    if (my_on_destruct->generation < 2)
                                    {
                                        auto new_entity = iface->duplicate_entity(me);
                                        if (auto* on_destruct = new_entity.cmp<CompOnDestruction>())
                                        {
                                            on_destruct->generation += 1;
                                        }
                                    }
                                }
                            }
                        });
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
        int choice_height = 80;
        int widget_width = 400;
        int widget_height = 100 + num_mods_choice*choice_height;
        bool active = true;
        static bool is_collapsed = false;
        if (!is_collapsed)
        {
            ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width/2 - widget_width/2, CompWidget::window_height/2 - widget_height/2));
        }
        else
        {
            ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width/2 - widget_width/2, 0));//CompWidget::window_height/2 - widget_height/2));
        }
        //ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
        ImGui::Begin("AbilityMods", &active, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::IsWindowCollapsed())
        {
            is_collapsed = true;
        }
        else
        {
            is_collapsed = false;
        }
        for (auto& mod : ability_mod_comp->currently_available_mods)
        {
            if (ImGui::Button(mod.mod_name.c_str(), ImVec2(widget_width - 10, choice_height)))
            {
                if (auto* ability_set = ability_mod_comp->sibling<CompAbilitySet>())
                {
                    for (auto& ability_entity : ability_set->abilities)
                    {
                        if (ability_entity.is_valid())
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
        }
        ImGui::End();
    }

    virtual void update(double dt) override
    {
        auto& ability_mod_comps = get_array<CompAbilityMod>();
        for (auto& ability_mod_comp : ability_mod_comps)
        {
            // I think this should be for the selected unit not every component?
            if (auto* ability_set = ability_mod_comp.sibling<CompAbilitySet>())
            {
                if (ability_mod_comp.mods_available > 0 && ability_mod_comp.currently_available_mods.empty())
                {
                    std::set<std::string> done_abilities;
                    for (int i = 0; i < num_mods_choice; ++i)
                    {

                        auto ability_mod = _mods.get_random_mod(ability_set->get_ability_names(), done_abilities);
                        if (ability_mod)
                        {
                            done_abilities.insert(ability_mod.value().mod_name);
                            EntityRef ability_ref = ability_set->get_ability_by_name(ability_mod.value().ability_name);
                            if (ability_ref.is_valid())
                            {
                                // if ImGui::Button(mod_name)
                                ability_mod_comp.currently_available_mods.push_back(ability_mod.value());
                            }
                            else
                            {

                            }
                        }
                    }
                }
            }
        }
    }
};

class SysAbilityDraft : public GuiSystem
{
    AbilityDraftDatabase<DraftableAbility> _abilities;
    const int num_ability_choice = 10;
public:

    virtual void init_update() override
    {
        _abilities.abilities.push_back(DraftableAbility());
        _abilities.abilities.back().name = "Jump";
        auto jump_proto = std::make_shared<JumpAbilityProto>();
        _abilities.abilities.back().ability_proto = jump_proto;

        _abilities.abilities.push_back(DraftableAbility());
        _abilities.abilities.back().name = "Crystal Nova";
        auto cn_proto = std::make_shared<CrystalNovaAbilityProto>();
        _abilities.abilities.back().ability_proto = cn_proto;

        _abilities.abilities.push_back(DraftableAbility());
        _abilities.abilities.back().name = "Cask";
        auto cask_proto = std::make_shared<CaskAbilityProto>();
        _abilities.abilities.back().ability_proto = cask_proto;

        _abilities.abilities.push_back(DraftableAbility());
        _abilities.abilities.back().name = "Dash";
        auto dash_proto = std::make_shared<DashAbilityProto>();
        _abilities.abilities.back().ability_proto = dash_proto;

        _abilities.abilities.push_back(DraftableAbility());
        _abilities.abilities.back().name = "Crush";
        auto fall_proto = std::make_shared<FallAbilityProto>();
        _abilities.abilities.back().ability_proto = fall_proto;
    }

    virtual void update_gui(double dt) override
    {
        auto& selected_entities = get_array<CompSelectedObjects>();
        if (selected_entities.size())
        {
            if (selected_entities[0].selected_objects.size())
            {
                auto selected_entity = selected_entities[0].selected_objects[0];
                if (auto* ability_set_comp = selected_entity.cmp<CompAbilitySet>())
                {
                    if (ability_set_comp->drafts_available)
                    {
                        update_draft_window(ability_set_comp);
                    }
                }
            }
        }
    }

    virtual void update_draft_window(CompAbilitySet* ability_set_comp)
    {
        int choice_height = 80;
        int widget_width = 400;
        int widget_height = 100 + ability_set_comp->draft_choices.size()*choice_height;
        bool active = true;
        static bool is_collapsed = false;
        if (!is_collapsed)
        {
            ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width/2 - widget_width/2, CompWidget::window_height/2 - widget_height/2));
        }
        else
        {
            ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width/2 - widget_width/2, 0));//CompWidget::window_height/2 - widget_height/2));
        }
        //ImGui::SetNextWindowSize(ImVec2(widget_width, widget_height));
        ImGui::Begin("AbilityDraft", &active, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::IsWindowCollapsed())
        {
            is_collapsed = true;
        }
        else
        {
            is_collapsed = false;
        }
        const std::vector<std::string> button_slots{ "Q", "W", "E", "R" };
        switch (ability_set_comp->selection_state)
        {
            case AbilityDraftSelectionState::AbilitySelection:
                {
                    ImGui::Text("Choose New Ability:");
                    for (auto& ability : ability_set_comp->draft_choices)
                    {
                        if (ImGui::Button(ability.name.c_str(), ImVec2(widget_width - 10, choice_height)))
                        {
                            ability_set_comp->selected_ability = ability;
                            ability_set_comp->selection_state = AbilityDraftSelectionState::SlotSelection;
                        }
                    }
                }
                break;
            case AbilityDraftSelectionState::SlotSelection:
                {
                    int slot_num = 0;
                    ImGui::Text("Choose Ability Slot:");
                    for (auto& ability : ability_set_comp->abilities)
                    {
                        std::string button_text = button_slots[slot_num].c_str();
                        if (ability_set_comp->abilities[slot_num].is_valid())
                        {
                            auto existing_text = std::string("Ability Currently In Slot: " + ability_set_comp->get_ability_name(slot_num).value());
                            button_text = "";
                            for (int i = 0; i < existing_text.size(); ++i) { button_text += " "; }
                            button_text[existing_text.size() / 2] = button_slots[slot_num][0];
                            button_text += "\n" + existing_text;

                        }
                        if (ImGui::Button(button_text.c_str(), ImVec2(widget_width - 10, choice_height)))
                        {
                            auto ability_entity = _interface->add_entity_from_proto(ability_set_comp->selected_ability.value().ability_proto.get());
                            ability_entity.cmp<CompAbility>()->max_level = 4;
                            ability = ability_entity;
                            ability_set_comp->drafts_available -= 1;
                        }
                        
                        slot_num++;
                        if (slot_num > 3)
                        {
                            break;
                        }
                    }
                    if (ImGui::Button("Back"))
                    {
                        ability_set_comp->selection_state = AbilityDraftSelectionState::AbilitySelection;
                    }
                }
                break;
            default:
                throw std::runtime_error("Ability menu invalid state");
                break;
        }
        ImGui::End();
    }

    virtual void update(double dt) override
    {
        auto& ability_set_comps = get_array<CompAbilitySet>();
        for (auto& ability_set_comp : ability_set_comps)
        {
            if (ability_set_comp.drafts_available && ability_set_comp.draft_choices.empty())
            {
                std::set<std::string> done_abilities;
                for (int i = 0; i < num_ability_choice; ++i)
                {
                    auto new_ability = _abilities.get_random_ability_mod(done_abilities);
                    if (new_ability)
                    {
                        ability_set_comp.draft_choices.push_back(new_ability.value());
                        done_abilities.insert(new_ability.value().name);
                    }
                }
            }
        }
    }
};
