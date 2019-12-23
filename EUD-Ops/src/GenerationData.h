#ifndef GENERATIONDATA_H
#define GENERATIONDATA_H
#include "CommonFiles\CommonFiles.h"
#include "DeathCounter.h"
#include <list>

class GenerationData
{
    public:
        GenerationData(const std::vector<DeathCounter> &slackSpace, bool useAddressesForMemory = true);
        DeathCounter getSlackSpace();
        void releaseSlackSpace(DeathCounter toRelease);
        const bool useMemoryCondition;
        const bool useMemoryAction;
        const bool useAddressesForMemory;

    private:
        std::list<DeathCounter> unusedSlackSpace;
        std::list<DeathCounter> usedSlackSpace;
        

};

#endif