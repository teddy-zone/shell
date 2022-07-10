#pragma once

#include <random>

#include "system.h"
#include "snowfall_component.h"
#include "camera_component.h"
#include "entity_prototype.h"
#include "static_mesh_component.h"

struct SnowfallEntityProto : public ActorProto
{

    SnowfallEntityProto(const std::vector<CompType>& extension_types={}):
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompSnowfall>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        std::shared_ptr<bgfx::Mesh> snow_mesh;
        snow_mesh = std::make_shared<bgfx::Mesh>(true);
        snow_mesh->load_obj("cube.obj", true);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(snow_mesh);
        entity.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec4(1.1,1.1,1.2,1.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(0.03);
        entity.cmp<CompPosition>()->pos = glm::vec3(0.00);
    }
};

class SysSnowfall : public System
{
    const float fall_vel = 1.0;
    const float spawn_chances = 5.0;
    const float spawn_probability = 0.25;
    std::uniform_real_distribution<float> dist_roll;
    std::mt19937 gen;
    int added = 10;

public:

    SysSnowfall():
        dist_roll(0,1.0)
    {}

    virtual void init_update()
    {
    }

    virtual void update(double dt)
    {
        added -= 1;
        if (!added)
        {
            _interface->add_entity_from_proto(std::make_shared<SnowfallEntityProto>().get());
        }
        auto& snowfall_comps = get_array<CompSnowfall>();
        auto& camera = get_array<CompCamera>()[0].graphics_camera;
        for (auto& snowfall_comp : snowfall_comps)
        {
            glm::vec3 camera_offset = glm::mod(camera.get_position(), snowfall_comp.volume);
            glm::vec3 camera_pos = camera.get_position();
            for (auto& snow_pos : snowfall_comp.snow_positions)
            {
                snow_pos -= glm::vec3(0,0,1)*dt*fall_vel;
                if (snow_pos.z < 0)
                {
                    snow_pos.z = snowfall_comp.volume.z;
                }
            }
            std::vector<float> instance_offsets;
            if (snowfall_comp.snow_positions.empty())
            {
                std::uniform_real_distribution<float> distx(0,snowfall_comp.volume.x);
                std::uniform_real_distribution<float> disty(0,snowfall_comp.volume.y);
                std::uniform_real_distribution<float> distz(0,snowfall_comp.volume.z);
                int num_snowflakes = int(snowfall_comp.density*snowfall_comp.volume.x*snowfall_comp.volume.y*snowfall_comp.volume.z);
                for (int i = 0; i < num_snowflakes; ++i)

                {
                    glm::vec3 new_pos(distx(gen), disty(gen), distz(gen));
                    snowfall_comp.snow_positions.push_back(new_pos);
                    instance_offsets.push_back(camera_pos.x - new_pos.x);
                    instance_offsets.push_back(camera_pos.y - new_pos.y);
                    instance_offsets.push_back(camera_pos.z - new_pos.z);
                }
            }
            else
            {
                float current_time = _interface->get_current_game_time();
                int i = 0;
                for (auto& snow_pos : snowfall_comp.snow_positions)
                {
                    int base_x = int((camera_pos.x + 10 - snow_pos.x)/(snowfall_comp.volume.x));
                    int base_y = int((camera_pos.y + 10 - snow_pos.y)/(snowfall_comp.volume.y));
                    int base_z = int((camera_pos.z + 10 - snow_pos.z)/(snowfall_comp.volume.z));
                    instance_offsets.push_back(snow_pos.x + base_x*snowfall_comp.volume.x + 0.2*cos(current_time*2 + 100*i));
                    instance_offsets.push_back(snow_pos.y + base_y*snowfall_comp.volume.y + 0.2*sin(current_time*2 + 100*i));
                    instance_offsets.push_back(snow_pos.z + base_z*snowfall_comp.volume.z);
                    if (glm::length(glm::vec3(*(instance_offsets.end() - 3), *(instance_offsets.end() - 2), *(instance_offsets.end() - 1)) - camera_pos) < 10)
                    {
                        *(instance_offsets.end() - 3) = -100;
                        *(instance_offsets.end() - 2) = -100;
                        *(instance_offsets.end() - 1) = -100;
                    }
                    i++;
                }
            }

            if (auto* static_mesh = snowfall_comp.sibling<CompStaticMesh>())
            {
                static_mesh->mesh.get_mesh()->set_instance_offsets(instance_offsets);
            }
        }


    }
};