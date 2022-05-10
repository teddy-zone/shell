#pragma once

#include <set>
#include <optional>

#include "status.h"
#include "component.h"
#include "stat_interface.h"
#include "stat_component.h"
#include "time_component.h"

struct StatusApplication
{
    float application_time;
    StatusType type;
    std::optional<unsigned int> stacks;
};

// ApplicationId is (entity_id, 0) except in simple stack case
// in which the ApplicationId is (entity_id, stack_id)
typedef std::tuple<EntityId, int> ApplicationId;

struct CompStatusManager : public Component, public StatInterface
{
    std::map<ApplicationId, StatusApplication> statuses; // Status entity ids mapped to last refresh time
    int stack_id = 0;

    virtual StatPart get_stat(Stat stat) override
    {
        StatPart out_part;
        for (auto& [application_id, status_application] : statuses)
        {
            auto& [entity_id, stack_id] = application_id;
            auto entity = EntityRef(entity_id);
            if (entity.is_valid())
            {
                auto* single_status_stat_comp = EntityRef(entity_id).cmp<CompStat>();
                out_part = out_part.join(single_status_stat_comp->get_stat(stat));
            }
        }
        return out_part;
    }

    void apply_status(CompStatus* status)
    {
        //auto status = (CompStatus*)get_component(type_id<CompStatus>, status_eid);
        float current_time = _interface->get_current_game_time();
        EntityId status_eid = status->get_id();
        switch(status->type)
        {
            case StatusType::Aura:
            case StatusType::SimpleRefresh:
                {
                    statuses[{status_eid, 0}] = StatusApplication{current_time, StatusType::SimpleRefresh, std::nullopt}; 
                }
                break;
            case StatusType::SimpleStack:
                {
                    statuses[{status_eid, stack_id++}] = StatusApplication{current_time, StatusType::SimpleStack, std::nullopt}; 
                }
                break;
            case StatusType::StackRefresh:
                {
                    auto& current_application = statuses[{status_eid, 0}];
                    statuses[{status_eid, 0}] = StatusApplication{current_time, 
                                                                  StatusType::StackRefresh, 
                                                                  current_application.stacks.value()+1}; 
                }
                break;
        }
    }
};