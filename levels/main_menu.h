#pragma once

#include <random>
#include "level.h"
#include "gui_system.h"
#include "widget_component.h"
#include "hud_control_component.h"
#include "main_menu_status_component.h"

enum class MainMenuState
{
    None,
    Main,
    Options,
    CharacterSelect
};

class SysMainMenu : public GuiSystem
{
    float anim_t = -1;
    glm::vec2 current_window_pos;
    glm::vec2 start_pos;
    const int main_menu_height = 400;
    const int main_menu_width = 400;
    const int choice_height = 80;
    bool first_update = true;
public:

    MainMenuState menu_state = MainMenuState::Main;


    virtual void update(double dt) override
    {

    }


    virtual void update_gui(double dt) override
    {
        if (first_update)
        {
            //ImGui::PushFont(CompWidget::fonts["default"]);
            current_window_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2);
            first_update = false;
        }
        auto title_window_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2 - 150);
        bool active = true;
        anim_t += dt;
        ImGui::SetNextWindowPos(ImVec2(current_window_pos.x, current_window_pos.y));
        //ImGui::SetNextWindowSize(ImVec2(main_menu_width, main_menu_width));
        switch (menu_state)
        {
            case MainMenuState::Main:
                {
                    draw_title();
                    const auto end_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2);
                    const float animation_length = 0.4;
                    if (anim_t < 0)
                    {
                        start_pos = current_window_pos;
                        anim_t = 0.0001;
                    }
                    if (anim_t < animation_length)
                    {
                        current_window_pos = start_pos + (end_pos - start_pos)*anim_t/animation_length;
                    }

                    ImGui::Begin("Main Menu", &active,  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                    if (ImGui::Button("New Game", ImVec2(main_menu_width-10, choice_height)))
                    {
                        menu_state = MainMenuState::CharacterSelect;
                        anim_t = -1;
                    }
                    if (ImGui::Button("Options", ImVec2(main_menu_width-10, choice_height)))
                    {
                        menu_state = MainMenuState::Options;
                    }
                    if (ImGui::Button("Quit", ImVec2(main_menu_width-10, choice_height)))
                    {
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::CharacterSelect:
                {
                    draw_title();
                    auto end_pos = glm::vec2(CompWidget::window_width/2/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2);
                    const float animation_length = 0.4;
                    if (anim_t < 0)
                    {
                        start_pos = current_window_pos;
                        anim_t = 0.0001;
                    }
                    if (anim_t < animation_length)
                    {
                        current_window_pos = start_pos + (end_pos - start_pos)*anim_t/animation_length;
                    }

                    ImGui::Begin("Character Select", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

                    auto& status_comp = get_array<CompMenuStatus>()[0];
                        //auto& active_entity = status_comp.preview_entities[status_comp.active_character.value()];
                    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
                    for (auto& [character_name, entity] : status_comp.preview_entities)
                    {
                        if (ImGui::Selectable(character_name.c_str(), &status_comp.button_status[character_name], 0, ImVec2(main_menu_width-10, choice_height)))
                        {
                            for (auto& [other_character_name, select_status] : status_comp.button_status)
                            {
                                if (other_character_name != character_name)
                                {
                                    select_status = false;
                                }
                            }
                        }
                    }
                    ImGui::PopStyleVar();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2,0.55,0.3,1.0));
                    if (ImGui::Button("Start Game", ImVec2(main_menu_width-10, choice_height)))
                    {
                        auto unit_proto = status_comp.character_protos[status_comp.active_character.value()];
                        auto& char_type_components = get_array<CompCharacterType>();
                        if (char_type_components.size())
                        {
                            char_type_components[0].type_proto = unit_proto;
                        }
                        _interface->unload_level("MainMenuLevel");
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        menu_state = MainMenuState::None;
                    }
                    ImGui::PopStyleColor(1);

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2,0.1,0.3,1.0));
                    if (ImGui::Button("Back", ImVec2(main_menu_width-10, choice_height)))
                    {
                        menu_state = MainMenuState::Main;
                        anim_t = -1.0;
                    }
                    ImGui::PopStyleColor(1);
                    ImGui::End();
                }
                break;
            case MainMenuState::Options:
                {
                    draw_title();
                    ImGui::Begin("Options", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                    if (ImGui::Button("Back"))
                    {
                        menu_state = MainMenuState::Main;
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::None:
                {
                    /*
                    ImGui::Begin("MainMenuNone");
                    ImGui::End();
                    */
                }
                break;
        }
    }
    void draw_title()
    {
        bool active = true;
        auto title_window_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2 - 150);
        ImGui::SetNextWindowPos(ImVec2(title_window_pos.x, title_window_pos.y));
        ImGui::SetNextWindowSize(ImVec2(350, 78));
        ImGui::PushFont(CompWidget::fonts["swansea"]);
        ImGui::Begin("Title", &active,  ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("b l u t o");
        ImGui::PopFont();
        ImGui::End();
    }
};

class LevelMainMenu : public Level 
{
public:

    EntityRef light_ref;
    EntityRef light_ref2;

    LevelMainMenu():
        Level("MainMenuLevel")
    {
    }

    virtual void update(double dt) override
    {
        auto& status_comp = get_array<CompMenuStatus>()[0];
        status_comp.active_character = std::nullopt;
        for (auto& [char_name, button_status] : status_comp.button_status)
        {
            if (button_status)
            {
                status_comp.active_character = char_name;
            }
        }
        for (auto& [character_name, entity] : status_comp.preview_entities)
        {
            entity.cmp<CompStaticMesh>()->set_visibility(false);
        }
        if (status_comp.active_character)
        {
            auto& active_entity = status_comp.preview_entities[status_comp.active_character.value()];
            for (auto& [character_name, entity] : status_comp.preview_entities)
            {
                if (character_name == status_comp.active_character.value())
                {
                    entity.cmp<CompStaticMesh>()->set_visibility(true);
                }
                else
                {
                    entity.cmp<CompStaticMesh>()->set_visibility(false);
                }
            }

            auto& camera = get_array<CompCamera>()[0];
            camera.graphics_camera.set_position(active_entity.cmp<CompPosition>()->pos*glm::vec3(1,1,0) - glm::vec3(10,-1,-2));
            camera.graphics_camera.set_position(active_entity.cmp<CompPosition>()->pos*glm::vec3(1,1,0) - glm::vec3(15,-1,-0.5));
            camera.graphics_camera.set_look_target(active_entity.cmp<CompPosition>()->pos*glm::vec3(1,1,0) + glm::vec3(0,0,2));

            light_ref.cmp<CompPointLight>()->light.location = glm::vec4(active_entity.cmp<CompPosition>()->pos + glm::vec3(0,5,5), 0);//camera.graphics_camera.get_position().x;
            light_ref.cmp<CompPointLight>()->light.location.z = 0.1;
            float phase = std::rand()*2.0*3.14159/RAND_MAX;
            light_ref.cmp<CompPointLight>()->light.intensity = 0.0015 + 0.0005*sin(_interface->get_current_game_time()*2.0 + 0.01*phase) + 0.0001*sin(_interface->get_current_game_time()*5.0 + 0.1*phase);
            light_ref.cmp<CompPointLight>()->light.color = glm::vec4(1.0,0.5,0.2,1.0);

            light_ref2.cmp<CompPointLight>()->light.location = glm::vec4(glm::vec3(5,5,5), 0);
            light_ref2.cmp<CompPointLight>()->light.intensity = 0.0015;
            light_ref2.cmp<CompPointLight>()->light.color = glm::vec4(0.2,0.2,1.0,1.0);

            auto move_dir = active_entity.cmp<CompSkeletalMeshNew>()->facing_vector;
            move_dir = glm::normalize(move_dir);
            auto move_angle = atan2(move_dir.y, move_dir.x);
            active_entity.cmp<CompPosition>()->rot = glm::rotate(float(move_angle + 3.14159f/2), glm::vec3(0.0f,0.0f,1.0f));
            auto& selected_objects = get_array<CompSelectedObjects>();
            for (auto& comp_selected_objects : selected_objects)
            {
                comp_selected_objects.selected_objects.resize(1);
                comp_selected_objects.selected_objects[0] = active_entity;
            }
        }
    }

    virtual void level_init() override
    {
        Sound music_sound;
        music_sound.path = "sounds\\main_theme.wav";
        music_sound.loop = true;
        music_sound.trigger = true;
        music_sound.range = 100;

        auto& hud_control = get_array<CompHudControl>()[0];
        hud_control.hud_enabled = false;
        LightEntityProto light_proto(glm::vec3(10, 10, 10));
        light_ref = _interface->add_entity_from_proto(&light_proto);

        LightEntityProto light_proto2(glm::vec3(-10, -10, 0));
        light_ref2 = _interface->add_entity_from_proto(&light_proto2);

        auto jugg_proto = std::make_shared<JuggernautProto>();
        auto tusk_proto = std::make_shared<TuskProto>();
        auto cm_proto = std::make_shared<CrystalMaidenProto>();
        _interface->add_system<SysMainMenu>();

        auto& status_comp = get_array<CompMenuStatus>()[0];
        status_comp.character_protos["Geggle"] = jugg_proto;
        status_comp.character_protos["Moodle"] = tusk_proto;
        status_comp.character_protos["Bluto"] = cm_proto;

        for (auto& [char_name, proto] : status_comp.character_protos)
        {
            auto ent = _interface->add_entity_from_proto(proto.get());
            status_comp.preview_entities[char_name] = ent;
            status_comp.button_status[char_name] = false;
            status_comp.character_protos[char_name] = proto;
            ent.cmp<CompTeam>()->team = 1;
            ent.cmp<CompPosition>()->pos = glm::vec3(10);
            ent.cmp<CompSkeletalMeshNew>()->set_animation("sleep", _interface->get_current_game_time());
            ent.cmp<CompSkeletalMeshNew>()->facing_vector = glm::normalize(glm::vec3(1,-1,0));
        }

        auto& camera = get_array<CompCamera>()[0];
        camera.graphics_camera.set_position(glm::vec3(10,10,10));
        camera.graphics_camera.set_look_target(glm::vec3(0));

        EntityRef ground = _interface->add_entity_with_components({ uint32_t(type_id<CompPhysics>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompPickupee>)
            });
        auto* bounds = ground.cmp<CompBounds>();
        auto* pos = ground.cmp<CompPosition>();

        auto landscape_mesh = std::make_shared<bgfx::Mesh>();
        landscape_mesh->load_obj("menu_land.obj", true);
        landscape_mesh->set_solid_color_by_hex(0x46835D*0.4);
        auto* lmesh = ground.cmp<CompStaticMesh>();
        lmesh->mesh.set_mesh(landscape_mesh);
        lmesh->mesh.set_id(-1);
        ground.cmp<CompVoice>()->sounds["music"] = music_sound;
        ground.set_name("LevelMesh" + std::to_string(ground.get_id()));
        
        //lmesh->mesh.set_scale(glm::vec3(5, 5, 1.0));
        auto tri_oct_comp = octree::vector_to_octree(lmesh->mesh.get_mesh()->_octree_vertices, lmesh->mesh.get_mesh()->_bmin, lmesh->mesh.get_mesh()->_bmax);
        lmesh->tri_octree = tri_oct_comp;

        bounds->is_static = true;

        bounds->set_bounds(lmesh->mesh.get_mesh()->_bmax - lmesh->mesh.get_mesh()->_bmin);
        bounds->insert_size = 5.0;
        //pos->pos = glm::vec3(1,1,1);
        //pos->scale = glm::vec3(50);
        //pos->pos= glm::vec3(10,10,0);

        EntityRef rocks = _interface->add_entity_with_components({ 
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompStaticMesh>),
                    uint32_t(type_id<CompBounds>),
                    uint32_t(type_id<CompPhysics>),
            });
        auto* rock_pos = rocks.cmp<CompPosition>();
        auto* rock_bounds = rocks.cmp<CompBounds>();
        auto* rock_physics = rocks.cmp<CompPhysics>();
        rock_bounds->is_static = true;
        rock_physics->has_gravity = false;
        rock_physics->has_collision = false;

        auto rock_mesh = std::make_shared<bgfx::Mesh>();
        rock_mesh->load_obj("menu_rocks.obj", true);
        rock_mesh->set_solid_color_by_hex(0x3F88C5);
        auto* rock_static_mesh = rocks.cmp<CompStaticMesh>();
        rock_static_mesh->mesh.set_mesh(rock_mesh);
        rock_static_mesh->mesh.set_id(10121);
        rocks.set_name("MenuRocks" + std::to_string(rocks.get_id()));
        rock_pos->pos = glm::vec3(10,9.0,0.2);

        auto rock_tri_oct_comp = octree::vector_to_octree(rock_mesh->_octree_vertices, rock_mesh->_bmin, rock_mesh->_bmax);
        //rock_static_mesh->tri_octree = tri_oct_comp;
    }

};