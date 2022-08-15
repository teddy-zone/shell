#pragma once

#include "component.h"

struct CompItem : public Component
{
    std::string name;
    bool has_charges;
    int num_charges;
    int max_charges;
};
