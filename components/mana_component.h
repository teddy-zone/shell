#pragma once

#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"

struct CompMana : public Component
{
    float mana_percentage = 100.0;
    float get_current_mana() const
    {
        auto stat_comp = sibling<CompStat>();
        if (stat_comp)
        {
            auto max_mana = stat_comp->get_abs_stat(Stat::MaxMana);
            return mana_percentage*max_mana/100.0;
        }
        else
        {
            return mana_percentage;
        }
    }

    bool use_mana(int mana_amount)
    {
        auto* stat_comp = sibling<CompStat>();
        const float max_mana = stat_comp->get_abs_stat(Stat::MaxMana);
        const float current_mana_value = get_current_mana();
        if (mana_amount > current_mana_value)
        {
            return false;
        }
        add_mana(-1.0*mana_amount);
        if (mana_percentage <= 0.0001)
        {
        }
        return true;
    }

    void add_mana(float mana_amount)
    {
        auto* stat_comp = sibling<CompStat>();
        const float max_mana = stat_comp->get_abs_stat(Stat::MaxMana);
        const float current_mana_value = get_current_mana();
        const float new_mana_value = current_mana_value + mana_amount;
        mana_percentage = std::max(0.0f, std::min(100.0f, 100.0f*new_mana_value*1.0f/max_mana));
    }


};