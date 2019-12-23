#include "AddressInput.h"
#include <iostream>
#include "EudOps.h"
#include "EudOpsDef.h"
#include "EudOpsTrigGen.h"

extern EudOps eudOps; // The main instance of EudOps

enum EnumAddressInput {
    ID_ADDRESS_TEXT = ID_FIRST,
    ID_ADDRESS_INPUT,
    ID_BYTE_LENGTH_TEXT,
    ID_BYTE_LENGTH_DROPDOWN,
    ID_EUD_OPERATION_TEXT,
    ID_EUD_OPERATION_DROPDOWN,
    ID_PLAYER_TEXT,
    ID_PLAYER_DROPDOWN,
    ID_UNIT_TEXT,
    ID_UNIT_DROPDOWN,
    ID_CONSTANT_TEXT,
    ID_CONSTANT_INPUT,
    ID_MEMORY_TEXT,
    ID_MEMORY_INPUT,
    ID_BUTTON_CALCULATE,
    ID_CHECK_DESTROY_SOURCE,
    ID_CHECK_USE_MEMORY_CONDITION,
    ID_CHECK_USE_MEMORY_ACTION,
    ID_CHECK_USE_ADDRESSES_FOR_MEMORY
};

AddressInput::AddressInput() : lastSelectedBitLength(32)
{

}

bool AddressInput::CreateThis(HWND hParent, int x, int y, int width, int height)
{
    if (!ClassWindow::WindowClassIsRegistered("wcAddressInput"))
    {
        DWORD classStyle = 0;
        HICON hIcon = NULL;
        HICON hIconSmall = NULL;
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = NULL;
        LPCTSTR menu = NULL;
        LPCTSTR wcName = "wcAddressInput";
        if (!ClassWindow::RegisterWindowClass(classStyle, hIcon, hCursor, hBackground, menu, wcName, hIconSmall, false))
            return false;
    }

    if ( ClassWindow::CreateClassWindow(0, "wcAddressInput", WS_CHILD, x, y, width, height, hParent, NULL) &&
         CreateSubWindows() )
    {
        ClassWindow::ShowNormal();
        return true;
    }
    return false;
}

void AddressInput::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch (idFrom) {
    case EnumAddressInput::ID_BYTE_LENGTH_DROPDOWN:
        ByteLengthChanged();
        break;
    case EnumAddressInput::ID_EUD_OPERATION_DROPDOWN:
        OperationChanged();
        break;
    }
}

void AddressInput::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch (idFrom) {
    case EnumAddressInput::ID_BUTTON_CALCULATE:
        Calculate();
        break;
    }
}

void AddressInput::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    switch (idFrom) {
    case EnumAddressInput::ID_ADDRESS_INPUT:
        AddressChanged();
        break;
    }
}

void AddressInput::AddressChanged()
{
    u32 addressVal;
    if (addressInput.GetEditNum(addressVal))
    {
        int remainder = addressVal % 4;
        int maxByteLength = 4 - remainder;
        LimitByteLengthTo(maxByteLength);
    }
    else
        LimitByteLengthTo(4);
}

void AddressInput::ByteLengthChanged()
{
    u8 byteLength;
    if (byteLengthDropdown.GetEditNum(byteLength))
    {
        lastSelectedBitLength = byteLength * 8;
    }
}

void AddressInput::OperationChanged()
{
    EudOpDef def = EudOpDef::GetDef(eudOperationDropdown.GetWinText());
    HideAllInputs();
    switch (def.inputSet) {
    case InputSet::Constant:
        ShowConstantInput();
        break;
    case InputSet::DeathCounter:
        ShowDeathInput();
        break;
    case InputSet::Memory:
        ShowMemoryInput();
        break;
    }
}

void AddressInput::LimitByteLengthTo(int maxByteLength)
{
    const std::vector<std::string> byteLengths = { "4", "3", "2", "1" };
    byteLengthDropdown.SetRedraw(false);
    byteLengthDropdown.ClearItems();
    if (maxByteLength > 4 || maxByteLength < 1)
        maxByteLength = 4;

    int start = 4 - maxByteLength;
    int max = byteLengths.size();
    for (int i = start; i < max; i++)
        byteLengthDropdown.AddItem(byteLengths[i]);
    
    if (lastSelectedBitLength >= 0 && lastSelectedBitLength <= maxByteLength*8
        && lastSelectedBitLength % 8 == 0 )
    {
        int lastSelectedByte = lastSelectedBitLength / 8;
        int selectionIndex = maxByteLength - lastSelectedByte;
        byteLengthDropdown.SetSel(selectionIndex);
    }
    else
        byteLengthDropdown.SetSel(0);

    byteLengthDropdown.SetRedraw(true);
    byteLengthDropdown.RedrawThis();
}

void AddressInput::Calculate()
{
    u32 address = 0;
    if (!addressInput.GetEditNum(address))
    {
        Error("Invalid address entry!");
        return;
    }

    u32 byteLength = 0;
    if (!byteLengthDropdown.GetEditNum(byteLength))
    {
        Error("Internal error on byte length input!");
        return;
    }

    std::string eudOperationText = eudOperationDropdown.GetWinText();
    const EudOpDef &eudOpDef = EudOpDef::GetDef(eudOperationText);
    EudOp eudOp = eudOpDef.eudOp;
    if (eudOp == EudOp::Null)
    {
        Error("Internal error on eud operation input!");
        return;
    }

    bool destroySource = checkDestroySource.isChecked();
    bool useAddressesForMemory = checkUseAddressesForMemory.isChecked();

    const std::vector<DeathCounter> slackDcs = {
        DeathCounter(181, 8), DeathCounter(181, 7), DeathCounter(181, 6)
    };
    const std::vector<u32> slackSwitches = {
        255, 254, 253, 252
    };
    GenerationData genData = GenerationData(slackDcs, slackSwitches, useAddressesForMemory);
    u32 bitLength = byteLength * 8;
    InputSet inputSet = eudOpDef.inputSet;
    std::string result("");
    
    if (inputSet == InputSet::None)
    {
        EudOpsTrigGen::GenerateNoArg(result, genData, eudOpDef, EudAddress(address, bitLength), destroySource);
    }
    else if (inputSet == InputSet::Constant)
    {
        u32 constantValue;
        if (!constantInput.GetEditNum(constantValue))
        {
            Error("Invalid constant entry!");
            return;
        }
        EudOpsTrigGen::GenerateWithConstant(result, genData, eudOpDef, EudAddress(address, bitLength), constantValue, destroySource);
    }
    else if (inputSet == InputSet::DeathCounter)
    {
        int unitId = unitDropdown.GetSel();
        if (unitId < 0)
        {
            if (!unitDropdown.GetEditNum(unitId))
            {
                Error("Invalid unit input!");
                return;
            }
        }

        u32 player;
        if (!playerDropdown.GetPlayerNum(player))
        {
            Error("Internal error on player input!");
            return;
        }
        EudOpsTrigGen::GenerateWithDeathCounter(result, genData, eudOpDef, EudAddress(address, bitLength), DeathCounter(unitId, player), destroySource);
    }
    else if (inputSet == InputSet::Memory)
    {
        u32 memory;
        if (!memoryInput.GetEditNum<u32>(memory))
        {
            Error("Invalid constant entry!");
            return;
        }
        EudOpsTrigGen::GenerateWithMemory(result, genData, eudOpDef, EudAddress(address, bitLength), EudAddress(memory, 32), destroySource);
    }
    if (result.length() > 0)
        eudOps.mainPlot.triggerOutput.SetOutput(result.c_str());
    else
        eudOps.mainPlot.triggerOutput.SetOutput("Result was empty!");
}

void AddressInput::HideAllInputs()
{
    HideConstantInput();
    HideDeathInput();
    HideMemoryInput();
}

bool AddressInput::CreateSubWindows()
{
    const std::vector<std::string> eudOperationName = EudOpDef::Names();

    const std::vector<std::string> byteLengths = { "4", "3", "2", "1" };

    addressText.CreateThis(ClassWindow::getHandle(), 5, 5, 85, 20, "Address: ", EnumAddressInput::ID_ADDRESS_TEXT);
    addressInput.CreateThis(ClassWindow::getHandle(), addressText.Right() + 5, 5, 100, 20, false, EnumAddressInput::ID_ADDRESS_INPUT);
    byteLengthText.CreateThis(ClassWindow::getHandle(), 5, addressInput.Bottom() + 5, 85, 20, "Length (Bytes): ", EnumAddressInput::ID_BYTE_LENGTH_TEXT);
    byteLengthDropdown.CreateThis(ClassWindow::getHandle(), byteLengthText.Right() + 5, addressInput.Bottom() + 5, 100, 250, false, true, EnumAddressInput::ID_BYTE_LENGTH_DROPDOWN, byteLengths, defaultFont);
    eudOperationText.CreateThis(ClassWindow::getHandle(), 5, byteLengthDropdown.Bottom() + 5, 85, 20, "EUD Opeartion: ", EnumAddressInput::ID_EUD_OPERATION_TEXT);
    eudOperationDropdown.CreateThis(ClassWindow::getHandle(), eudOperationText.Right() + 5, byteLengthDropdown.Bottom() + 5, 100, 250, false, true, EnumAddressInput::ID_EUD_OPERATION_DROPDOWN, eudOperationName, defaultFont);
    LimitByteLengthTo(4);
    eudOperationDropdown.SetSel(0);

    // Death Input
    playerText.CreateThis(ClassWindow::getHandle(), 5, eudOperationDropdown.Bottom() + 5, 85, 20, "Death Player: ", EnumAddressInput::ID_PLAYER_TEXT);
    playerDropdown.CreateThis(ClassWindow::getHandle(), playerText.Right() + 5, eudOperationDropdown.Bottom() + 5, 100, 250, EnumAddressInput::ID_PLAYER_DROPDOWN, true);
    unitText.CreateThis(ClassWindow::getHandle(), 5, playerDropdown.Bottom() + 5, 85, 20, "Death Unit: ", EnumAddressInput::ID_UNIT_TEXT);
    unitDropdown.CreateThis(ClassWindow::getHandle(), unitText.Right() + 5, playerDropdown.Bottom() + 5, 100, 250, true, false, EnumAddressInput::ID_UNIT_DROPDOWN, NumUnitNames, DefaultUnitDisplayName, defaultFont);
    unitDropdown.SetSel(0);
    unitDropdown.ClearEditSel();

    // Constant Input
    constantText.CreateThis(ClassWindow::getHandle(), 5, eudOperationDropdown.Bottom() + 5, 85, 20, "Constant Value: ", EnumAddressInput::ID_CONSTANT_TEXT);
    constantInput.CreateThis(ClassWindow::getHandle(), constantText.Right() + 5, eudOperationDropdown.Bottom() + 5, 100, 20, false, EnumAddressInput::ID_CONSTANT_INPUT);
    constantInput.SetEditNum(0);

    // Memory Input
    memoryText.CreateThis(ClassWindow::getHandle(), 5, eudOperationDropdown.Bottom() + 5, 85, 20, "Memory Address: ", EnumAddressInput::ID_MEMORY_TEXT);
    memoryInput.CreateThis(ClassWindow::getHandle(), memoryText.Right() + 5, eudOperationDropdown.Bottom() + 5, 100, 20, false, EnumAddressInput::ID_MEMORY_INPUT);
    memoryInput.SetText("0x0");

    buttonCalculate.CreateThis(ClassWindow::getHandle(), 5, unitDropdown.Bottom() + 5, 150, 25, "Calculate!", EnumAddressInput::ID_BUTTON_CALCULATE);

    checkDestroySource.CreateThis(ClassWindow::getHandle(), addressInput.Right() + 5, addressInput.Top(), 150, 25, false, "Destroy Source", EnumAddressInput::ID_CHECK_DESTROY_SOURCE);
    checkUseAddressesForMemory.CreateThis(ClassWindow::getHandle(), checkDestroySource.Left(), checkDestroySource.Bottom() + 5, 150, 25, true, "Use Addresses For Memory", EnumAddressInput::ID_CHECK_USE_ADDRESSES_FOR_MEMORY);

    addressInput.FocusThis();
    addressInput.SetText("0x0");
    addressInput.SetSel(3, 3);
    HideDeathInput();
    HideMemoryInput();
    return true;
}

void AddressInput::HideDeathInput()
{
    playerText.Hide();
    playerDropdown.Hide();
    unitText.Hide();
    unitDropdown.Hide();
}

void AddressInput::HideConstantInput()
{
    constantText.Hide();
    constantInput.Hide();
}

void AddressInput::HideMemoryInput()
{
    memoryText.Hide();
    memoryInput.Hide();
}

void AddressInput::ShowDeathInput()
{
    playerText.Show();
    playerDropdown.Show();
    unitText.Show();
    unitDropdown.Show();
}

void AddressInput::ShowConstantInput()
{
    constantText.Show();
    constantInput.Show();
}

void AddressInput::ShowMemoryInput()
{
    memoryText.Show();
    memoryInput.Show();
}
