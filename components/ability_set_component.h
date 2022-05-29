#pragma once
#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"
#include "ability.h"

enum class AbilityDraftSelectionState
{
    AbilitySelection,
    SlotSelection,
    Done
};

struct DraftableAbility
{
    std::shared_ptr<EntityProto> ability_proto;
    std::string ability_name;
};

struct CompAbilitySet : public Component, public StatInterface
{

    std::array<EntityRef, 6> abilities;

    // Drafting states
    std::optional<DraftableAbility> selected_ability;
    std::vector<DraftableAbility> draft_choices;
    int drafts_available = 0;
    AbilityDraftSelectionState selection_state = AbilityDraftSelectionState::Done;

    std::vector<std::string> get_ability_names()
    {
        std::vector<std::string> out_names;
        for (auto& ability_ref : abilities)
        {
            if (ability_ref.is_valid())
            {
                if (auto* ab = ability_ref.cmp<CompAbility>())
                {
                    out_names.push_back(ab->ability_name);
                }
            }
        }
        return out_names;
    }

    virtual StatPart get_stat(Stat stat) override
    {
        StatPart out_part;
        for (auto& ability_ref : abilities)
        {
            if (ability_ref.is_valid())
            {
                if (auto* single_ability_stat_comp = ability_ref.cmp<CompStat>())
                {
                    out_part = out_part.join(single_ability_stat_comp->get_stat(stat));
                }
            }
        }
        return out_part;
    }

    virtual bool get_status_state(StatusState state) override
    {
        for (auto& ability_ref : abilities)
        {
            if (ability_ref.is_valid())
            {
                if (auto* single_ability_stat_comp = ability_ref.cmp<CompStat>())
                {
                    if (single_ability_stat_comp->get_status_state(state))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    EntityRef get_ability_by_name(const std::string& ability_name)
    {
        for (int i = 0; i < abilities.size(); ++i)
        {
            if (auto* comp_ability = abilities[i].cmp<CompAbility>())
            {
                if (comp_ability->ability_name == ability_name)
                {
                    return abilities[i];
                }
            }
        }
        return EntityRef();
    }

    CompAbility* get_ability_component_by_index(int index)
    {
        return abilities[index].cmp<CompAbility>();
    }

};
