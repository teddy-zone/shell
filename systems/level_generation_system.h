#pragma once

#include "system.h"
#include "loading_system.h"
#include "procedural_level_component.h"

#include "octree_triangle.h"
#include "serial_octree.h"

class SysLevelGeneration : public LoadingSystem
{
	struct WorkState
	{
		octree::TriangleOctree<octree::SerialOctree<Triangle>>* tri_octree;
		std::vector<float>* triangle_floats;
		glm::vec3 bmin;
		glm::vec3 bmax;
		glm::mat3 rot;
		glm::vec3 bounds;
		int num_triangles;
		int current_node_array_size = 1;
		float max_dim;
		int i_state = 0;
		EntityRef level_entity;
		std::shared_ptr<bgfx::Mesh> level_bgfx_mesh;
		float x_res;
		float y_res;
		float extra_width;
		float vertex_density_per_x;
		float vertex_density_per_y;
		float max_height;
		float min_height;
		float min_x;
		float max_x;
		float min_y;
		float max_y;
		std::vector<glm::vec3> path;
		std::vector<float> widths;
		std::vector<std::vector<bool>> in_path_matrix;
	};

	WorkState state;

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
		state = WorkState();
		auto* mesh_component = proc_level.sibling<CompStaticMesh>();
		if (!mesh_component)
		{
			return;
		}
		std::uniform_real_distribution<float> dist(0, 1.0);
		std::normal_distribution<float> normal_dist(0, 1.0);
		std::mt19937 gen(int(_interface->get_current_game_time() * 1000));
		const float probability_of_circle = 0.1;

		std::vector<std::tuple<float, float>> circles;
		state.path.push_back(glm::vec3(0, 0, 0));
		float arclength = 0.0;
		glm::vec3 travel_vector = glm::vec3(0, 1, 0);
		state.widths.push_back(generate_width(arclength, dist, gen));
		for (int i = 1; i < proc_level.length; ++i)
		{
			float x_dev = (dist(gen) - 0.5) * proc_level.x_amplitude;
			float y_dev = (dist(gen)) * proc_level.y_amplitude;
			arclength += glm::length(glm::vec2(x_dev, y_dev));
			float width = generate_width(arclength, dist, gen);
			state.widths.push_back(width);
			state.path.push_back(state.path.back() + glm::vec3(x_dev, y_dev, 0.0));
			if (state.path.back().x - state.widths.back() < state.min_x) { state.min_x = state.path.back().x - state.widths.back(); }
			if (state.path.back().x + state.widths.back() > state.max_x) { state.max_x = state.path.back().x + state.widths.back(); }
			if (state.path.back().y < state.min_y) { state.min_y = state.path.back().y; }
			if (state.path.back().y > state.max_y) { state.max_y = state.path.back().y; }
			if (dist(gen) < probability_of_circle)
			{
				float radius = normal_dist(gen) * proc_level.radius_std_dev + proc_level.radius_mean;
			}
		}

		proc_level.total_arclength = arclength;

		generate_mesh(proc_level);

	}

	void generate_mesh(CompProceduralLevel& proc_level)
	{
		state.extra_width = 10;
		state.vertex_density_per_x = 1;
		state.vertex_density_per_y = 1;

		state.max_height = 30.0;
		state.min_height = 20.0;

		state.x_res = ((state.max_x + state.extra_width) - (state.min_x - state.extra_width)) * state.vertex_density_per_x;
		state.y_res = ((state.max_y + state.extra_width) - (state.min_y - state.extra_width)) * state.vertex_density_per_y;

		std::vector<float> vertices;
		std::vector<float> octree_vertices;
		std::vector<glm::vec3> full_vertices;
		std::vector<float> normals;
		std::vector<unsigned int> vertex_indices;
		std::map<std::pair<int, int>, int> coord_to_index;
		for (int xi = 0; xi < state.x_res; ++xi)
		{
			state.in_path_matrix.push_back(std::vector<bool>());
			float x_loc = xi * 1.0 / state.vertex_density_per_x + state.min_x - state.extra_width;
			for (int yi = 0; yi < state.y_res; ++yi)
			{
				float y_loc = yi * 1.0 / state.vertex_density_per_y + state.min_y - state.extra_width;
				vertices.push_back(x_loc);
				vertices.push_back(y_loc);
				float z_val = state.min_height;
				float in_path_value = in_path(state.path, state.widths, glm::vec3(x_loc, y_loc, 0.0));
				if (in_path_value < 0)
				{
					state.in_path_matrix.back().push_back(false);
					z_val = state.max_height;
				}
				else
				{
					state.in_path_matrix.back().push_back(true);
					z_val = state.min_height;// +in_path_value;
				}
				vertices.push_back(z_val);
				full_vertices.push_back(glm::vec3(x_loc, y_loc, z_val));
				coord_to_index[std::make_pair(xi, yi)] = vertices.size() / 3 - 1;
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
		vertices.back() = 50;
		for (int xi = 0; xi < state.x_res; ++xi)
		{
			for (int yi = 0; yi < state.y_res; ++yi)
			{
				if (xi == 0 || yi == 0 || xi == state.x_res - 1 || yi == state.y_res - 1)
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
		state.level_bgfx_mesh = std::make_shared<bgfx::Mesh>();
		state.level_bgfx_mesh->set_normals(normals);
		state.level_bgfx_mesh->set_vertices(vertices);
		state.level_bgfx_mesh->set_vertex_indices(vertex_indices);
		state.level_bgfx_mesh->_bmax = glm::vec3(state.max_x, state.max_y, state.max_height);
		state.level_bgfx_mesh->_bmin = glm::vec3(state.min_x, state.min_y, 0.0);
		state.level_bgfx_mesh->_octree_vertices = octree_vertices;
		state.level_bgfx_mesh->_saved_vertices = vertices;
		state.level_bgfx_mesh->_indexed = true;
		auto* mesh_component = proc_level.sibling<CompStaticMesh>();
		mesh_component->mesh.set_mesh(state.level_bgfx_mesh);
		mesh_component->mesh.set_id(-1);

		// WORK

		auto& in_vec = mesh_component->mesh.get_mesh()->_octree_vertices;
		state.tri_octree = new octree::TriangleOctree<octree::SerialOctree<Triangle>>;
		state.num_triangles = in_vec.size() / 3 / 3;
		state.tri_octree->data = new Triangle[state.num_triangles];
		const int num_possible_nodes = 10000000;
		state.tri_octree->nodes = new octree::SerialNode<uint32_t>[num_possible_nodes];
		state.triangle_floats = &mesh_component->mesh.get_mesh()->_octree_vertices;

		state.bmin = mesh_component->mesh.get_mesh()->_bmin;
		state.bmax = mesh_component->mesh.get_mesh()->_bmax;
		state.max_dim = std::max(state.bounds.x, std::max(state.bounds.y, state.bounds.z));
		//auto tri_oct_comp = octree::vector_to_octree(mesh_component->mesh.get_mesh()->_octree_vertices, mesh_component->mesh.get_mesh()->_bmin, mesh_component->mesh.get_mesh()->_bmax, glm::mat3(1));

		do_work = true;
		state.level_entity = proc_level.get_entity();
		// END WORK

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
			out_value += (amplitude * sin(arclength * frequency + 2 * dist(gen)));
		}
		std::cout << out_value << "\n";
		return std::max(10.0f, std::abs(out_value));
	}

	float in_path(const std::vector<glm::vec3>& path, const std::vector<float>& widths, const glm::vec3& in_point)
	{
		float prev_width = widths[0];
		for (int i = 1; i < path.size(); ++i)
		{
			auto path_vec = path[i] - path[i - 1];
			auto normal = glm::cross(path_vec, glm::vec3(0, 0, 1));
			float avg_width = (prev_width + widths[i]) / 2.0;
			glm::vec3 avg_loc = (path[i - 1] + path[i]) / 2.0;

			float val1 = std::max(widths[i] - glm::length(path[i] - in_point), -1.0f);
			float val2 = std::max(avg_width - glm::length(avg_loc - in_point), -1.0f);
			float avg_val = (val1 + val2) / 2.0;

			if (avg_val > 0)
			{
				return avg_val;
			}

			prev_width = widths[i];
		}
		return -1;
	}

	virtual void work_loop() override
	{
		for (int i = state.i_state; i < state.num_triangles; ++i)
		{
			Triangle temp_tri;
			int vi = i*9;
			temp_tri.v[0] = state.rot*glm::vec3(state.triangle_floats->at(vi + 0), state.triangle_floats->at(vi + 1), state.triangle_floats->at(vi + 2));
			temp_tri.v[1] = state.rot*glm::vec3(state.triangle_floats->at(vi + 3), state.triangle_floats->at(vi + 4), state.triangle_floats->at(vi + 5));
			temp_tri.v[2] = state.rot*glm::vec3(state.triangle_floats->at(vi + 6), state.triangle_floats->at(vi + 7), state.triangle_floats->at(vi + 8));
			float l1 = glm::length(temp_tri.v[0] - temp_tri.v[1]);
			float l2 = glm::length(temp_tri.v[1] - temp_tri.v[2]);
			float l3 = glm::length(temp_tri.v[2] - temp_tri.v[0]);
			const float max_side = std::max(0.2f, std::min(2.0f, std::max(l1, std::max(l2, l3))));
			
			state.tri_octree->data[i] = temp_tri;
			octree::insert_triangle(state.tri_octree->nodes, 0, state.current_node_array_size, i, state.bmin, state.max_dim, temp_tri, 0.5);
			if (is_time_up())
			{
				state.i_state = i+1;
				return;
			}
		}
		state.tri_octree->size = state.max_dim;
		state.tri_octree->loc = state.bmin;
		i_am_done();
	}

	virtual void on_work_done() override
	{
		auto* mesh_component = state.level_entity.cmp<CompStaticMesh>();
		auto& proc_level = *state.level_entity.cmp<CompProceduralLevel>();
		mesh_component->tri_octree = state.tri_octree;
		state.level_bgfx_mesh->set_solid_color(glm::vec4(0.5, 0.5, 0.7, 1));

		proc_level.sibling<CompBounds>()->is_loaded = false;
		proc_level.sibling<CompBounds>()->bounds = state.level_bgfx_mesh->_bmax - state.level_bgfx_mesh->_bmin;

		int average_span = 6;
		std::map<int, glm::vec3> y_indexed_path_bottom_edges;
		std::map<int, glm::vec3> y_indexed_path_top_edges;
		std::map<int, glm::vec3> y_indexed_path_top_normals;
		std::map<int, glm::vec3> y_indexed_path_bottom_normals;
		for (int yi = 0; yi < state.y_res; ++yi)
		{
			std::vector<glm::vec3> current_bottom_edges;
			std::vector<glm::vec3> current_top_edges;
			bool bottom_edge_done = false;
			bool top_edge_done = false;
			float y_loc = yi * 1.0 / state.vertex_density_per_y + state.min_y;
			for (int xi = 1; xi < state.x_res; ++xi)
			{
				float x_loc = xi * 1.0 / state.vertex_density_per_x + state.min_x;
				if ((!state.in_path_matrix[xi][yi] && state.in_path_matrix[xi - 1][yi]))
				{
				    float inside_x_loc = (xi-1) * 1.0 / state.vertex_density_per_x + state.min_x;
					current_top_edges.push_back(glm::vec3(inside_x_loc, y_loc, 0));
				}
				if (state.in_path_matrix[xi][yi] && !state.in_path_matrix[xi - 1][yi])
				{
					current_bottom_edges.push_back(glm::vec3(x_loc, y_loc, 0));
				}
			}
			if (current_bottom_edges.size() != current_top_edges.size())
			{
				throw "Non matching edges";
			}
			if (current_bottom_edges.size())
			{
				int max_dist_i = 0;
				float max_dist = glm::length(current_bottom_edges[0] - current_top_edges[0]);
				for (int i = 1; i < current_bottom_edges.size(); ++i)
				{
					float try_dist = glm::length(current_bottom_edges[i] - current_top_edges[i]);
					if (try_dist > max_dist)
					{
						max_dist = try_dist;
						max_dist_i = i;
					}
				}
				y_indexed_path_bottom_edges[yi] = current_bottom_edges[max_dist_i] - state.level_bgfx_mesh->_bmin - glm::vec3(state.extra_width, state.extra_width, 0);
				y_indexed_path_top_edges[yi] = current_top_edges[max_dist_i] - state.level_bgfx_mesh->_bmin - glm::vec3(state.extra_width, state.extra_width, 0);
			}
		}
		for (auto& [yi, bottom_edge] : y_indexed_path_bottom_edges)
		{
			std::optional<glm::vec3> start_point;
			std::optional<glm::vec3> top_start_point;
			std::optional<glm::vec3> dir;
			std::optional<glm::vec3> top_dir;
			for (int i = -average_span / 2; i < average_span / 2; ++i)
			{
				if (y_indexed_path_bottom_edges.find(yi + i) != y_indexed_path_bottom_edges.end())
				{
					if (!start_point)
					{
						start_point = y_indexed_path_bottom_edges[yi + i];
						top_start_point = y_indexed_path_top_edges[yi + i];
						dir = glm::vec3(0);
						top_dir = glm::vec3(0);
					}
					else
					{
						dir.value() += y_indexed_path_bottom_edges[yi + i] - start_point.value();
						top_dir.value() += y_indexed_path_top_edges[yi + i] - top_start_point.value();
						start_point = y_indexed_path_bottom_edges[yi + i];
						top_start_point = y_indexed_path_top_edges[yi + i];
					}
				}
			}
			if (dir)
			{
				y_indexed_path_bottom_normals[yi] = glm::normalize(glm::cross(glm::normalize(dir.value()), glm::vec3(0, 0, 1)));
				y_indexed_path_top_normals[yi] = glm::normalize(glm::cross(glm::normalize(top_dir.value()), glm::vec3(0, 0, 1)));
			}
		}


		for (int i = 0; i < state.path.size(); ++i)
		{
			proc_level.path.push_back(state.path[i] - state.level_bgfx_mesh->_bmin);
			proc_level.widths.push_back(state.widths[i]);
		}

		proc_level.spawn_point = proc_level.path[3];// -mesh_component->mesh.get_mesh()->_bmin + glm::vec3(0, 10, 0);

		proc_level.floor_level = state.min_height;
		proc_level.cliff_level = state.max_height;
		proc_level.y_indexed_path_bottom_edges = y_indexed_path_bottom_edges;
		proc_level.y_indexed_path_top_edges = y_indexed_path_top_edges;
		proc_level.y_indexed_path_bottom_normals = y_indexed_path_bottom_normals;
		proc_level.y_indexed_path_top_normals = y_indexed_path_top_normals;


	}

	/*
	inline TriangleOctree<SerialOctree<Triangle>>* vector_to_octree(const std::vector<float>& in_vec, const glm::vec3& bmin, const glm::vec3& bmax, const glm::mat3& rot)
	{
		auto out_comp = new TriangleOctree<SerialOctree<Triangle>>;
		const int num_triangles = in_vec.size() / 3 / 3;
		out_comp->data = new Triangle[num_triangles];
		const int num_possible_nodes = 10000000;
		out_comp->nodes = new SerialNode<uint32_t>[num_possible_nodes];

		const glm::vec3 bounds = bmax - bmin;
		const float max_dim = std::max(bounds.x, std::max(bounds.y, bounds.z));
		int current_node_array_size = 1;
		for (int i = 0; i < num_triangles; ++i)
		{
			Triangle temp_tri;
			int vi = i*9;
			temp_tri.v[0] = rot*glm::vec3(in_vec[vi + 0], in_vec[vi + 1], in_vec[vi + 2]);
			temp_tri.v[1] = rot*glm::vec3(in_vec[vi + 3], in_vec[vi + 4], in_vec[vi + 5]);
			temp_tri.v[2] = rot*glm::vec3(in_vec[vi + 6], in_vec[vi + 7], in_vec[vi + 8]);
			float l1 = glm::length(temp_tri.v[0] - temp_tri.v[1]);
			float l2 = glm::length(temp_tri.v[1] - temp_tri.v[2]);
			float l3 = glm::length(temp_tri.v[2] - temp_tri.v[0]);
			const float max_side = std::max(0.2f, std::min(2.0f, std::max(l1, std::max(l2, l3))));
			
			out_comp->data[i] = temp_tri;
			insert_triangle(out_comp->nodes, 0, current_node_array_size, i, bmin, max_dim, temp_tri, 0.5);
		}
		out_comp->size = max_dim;
		out_comp->loc = bmin;
		return out_comp;
	}
	*/
};
