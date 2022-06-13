#pragma once

#include "level.h"
#include "gui_system.h"
#include "widget_component.h"

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
            current_window_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2);
            first_update = false;
        }
        anim_t += dt;
        bool active = true;
        ImGui::SetNextWindowPos(ImVec2(current_window_pos.x, current_window_pos.y));
        //ImGui::SetNextWindowSize(ImVec2(main_menu_width, main_menu_width));
        switch (menu_state)
        {
            case MainMenuState::Main:
                {
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
                    if (ImGui::Button("Juggernaut", ImVec2(main_menu_width-10, choice_height)))
                    {
                        auto unit_proto = std::make_shared<JuggernautProto>();
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
                    if (ImGui::Button("Tusk", ImVec2(main_menu_width-10, choice_height)))
                    {
                        auto unit_proto = std::make_shared<TuskProto>();
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
                    if (ImGui::Button("Crystal Maiden", ImVec2(main_menu_width-10, choice_height)))
                    {
                        auto unit_proto = std::make_shared<CrystalMaidenProto>();
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
};

class LevelMainMenu : public Level 
{
public:

    EntityRef light_ref;
    EntityRef light_ref2;
    EntityRef jugg;

    LevelMainMenu():
        Level("MainMenuLevel")
    {
    }

    virtual void update(double dt) override
    {
        auto& camera = get_array<CompCamera>()[0];
        camera.graphics_camera.set_position(jugg.cmp<CompPosition>()->pos*glm::vec3(1,1,0) + glm::vec3(10,-1,2));
        camera.graphics_camera.set_look_target(jugg.cmp<CompPosition>()->pos*glm::vec3(1,1,0) + glm::vec3(0,0,2));
        light_ref.cmp<CompPointLight>()->light.location.x = camera.graphics_camera.get_position().x;
        light_ref.cmp<CompPointLight>()->light.intensity = 0.2;

        light_ref2.cmp<CompPointLight>()->light.location.x = camera.graphics_camera.get_position().x-5;
        light_ref2.cmp<CompPointLight>()->light.intensity = 0.05;
        light_ref2.cmp<CompPointLight>()->light.color = glm::vec4(0.6,0.6,1.0,1.0);
    }

    virtual void level_init() override
    {
        LightEntityProto light_proto(glm::vec3(10, 10, 10));
        light_ref = _interface->add_entity_from_proto(&light_proto);

        LightEntityProto light_proto2(glm::vec3(-10, -10, 0));
        light_ref2 = _interface->add_entity_from_proto(&light_proto2);

        auto jugg_proto = std::make_shared<JuggernautProto>();
        auto tusk_proto = std::make_shared<TuskProto>();
        auto cm_proto = std::make_shared<CrystalMaidenProto>();
        _interface->add_system<SysMainMenu>();
        jugg = _interface->add_entity_from_proto(jugg_proto.get());
        jugg.cmp<CompTeam>()->team = 1;
        //auto tusk = _interface->add_entity_from_proto(tusk_proto.get());
        //auto cm = _interface->add_entity_from_proto(cm_proto.get());
        jugg.cmp<CompPosition>()->pos = glm::vec3(0);
        jugg.cmp<CompSkeletalMesh>()->walking = true;
        jugg.cmp<CompSkeletalMesh>()->walk_direction = glm::vec3(1,0,0);
        auto move_dir = jugg.cmp<CompSkeletalMesh>()->walk_direction;
        move_dir = glm::normalize(move_dir);
        auto move_angle = atan2(move_dir.y, move_dir.x);
        jugg.cmp<CompPosition>()->rot = glm::rotate(float(move_angle + 3.14159f/2), glm::vec3(0.0f,0.0f,1.0f));
        auto& selected_objects = get_array<CompSelectedObjects>();
        for (auto& comp_selected_objects : selected_objects)
        {
            comp_selected_objects.selected_objects.resize(1);
            comp_selected_objects.selected_objects[0] = jugg;
        }
        auto& camera = get_array<CompCamera>()[0];
        camera.graphics_camera.set_position(glm::vec3(10,10,10));
        camera.graphics_camera.set_look_target(glm::vec3(0));
    }
};