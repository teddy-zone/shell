#pragma once

#include "system.h"
#include "keystate_component.h"
#include "nav_system.h"
#include "projectile_component.h"
#include "lifetime_system.h"
#include "command_component.h"
#include "graphics_component.h"
#include "unit_prototypes.h"
#include "attackable_component.h"

class ShellPlayerController : public System
{
    EntityRef targeting_entity;
public:

    ShellPlayerController()
    {
        _type_name = "shell_player_controller";
    }

    virtual void init_update() override
    {

    }

    virtual void update(double dt) override
    {
        auto& selected_unit_comps = get_array<CompSelectedObjects>();
        EntityRef selected_unit;
        if (selected_unit_comps.size())
        {
            if (selected_unit_comps[0].selected_objects.size())
            {
                selected_unit = selected_unit_comps[0].selected_objects[0];
            }
        }
        CompPosition* player_comp; 
        if (selected_unit.is_valid())
        {
            player_comp = selected_unit.cmp<CompPosition>();
        }
        else
        {
            player_comp = &get_array<CompPosition>()[0];
        } 
        auto player_pos = player_comp->sibling<CompPosition>()->pos;
        auto* command_comp = player_comp->sibling<CompCommand>();
        auto* caster_comp = player_comp->sibling<CompCaster>();
        auto* attacker_comp = player_comp->sibling<CompAttacker>();
        auto& graphics_comp = get_array<CompGraphics>()[0];
        auto& keystate = get_array<CompKeyState>()[0];

        if (targeting_entity.is_valid())
        {
            if (keystate.cursor_mode == CursorMode::Select)
            {
                _interface->delete_entity(targeting_entity.get_id());
                targeting_entity = EntityRef();
            }
            else
            {
                auto& camera = get_array<CompCamera>()[0];
                auto click_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
                    keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
                auto full_ray = ray::New(camera.graphics_camera.get_position(), click_ray);
                auto result = _interface->fire_ray(full_ray, ray::HitType::StaticOnly);
                if (result)
                targeting_entity.cmp<CompPosition>()->pos = result.value().hit_point;
                auto* ab = caster_comp->get_ability();
                auto* decal = targeting_entity.cmp<CompDecal>();
                decal->decal.radius = ab->radius;
                decal->decal.location2 = glm::vec4(player_pos, 1.0);
            }
        }

        if (caster_comp)
        {
            switch (caster_comp->state)
            {
                case AbilityState::UnitTargeting:
                case AbilityState::GroundTargeting:
                    graphics_comp.set_cursor(CursorType::Hand);
                    break;
                default:
                    graphics_comp.set_cursor(CursorType::DefaultArrow);
                    break;
            }
        }
        else
        {
            graphics_comp.set_cursor(CursorType::DefaultArrow);
        }

        {
            // Left click!
            if (keystate.push[0] && caster_comp)
            {
                if (attacker_comp->attack_targeting)
                {
                    auto& camera = get_array<CompCamera>()[0];
                    auto click_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
                        keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
                    auto full_ray = ray::New(camera.graphics_camera.get_position(), click_ray);
                    auto result = _interface->fire_ray(full_ray, ray::HitType::StaticOnly);
                    if (result)
                    {
                        AttackMoveCommand a_move_command;
                        a_move_command.target = result.value().hit_point;
                        attacker_comp->attack_targeting = false;
                        auto* command_sys = player_comp->sibling<CompCommand>();
                        command_sys->set_command(StopCommand());
                        command_sys->queue_command(a_move_command);
                    }
                }
                else
                {
                    auto& camera = get_array<CompCamera>()[0];
                    switch (caster_comp->state)
                    {
                        case AbilityState::GroundTargeting:
                            {
                                caster_comp->state = AbilityState::None;
                                auto click_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
                                    keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
                                auto full_ray = ray::New(camera.graphics_camera.get_position(), click_ray);
                                auto result = _interface->fire_ray(full_ray, ray::HitType::StaticOnly);
                                if (result)
                                {
                                    AbilityCommand ability_command;
                                    ability_command.ability_index = caster_comp->ability_index;
                                    ability_command.ground_target = result.value().hit_point;
                                    auto* command_sys = player_comp->sibling<CompCommand>();
                                    command_sys->set_command(StopCommand());
                                    command_sys->queue_command(ability_command);
                                }
                                keystate.cursor_mode = CursorMode::Select;
                            }
                            break;
                        case AbilityState::UnitTargeting:
                            {
                                caster_comp->state = AbilityState::None;
                                auto click_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
                                    keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
                                auto full_ray = ray::New(camera.graphics_camera.get_position(), click_ray);
                                auto result = _interface->fire_ray(full_ray, ray::HitType::DynamicOnly);
                                keystate.cursor_mode = CursorMode::Select;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            if (keystate.push[1] && command_comp)
            {
                auto& oct = get_array<CompBoundsOctree>()[0];
                auto& camera = get_array<CompCamera>()[0];
                auto& nav_pointer_array = get_array<CompNavPointer>();
                auto my_team = 0;
                if (auto* my_teamp_comp = command_comp->sibling<CompTeam>())
                {
                    my_team = my_teamp_comp->team;
                }
                if (nav_pointer_array.size())
                {
                    auto nav_pointer_pos = nav_pointer_array[0].sibling<CompPosition>();
                }
                
                auto click_dir = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
                    keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
                /*
                oct._enable_static = true;
                oct._enable_dynamic = false;
                //octree::ray_intersect_octree(new_ray, oct.root, oct.loc, real_size, possible_hits);
                auto result = oct.ray_intersect(ray::Ray{ camera.graphics_camera.get_position(), click_ray, glm::vec3(1.0 / click_ray.x, 1.0 / click_ray.y, 1.0 / click_ray.z) });
                oct._enable_static = true;
                oct._enable_dynamic = true;
                */
                auto click_ray = ray::New(camera.graphics_camera.get_position(), click_dir);
                auto result = _interface->fire_ray(click_ray, ray::HitType::StaticAndDynamic);
                // Clicked on something!
                if (result)
                {
                    int clicked_team = 0;
                    if (auto* team_comp = result.value().entity.cmp<CompTeam>())
                    {
                        clicked_team = team_comp->team;
                    }
                    auto* attackable_comp = result.value().entity.cmp<CompAttackable>();
                    if (attackable_comp && clicked_team != my_team)
                    {
                        AttackCommand attack_command;
                        attack_command.target = result.value().entity;
                        command_comp->set_command(attack_command);
                        command_comp->set_command(StopCommand());
                        command_comp->queue_command(attack_command);
                    }
                    else if (auto* interact_comp = result.value().entity.cmp<CompInteractable>())
                    {
                        InteractCommand interact_command;
                        interact_command.interact_target = result.value().entity;
                        command_comp->set_command(interact_command);
                    }
                    else
                    {
                        CommandIndicatorProto command_indicator_proto(result.value().hit_point);
                        _interface->add_entity_from_proto(&command_indicator_proto);
                        //on_right_click(result->entity);
                        MoveCommand move_command;
                        move_command.target = result.value().hit_point;
                        command_comp->set_command(move_command);
                    }
                }
                keystate.cursor_mode = CursorMode::Select;
            }
            if (keystate.push[GLFW_KEY_SPACE])
            {
                EntityRef test_ent = _interface->add_entity_with_components({
                        uint32_t(type_id<CompPosition>),
                        uint32_t(type_id<CompBounds>),
                        uint32_t(type_id<CompPhysics>),
                        uint32_t(type_id<CompStaticMesh>),
                        uint32_t(type_id<CompProjectile>),
                        uint32_t(type_id<CompLifetime>),
                        });
                auto* pos_comp = test_ent.cmp<CompPosition>();
                pos_comp->pos = player_pos;
                auto sphere_mat = std::make_shared<bgfx::Material>();
                std::ifstream t_sphere("C:\\Users\\tjwal\\projects\\ECS\\materials\\sphere_mat\\VertexShader.glsl");
                std::stringstream buffer_sphere;
                buffer_sphere << t_sphere.rdbuf();
                auto vshader_sphere = std::make_shared<Shader>(Shader::Type::Vertex, buffer_sphere.str(), true);
                std::ifstream t_sphere_f("C:\\Users\\tjwal\\projects\\ECS\\materials\\sphere_mat\\FragmentShader.glsl");
                std::stringstream buffer2_sphere;
                buffer2_sphere << t_sphere_f.rdbuf();
                auto fshader_sphere = std::make_shared<Shader>(Shader::Type::Fragment, buffer2_sphere.str(), true);
                sphere_mat->set_vertex_shader(vshader_sphere);
                sphere_mat->set_fragment_shader(fshader_sphere);
                sphere_mat->link();

                auto sphere_mesh = std::make_shared<bgfx::Mesh>();
                sphere_mesh->load_obj("sphere.obj");
                auto* mesh = test_ent.cmp<CompStaticMesh>();
                mesh->mesh.set_mesh(sphere_mesh);
                mesh->mesh.set_material(sphere_mat);
                mesh->mesh.set_id(test_ent.get_id());
                test_ent.cmp<CompProjectile>()->speed = 10;
                test_ent.cmp<CompProjectile>()->direction = glm::vec3(1,0,0);
                test_ent.cmp<CompPhysics>()->has_collision = false;
                test_ent.cmp<CompLifetime>()->lifetime = 2;
            }
            if (keystate.push[GLFW_KEY_A])
            {
                attacker_comp->attack_targeting = true;
            }
            std::vector<int> items_to_service;
            std::vector<int> abilities_to_service;
            if (keystate.push[GLFW_KEY_Q]) {abilities_to_service.push_back(0);}
            if (keystate.push[GLFW_KEY_W]) {abilities_to_service.push_back(1);}
            if (keystate.push[GLFW_KEY_E]) {abilities_to_service.push_back(2);}
            if (keystate.push[GLFW_KEY_R]) {abilities_to_service.push_back(3);}
            if (keystate.push[GLFW_KEY_1]) {items_to_service.push_back(0);}
            if (keystate.push[GLFW_KEY_2]) {items_to_service.push_back(1);}
            if (keystate.push[GLFW_KEY_3]) {items_to_service.push_back(2);}
            if (keystate.push[GLFW_KEY_4]) {items_to_service.push_back(3);}
            if (keystate.push[GLFW_KEY_5]) {items_to_service.push_back(4);}
            if (keystate.push[GLFW_KEY_6]) {items_to_service.push_back(5);}
            for (auto& ability_index : abilities_to_service)
            {
                auto* ability_set = player_comp->sibling<CompAbilitySet>();
                if (ability_set)
                {
                    if (ability_set->abilities[ability_index].is_valid())
                    {
                        auto* ability = ability_set->abilities[ability_index].cmp<CompAbility>();
                        if (ability)
                        {
                            if (!ability->ground_targeted && !ability->unit_targeted)
                            {
                                auto* command_sys = player_comp->sibling<CompCommand>();
                                StopCommand stop_command;
                                command_sys->set_command(stop_command);
                                AbilityCommand new_command;
                                new_command.ability_index = ability_index;
                                command_sys->queue_command(new_command);
                            }
                            else if (ability->ground_targeted)
                            {
                                caster_comp->state = AbilityState::GroundTargeting;
                                caster_comp->ability_index = ability_index;
                                keystate.cursor_mode = CursorMode::Gameplay;
                                TargetingProto targeting_proto(glm::vec3(0), ability->target_decal_type);
                                targeting_entity = _interface->add_entity_from_proto(&targeting_proto);
                            }
                        }
                    }
                }
            }
            for (auto& item_index : items_to_service)
            {
                auto* inventory = player_comp->sibling<CompInventory>();
                if (inventory)
                {
                    if (inventory->items[item_index].is_valid())
                    {
                        auto* ability = inventory->items[item_index].cmp<CompAbility>();
                        if (ability)
                        {
                            if (!ability->ground_targeted && !ability->unit_targeted)
                            {
                                auto* command_sys = player_comp->sibling<CompCommand>();
                                StopCommand stop_command;
                                command_sys->set_command(stop_command);
                                AbilityCommand new_command;
                                new_command.ability_index = item_index + 4;
                                command_sys->queue_command(new_command);
                            }
                            else if (ability->ground_targeted)
                            {
                                caster_comp->state = AbilityState::GroundTargeting;
                                caster_comp->ability_index = item_index + 4;
                                keystate.cursor_mode = CursorMode::Gameplay;
                                TargetingProto targeting_proto(glm::vec3(0), ability->target_decal_type);
                                targeting_entity = _interface->add_entity_from_proto(&targeting_proto);
                            }
                        }
                    }
                }
            }
        }
    }
};
