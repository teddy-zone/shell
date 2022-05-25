#pragma once

#include "level.h"
#include "gui_system.h"

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
        switch (menu_state)
        {
            case MainMenuState::Main:
                {
                    ImGui::Begin("MainMenu");
                    if (ImGui::Button("New Game"))
                    {
                        menu_state = MainMenuState::CharacterSelect;
                    }
                    if (ImGui::Button("Options"))
                    {
                        menu_state = MainMenuState::Options;
                    }
                    if (ImGui::Button("Quit"))
                    {
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::CharacterSelect:
                {
                    ImGui::Begin("CharacterSelect");
                    if (ImGui::Button("Crystal Maiden"))
                    {
                        auto unit_proto = std::make_shared<UnitProto>(glm::vec3(1.0));
                        auto& char_type_components = get_array<CompCharacterType>();
                        if (char_type_components.size())
                        {
                            char_type_components[0].type_proto = unit_proto;
                        }
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        menu_state = MainMenuState::None;
                    }
                    if (ImGui::Button("Tusk"))
                    {
                        auto unit_proto = std::make_shared<UnitProto>(glm::vec3(1.0));
                        auto& char_type_components = get_array<CompCharacterType>();
                        if (char_type_components.size())
                        {
                            char_type_components[0].type_proto = unit_proto;
                        }
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        menu_state = MainMenuState::None;
                    }
                    if (ImGui::Button("Sniper"))
                    {
                        auto unit_proto = std::make_shared<UnitProto>(glm::vec3(1.0));
                        auto& char_type_components = get_array<CompCharacterType>();
                        if (char_type_components.size())
                        {
                            char_type_components[0].type_proto = unit_proto;
                        }
                        _interface->load_level("BaseLevel");
                        _interface->load_level("TestLevel");
                        menu_state = MainMenuState::None;
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::Options:
                {
                    ImGui::Begin("Options");
                    if (ImGui::Button("Back"))
                    {
                        menu_state = MainMenuState::Main;
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::None:
                {
                    ImGui::Begin("MainMenuNone");
                    ImGui::End();
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