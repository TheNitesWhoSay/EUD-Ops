#ifndef GENERATIONDATA_H
#define GENERATIONDATA_H
#include "CommonFiles\CommonFiles.h"
#include "DeathCounter.h"
#include <list>

class GenerationData
{
    public:
        GenerationData(const std::vector<DeathCounter> &slackSpace);
        DeathCounter getSlackSpace();
        void releaseSlackSpace(DeathCounter toRelease);

    private:
        std::list<DeathCounter> unusedSlackSpace;
        std::list<DeathCounter> usedSlackSpace;
};

#endif