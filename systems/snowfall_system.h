#pragma once

#include <random>

#include "system.h"
#include "snowfall_component.h"
#include "camera_component.h"
#include "entity_prototype.h"
#include "static_mesh_component.h"
#include "weather_component.h"

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

struct RainfallEntityProto : public ActorProto
{

    RainfallEntityProto(const std::vector<CompType>& extension_types = {}) :
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = { {
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompSnowfall>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        std::shared_ptr<bgfx::Mesh> rain_mesh;
        rain_mesh = std::make_shared<bgfx::Mesh>(true);
        rain_mesh->load_obj("cube.obj", true);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(rain_mesh);
        entity.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec4(1.1, 1.1, 1.2, 1.0));
        entity.cmp<CompPosition>()->scale = glm::vec3(0.01, 0.01, 0.5);
        entity.cmp<CompPosition>()->pos = glm::vec3(0.00);
        entity.cmp<CompSnowfall>()->density = 0.01;
        entity.cmp<CompSnowfall>()->meander_factor = 0.00;
        entity.cmp<CompSnowfall>()->fall_velocity = 18;
    }
};

struct RainRippleProto : public ActorProto
{

    RainRippleProto(const std::vector<CompType>& extension_types = {}) :
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = { {
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompLifetime>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        entity.cmp<CompDecal>()->decal.type = 7;
        entity.cmp<CompDecal>()->decal.radius = 0.5;
        entity.cmp<CompLifetime>()->lifetime = 1.0;
    }
};

struct CloudProto : public ActorProto
{

    CloudProto(const std::vector<CompType>& extension_types = {}) :
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = { {
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompProjectile>),
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        entity.cmp<CompDecal>()->decal.type = 8;
        entity.cmp<CompDecal>()->decal.radius = 20;
        entity.cmp<CompLifetime>()->lifetime = 100;
        entity.cmp<CompProjectile>()->direction = glm::normalize(glm::vec3(1, 0, 0));
        entity.cmp<CompProjectile>()->speed = 5;
        entity.cmp<CompPhysics>()->has_collision = false;
        entity.cmp<CompPhysics>()->has_gravity = false;
        entity.cmp<CompBounds>()->bounds = glm::vec3(1);
    }
};

class SysSnowfall : public System
{
    std::mt19937 gen;
    int added = 10;
    const float ripple_rate = 10;
    const float cloud_rate = 0.05;
    const int cloud_chances = 2;
    const glm::vec2 ripple_area;
    std::uniform_real_distribution<float> x_dist;
    std::uniform_real_distribution<float> y_dist;
    std::uniform_real_distribution<float> x_dist_small;
    std::uniform_real_distribution<float> y_dist_small;
    std::uniform_real_distribution<float> ripple_spawn_roll;
    std::uniform_real_distribution<float> cloud_size;

public:

    SysSnowfall():
        ripple_spawn_roll(0,1.0),
        x_dist(0, 200),
        y_dist(0, 200),
        x_dist_small(5,25),
        y_dist_small(5, 15),
        cloud_size(5,18)
    {}

    virtual void init_update()
    {
    }

    virtual void update(double dt)
    {
        added -= 1;
        if (!added)
        {
            auto snow_entity = _interface->add_entity_from_proto(std::make_shared<SnowfallEntityProto>().get());
            snow_entity.set_name("Snow");
            auto rain_entity = _interface->add_entity_from_proto(std::make_shared<RainfallEntityProto>().get());
            rain_entity.set_name("Rain");
        }
        auto& snowfall_comps = get_array<CompSnowfall>();
        auto& camera = get_array<CompCamera>()[0].graphics_camera;
        auto weather_comps = get_array<CompWeather>();
        for (auto& snowfall_comp : snowfall_comps)
        {
            
            if (weather_comps.size())
            {
                if (weather_comps[0].state == WeatherState::Rain)
                {
                    if (snowfall_comp.get_entity().get_name() == "Rain")
                    {
                        snowfall_comp.sibling<CompStaticMesh>()->visible = true;
                    }
                    else 
                    {
                        snowfall_comp.sibling<CompStaticMesh>()->visible = false;
                    }
                }
                else if (weather_comps[0].state == WeatherState::Snow)
                {
                    if (snowfall_comp.get_entity().get_name() == "Snow")
                    {
                        snowfall_comp.sibling<CompStaticMesh>()->visible = true;
                    }
                    else
                    {
                        snowfall_comp.sibling<CompStaticMesh>()->visible = false;
                    }
                }
                else
                {
                    snowfall_comp.sibling<CompStaticMesh>()->visible = false;
                }
            }
            glm::vec3 camera_offset = glm::mod(camera.get_position(), snowfall_comp.volume);
            glm::vec3 camera_pos = camera.get_position();
            for (auto& snow_pos : snowfall_comp.snow_positions)
            {
                snow_pos -= glm::vec3(0,0,1)*dt*snowfall_comp.fall_velocity;
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
                    
                    instance_offsets.push_back(snow_pos.x + base_x*snowfall_comp.volume.x + 0.2*cos(current_time*2 + 100*i)*snowfall_comp.meander_factor);
                    instance_offsets.push_back(snow_pos.y + base_y*snowfall_comp.volume.y + 0.2*sin(current_time*2 + 100*i) * snowfall_comp.meander_factor);
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


        if (weather_comps.size())
        {
            if (weather_comps[0].state == WeatherState::Rain)
            {
                for (int i = 0; i < ripple_rate; ++i)
                {
                    float ripple_roll = ripple_spawn_roll(gen);
                    if (ripple_roll < dt)
                    {
                        float x = x_dist(gen);
                        float y = y_dist(gen);
                        auto new_ripple = _interface->add_entity_from_proto(std::make_shared<RainRippleProto>().get());
                        new_ripple.cmp<CompPosition>()->pos = camera.get_position() - glm::vec3(x, y, 0);
                    }
                }
            }
            else if (weather_comps[0].state == WeatherState::PartlyCloudy)
            {
                for (int i = 0; i < cloud_chances; ++i)
                {
                    float ripple_roll = ripple_spawn_roll(gen);
                    if (ripple_roll < dt*cloud_rate)
                    {
                        float x = x_dist(gen);
                        float y = y_dist(gen);
                        for (int j = 0; j < 5; j++)
                        {
                            float x_offset = x_dist_small(gen);
                            float y_offset = y_dist_small(gen);
                            auto new_ripple = _interface->add_entity_from_proto(std::make_shared<CloudProto>().get());
                            new_ripple.cmp<CompPosition>()->pos = glm::vec3(0, y, 0) + glm::vec3(x_offset, y_offset, 0);
                            new_ripple.cmp<CompDecal>()->decal.radius = cloud_size(gen);
                        }
                    }
                }
            }
        }
    }
};