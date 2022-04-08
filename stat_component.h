#pragma once

#include <map>


#include "component.h"
#include "stat_interface.h"

class CompStat : public Component, public StatInterface
{
    std::map<Stat, StatPart> _parts; 
public:
    virtual StatPart get_stat(Stat stat) override;
};