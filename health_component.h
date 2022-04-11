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

    float health_percentage;
    float get_current_health() const
    {
        auto stat_comp = sibling<CompStat>();
        if (stat_comp)
        {
            auto max_health = stat_comp->get_stat(Stat::MaxHealth);
            return health_percentage*max_health.addition;
        }
        else
        {
            return health_percentage;
        }
    }

    void apply_damage(const DamageInstance& instance)
    {
        const float max_health = stat_comp->get_stat(Stat::MaxHealth);
        const float current_health_value = get_current_health();
        auto* stat_comp = sibling<CompStat>();
        float net_damage = instance.damage;
        switch (instance.type)
        {
            case DamageType::Magical:
                {
                    StatPart magic_resist = stat_comp->get_stat(Stat::MagicResist);
                    net_damage = instance.damage*(1.0 - magic_resist);
                }
                break;
            case DamageType::Physical:
                {
                    StatPart armor = stat_comp->get_stat(Stat::Armor);
                    net_damage = instance.damage*(1.0 - armor);
                }
                break;
        }
        const float new_health_value = current_health_value - net_damage;
        health_percentage = new_health_value*1.0/max_health;
    }

};