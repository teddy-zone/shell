#pragma once

#include "component.h"

struct CompRespawn : public Component
{
    float default_respawn_time;
    std::optional<float> current_respawn_time;
    glm::vec3 death_location;
};