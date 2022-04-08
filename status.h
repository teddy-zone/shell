#pragma once

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
    EntityId owner;
};

struct CompStatus : public Component
{
    float duration;
    StatusType type; 
    float get_linger_time();
    bool is_stackable();
};