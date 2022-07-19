#pragma once

#include "system.h"
#include "procedural_level_component.h"

class SysLevelGeneration : public System
{
public:
	virtual void update(double dt) override
	{
		auto& procedural_level_components = get_array<CompProceduralLevel>();
		for (auto& proc_level : procedural_level_components)
		{
			if (!proc_level.generated)
			{
				generate_level(proc_level);
				proc_level.generated = true;
			}
		}
	}

	void generate_level(CompProceduralLevel& proc_level)
	{
		auto* mesh_component = proc_level.sibling<CompStaticMesh>();
		if (!mesh_component)
		{
			return;
		}
		std::uniform_real_distribution<float> dist(0, 1.0);
		std::normal_distribution<float> normal_dist(0, 1.0);
		std::mt19937 gen;

		float vertex_density_per_x = 1;
		float vertex_density_per_y = 1;

		const float probability_of_circle = 0.1;
		const float max_height = 10.0;
		const float min_height = 1.0;

		std::vector<glm::vec3> path;
		std::vector<float> widths;
		std::vector<std::tuple<float, float>> circles;
		path.push_back(glm::vec3(0, 0, 0));
		float arclength = 0.0;
		float min_x = 0.0;
		float min_y = 0.0;
		float max_x = 0.0;
		float max_y = 0.0;
		widths.push_back(generate_width(arclength, dist, gen));
		for (int i = 1; i < proc_level.length; ++i)
		{
			float x_dev = (dist(gen) - 0.5) * proc_level.x_amplitude;
			float y_dev = (dist(gen)) * proc_level.y_amplitude;
			arclength += glm::length(glm::vec2(x_dev, y_dev));
			float width = generate_width(arclength, dist, gen);
			widths.push_back(width);
			path.push_back(path.back() + glm::vec3(x_dev, y_dev, 0.0));
			if (path.back().x - widths.back() < min_x) { min_x = path.back().x - widths.back(); }
			if (path.back().x + widths.back() > max_x) { max_x = path.back().x + widths.back(); }
			if (path.back().y < min_y) { min_y = path.back().y; }
			if (path.back().y > max_y) { max_y = path.back().y; }
			if (dist(gen) < probability_of_circle)
			{
				float radius = normal_dist(gen) * proc_level.radius_std_dev + proc_level.radius_mean;
			}
		}

		float x_res = (max_x - min_x) * vertex_density_per_x;
		float y_res = (max_y - min_y) * vertex_density_per_y;
		std::vector<float> vertices;
		std::vector<float> octree_vertices;
		std::vector<glm::vec3> full_vertices;
		std::vector<float> normals;
		std::vector<unsigned int> vertex_indices;
		std::map<std::pair<int, int>, int> coord_to_index;
		for (int xi = 0; xi < x_res; ++xi)
		{
			float x_loc = xi*1.0 / vertex_density_per_x + min_x;
			for (int yi = 0; yi < y_res; ++yi)
			{
				float y_loc = yi*1.0 / vertex_density_per_y + min_y;
				vertices.push_back(x_loc);
				vertices.push_back(y_loc);
				float z_val = min_height;
				if (!in_path(path, widths, glm::vec3(x_loc, y_loc, 0.0)))
				{
					z_val = max_height;
				}
				vertices.push_back(z_val);
				full_vertices.push_back(glm::vec3(x_loc, y_loc, z_val));
				coord_to_index[std::make_pair(xi, yi)] = vertices.size()/3 - 1;
				if (xi > 0 && yi > 0)
				{
					vertex_indices.push_back(coord_to_index[{xi - 1, yi - 1}]);
					vertex_indices.push_back(coord_to_index[{xi, yi}]);
					vertex_indices.push_back(coord_to_index[{xi, yi - 1}]);
					for (int i = 0; i < 3; ++i)
					{
						auto v = full_vertices[*(vertex_indices.end() - 3 + i)];
						octree_vertices.push_back(v.x);
						octree_vertices.push_back(v.y);
						octree_vertices.push_back(v.z);
					}

					vertex_indices.push_back(coord_to_index[{xi - 1, yi - 1}]);
					vertex_indices.push_back(coord_to_index[{xi - 1, yi}]);
					vertex_indices.push_back(coord_to_index[{xi, yi}]);
					for (int i = 0; i < 3; ++i)
					{
						auto v = full_vertices[*(vertex_indices.end() - 3 + i)];
						octree_vertices.push_back(v.x);
						octree_vertices.push_back(v.y);
						octree_vertices.push_back(v.z);
					}
				}
			}
		}
		vertices[2] = 0.0;
		for (int xi = 0; xi < x_res; ++xi)
		{
			for (int yi = 0; yi < y_res; ++yi)
			{
				if (xi == 0 || yi == 0 || xi == x_res - 1 || yi == y_res - 1)
				{
					normals.push_back(0);
					normals.push_back(0);
					normals.push_back(1);
				}
				else
				{
						
					glm::vec3 cross1 = full_vertices[coord_to_index[{xi - 1, yi - 1}]] - full_vertices[coord_to_index[{xi + 1, yi + 1}]];
					glm::vec3 cross2 = full_vertices[coord_to_index[{xi + 1, yi - 1}]] - full_vertices[coord_to_index[{xi - 1, yi + 1}]];
					glm::vec3 norm = glm::normalize(glm::cross(cross1, cross2));
					normals.push_back(norm.x);
					normals.push_back(norm.y);
					normals.push_back(norm.z);
				}
			}
		}
		auto level_bgfx_mesh = std::make_shared<bgfx::Mesh>();
		level_bgfx_mesh->set_normals(normals);
		level_bgfx_mesh->set_vertices(vertices);
		level_bgfx_mesh->set_vertex_indices(vertex_indices);
		level_bgfx_mesh->_bmax = glm::vec3(max_x, max_y, max_height);
		level_bgfx_mesh->_bmin = glm::vec3(min_x, min_y, 0.0);
		level_bgfx_mesh->_octree_vertices = octree_vertices;
		level_bgfx_mesh->_saved_vertices = vertices;
		level_bgfx_mesh->_indexed = true;
		mesh_component->mesh.set_mesh(level_bgfx_mesh);
		mesh_component->mesh.set_id(-1);
        auto tri_oct_comp = octree::vector_to_octree(mesh_component->mesh.get_mesh()->_octree_vertices, mesh_component->mesh.get_mesh()->_bmin, mesh_component->mesh.get_mesh()->_bmax, glm::mat3(1));
        mesh_component->tri_octree = tri_oct_comp;
		level_bgfx_mesh->set_solid_color(glm::vec4(0, 1, 0, 1));
	}

	float generate_width(float arclength, const std::uniform_real_distribution<float>& dist, std::mt19937& gen)
	{
		std::vector<std::tuple<float, float>> sinusoids =
		{
			{5.0, 0.002},
			{20.0, 0.005},
			{1.0, 0.01}
		};
		float out_value = 0.0;
		for (auto& [amplitude, frequency] : sinusoids)
		{
			out_value += (amplitude * sin(arclength * frequency + 2*dist(gen)));
		}
		std::cout << out_value << "\n";
		return std::max(10.0f,std::abs(out_value));
	}

	bool in_path(const std::vector<glm::vec3>& path, const std::vector<float>& widths, const glm::vec3& in_point)
	{
		float prev_width = widths[0];
		for (int i = 1; i < path.size(); ++i)
		{
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
};
