#pragma once

#include "level.h"
#include "gui_system.h"

enum class MainMenuState
{
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
                        _interface->load_level("TestLevel");

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