#pragma once

#include "component.h"
#include <glm/glm.hpp>
#include <random>

struct CompSnowfall : public Component
{

    CompSnowfall():
        volume(glm::vec3(30,30,40)),
        density(0.008)
    {

    }

    std::vector<glm::vec3> snow_positions;
    glm::vec3 volume;
    float density;
};