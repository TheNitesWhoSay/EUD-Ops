#include "GenerationData.h"

GenerationData::GenerationData(const std::vector<DeathCounter> &slackSpace)
{
    for (DeathCounter dc : slackSpace)
        unusedSlackSpace.push_back(dc);
}

DeathCounter GenerationData::getSlackSpace()
{
    DeathCounter slackSpace = unusedSlackSpace.back();
    unusedSlackSpace.pop_back();
    usedSlackSpace.push_back(slackSpace);
    return slackSpace;
}

void GenerationData::releaseSlackSpace(DeathCounter toRelease)
{
    usedSlackSpace.remove(toRelease);
    bool notDuplicate = false;
    for (DeathCounter dc : unusedSlackSpace)
    {
        if (dc.unitId == toRelease.unitId && dc.playerId == toRelease.playerId)
        {
            notDuplicate = true;
            break;
        }
    }
    if (notDuplicate)
        unusedSlackSpace.push_back(toRelease);
}
