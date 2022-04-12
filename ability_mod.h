#pragma once

#include <functional>
#include "component.h"
#include "entity_wrapper.h"

struct AbilityMod
{
    std::string ability_name;
    std::string mod_name;
    std::function<void(EntityRef)> mod_function;
};

struct CompAbilityMod : public Component
{
    std::string hero_name;
    std::vector<AbilityMod> currently_available_mods;
    int mods_available;
};