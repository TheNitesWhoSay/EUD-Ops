#ifndef EUDADDRESS_H
#define EUDADDRESS_H
#include "CommonFiles\CommonFiles.h"

class EudAddress
{
    public:
        const u32 address;
        const u32 bitLength;
        const u32 unitId;
        const u32 playerId;
        EudAddress(u32 address, u32 bitLength);

    private:
        EudAddress();
};

#endif