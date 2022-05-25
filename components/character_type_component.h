#pragma once

#include "component.h"

struct CompCharacterType : public Component
{

    std::shared_ptr<EntityProto> type_proto;

};