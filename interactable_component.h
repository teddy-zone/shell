#pragma once

#include <functional>

#include "component.h"
#include "system_interface.h"
#include "command_component.h"

struct InteractCommand : public Command
{
    EntityRef interact_target;
    float interact_range;
}

struct CompInteractable : public Component
{
    std::function<void(SystemInterface*, EntityRef, EntityRef)> interaction_callback;
};