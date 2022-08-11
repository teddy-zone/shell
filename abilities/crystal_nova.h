#pragma once

#include "ability_proto.h"

struct CrystalNovaStatusProto : public EntityProto 
{
    CrystalNovaStatusProto(const std::vector<CompType>& extension_types={}):
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
        entity.cmp<CompStat>()->set_status_state(StatusState::KnockedUp, true);
        entity.cmp<CompStatus>()->duration = 1;
    }
};

struct CrystalNovaInstanceProto : public ActorProto
{

    CrystalNovaInstanceProto(const glm::vec3& in_pos, const std::vector<CompType>& extension_types={}):
        ActorProto(in_pos, extension_types)
    {
        std::vector<CompType> unit_components = {{
                    //uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompLifetime>),
                    //uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompRadiusApplication>),
                    uint32_t(type_id<CompDecal>),
                    uint32_t(type_id<CompTeam>),
                    uint32_t(type_id<CompLineObject>),
                    uint32_t(type_id<CompOnCast>),
                    uint32_t(type_id<CompAnimation>),
                    uint32_t(type_id<CompOnHit>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        auto monkey_mesh = std::make_shared<bgfx::Mesh>();
        monkey_mesh->load_obj("crystal_nova.obj" );
        monkey_mesh->set_solid_color(glm::vec3(0.0,1,0.0));
        //entity.cmp<CompPosition>()->scale = glm::vec3(1, 1, 1);

        //entity.cmp<CompPosition>()->pos = pos;
        entity.cmp<CompLifetime>()->lifetime = 2.0;

        //entity.cmp<CompAnimation>()->start_time = 0;
        //entity.cmp<CompAnimation>()->end_time = 0.5;
        //entity.cmp<CompAnimation>()->start_scale = glm::vec3(0.2);
        //entity.cmp<CompAnimation>()->end_scale = glm::vec3(10.0);

        //entity.cmp<CompRadiusApplication>()->radius = 5;
        
        entity.cmp<CompRadiusApplication>()->tick_time = 100;
        entity.cmp<CompRadiusApplication>()->damage = {entity, DamageType::Magical, 80, false};

        entity.cmp<CompDecal>()->decal.location = glm::vec4(pos, 1);
        entity.cmp<CompDecal>()->decal.color = glm::vec4(0.0,0.0,1.0,1.0);
        entity.cmp<CompDecal>()->decal.t = 0;
        //entity.cmp<CompDecal>()->decal.radius = 5;
        entity.cmp<CompDecal>()->decal.type = 1;
        entity.set_name("crystal_nova");

        entity.cmp<CompAnimation>()->scale_enabled = true;
        entity.cmp<CompAnimation>()->start_scale = glm::vec3(1);
        entity.cmp<CompAnimation>()->end_scale = glm::vec3(1,1,0.1);
        entity.cmp<CompAnimation>()->start_time = 0.2;
        entity.cmp<CompAnimation>()->end_time = 2.0;

        entity.cmp<CompLineObject>()->mesh.absolute_position = false;

        auto line_mesh = std::make_shared<bgfx::Mesh>();
        line_mesh->set_solid_color(glm::vec3(0,0,0));
        auto* mesh = entity.cmp<CompLineObject>();
        mesh->mesh.strip = false;
        mesh->mesh.set_mesh(line_mesh);

        
        CrystalNovaStatusProto status_proto;
        auto status_entity = iface->add_entity_from_proto(&status_proto);
        entity.cmp<CompOnHit>()->on_hit_callbacks.push_back(
            [status_entity](SystemInterface* iface, EntityRef projectile, EntityRef hittee)
            {
                if (hittee.is_valid())
                {
                    if (auto* status_manager = hittee.cmp<CompStatusManager>())
                    {
                        auto* status_comp = status_entity.cmp<CompStatus>();
                        status_manager->apply_status(status_comp);
                    }
                }
            });

        // Generate line particles


    }
};

class CrystalNovaAbilityProto : public AbilityProto 
{
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> colors;
public:
    CrystalNovaAbilityProto(const std::vector<CompType>& extension_types={}):
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
		const int number_of_particles = 200;
		for (int i = 0; i < number_of_particles; ++i)
		{
            float inward_radius = std::abs(norm_dist(gen) * 0.08);
			const float element_radius = -inward_radius + 1.0;
			const float element_azimuth = dist(gen) * 2*3.1415926;
			const float x = element_radius * cos(element_azimuth);// +location.x;
			const float y = element_radius * sin(element_azimuth);// +location.y;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(-2);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back((norm_dist(gen)*0.5 + average_length)*inward_radius);
			normals.push_back(0);
			normals.push_back(0);
			normals.push_back(-1);
			normals.push_back(0);
			normals.push_back(0);
			normals.push_back(-1);
			colors.push_back(0.8);
			colors.push_back(0.8);
			colors.push_back(0.8);
			colors.push_back(1);
			colors.push_back(1.8);
			colors.push_back(1.8);
			colors.push_back(1.8);
			colors.push_back(1);
			std::cout << "X: " << x << "\n";
			std::cout << "Y: " << y << "\n";
		}
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {
        entity.cmp<CompAbility>()->cast_point = 0.5;
        entity.cmp<CompAbility>()->backswing = 0.5;
        entity.cmp<CompAbility>()->ground_targeted = true;
        entity.cmp<CompAbility>()->unit_targeted = false;
        entity.cmp<CompAbility>()->cooldown = 8;
        entity.cmp<CompAbility>()->cast_range = 100;
        entity.cmp<CompAbility>()->radius = 5;
        entity.cmp<CompAbility>()->target_decal_type = TargetDecalType::Circle;
        entity.cmp<CompAbility>()->max_level = 4;
        entity.cmp<CompAbility>()->ability_name = "Crystal Nova";
        auto crystal_nova_proto = std::make_shared<CrystalNovaInstanceProto>(glm::vec3(0,0,0));
        auto ab_inst = entity.cmp<CompAbilityInstance>();
        auto cn_proto = std::dynamic_pointer_cast<EntityProto>(crystal_nova_proto);
        ab_inst->proto = cn_proto;

        entity.cmp<CompOnCast>()->on_cast_callbacks.push_back([*this](SystemInterface* iface, EntityRef caster, std::optional<glm::vec3> ground_target, std::optional<EntityRef> unit_target, std::optional<EntityRef> instance_entity)
            {
                std::vector<float> scaled_vertices(vertices.size());
                float radius = instance_entity.value().cmp<CompRadiusApplication>()->radius;
                for (int i = 0; i < vertices.size(); i += 3)
                {
                    scaled_vertices[i] = vertices[i] * radius;
                    scaled_vertices[i + 1] = vertices[i + 1] * radius;
                    scaled_vertices[i + 2] = vertices[i + 2];
                }
                auto line_mesh = instance_entity.value().cmp<CompLineObject>()->mesh.get_mesh();
                line_mesh->set_vertices(scaled_vertices);
                line_mesh->set_normals(normals);
                line_mesh->_saved_vertices = vertices;
                line_mesh->set_vertex_colors(colors);
            });
    }
};
