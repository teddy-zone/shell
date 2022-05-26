#pragma once

#include <map>


#include "component.h"
#include "stat_interface.h"

class CompStat : public Component, public StatInterface
{
    std::map<Stat, StatPart> _parts; 
public:
    virtual StatPart get_stat(Stat stat) override;
    virtual bool get_status_state(StatusState state) override;
    void set_stat(Stat stat, float value)
    {
        _parts[stat].addition = value;
    }
    void set_stat_mult(Stat stat, float value)
    {
        _parts[stat].multiplication = value;
    }
};