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
public:

    MainMenuState menu_state = MainMenuState::Main;

    virtual void update(double dt) override
    {

    }

    virtual void update_gui(double dt) override
    {
        const int choice_height = 80;
        const int main_menu_height = 400;
        const int main_menu_width = 400;
        bool active = true;
        ImGui::SetNextWindowPos(ImVec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2));
        //ImGui::SetNextWindowSize(ImVec2(main_menu_width, main_menu_width));
        switch (menu_state)
        {
            case MainMenuState::Main:
                {
                    ImGui::Begin("Main Menu", &active,  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                    if (ImGui::Button("New Game", ImVec2(main_menu_width-10, choice_height)))
                    {
                        menu_state = MainMenuState::CharacterSelect;
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
                    ImGui::Begin("Character Select", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                    if (ImGui::Button("Juggernaut", ImVec2(main_menu_width-10, choice_height)))
                    {
                        auto unit_proto = std::make_shared<JuggernautProto>();
                        auto& char_type_components = get_array<CompCharacterType>();
                        if (char_type_components.size())
                        {
                            char_type_components[0].type_proto = unit_proto;
                        }
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        _interface->unload_level("MainMenu");
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
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        _interface->unload_level("MainMenu");
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
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        _interface->unload_level("MainMenu");
                        menu_state = MainMenuState::None;
                    }
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

    LevelMainMenu():
        Level("MainMenuLevel")
    {
    }

    virtual void update(double dt) override
    {

    }

    virtual void level_init() override
    {
        _interface->add_system<SysMainMenu>();
    }
};