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
    Armor
};

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
    virtual float get_abs_stat(Stat stat)
    {
        auto out_part = get_stat(stat);
        return out_part.addition*out_part.multiplication;
    }
};