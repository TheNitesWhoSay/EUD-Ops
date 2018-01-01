#include "EudAddress.h"

constexpr u32 deathTableOffset = 0x0058A364;

EudAddress::EudAddress(u32 address, u32 bitLength) : address(address), bitLength(bitLength),
    unitId(0), playerId((address-deathTableOffset)/4)
{

}
