#pragma once

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
            camera.graphics_camera.set_position(active_entity.cmp<CompPosition>()->pos*glm::vec3(1,0,0) + glm::vec3(10,-1,2));
            camera.graphics_camera.set_look_target(active_entity.cmp<CompPosition>()->pos*glm::vec3(1,0,0) + glm::vec3(0,0,2));
            light_ref.cmp<CompPointLight>()->light.location.x = camera.graphics_camera.get_position().x;
            light_ref.cmp<CompPointLight>()->light.intensity = 0.02;

            light_ref2.cmp<CompPointLight>()->light.location.x = camera.graphics_camera.get_position().x-5;
            light_ref2.cmp<CompPointLight>()->light.intensity = 0.001;
            light_ref2.cmp<CompPointLight>()->light.color = glm::vec4(0.6,0.6,1.0,1.0);

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
        status_comp.character_protos["Juggernaut"] = jugg_proto;
        status_comp.character_protos["Tusk"] = tusk_proto;
        status_comp.character_protos["Crystal Maiden"] = cm_proto;

        for (auto& [char_name, proto] : status_comp.character_protos)
        {
            auto ent = _interface->add_entity_from_proto(proto.get());
            status_comp.preview_entities[char_name] = ent;
            status_comp.button_status[char_name] = false;
            status_comp.character_protos[char_name] = proto;
            ent.cmp<CompTeam>()->team = 1;
            ent.cmp<CompPosition>()->pos = glm::vec3(10);
            ent.cmp<CompSkeletalMeshNew>()->current_animation = "idle";
            ent.cmp<CompSkeletalMeshNew>()->facing_vector = glm::vec3(1,0,0);
        }

        auto& camera = get_array<CompCamera>()[0];
        camera.graphics_camera.set_position(glm::vec3(10,10,10));
        camera.graphics_camera.set_look_target(glm::vec3(0));
    }
};