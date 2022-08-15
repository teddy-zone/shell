#pragma once

#include <map>

enum class Stat
{
    Movespeed,
    AttackRange,
    AttackDamage,
    MaxHealth,
    HealthRegen,
    MaxMana,
    ManaRegen,
    MagicResist,
    Armor,
    Lifesteal,
};

enum class StatusState 
{
    Stunned,
    Rooted,
    Silenced,
    Phased,
    Disarmed,
    KnockedUp
};

inline std::string stat_to_string(const Stat& in_stat)
{
    switch (in_stat)
    {
    case Stat::Movespeed:
        return "Movespeed";
    case Stat::AttackRange:
        return "Attack Range";
    case Stat::AttackDamage:
        return "Attack Damage";
    case Stat::MaxHealth:
        return "Max Health";
    case Stat::HealthRegen:
        return "Health Regen";
    case Stat::MaxMana:
        return "Max Mana";
    case Stat::ManaRegen:
        return "Mana Regen";
    case Stat::MagicResist:
        return "Magic Resist";
    case Stat::Armor:
        return "Armor";
    case Stat::Lifesteal:
        return "Lifesteal";
    default:
        break;
    }
    return "";
}

struct StatPart
{
    float addition = 0.0;
    float multiplication = 1.0;

    StatPart join(const StatPart& b)
    {
        return StatPart{addition + b.addition, multiplication*b.multiplication};
    }
};

class StatInterface
{
public:
    virtual StatPart get_stat(Stat stat) = 0;
    virtual bool get_status_state(StatusState state) = 0;
    virtual void set_status_state(StatusState state, bool new_state) {};
    virtual float get_abs_stat(Stat stat)
    {
        auto out_part = get_stat(stat);
        return out_part.addition*out_part.multiplication;
    }
};