#ifndef GENERATIONDATA_H
#define GENERATIONDATA_H
#include "CommonFiles\CommonFiles.h"
#include "DeathCounter.h"
#include <stack>
#include <list>

class GenerationData
{
    public:
        GenerationData(const std::vector<DeathCounter> &slackSpaceDcs, const std::vector<u32> &slackSpaceSwitchNums);
        DeathCounter getSlackSpaceDc();
        void releaseSlackSpaceDc(DeathCounter toRelease);
        u32 getSlackSpaceSwitchNum();
        void releaseSlackSpaceSwitchNum(u32 toRelease);

    private:
        std::list<DeathCounter> unusedSlackSpaceDcs;
        std::list<DeathCounter> usedSlackSpaceDcs;
        std::list<u32> unusedSwitchNums;
        std::list<u32> usedSwitchNums;
};

#endif