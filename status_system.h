#pragma once

#include "system.h"
#include "status_manager.h"
#include "time_component.h"

class SysStatus : public System
{

public:
    virtual void update(double dt) override
    {
        auto& status_managers = get_array<CompStatusManager>();
        auto current_time = get_array<CompTime>()[0].current_time;
        for (auto& status_manager : status_managers)
        {
            std::vector<ApplicationId> to_remove;
            for (auto& [status_application_id, status_application] : status_manager.statuses)
            {
                auto& [status_entity_id, stack_id] = status_application_id;
                auto status_comp = (CompStatus*)_interface->get_component(type_id<CompStatus>, status_entity_id);
                if (current_time - status_application.application_time > status_comp->duration)
                {
                    to_remove.push_back(status_application_id);
                }
            }
            for (auto& to_remove_entity : to_remove)
            {
                status_manager.statuses.erase(to_remove_entity);
            }
        }
    }

};