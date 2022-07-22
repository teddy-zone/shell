#pragma once

#include "component.h"

struct CompMenuStatus : public Component
{
    std::unordered_map<std::string, EntityRef> preview_entities;
    std::unordered_map<std::string, std::shared_ptr<EntityProto>> character_protos;
    std::unordered_map<std::string, bool> button_status;
    bool resolution_button_pressed = false;
    std::optional<std::string> active_character;
};