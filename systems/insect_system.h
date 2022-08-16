#pragma once

#include <random>
#include <vector>

#include "component.h"
#include "procedural_level_component.h"
#include "system.h"

struct InsectType : public Component
{
	float erraticness = 1.0;
	float pausingness = 1.0;
    float speed = 200.0;
	bool flying = false;
    float flying_height = 1.5;
    float current_floor_level;
	std::string name = "default_insect";
    std::shared_ptr<EntityProto> item_proto;

    glm::vec3 direction;
    float next_pause;
};


struct InsectProto : public ActorProto
{
    InsectProto(const std::vector<CompType>& extension_types = {}) :
        ActorProto(glm::vec3(0), extension_types)
    {
        std::vector<CompType> unit_components = { {
                    uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<InsectType>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
    }
};

struct PillBugItemProto : public ItemProto
{
    PillBugItemProto(const std::vector<CompType>& extension_types = {}) :
        ItemProto(extension_types)
    {
        std::vector<CompType> unit_components = { {
                uint32_t(type_id<CompStat>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        entity.cmp<CompStat>()->set_stat(Stat::MaxHealth, 40);
        entity.cmp<CompItem>()->name = "Pill Bug";
        entity.cmp<CompItem>()->has_charges = true;
        entity.cmp<CompItem>()->num_charges = 1;
        entity.cmp<CompItem>()->max_charges = 100;
    }
};


struct FlyItemProto : public ItemProto
{
    FlyItemProto(const std::vector<CompType>& extension_types = {}) :
        ItemProto(extension_types)
    {
        std::vector<CompType> unit_components = { {
                uint32_t(type_id<CompStat>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        entity.cmp<CompStat>()->set_stat(Stat::Movespeed, 10);
        entity.cmp<CompItem>()->name = "Fly";
        entity.cmp<CompItem>()->has_charges = true;
        entity.cmp<CompItem>()->num_charges = 1;
        entity.cmp<CompItem>()->max_charges = 100;
    }
};

struct PillBugProto : public InsectProto
{
    PillBugProto(const std::vector<CompType>& extension_types = {}) :
        InsectProto(extension_types)
    {
        /*
        std::vector<CompType> unit_components = { {
            } };
        append_components(unit_components);
        */
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        entity.cmp<InsectType>()->erraticness = 0.1;
        entity.cmp<InsectType>()->pausingness = 0.5;
        entity.cmp<InsectType>()->speed = 2.0;
        entity.cmp<InsectType>()->flying = false;
        entity.cmp<InsectType>()->name = "Pill Bug";
        entity.cmp<InsectType>()->item_proto = std::make_shared<PillBugItemProto>();
        entity.cmp<CompPhysics>()->has_gravity = false;
        entity.cmp<CompPhysics>()->has_collision = false;

        /*
        Sound new_sound;
        new_sound.path = "sounds/fly.wav";
        new_sound.trigger = true;
        new_sound.range = 5;
        new_sound.loop = true;
        entity.cmp<CompVoice>()->sounds["fly"] = new_sound;
        */

        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        cube_mesh->set_solid_color(glm::vec3(0.4,0.2,0.1));
        entity.cmp<CompPosition>()->scale = glm::vec3(0.1, 0.1, 0.1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(cube_mesh);
    }
};

struct FlyProto : public InsectProto
{
    FlyProto(const std::vector<CompType>& extension_types = {}) :
        InsectProto(extension_types)
    {
        /*
        std::vector<CompType> unit_components = { {
            } };
        append_components(unit_components);
        */
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        entity.cmp<InsectType>()->erraticness = 1.0;
        entity.cmp<InsectType>()->pausingness = 0.5;
        entity.cmp<InsectType>()->speed = 10.0;
        entity.cmp<InsectType>()->flying = true;
        entity.cmp<InsectType>()->name = "Fly";
        entity.cmp<InsectType>()->item_proto = std::make_shared<FlyItemProto>();
        entity.cmp<CompPhysics>()->has_gravity = false;
        entity.cmp<CompPhysics>()->has_collision = false;

        Sound new_sound;
        new_sound.path = "sounds/fly.wav";
        new_sound.trigger = true;
        new_sound.range = 5;
        new_sound.loop = true;
        new_sound.sound_name = "fly";
        new_sound.volume = 0.2;
        entity.cmp<CompVoice>()->sounds["fly"] = new_sound;

        auto cube_mesh = std::make_shared<bgfx::Mesh>();
        cube_mesh->load_obj("cube.obj");
        cube_mesh->set_solid_color(glm::vec3(0));
        entity.cmp<CompPosition>()->scale = glm::vec3(0.1, 0.1, 0.1);
        entity.cmp<CompStaticMesh>()->mesh.set_mesh(cube_mesh);
    }
};

class SysInsect : public System
{
    std::normal_distribution<float> normal_dist;
    std::mt19937 gen;

    float last_spawn = 0;

public:

    SysInsect() :
        normal_dist(0, 1.0)
    {

    }

	virtual void init_update() override
	{
	}

	virtual void update(double dt) override
	{
        const float spawn_delay = 10;
        auto& level_comps = get_array<CompProceduralLevel>();
        auto& insect_comps = get_array<InsectType>();

        // Movement iteration
        {
            const float height_tolerance = 0.2;
            for (auto& insect_comp : insect_comps)
            {
                auto& current_pos = insect_comp.sibling<CompPosition>()->pos;

                if (insect_comp.current_floor_level < 0)
                {
                    auto down_ray = ray::New(current_pos + glm::vec3(0,0,30), glm::vec3(0, 0, -1));
                    auto possible_hit_result = _interface->fire_ray(down_ray, ray::HitType::StaticOnly, 100);
                    if (possible_hit_result)
                    {
                        insect_comp.current_floor_level = possible_hit_result.value().hit_point.z;
                    }
                }

                float increasing_diff = 1.0;
                //while (1)
                {
                    auto possible_direction_x = increasing_diff*insect_comp.erraticness * normal_dist(gen)*0.5;
                    auto possible_direction_y = increasing_diff*insect_comp.erraticness * normal_dist(gen)*0.5;
                    glm::vec3 possible_direction = glm::normalize(insect_comp.direction + glm::vec3(possible_direction_x, 0, 0) + glm::vec3(0, possible_direction_y, 0));
                    auto down_ray = ray::New(current_pos + possible_direction * dt * insect_comp.speed + glm::vec3(0,0,20), glm::vec3(0, 0, -1));
                    //std::cout << "Current Pos " << glm::to_string(current_pos) << "\n";
                    //std::cout << "Ray origin" << glm::to_string(down_ray.origin) << "\n";
                    //std::cout << "Dir" << glm::to_string(insect_comp.direction) << "\n";
                    //std::cout << "Dir" << glm::to_string(possible_direction) << "\n";
                    auto possible_hit_result = _interface->fire_ray(down_ray, ray::HitType::StaticOnly,50);
                    if (possible_hit_result)
                    {
                        auto& hit_result = *possible_hit_result;
                        //std::cout << "\tHIT!: " << glm::to_string(hit_result.hit_point) << "\n";
                        //std::cout << "\t\t: " << insect_comp.current_floor_level << "\n";
                        if (std::abs(hit_result.hit_point.z - insect_comp.current_floor_level) < height_tolerance)
                        {
                            current_pos = down_ray.origin - glm::vec3(0,0,20);
                            insect_comp.current_floor_level = hit_result.hit_point.z;
                            if (insect_comp.flying)
                            {
                                current_pos.z = insect_comp.current_floor_level + 2;
                            }
                            else
                            {
                                current_pos.z = insect_comp.current_floor_level;
                            }
                            insect_comp.direction = possible_direction;
                            continue;
                        }
                        else
                        {
                            insect_comp.direction = -insect_comp.direction;
                        }
                    }
                }
            }
        }

        // Insect spawning
        {
            if (_interface->get_current_game_time() - last_spawn > spawn_delay)
            {
                if (level_comps.size() && insect_comps.size() < 10)
                {
					auto fly_proto = std::make_shared<FlyProto>();
					auto fly_entity = _interface->add_entity_from_proto(fly_proto.get());
                    std::uniform_int_distribution<int> spawn_path_point_distribution(5, level_comps[0].path.size() - 5);
                    int spawn_path_index = spawn_path_point_distribution(gen);
                    glm::vec3 spawn_point = level_comps[0].path[spawn_path_index];
                    fly_entity.cmp<CompPosition>()->pos = spawn_point + glm::vec3(0, 0, level_comps[0].floor_level + 3);
                    last_spawn = _interface->get_current_game_time();
                    fly_entity.cmp<InsectType>()->direction = glm::vec3(1, 0, 0);
                    fly_entity.cmp<InsectType>()->current_floor_level = -1;// level_comps[0].floor_level;
                }

            }
        }
	}
};