#pragma once

#include <functional>

#include "component.h"
#include "system_interface.h"
#include "command_component.h"

struct InteractCommand : public Command
{
    EntityRef interact_target;
};

struct CompInteractable : public Component
{
    float interact_range;
    std::function<void(SystemInterface*, EntityRef, EntityRef)> interaction_callback;
};