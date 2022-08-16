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
#include "materials/sphere_mat/VertexShader.glsl.h"
#include "materials/sphere_mat/FragmentShader.glsl.h"

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
        constexpr float movespeed_translation_factor = 1.0/10.0;
        auto& stat_comps = get_array<CompStat>();
        for (auto& stat_comp : stat_comps)
        {
            if (auto* nav_comp = stat_comp.sibling<CompNav>())
            {
                nav_comp->max_speed = stat_comp.get_abs_stat(Stat::Movespeed)*movespeed_translation_factor;
            }
        }

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
            return;
            player_comp = &get_array<CompPosition>()[0];
        } 
        auto player_pos = player_comp->sibling<CompPosition>()->pos;
        auto* command_comp = player_comp->sibling<CompCommand>();
        auto* caster_comp_d = player_comp->sibling<CompCaster>();
        if (caster_comp_d == nullptr)
        {
            std::cout << "Null caster comp" << "\n";
        }
        auto* caster_comp = player_comp->sibling<CompCaster>();
        
        auto* attacker_comp = player_comp->sibling<CompAttacker>();
        auto* physics_comp = player_comp->sibling<CompPhysics>();
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
                {
                    targeting_entity.cmp<CompPosition>()->pos = result.value().hit_point;
                }
                auto* ab = caster_comp->get_ability();
                if (ab)
                {
                    auto* decal = targeting_entity.cmp<CompDecal>();
                    decal->decal.radius = ab->radius;
                    decal->decal.location2 = glm::vec4(player_pos, 1.0);
                }
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

        auto& camera = get_array<CompCamera>()[0];
		auto hover_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
			keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
		auto full_hover_ray = ray::New(camera.graphics_camera.get_position(), hover_ray);
		auto hover_result = _interface->fire_ray(full_hover_ray, ray::HitType::DynamicOnly);
        bool found_hover = false;
        if (hover_result)
        {
			auto my_team = 0;
			if (auto* my_teamp_comp = command_comp->sibling<CompTeam>())
			{
				my_team = my_teamp_comp->team;
			}
            if (auto* hover_team_comp = hover_result.value().entity.cmp<CompTeam>())
            {
                if (hover_team_comp->team != my_team)
                {
                    selected_unit_comps[0].hovered_object = hover_result.value().entity;
                    found_hover = true;
                }
            }
        }
        if (!found_hover)
        {
            selected_unit_comps[0].hovered_object = EntityRef();
        }

        {
            // Left click!
            if (keystate.push[0] && caster_comp)
            {
                if (attacker_comp->attack_targeting)
                {
                    auto click_ray = camera.graphics_camera.get_ray(keystate.mouse_pos_x * 1.0f / camera.graphics_camera._width,
                        keystate.mouse_pos_y * 1.0f / camera.graphics_camera._height);
                    auto full_ray = ray::New(camera.graphics_camera.get_position(), click_ray);
                    auto result = _interface->fire_ray(full_ray, ray::HitType::StaticOnly);
                    if (result)
                    {
                        AttackMoveCommand a_move_command;
                        a_move_command.target = result.value().hit_point;

                        CommandIndicatorProto command_indicator_proto(result.value().hit_point);
                        auto a_move_indicator = _interface->add_entity_from_proto(&command_indicator_proto);
                        a_move_indicator.cmp<CompStaticMesh>()->mesh.get_mesh()->set_solid_color(glm::vec3(1, 0, 0));

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
                        if (result)
                        {
                            AbilityCommand ability_command;
                            ability_command.ability_index = caster_comp->ability_index;
                            ability_command.entity_target = result.value().entity;
                            auto* command_sys = player_comp->sibling<CompCommand>();
                            command_sys->set_command(StopCommand());
                            command_sys->queue_command(ability_command);
                        }
                        keystate.cursor_mode = CursorMode::Select;
                    }
                    break;
                    default:
                        break;
                    }
                }
            }
            // Right click
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
                if (result)// && physics_comp->on_ground)
                {
                    int clicked_team = 0;
                    if (auto* team_comp = result.value().entity.cmp<CompTeam>())
                    {
                        clicked_team = team_comp->team;
                    }
                    auto* attackable_comp = result.value().entity.cmp<CompAttackable>();
                    if (attackable_comp && clicked_team != my_team)
                    {
                        std::cout << "Attack clicked " << result.value().entity.get_name() << "\n";
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
                        std::cout << "Move clicked " << result.value().entity.get_name() << "\n";
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
            if (keystate.push[GLFW_KEY_T])
            {
                if (auto* comp_experience = caster_comp->sibling<CompExperience>())
                {
                    if (caster_comp->get_has_extra_levels(comp_experience->get_level()))
                    {
                        caster_comp->ability_level_mode = true;
                    }
                }
            }
            if (keystate.push[GLFW_KEY_A])
            {
                attacker_comp->attack_targeting = true;
            }
            std::vector<int> items_to_service;
            std::vector<int> abilities_to_service;
            if (keystate.push[GLFW_KEY_Q]) { abilities_to_service.push_back(0); }
            if (keystate.push[GLFW_KEY_W]) { abilities_to_service.push_back(1); }
            if (keystate.push[GLFW_KEY_E]) { abilities_to_service.push_back(2); }
            if (keystate.push[GLFW_KEY_R]) { abilities_to_service.push_back(3); }
            if (keystate.push[GLFW_KEY_1]) { items_to_service.push_back(0); }
            if (keystate.push[GLFW_KEY_2]) { items_to_service.push_back(1); }
            if (keystate.push[GLFW_KEY_3]) { items_to_service.push_back(2); }
            if (keystate.push[GLFW_KEY_4]) { items_to_service.push_back(3); }
            if (keystate.push[GLFW_KEY_5]) { items_to_service.push_back(4); }
            if (keystate.push[GLFW_KEY_6]) { items_to_service.push_back(5); }
            if (caster_comp->ability_level_mode)
            {
                for (auto ability_to_service : abilities_to_service)
                {
                    if (auto* comp_ability = caster_comp->get_ability(ability_to_service))
                    {
                        if (auto* comp_experience = caster_comp->sibling<CompExperience>())
                        {
                            if (caster_comp->get_is_levelable(ability_to_service, comp_experience->get_level()))
                            {
                                comp_ability->level += 1;
                                caster_comp->ability_level_mode = false;
                            }
                        }
                    }
                }
            }
            else if (keystate.cursor_mode != CursorMode::Gameplay)
            {
                // For abilities that have been pressed and if we're not in gameplay cursor mode
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
                                if (!ability->ground_targeted && !ability->unit_targeted && !ability->self_targeted)
                                {
                                    /*
                                    auto* command_sys = player_comp->sibling<CompCommand>();
                                    StopCommand stop_command;
                                    command_sys->set_command(stop_command);
                                    AbilityCommand new_command;
                                    new_command.ability_index = ability_index;
                                    command_sys->queue_command(new_command);
                                    */
									AbilityCommand ability_command;
									ability_command.ability_index = caster_comp->ability_index;
									auto* command_sys = player_comp->sibling<CompCommand>();
									command_sys->set_command(StopCommand());
									command_sys->queue_command(ability_command);
                                }
                                else if (ability->self_targeted)
                                {
                                    auto* command_sys = player_comp->sibling<CompCommand>();
                                    StopCommand stop_command;
                                    command_sys->set_command(stop_command);
                                    AbilityCommand new_command;
                                    new_command.ability_index = ability_index;
                                    new_command.entity_target = player_comp->get_entity();
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
                                else if (ability->unit_targeted)
                                {
                                    caster_comp->state = AbilityState::UnitTargeting;
                                    caster_comp->ability_index = ability_index;
                                    keystate.cursor_mode = CursorMode::Gameplay;
                                    TargetingProto targeting_proto(glm::vec3(0), ability->target_decal_type);
                                    targeting_entity = _interface->add_entity_from_proto(&targeting_proto);
                                }
                            }
                        }
                    }
                }
            }
            if (keystate.cursor_mode != CursorMode::Gameplay)
            {
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
                                else if (ability->unit_targeted)
                                {
                                    caster_comp->state = AbilityState::UnitTargeting;
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
    }
};
