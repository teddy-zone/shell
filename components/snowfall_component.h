#pragma once

#include "component.h"
#include <glm/glm.hpp>
#include <random>

struct CompSnowfall : public Component
{

    CompSnowfall():
        volume(glm::vec3(50,50,50)),
        density(2.0)
    {

    }

    std::vector<glm::vec3> snow_positions;
    glm::vec3 volume;
    float density;
};