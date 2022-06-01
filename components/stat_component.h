#pragma once

#include <map>


#include "component.h"
#include "stat_interface.h"

class CompStat : public Component, public StatInterface
{
    std::map<Stat, StatPart> _parts; 
    std::map<StatusState, bool> _states; 
public:
    virtual StatPart get_stat(Stat stat) override;
    virtual bool get_status_state(StatusState state) override;
    virtual void set_status_state(StatusState state, bool new_state) override
    {
        _states[state] = new_state;
    }
    void set_stat(Stat stat, float value)
    {
        _parts[stat].addition = value;
    }
    void set_stat_mult(Stat stat, float value)
    {
        _parts[stat].multiplication = value;
    }
};