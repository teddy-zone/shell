#pragma once

#include <algorithm>

#include "component.h"
#include "stat_interface.h"

enum class StatusType
{
    SimpleRefresh,
    SimpleStack,
    StackRefresh,
    Aura
};

struct CompAura : public Component
{
    float radius;
    EntityRef aura_status;
    bool apply_to_same_team;
    bool apply_to_other_team;
};

struct CompAuraHolder : public Component
{
    std::vector<EntityRef> auras;
};

struct CompStatus : public Component
{
    float duration;
    StatusType type; 
    float get_linger_time();
    bool is_stackable();

    void set_name(const std::string& in_name)
    {
        size_t to_copy = std::min(int(in_name.size()), 19);
        std::copy(in_name.begin(), in_name.begin() + to_copy, name.begin());
    }

    const char* get_name()
    {
        return name.data();
    }

private:
    std::array<char, 20> name;
};