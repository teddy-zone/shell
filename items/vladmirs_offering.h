#pragma once

#include "item_proto.h"
#include "on_cast_component.h"

struct VladmirAuraProto : public EntityProto 
{
    VladmirAuraProto(const std::vector<CompType>& extension_types={}):
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
        entity.cmp<CompStat>()->set_stat(Stat::Armor, 3);
        entity.cmp<CompStat>()->set_stat_mult(Stat::MaxHealth, 1.2);
    }
};

struct VladmirProto : public ItemProto
{
    VladmirProto(const std::vector<CompType>& extension_types={}):
        ItemProto(extension_types)
    {
        std::vector<CompType> unit_components = {{
                uint32_t(type_id<CompAura>),
            }};
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface)
    {
        VladmirAuraProto aura_proto;
        auto status_entity = iface->add_entity_from_proto(&aura_proto);
        entity.cmp<CompAura>()->aura_status = status_entity;
        entity.cmp<CompAura>()->radius = 20;
        entity.cmp<CompAura>()->apply_to_same_team = true;
        entity.cmp<CompAura>()->apply_to_other_team = false;
    }
};
