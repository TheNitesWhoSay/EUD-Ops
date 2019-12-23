#include "GenerationData.h"

GenerationData::GenerationData(const std::vector<DeathCounter> &slackSpaceDcs, const std::vector<u32> &slackSpaceSwitchNums)
{
    for (DeathCounter dc : slackSpaceDcs)
        unusedSlackSpaceDcs.push_back(dc);
    for (u32 slackSpaceSwitchNum : slackSpaceSwitchNums)
        unusedSwitchNums.push_back(slackSpaceSwitchNum);
}

DeathCounter GenerationData::getSlackSpaceDc()
{
    DeathCounter slackSpaceDc = unusedSlackSpaceDcs.back();
    unusedSlackSpaceDcs.pop_back();
    usedSlackSpaceDcs.push_back(slackSpaceDc);
    return slackSpaceDc;
}

void GenerationData::releaseSlackSpaceDc(DeathCounter toRelease)
{
    usedSlackSpaceDcs.remove(toRelease);
    bool notDuplicate = false;
    for (DeathCounter dc : unusedSlackSpaceDcs)
    {
        if (dc.unitId == toRelease.unitId && dc.playerId == toRelease.playerId)
        {
            notDuplicate = true;
            break;
        }
    }
    if (notDuplicate)
        unusedSlackSpaceDcs.push_back(toRelease);
}

u32 GenerationData::getSlackSpaceSwitchNum()
{
    u32 slackSpaceSwitchNum = unusedSwitchNums.back();
    unusedSwitchNums.pop_back();
    usedSwitchNums.push_back(slackSpaceSwitchNum);
    return slackSpaceSwitchNum;
}

void GenerationData::releaseSlackSpaceSwitchNum(u32 toRelease)
{
    usedSwitchNums.remove(toRelease);
    unusedSwitchNums.push_back(toRelease);
}
