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
        auto& auras = get_array<CompAura>();
        for (auto& aura : auras)
        {
            auto owner = aura.sibling<CompHasOwner>()->get_root_owner();
            if (owner.is_valid())
            {
                if (auto* comp_status = aura.aura_status.cmp<CompStatus>())
                {
                    if (auto* aura_pos_comp = owner.cmp<CompPosition>())
                    {
                        auto owner_pos = aura_pos_comp->pos;
                        auto other_entities = _interface->data_within_sphere_selective(owner_pos, aura.radius, {uint32_t(type_id<CompStatusManager>)});
                        for (auto& other_entity : other_entities)
                        {
                            Team other_team = 0;
                            Team my_team = 0;
                            if (auto* other_team_comp = other_entity.cmp<CompTeam>())
                            {
                                other_team = other_team_comp->team;
                            }
                            if (auto* my_team_comp = owner.cmp<CompTeam>())
                            {
                                my_team = my_team_comp->team;
                            }
                            if (my_team == other_team && aura.apply_to_same_team ||
                                my_team != other_team && aura.apply_to_other_team)
                            {
                                auto* other_status_manager = other_entity.cmp<CompStatusManager>();
                                other_status_manager->apply_status(comp_status);
                            }
                        }
                    }
                }
            }
        }
    }

};