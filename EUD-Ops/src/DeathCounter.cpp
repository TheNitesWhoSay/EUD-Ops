#include "DeathCounter.h"

bool operator==(const DeathCounter & lhs, const DeathCounter & rhs)
{
    return lhs.unitId == rhs.unitId && lhs.playerId == rhs.playerId;
}
