#include "EudOpsTrigGen.h"
#include <stdarg.h>
#include <cstdarg>
#include <cmath>

bool EudOpsTrigGen::GenerateNoArg(std::string &output, GenerationData genData, EudOpDef def, EudAddress eudAddress)
{
    output = "Generate case for " + def.eudOpName + " not found!";
    return false;
}

bool EudOpsTrigGen::GenerateWithConstant(std::string &output, GenerationData genData, EudOpDef def, EudAddress eudAddress, u32 constant)
{
    EudOpsTrigGen eudOpsGen = EudOpsTrigGen(genData, eudAddress);
    bool caseNotFound = false;
    bool success = false;
    switch (def.eudOp)
    {
    case EudOp::SetToConstant:
        success = eudOpsGen.setToConstant(constant);
        break;
    case EudOp::CheckEqual:
        success = eudOpsGen.checkEqual(constant);
        break;
    case EudOp::CheckAtLeast:
        success = eudOpsGen.checkAtLeast(constant);
        break;
    case EudOp::CheckAtMost:
        success = eudOpsGen.checkAtMost(constant);
        break;
    case EudOp::CheckGreaterThan:
        success = eudOpsGen.checkGreaterThan(constant);
        break;
    case EudOp::CheckLessThan:
        success = eudOpsGen.checkLessThan(constant);
        break;
    default:
        caseNotFound = true;
        break;
    }

    if (caseNotFound)
    {
        output = "Generate case for " + def.eudOpName + " not found!";
        return false;
    }
    else if ( success )
    {
        eudOpsGen.end();
        eudOpsGen.gen = TextTrigGenerator();
        std::string trigString("");
        return eudOpsGen.gen.GenerateTextTrigs(eudOpsGen.dummyMap, output);
    }
    return success;
}

bool EudOpsTrigGen::GenerateWithDeathCounter(std::string &output, GenerationData genData, EudOpDef def, EudAddress eudAddress, DeathCounter deathCounter)
{
    EudOpsTrigGen eudOpsGen = EudOpsTrigGen(genData, eudAddress);
    bool caseNotFound = false;
    bool success = false;
    switch (def.eudOp)
    {
    case EudOp::SetToDeaths:
        success = eudOpsGen.setToDeaths(deathCounter);
        break;
    case EudOp::CopyToDeaths:
        success = eudOpsGen.copyToDeaths(deathCounter);
        break;
    default:
        caseNotFound = true;
        break;
    }

    if (caseNotFound)
    {
        output = "Generate case for " + def.eudOpName + " not found!";
        return false;
    }
    else if ( success )
    {
        eudOpsGen.end();
        eudOpsGen.gen = TextTrigGenerator();
        std::string trigString("");
        return eudOpsGen.gen.GenerateTextTrigs(eudOpsGen.dummyMap, output);
    }
    return success;
}

bool EudOpsTrigGen::setToConstant(u32 constant)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;
    
    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);
    if (bitsAfterAddress > 0)
    {
        u32 bitsBeforeRemainder = bit + bitLength;
        for (; bit < bitsBeforeRemainder; bit++)
            stripBit(slackSpace, bit, false);
        u32 valueToAdd = constant << bitsAfterAddress;
        if (valueToAdd != 0)
        {
            trigger(owners);
            always();
            setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, valueToAdd);
        }
    }
    else // bitsAfterAddress == 0
    {
        trigger(owners);
        always();
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::SetTo, constant);
    }
    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::setToDeaths(DeathCounter srcValue)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);
    u32 bitsBeforeRemainder = bit + bitLength;
    for (; bit < bitLength; bit++)
        stripBit(slackSpace, bit, false);
    for (bit = 0; bit < bitLength; bit++)
    {
        trigger(owners);
        u32 unshiftedValue = pow(2, bitLength-bit-1);
        u32 shiftedValue = unshiftedValue << bitsAfterAddress;
        deaths(srcValue.playerId, srcValue.unitId, NumericComparison::AtLeast, unshiftedValue);
        setDeaths(srcValue.playerId, srcValue.unitId, NumericModifier::Subtract, unshiftedValue);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, shiftedValue);
    }
    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::copyToDeaths(DeathCounter destValue)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);
    u32 bitsBeforeRemainder = bit + bitLength;
    for (; bit < bitLength; bit++)
        stripBit(slackSpace, bit, false);
    for (bit = 0; bit < bitLength; bit++)
    {
        trigger(owners);
        u32 unshiftedValue = pow(2, bitLength-bit-1);
        u32 shiftedValue = unshiftedValue << bitsAfterAddress;
        deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtLeast, shiftedValue);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Subtract, shiftedValue);
        setDeaths(destValue.playerId, destValue.unitId, NumericModifier::Add, shiftedValue);
    }
    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::checkEqual(u32 constant)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);

    u32 valueMin = constant << bitsAfterAddress;
    u32 valueMax = ((constant+1) << bitsAfterAddress)-1;

    trigger(owners);
    deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtLeast, valueMin);
    if ( valueMin != valueMax )
        deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtMost, valueMax);

    setSwitch(0, SwitchModifier::Set);
    
    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::checkAtLeast(u32 constant)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);

    u32 valueMin = constant << bitsAfterAddress;
    trigger(owners);
    deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtLeast, valueMin);
    setSwitch(0, SwitchModifier::Set);

    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::checkAtMost(u32 constant)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);

    u32 valueMax = ((constant+1) << bitsAfterAddress)-1;
    trigger(owners);
    deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtMost, valueMax);
    setSwitch(0, SwitchModifier::Set);

    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::checkGreaterThan(u32 constant)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);

    u32 valueMin = (constant+1) << bitsAfterAddress;
    trigger(owners);
    deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtLeast, valueMin);
    setSwitch(0, SwitchModifier::Set);

    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

bool EudOpsTrigGen::checkLessThan(u32 constant)
{
    u32 address = targetAddress.address;
    u32 bitLength = targetAddress.bitLength;
    u32 bitsBeforeAddress = 8 * (address % 4 == 0 ? 0 : 3-address % 4);
    u32 bitsAfterAddress = 32 - bitsBeforeAddress - bitLength;
    DeathCounter slackSpace = genData.getSlackSpace();
    u32 bit = 0;

    for (; bit < bitsBeforeAddress; bit++)
        stripBit(slackSpace, bit, true);

    u32 valueMax = (constant << bitsAfterAddress)-1;
    trigger(owners);
    deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtMost, valueMax);
    setSwitch(0, SwitchModifier::Set);

    while (!restoreActions.empty())
    {
        RestoreAction action = restoreActions.top();
        restoreActions.pop();
        trigger(owners);
        deaths(slackSpace.playerId, slackSpace.unitId, NumericComparison::AtLeast, action.modification);
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Subtract, action.modification);
        setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Add, action.modification);
    }
    return true;
}

void EudOpsTrigGen::stripBit(DeathCounter slackSpace, u32 bit, bool restore)
{
    s64 change = pow(2, (31 - bit));
    trigger(owners);
    deaths(targetAddress.playerId, targetAddress.unitId, NumericComparison::AtLeast, change);
    setDeaths(targetAddress.playerId, targetAddress.unitId, NumericModifier::Subtract, change);
    if (restore)
    {
        setDeaths(slackSpace.playerId, slackSpace.unitId, NumericModifier::Add, change);
        restoreActions.push(RestoreAction(slackSpace, change));
    }
}

void EudOpsTrigGen::trigger(u8* players) {
    if ( triggerCount > 0 )
        dummyMap->addTrigger(currTrig);

    triggerCount++;
    didComment = false;
    currTrig = Trigger();
    memcpy(currTrig.players, players, 28);
    /*out << "Trigger(" << players << "){" << endl
        << "Conditions:" << endl;*/
}

void EudOpsTrigGen::end() {
    if ( triggerCount > 0 )
        dummyMap->addTrigger(currTrig);
}

// Accumulate
// Always
bool EudOpsTrigGen::always() {
    Condition condition = Condition(ConditionId::Always);
    return currTrig.addCondition(condition);
    //out << "	Always();" << endl;
}
// Bring
// Command
// Command the Least
// Command the Least At
// Command the Most
// Commands the Most At
// Countdown Timer
// Deaths
bool EudOpsTrigGen::deaths(u32 playerId, u32 unitId, NumericComparison numericComparison, u32 amount) {
    Condition condition(ConditionId::Deaths);
    condition.players = playerId;
    condition.unitID = unitId;
    condition.comparison = (u8)numericComparison;
    condition.amount = amount;
    return currTrig.addCondition(condition);
    //out << "	Deaths(\"" << player << "\", \"" << unit << "\", " << mod << ", " << amount << ");" << endl;
}
// Elapsed Time
// Highest Score
// Kill
// Least Kills
// Least Resources
// Lowest Score
// Memory
bool EudOpsTrigGen::memory(u32 address, NumericComparison numericComparison, u32 value) {
    Condition condition(ConditionId::Deaths);
    condition.players = address;
    condition.unitID = 0;
    condition.comparison = (u8)numericComparison;
    condition.amount = value;
    return currTrig.addCondition(condition);
    //out << "	Memory(" << address << ", " << mod << ", " << value << ");" << endl;
}
// Most Kills
// Most Resources
// Never
// Opponents
// Score
// Switch
bool EudOpsTrigGen::switchState(u32 switchNum, SwitchState state) {
    Condition condition(ConditionId::Switch);
    condition.typeIndex = switchNum;
    condition.comparison = (u8)state;
    return currTrig.addCondition(condition);
    //out << "	Switch(\"" << switchTitle << "\", " << state << ");" << endl;
}


// Center View
// Comment
bool EudOpsTrigGen::comment(const std::string &text) {
    bool success = false;
    if (!noComments)
    {
        ChkdString str(emptyComments ? "" : text);
        Action action(ActionId::Comment);
        u32 stringNum = 0;
        if ( dummyMap->addString<u32>(ChkdString(text), stringNum, false) )
        {
            action.stringNum = stringNum;
            success = currTrig.addAction(action);
        }
        //out << "	Comment(\"\");" << endl;
        //out << "	Comment(\"" << text << "\");" << endl;
    }
    didComment = true;
    return success;
}
// Create Unit
// Create Unit with Properties
// Defeat
// Display Text Message
// Draw
// Give Units to Player
// Kill Unit
// Kill Unit At Location
// Leaderboard (Control At Location)
// Leaderboard (Control)
// Leaderboard (Greed)
// Leaderboard (Kills)
// Leaderboard (Points)
// Leaderboard (Resources)
// Leaderboard Computer Players(State)
// Leaderboard Goal (Control At Location)
// Leaderboard Goal (Control)
// Leaderboard Goal (Kills)
// Leaderboard Goal (Points)
// Leaderboard Goal (Resources)
// Minimap Ping
// Modify Unit Energy
// Modify Unit Hanger Count
// Modify Unit Hit Points
// Modify Unit Resource Amount
// Modify Unit Shield Points
// Move Location
// Move Unit
// Mute Unit Speech
// Order
// Pause Game
// Pause Timer
// Play WAV
// Preserve Trigger
bool EudOpsTrigGen::preserveTrigger() {
    Action action = Action(ActionId::PreserveTrigger);
    return currTrig.addAction(action);
    //out << "	Preserve Trigger();" << endl;
}
// Remove Unit
// Remove Unit At Location
// Run AI Script
// Run AI Script At Location
// Set Alliance Status
// Set Countdown Timer
// Set Deaths
bool EudOpsTrigGen::setDeaths(u32 playerId, u32 unitId, NumericModifier numericModifier, u32 value) {
    Action action = Action(ActionId::SetDeaths);
    action.group = playerId;
    action.type = unitId;
    action.type2 = (u8)numericModifier;
    action.number = value;
    return currTrig.addAction(action);
}
// Set Doodad State
// Set Invincibility
// Set Mission Objectives
// Set Next Scenario
// Set Resources
// Set Score
// Set Switch
bool EudOpsTrigGen::setSwitch(u32 switchNum, SwitchModifier switchModifier) {
    Action action = Action(ActionId::SetSwitch);
    action.number = switchNum;
    action.type2 = (u8)switchModifier;
    return currTrig.addAction(action);
    //out << "	Set Switch(\"" << Switch << "\", " << state << ");" << endl;
}
// Talking Portrait
// Transmission
// Unmute Unit Speech
// Unpause Game
// Unpause Timer
// Victory
// Wait
