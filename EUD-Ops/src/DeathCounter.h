#ifndef DEATHCOUNTER_H
#define DEATHCOUNTER_H
#include "CommonFiles/CommonFiles.h"
#include <iostream>

class DeathCounter
{
    public:
        const u32 unitId;
        const u32 playerId; // Zero-based

        DeathCounter(u32 unitId, u32 playerId) : unitId(unitId), playerId(playerId) {}
        bool operator==(const DeathCounter &rhs) {
            return unitId == rhs.unitId && playerId == rhs.playerId;
        }
        //friend std::ostream& operator<<(std::ostream& os, const DeathCounter& dc);
        std::string toString() {
            return "Death(unitId:" + std::to_string(unitId) + ", playerId:" + std::to_string(playerId) + ")";
        }
};

//std::ostream& operator<<(std::ostream &os, const DeathCounter &dc)
//{
//    os << "Death(unitId:" << dc.unitId << ", playerId:" << dc.playerId << ")";
//}

#endif