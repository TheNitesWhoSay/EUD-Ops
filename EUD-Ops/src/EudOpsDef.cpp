#include "EudOpsDef.h"

const EudOpDef EudOpDef::nullOpDef = EudOpDef(EudOp::Null, "Null", InputSet::None);

const std::map<u32, EudOpDef> EudOpDef::eudOpsToDefs = {
    { (u32)EudOp::Null, nullOpDef },
    { (u32)EudOp::SetToConstant, EudOpDef(EudOp::SetToConstant, "Set to Constant", InputSet::Constant) },
    { (u32)EudOp::SetToDeaths, EudOpDef(EudOp::SetToDeaths, "Set to Deaths", InputSet::DeathCounter) },
    { (u32)EudOp::SetToMemory, EudOpDef(EudOp::SetToMemory, "Set to Memory", InputSet::Memory) },
    { (u32)EudOp::CopyToDeaths, EudOpDef(EudOp::CopyToDeaths, "Copy to Deaths", InputSet::DeathCounter) },
    { (u32)EudOp::CopyToMemory, EudOpDef(EudOp::CopyToMemory, "Copy to Memory", InputSet::Memory) },
    { (u32)EudOp::CheckEqual, EudOpDef(EudOp::CheckEqual, "Check Equal", InputSet::Constant) },
    { (u32)EudOp::CheckAtLeast, EudOpDef(EudOp::CheckAtLeast, "Check At Least", InputSet::Constant) },
    { (u32)EudOp::CheckAtMost, EudOpDef(EudOp::CheckAtMost, "Check At Most", InputSet::Constant) },
    { (u32)EudOp::CheckGreaterThan, EudOpDef(EudOp::CheckGreaterThan, "Check Greater Than", InputSet::Constant) },
    { (u32)EudOp::CheckLessThan, EudOpDef(EudOp::CheckLessThan, "Check Less Than", InputSet::Constant) },
};

static const std::map<u32, int> inputSetToNumArgs;

const std::map<u32, int> EudOpDef::inputSetToNumArgs = {
    { (u32)InputSet::None, 0 },
    { (u32)InputSet::Constant, 1 },
    { (u32)InputSet::DeathCounter, 2}
};

std::vector<std::string> EudOpDef::Names()
{
    std::vector<std::string> names;
    for (auto &entry : eudOpsToDefs)
    {
        if (entry.first != (u32)EudOp::Null)
            names.push_back(entry.second.eudOpName);
    }

    return names;
}

const EudOpDef & EudOpDef::GetDef(std::string &eudOpName)
{
    for (auto &entry : eudOpsToDefs)
    {
        if (entry.second.eudOpName.compare(eudOpName) == 0)
            return entry.second;
    }
    return EudOpDef::nullOpDef;
}

EudOpDef::EudOpDef(const EudOp eudOp, const std::string &eudOpName, InputSet inputSet)
    : eudOp(eudOp), eudOpName(eudOpName), inputSet(inputSet)
{

}

int EudOpDef::getNumArgs()
{
    return inputSetToNumArgs.find((u32)inputSet)->second;
}
