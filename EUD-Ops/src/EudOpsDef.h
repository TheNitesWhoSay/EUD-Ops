#ifndef EUDOPSDEF_H
#define EUDOPSDEF_H
#include "CommonFiles\CommonFiles.h"

class EudOpDef;

enum class InputSet : u32 {
    None,
    Constant,
    DeathCounter,
    Memory
};

enum class EudOp : u32 {
    Null,
    SetToConstant,
    AddConstant,
    SubtractConstant,
    SetToDeaths,
    SetToMemory,
    CopyToDeaths,
    CopyToMemory,
    AddDeaths,
    SubtractDeaths,
    CheckEqual,
    CheckAtLeast,
    CheckAtMost,
    CheckGreaterThan,
    CheckLessThan
};

class EudOpDef
{
    public:
        static std::vector<std::string> Names();
        static const EudOpDef & GetDef(std::string &eudOpName);
        EudOpDef(EudOp eudOp, const std::string &eudOpName, InputSet inputSet);
        int getNumArgs();

        const EudOp eudOp;
        const std::string eudOpName;
        const InputSet inputSet;

    private:
        EudOpDef(); // Disallow ctor
        static const std::map<u32, EudOpDef> eudOpsToDefs;
        static const std::map<u32, int> inputSetToNumArgs;
        static const EudOpDef nullOpDef;
};

#endif