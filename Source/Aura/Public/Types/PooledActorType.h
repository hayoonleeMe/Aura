#pragma once

#include "PooledActorType.generated.h"

/**
 * Object Pool에서 Spawn을 요청할 때 PooledActor를 식별할 수 있는 enum
 */
UENUM()
enum class EPooledActorType : uint8
{
    FireBolt
};
