#pragma once

#include "component.h"

struct CompProceduralLevel : public Component
{
	bool generated;
	int length;
	float x_amplitude;
	float y_amplitude;
	float radius_mean;
	float radius_std_dev;
	glm::vec3 spawn_point;
	std::vector<glm::vec3> path;
	std::vector<float> widths;
	float floor_level;
	float cliff_level;
	float total_arclength;
	std::map<int, glm::vec3> y_indexed_path_bottom_edges;
	std::map<int, glm::vec3> y_indexed_path_top_edges;
	std::map<int, glm::vec3> y_indexed_path_bottom_normals;
	std::map<int, glm::vec3> y_indexed_path_top_normals;

	bool in_path(const glm::vec3& in_point) const
	{
		float prev_width = widths[0];
		for (int i = 1; i < path.size(); ++i)
		{
			auto path_vec = path[i] - path[i - 1];
			auto normal = glm::cross(path_vec, glm::vec3(0, 0, 1));
			float avg_width = (prev_width + widths[i]) / 2.0;
			glm::vec3 avg_loc = (path[i - 1] + path[i]) / 2.0;
			if (glm::length(path[i] - in_point) < widths[i])
			{
				return true;
			}
			if (glm::length(avg_loc - in_point) < avg_width)
			{
				return true;
			}
			prev_width = widths[i];
		}
		return false;
	}

	glm::vec3 path_bottom_edge(int path_index) const
	{
		float current_arclength = 0;
		for (int i = 0; i < path.size(); ++i)
		{
			if (i == path_index)
			{
				return path_bottom_edge(current_arclength);
			}
			auto next_path_vector = path[i+1] - path[i];
			float next_length = glm::length(next_path_vector);
			current_arclength += next_length;
		}
	}

	glm::vec3 path_bottom_edge(float arclength) const
	{
		float current_arclength = 0;
		for (int i = 1; i < path.size(); ++i)
		{
			auto next_path_vector = path[i] - path[i - 1];
			float next_length = glm::length(next_path_vector);
			glm::vec3 norm_next_path_vector = glm::normalize(next_path_vector);
			if (current_arclength + next_length > arclength)
			{
				auto point = path[i - 1] + (norm_next_path_vector * (arclength - current_arclength));
				auto norm = glm::normalize(0.7*glm::vec3(1,0,0) + 0.3*glm::normalize(glm::cross(norm_next_path_vector, glm::vec3(0, 0, 1))));
				auto start_point = point + norm * (widths[i] + widths[i - 1]) / 2;
				auto prev_start_point = start_point;
				auto start_point_in_path = in_path(start_point);
				float deviation = 1.0;
				const int max_deviations = 100;
				int number_of_deviations = 0;
				if (start_point_in_path)
				{
					while (start_point_in_path && number_of_deviations < max_deviations)
					{
						start_point -= norm * deviation;
						start_point_in_path = in_path(start_point);
						prev_start_point = start_point;
						number_of_deviations++;
					}
					return prev_start_point;
				}
				else
				{
					while (!start_point_in_path && number_of_deviations < max_deviations)
					{
						start_point += norm * deviation;
						start_point_in_path = in_path(start_point);
						prev_start_point = start_point;
						number_of_deviations++;
					}
					return start_point;
				}
			}
			current_arclength += next_length;
		}
	}
};
