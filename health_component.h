#pragma once

#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"

enum class DamageType
{
    Physical,
    Magical,
    Pure
};

struct DamageInstance
{
    DamageType type;
    float damage;
    bool is_attack;
};

struct CompHealth : public Component
{

    float health_percentage = 100.0;
    float filtered_health_percentage = 100.0;
    float get_current_health() const
    {
        auto stat_comp = sibling<CompStat>();
        if (stat_comp)
        {
            auto max_health = stat_comp->get_stat(Stat::MaxHealth);
            return health_percentage*max_health.addition/100.0;
        }
        else
        {
            return health_percentage;
        }
    }

    void apply_damage(const DamageInstance& instance)
    {
        auto* stat_comp = sibling<CompStat>();
        const float max_health = stat_comp->get_stat(Stat::MaxHealth).addition;
        const float current_health_value = get_current_health();
        float net_damage = instance.damage;
        switch (instance.type)
        {
            case DamageType::Magical:
                {
                    StatPart magic_resist = stat_comp->get_stat(Stat::MagicResist);
                    net_damage = instance.damage*(1.0 - magic_resist.addition);
                }
                break;
            case DamageType::Physical:
                {
                    StatPart armor = stat_comp->get_stat(Stat::Armor);
                    const float damage_multiplier = 1 - (0.06*armor.addition)/(1+0.06*std::abs(armor.addition));
                    net_damage = instance.damage*damage_multiplier;
                }
                break;
        }
        const float new_health_value = current_health_value - net_damage;
        health_percentage = 100.0*new_health_value*1.0/max_health;
    }

};