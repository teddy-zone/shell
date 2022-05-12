#pragma once

#include <unordered_set>
#include <map>

#include "component.h"
#include "tsl/hopscotch_map.h"
#include "tsl/hopscotch_set.h"
#include "team_component.h"

struct CompVisionStatus : public Component
{
    std::map<Team,std::unordered_set<EntityId>> team_vision;
};