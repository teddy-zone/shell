#pragma once

#include "component.h"


struct CompExperience : public Component
{
    inline static std::vector<float> exp_curve =
    {
        0,
        100,
        250,
        600,
        1100,
        1900,
        3000,
        4500,
    };

    float experience;
    unsigned int get_level() const
    {
        int lower_level = 1;
        int higher_level = exp_curve.size();
        if (experience > exp_curve.back())
        {
            return exp_curve.size();
        }
        if (experience < exp_curve[0])
        {
            return 0;
        }
        while (higher_level != lower_level + 1)
        {
            int mid_point = std::floor((higher_level - lower_level)/2);
            if (experience > exp_curve[mid_point])
            {
                if (lower_level == mid_point)
                {
                    lower_level += 1;
                }
                else
                {
                    lower_level = mid_point;
                }
            }
            else if (experience < exp_curve[mid_point])
            {
                higher_level = mid_point;
            }
            else
            {
                return lower_level;
            }
        }
        return lower_level;
    }

    void add_experience(float in_exp)
    {
        experience = std::min(std::max(experience+in_exp,0.0f), exp_curve.back());
    }

    void add_levels(int levels)
    {
        auto cur_level = int(get_level());
        auto new_level = std::min(std::max(cur_level + levels, 0), int(exp_curve.size() - 1));
        experience = exp_curve[new_level];
    }

};