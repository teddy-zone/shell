#pragma once

#include <random>
#include "level.h"
#include "gui_system.h"
#include "widget_component.h"
#include "hud_control_component.h"
#include "main_menu_status_component.h"
#include "volume_settings_component.h"
#include "spawner_system.h"
#include "keystate_component.h"

enum class PauseMenuState
{
    None,
    Main,
    Options,
    VideoOptions,
    AudioOptions,
    AreYouSure,
};

class SysPause : public GuiSystem
{
    float anim_t = -1;
    glm::vec2 current_window_pos;
    glm::vec2 start_pos;
    const int main_menu_height = 400;
    const int main_menu_width = 400;
    const int choice_height = 80;
    bool first_update = true;
public:

    PauseMenuState menu_state = PauseMenuState::Main;

    virtual void update(double dt) override
    {

    }
    
    virtual void update_gui(double dt) override
    {
        auto& keystate = get_array<CompKeyState>()[0];
        auto& time_comp = get_array<CompTime>()[0];
        if (keystate.push[GLFW_KEY_ESCAPE])
        {
            if (time_comp.is_paused)
            {
                std::cout << "UNPAUSEEEEE" << "\n";
                _interface->unpause_game();
            }
            else
            {
                std::cout << "PAUSEEEEE" << "\n";
                _interface->pause_game();
                menu_state = PauseMenuState::Main;
            }
        }
        glm::vec2 center_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2);
        if (first_update)
        {
            current_window_pos = center_pos;
            first_update = false;
        }
        auto title_window_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2 - 150);
        bool active = true;
        anim_t += dt;
        if (time_comp.is_paused)
        {
			if (menu_state != PauseMenuState::None)
			{
				draw_title();
			}
			ImGui::SetNextWindowPos(ImVec2(current_window_pos.x, current_window_pos.y));
			ImGui::SetNextWindowSizeConstraints(ImVec2(main_menu_width, choice_height*2), ImVec2(main_menu_width+5, choice_height*8));
			ImVec2 button_size(main_menu_width - 10, choice_height);

            switch (menu_state)
            {
            case PauseMenuState::Main:
            {
                const auto end_pos = glm::vec2(CompWidget::window_width / 2 - main_menu_width / 2, CompWidget::window_height / 2 - main_menu_height / 2);
                const float animation_length = 0.4;
                if (anim_t < 0)
                {
                    start_pos = current_window_pos;
                    anim_t = 0.0001;
                }
                if (anim_t < animation_length)
                {
                    current_window_pos = start_pos + (end_pos - start_pos) * anim_t / animation_length;
                }

                ImGui::Begin("Main Menu", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                if (ImGui::Button("Resume", ImVec2(main_menu_width - 10, choice_height)))
                {
                    menu_state = PauseMenuState::None;
                    _interface->unpause_game();
                }
                else if (ImGui::Button("Options", ImVec2(main_menu_width - 10, choice_height)))
                {
                    menu_state = PauseMenuState::Options;
                }
                else if (ImGui::Button("Quit To Menu", ImVec2(main_menu_width - 10, choice_height)))
                {
                    menu_state = PauseMenuState::AreYouSure;
                }
                ImGui::End();
            }
            break;
            case PauseMenuState::Options:
                {
                    ImGui::Begin("Options", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                    if (ImGui::Button("Video", button_size))
                    {
                        menu_state = PauseMenuState::VideoOptions;
                    }
                    if (ImGui::Button("Audio", button_size))
                    {
                        menu_state = PauseMenuState::AudioOptions;
                    }
                    if (ImGui::Button("Back", button_size))
                    {
                        menu_state = PauseMenuState::Main;
                    }
                    ImGui::End();
                }
                break;
            case PauseMenuState::VideoOptions:
            {
                auto& status_comp = get_array<CompMenuStatus>()[0];
                ImGui::Begin("Options", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                if (ImGui::Button("Resolution", button_size))
                {
                    status_comp.resolution_button_pressed ^= 1;
                }
                if (status_comp.resolution_button_pressed)
                {
                    auto& comp_graphics = get_array<CompGraphics>()[0];
                    int num_modes = -1;
                    const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &num_modes);

                    auto& status_comp = get_array<CompMenuStatus>()[0];
                    //auto& active_entity = status_comp.preview_entities[status_comp.active_character.value()];
                    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

                    std::set<std::pair<int, int>> done_modes;
                    if (num_modes > 0)
                    {
                        for (int i = num_modes - 1; i >= 0; --i)
                        {
                            if (done_modes.find(std::make_pair(modes[i].width, modes[i].height)) == done_modes.end())
                            {
                                if (ImGui::Button((std::to_string(modes[i].width) + "x" + std::to_string(modes[i].height)).c_str(),
                                    ImVec2(button_size.x - 40, button_size.y - 15)))
                                {
                                    comp_graphics.resolution_setting = glm::ivec2(modes[i].width, modes[i].height);
                                }
                            }
                            done_modes.insert(std::make_pair(modes[i].width, modes[i].height));
                        }
                    }
                    ImGui::PopStyleVar();
                }
                if (ImGui::Button("Windowed/Fullscreen", button_size))
                {

                }
                if (ImGui::Button("Back", button_size))
                {
                    menu_state = PauseMenuState::Options;
                }
                ImGui::End();
            }
            break;
            case PauseMenuState::AudioOptions:
            {
                ImGui::Begin("Options", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                auto& volume_settings_array = get_array<CompVolumeSettings>();
                if (volume_settings_array.size())
                {
                    if (ImGui::SliderFloat("Master Volume", &volume_settings_array[0].master_volume, 0, 1.0))
                    {
                        volume_settings_array[0].do_update = true;
                    }
                    if (ImGui::SliderFloat("Sound FX Volume", &volume_settings_array[0].sound_fx_volume, 0, 1.0))
                    {
                        volume_settings_array[0].do_update = true;
                    }
                    if (ImGui::SliderFloat("Music Volume", &volume_settings_array[0].music_volume, 0, 1.0))
                    {
                        volume_settings_array[0].do_update = true;
                    }
                    if (ImGui::Button(""))
                    {

                    }
                }
                if (ImGui::Button("Back"))
                {
                    menu_state = PauseMenuState::Options;
                }
                ImGui::End();
            }
            break;
            case PauseMenuState::AreYouSure:
            {
                ImGui::Begin("Options", &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                ImGui::Text("Are you sure you want to quit?");
                ImGui::Text("This run will be lost to time...");
                if (ImGui::Button("Yes"))
                {
                    _interface->unload_all();
                    _interface->load_level("MainMenuLevel");
                    menu_state = PauseMenuState::None;
                    _interface->unpause_game();
                }
                else if (ImGui::Button("No"))
                {
                    menu_state = PauseMenuState::Main;
                }
                ImGui::End();
            }
            case PauseMenuState::None:
            {
                /*
                ImGui::Begin("MainMenuNone");
                ImGui::End();
                */
            }
            break;
            }
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
