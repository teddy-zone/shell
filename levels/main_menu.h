#pragma once

#include <random>
#include "level.h"
#include "gui_system.h"
#include "widget_component.h"
#include "hud_control_component.h"
#include "main_menu_status_component.h"
#include "volume_settings_component.h"
#include "spawner_system.h"

enum class MainMenuState
{
    None,
    Main,
    Options,
    CharacterSelect,
    VideoOptions,
    AudioOptions
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
        glm::vec2 center_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2);
        if (first_update)
        {
            current_window_pos = center_pos;
            first_update = false;
        }
        auto title_window_pos = glm::vec2(CompWidget::window_width/2 - main_menu_width/2, CompWidget::window_height/2 - main_menu_height/2 - 150);
        bool active = true;
        anim_t += dt;
        //ImGui::SetNextWindowSize(ImVec2(main_menu_width, main_menu_width));

        if (menu_state != MainMenuState::None)
        {
            draw_title();
        }
        ImGui::SetNextWindowPos(ImVec2(current_window_pos.x, current_window_pos.y));
        ImGui::SetNextWindowSizeConstraints(ImVec2(main_menu_width, choice_height*3), ImVec2(main_menu_width+5, choice_height*8));
        ImVec2 button_size(main_menu_width - 10, choice_height);
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
                        _interface->quit_game();
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
                        if (ImGui::Selectable(character_name.c_str(), &status_comp.button_status[character_name], 0, button_size))
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
                    if (ImGui::Button("Start Game", button_size))
                    {
                        if (status_comp.active_character)
                        {
                            auto unit_proto = status_comp.character_protos[status_comp.active_character.value()];
                            auto& char_type_components = get_array<CompCharacterType>();
                            if (char_type_components.size())
                            {
                                char_type_components[0].type_proto = unit_proto;
                            }
                            auto& spawn_controller = get_array<CompSpawnController>()[0];
                            spawn_controller.enable_spawning = true;
                            _interface->unload_level("MainMenuLevel");
                            _interface->unload_level("TestLevel");
                            _interface->load_level("BaseLevel");
                            _interface->load_level("TestLevel");
                            menu_state = MainMenuState::None;
                        }
                        else
                        {

                        }
                    }
                    ImGui::PopStyleColor(1);

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2,0.1,0.3,1.0));
                    if (ImGui::Button("Back", button_size))
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
                    if (ImGui::Button("Video", button_size))
                    {
                        menu_state = MainMenuState::VideoOptions;
                    }
                    if (ImGui::Button("Audio", button_size))
                    {
                        menu_state = MainMenuState::AudioOptions;
                    }
                    if (ImGui::Button("Back", button_size))
                    {
                        menu_state = MainMenuState::Main;
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::VideoOptions:
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
                        menu_state = MainMenuState::Options;
                    }
                    ImGui::End();
                }
                break;
            case MainMenuState::AudioOptions:
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
                        menu_state = MainMenuState::Options;
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
    glm::vec3 player_pos;

    LevelMainMenu():
        Level("MainMenuLevel")
    {
    }

    virtual void update(double dt) override
    {
        static int first_update = 4;
        if (first_update == 0)
        {
            
        }
        else
        {
            first_update -= 1;
        }
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

        auto ray_result = _interface->fire_ray(ray::New(player_pos + glm::vec3(0, 0, 10), glm::vec3(0, 0, -1)), ray::HitType::StaticOnly, 100);

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
            camera.graphics_camera.set_position(player_pos + glm::vec3(10,-1,-2));
            camera.graphics_camera.set_position(player_pos + glm::vec3(15,-1,-0.5));
            camera.graphics_camera.set_look_target(player_pos + glm::vec3(0,0,2));

            /*
            light_ref.cmp<CompPointLight>()->light.location = glm::vec4(player_pos + glm::vec3(0,5,5), 0);//camera.graphics_camera.get_position().x;
            light_ref.cmp<CompPointLight>()->light.location.z = 0.1;
            float phase = std::rand()*2.0*3.14159/RAND_MAX;
            light_ref.cmp<CompPointLight>()->light.intensity = 0.0015 + 0.0005*sin(_interface->get_current_game_time()*2.0 + 0.01*phase) + 0.0001*sin(_interface->get_current_game_time()*5.0 + 0.1*phase);
            light_ref.cmp<CompPointLight>()->light.color = glm::vec4(1.0,0.5,0.2,1.0);

            light_ref2.cmp<CompPointLight>()->light.location = glm::vec4(glm::vec3(5,5,5), 0);
            light_ref2.cmp<CompPointLight>()->light.intensity = 0.0015;
            light_ref2.cmp<CompPointLight>()->light.color = glm::vec4(0.2,0.2,1.0,1.0);
            */

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

        /*
        auto& status_comp = get_array<CompMenuStatus>()[0];

        for (auto& [char_name, proto] : status_comp.character_protos)
        {
            auto ent = _interface->add_entity_from_proto(proto.get());
            status_comp.preview_entities[char_name] = ent;
            status_comp.button_status[char_name] = false;
            status_comp.character_protos[char_name] = proto;
            ent.cmp<CompTeam>()->team = 1;
            ent.cmp<CompPosition>()->pos = player_pos;
            ent.cmp<CompSkeletalMeshNew>()->set_animation("sleep", _interface->get_current_game_time());
            ent.cmp<CompSkeletalMeshNew>()->facing_vector = glm::normalize(glm::vec3(1, -1, 0));
        }
        */
    }

    virtual void level_init() override
    {
        Sound music_sound;
        music_sound.path = "sounds\\main_theme.wav";
        music_sound.loop = true;
        music_sound.trigger = true;
        music_sound.volume_type = SoundVolumeType::Music;
        music_sound.range = 100;
        music_sound.sound_name = "main_theme_menu";

        _interface->load_level("TestLevel");

        auto& hud_control = get_array<CompHudControl>()[0];
        hud_control.hud_enabled = false;

        /*
        LightEntityProto light_proto(glm::vec3(10, 10, 10));
        light_ref = _interface->add_entity_from_proto(&light_proto);

        LightEntityProto light_proto2(glm::vec3(-10, -10, 0));
        light_ref2 = _interface->add_entity_from_proto(&light_proto2);
        */

        auto jugg_proto = std::make_shared<JuggernautProto>();
        auto tusk_proto = std::make_shared<TuskProto>();
        auto cm_proto = std::make_shared<CrystalMaidenProto>();
        _interface->add_system<SysMainMenu>();

        auto& status_comp = get_array<CompMenuStatus>()[0];
        status_comp.character_protos["Geggle"] = jugg_proto;
        status_comp.character_protos["Moodle"] = tusk_proto;
        status_comp.character_protos["Bluto"] = cm_proto;

        player_pos = glm::vec3(115, 140, 20);

        for (auto& [char_name, proto] : status_comp.character_protos)
        {
            auto ent = _interface->add_entity_from_proto(proto.get());
            status_comp.preview_entities[char_name] = ent;
            status_comp.button_status[char_name] = false;
            status_comp.character_protos[char_name] = proto;
            ent.cmp<CompTeam>()->team = 1;
            ent.cmp<CompPosition>()->pos = player_pos;
            ent.cmp<CompSkeletalMeshNew>()->set_animation("sleep", _interface->get_current_game_time());
            ent.cmp<CompSkeletalMeshNew>()->facing_vector = glm::normalize(glm::vec3(-0.5,1,0));
        }

        auto& camera = get_array<CompCamera>()[0];
        camera.graphics_camera.set_position(glm::vec3(10,10,10));
        camera.graphics_camera.set_look_target(glm::vec3(0));

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

        auto rock_tri_oct_comp = octree::vector_to_octree(rock_mesh->_octree_vertices, rock_mesh->_bmin, rock_mesh->_bmax, glm::mat3(1));
        //rock_static_mesh->tri_octree = tri_oct_comp;
        auto& spawn_controller = get_array<CompSpawnController>()[0];
        spawn_controller.enable_spawning = false;
    }

};