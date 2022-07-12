#pragma once

#include "system.h"
#include "selected_objects_component.h"
#include "eye_component.h"
#include "vision_affected_component.h"
#include "vision_status_component.h"


class SysVision : public System
{
public:

    virtual void init_update() override
    {
        _frame_skip = 0;
    }

    virtual void update(double dt) override
    {
        static std::ofstream fd("vision.log");
        fd << "Start Tick" << "\n";
        Team my_team = 0;
        auto& selected_entities = get_array<CompSelectedObjects>();
        if (selected_entities.size())
        {
            if (selected_entities[0].selected_objects.size())
            {
                auto selected_entity = selected_entities[0].selected_objects[0];
                if (auto* team_comp = selected_entity.cmp<CompTeam>())
                {
                    my_team = team_comp->team;
                }
            }
        }
        auto& eyes = get_array<CompEye>();
        auto& vision_statuses = get_array<CompVisionStatus>();
        for (auto& vision_status : vision_statuses)
        {
            for (auto& [team, team_vision] : vision_status.team_vision)
            {
                team_vision.clear();
            }
        }
        for (auto& eye : eyes)
        {
            Team eye_team = 0;
            if (auto* eye_team_comp = eye.sibling<CompTeam>())
            {
                eye_team = eye_team_comp->team;
            }
            fd << "\tProcessing eye: " << eye.get_entity().get_name() << " on team " << eye_team << "\n";
            for (auto& vision_status : vision_statuses)
            {
                if (vision_status.team_vision.find(eye_team) == vision_status.team_vision.end())
                {
                    vision_status.team_vision.insert(std::make_pair(eye_team, std::unordered_set<EntityId>()));
                }
                if (auto* vision_affected_comp = eye.sibling<CompVisionAffected>())
                {
                    vision_status.team_vision.at(eye_team).insert(eye.get_id());
                }
            }
            if (auto* eye_pos_comp = eye.sibling<CompPosition>())
            {
                if (eye_team == my_team)
                {
                    glm::vec3 ray_origin = eye_pos_comp->pos;
                    ray::Ray height_ray = ray::New(eye_pos_comp->pos + glm::vec3(0,0,10), glm::vec3(0,0,-1));
                    auto height_ray_result = _interface->fire_ray(height_ray, ray::HitType::StaticOnly, 20);
                    if (height_ray_result)
                    {
                        ray_origin.z = height_ray_result.value().hit_point.z + 2;
                    }
                    constexpr float PI = 3.1415926;
                    const int num_evenly_spaced_rays = eye.NumAngles;
                    const float angle_increment = PI*2/num_evenly_spaced_rays;
                    int index = 0;
                    for (float current_angle = 0; current_angle < PI*2; current_angle += angle_increment)
                    {
                        glm::vec3 ray_dir(cos(current_angle), sin(current_angle), 0);
                        ray::Ray vision_ray = ray::New(ray_origin, glm::normalize(ray_dir));
                        auto ray_result = _interface->fire_ray(vision_ray, ray::HitType::StaticOnly, eye.vision_range);
                        
                        if (ray_result)
                        {
                            eye.fow[index] = ray_result.value().t;
                        }
                        else
                        {
                            eye.fow[index] = eye.vision_range;
                        }
                        index++;
                    }
                    eye.do_display = true;
                }
                else
                {
                    eye.do_display = false;
                }

                auto entities_in_range = _interface->data_within_sphere_selective(eye_pos_comp->pos, eye.vision_range, {uint32_t(type_id<CompVisionAffected>)}); 
                for (auto& entity_in_range : entities_in_range)
                {
                    Team entity_team = 0;
                    if (auto* entity_team_comp = entity_in_range.cmp<CompTeam>())
                    {
                        entity_team = entity_team_comp->team;
                    }
                    fd << "\t\tentity in range: " << entity_in_range.get_name() << " on team " << entity_team << "\n";
                    if (eye_team != entity_team)
                    {
                        fd << "\t\tnot on my team!" << "\n";
                        if (auto* entity_pos_comp = entity_in_range.cmp<CompPosition>())
                        {
                            glm::vec3 ray_dir = entity_pos_comp->pos - eye_pos_comp->pos;
                            ray_dir.z = 0;
                            float distance = glm::length(ray_dir);
                            if (distance < eye.vision_range)
                            {
                                ray::Ray vision_ray = ray::New(eye_pos_comp->pos, glm::normalize(ray_dir));
                                auto ray_result = _interface->fire_ray(vision_ray, ray::HitType::StaticOnly, eye.vision_range);
                                if (!ray_result)
                                {
                                    for (auto& vision_status : vision_statuses)
                                    {
                                        fd << "\t\tdeclared visible 1!" << "\n";
                                        vision_status.team_vision[eye_team].insert(entity_in_range.get_id());
                                    }
                                }
                                else
                                {
                                    fd << "\t\tray hit: " << ray_result.value().entity.get_name() << "\n";
                                    if (ray_result.value().t >= distance)
                                    {
                                        for (auto& vision_status : vision_statuses)
                                        {
                                            fd << "\t\tdeclared visible 2!" << "\n";
                                            vision_status.team_vision[eye_team].insert(entity_in_range.get_id());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (vision_statuses.size())
        {
            auto& vision_affected = get_array<CompVisionAffected>();
            for (auto& vision_affected_comp : vision_affected)
            {
                Team other_team = 0;
                if (auto* team_comp = vision_affected_comp.sibling<CompTeam>())
                {
                    other_team = team_comp->team;
                }
                if (auto* static_mesh_comp = vision_affected_comp.sibling<CompStaticMesh>())
                {
                    if (vision_statuses[0].team_vision.find(my_team) == vision_statuses[0].team_vision.end())
                    {
                        vision_statuses[0].team_vision.insert(std::make_pair(my_team, std::unordered_set<EntityId>()));
                    }
                    if (vision_statuses[0].team_vision.at(my_team).find(vision_affected_comp.get_id()) != vision_statuses[0].team_vision.at(my_team).end())
                    {
                        static_mesh_comp->set_visibility(true);
                    }
                    else
                    {
                        static_mesh_comp->set_visibility(false);
                    }
                }
            }
        }
    }
};
