#pragma once

#include <vector>
#include <array>

#include "component.h"

template <int Angles>
struct CompEyeBase : public Component
{
    static constexpr int NumAngles = Angles;
    float vision_range;
    std::array<float, Angles> fow;
    bool do_display = false;
};

typedef CompEyeBase<120> CompEye;