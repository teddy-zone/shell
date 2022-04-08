#pragma once

#include <map>

enum class Stat
{
    Movespeed,
    AttackRange,
    AttackDamage,
    MaxHealth,
    MaxMana
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
    virtual StatPart get_stat(Stat stat) = 0;
};