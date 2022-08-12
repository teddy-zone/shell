#pragma once

#include "ability_proto.h"
#include "attachment_component.h"

struct BladefuryStatusProto : public EntityProto 
{
    BladefuryStatusProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompStat>),
                    uint32_t(type_id<CompStatus>),
                    uint32_t(type_id<CompHasOwner>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompStat>()->set_stat_mult(Stat::Movespeed, 2);
        entity.cmp<CompStatus>()->duration = 0.5;
    }
};

struct BladefuryInstanceProto : public EntityProto 
{

    BladefuryInstanceProto(const std::vector<CompType>& extension_types={}):
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompLifetime>),
                    uint32_t(type_id<CompHasOwner>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompRadiusApplication>),
                    uint32_t(type_id<CompLineObject>),
                    uint32_t(type_id<CompAnimation>),
            }};
        append_components(unit_components);

    }

    virtual void init(EntityRef entity, SystemInterface* iface) 
    {
        entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 90/10.0, false};
        entity.cmp<CompRadiusApplication>()->tick_time = 0.1;
        entity.cmp<CompDecal>()->decal.type = 3;
        entity.cmp<CompLifetime>()->lifetime = 5;

        entity.cmp<CompLineObject>()->mesh.absolute_position = false;

        auto line_mesh = std::make_shared<bgfx::Mesh>();
        line_mesh->set_solid_color(glm::vec3(0,0,0));
        auto* mesh = entity.cmp<CompLineObject>();
        mesh->mesh.strip = false;
        mesh->mesh.set_mesh(line_mesh);

        entity.cmp<CompAnimation>()->spin_enabled = true;
        entity.cmp<CompAnimation>()->spin_speed = 40.0;
        entity.cmp<CompAnimation>()->spin_axis = glm::vec3(0, 0, 1);
    }
};

class BladefuryAbilityProto : public AbilityProto 
{
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> colors;
public:
    BladefuryAbilityProto(const std::vector<CompType>& extension_types={}):
        AbilityProto(TargetDecalType::None, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    uint32_t(type_id<CompOnCast>),
                    uint32_t(type_id<CompAbilityInstance>),
            }};
        append_components(unit_components);

		const float average_length = 8.5;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dist;
		std::normal_distribution<float> norm_dist(0.0, 1.0);
		const int number_of_lines = 15;
        const int num_vertices_per_line = 8;
        const float angle_per_vertex = 3.14159 / 10;
		for (int i = 0; i < number_of_lines; ++i)
		{
            float inward_radius = std::abs(norm_dist(gen));
			const float element_radius = -inward_radius + 1.0;
			const float element_azimuth = dist(gen) * 2*3.1415926;
            for (int j = 0; j < num_vertices_per_line; ++j)
            {
				const float x = element_radius * cos(element_azimuth + angle_per_vertex*j);// +location.x;
				const float y = element_radius * sin(element_azimuth + angle_per_vertex*j);// +location.y;
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(-1.5);
                normals.push_back(0);
                normals.push_back(0);
                normals.push_back(-1);
                colors.push_back(0.8);
                colors.push_back(1.5);
                colors.push_back(0.8);
                colors.push_back(1);
            }
		}
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        BladefuryStatusProto status_proto;
        auto status_entity = iface->add_entity_from_proto(&status_proto);
        auto projectile_proto = std::make_shared<BladefuryInstanceProto>();
        entity.cmp<CompAbilityInstance>()->proto = projectile_proto;
        entity.cmp<CompAbility>()->cooldown = 10;
        entity.cmp<CompAbility>()->cast_range = 50;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->self_targeted = true;
        entity.cmp<CompAbility>()->ground_targeted = false;
        entity.cmp<CompAbility>()->ability_name = "Bladefury";
        entity.cmp<CompAbility>()->mana_cost = 80;
        entity.cmp<CompAbility>()->radius = 7;
        entity.cmp<CompAbility>()->max_level = 4;
        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back(
            [*this, status_entity](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
            {
                if (unit_target)
                {
                    if (auto* status_manager = unit_target->cmp<CompStatusManager>())
                    {
                        auto* status_comp = status_entity.cmp<CompStatus>();
                        status_manager->apply_status(status_comp);
                    }
                }
                if (instance_entity)
                {
                    std::vector<float> scaled_vertices(vertices.size());
                    float radius = instance_entity.value().cmp<CompRadiusApplication>()->radius;
                    for (int i = 0; i < vertices.size(); i += 3)
                    {
                        scaled_vertices[i] = vertices[i] * radius;
                        scaled_vertices[i + 1] = vertices[i + 1] * radius;
                        scaled_vertices[i + 2] = vertices[i + 2];
                    }
                    caster.cmp<CompAttachment>()->attached_entities.push_back(instance_entity.value());
					auto line_mesh = instance_entity.value().cmp<CompLineObject>()->mesh.get_mesh();
                    line_mesh->set_vertices(scaled_vertices);
                    line_mesh->set_normals(normals);
                    line_mesh->set_vertex_colors(colors);

                }
            }
        );
    }
};
