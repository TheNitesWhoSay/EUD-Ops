#ifndef ADDRESSINPUT_H
#define ADDRESSINPUT_H
#include "WindowsUI/WindowsUI.h"
#include "PlayerDropdown.h"

class AddressInput : public ClassWindow
{
    public:

        GroupBoxControl operation;

        TextControl addressText;
        EditControl addressInput;

        TextControl byteLengthText;
        DropdownControl byteLengthDropdown;

        TextControl eudOperationText;
        DropdownControl eudOperationDropdown;

        TextControl playerText;
        PlayerDropdown playerDropdown;

        TextControl unitText;
        DropdownControl unitDropdown;

        TextControl constantText;
        EditControl constantInput;

        TextControl memoryText;
        EditControl memoryInput;

        ButtonControl buttonCalculate;

        CheckBoxControl checkDestroySource;
        CheckBoxControl checkUseAddressesForMemory;

        AddressInput();
        bool CreateThis(HWND hParent, int x, int y, int width, int height);

        
    protected:

        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom);
        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom);
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom);
        void AddressChanged();
        void ByteLengthChanged();
        void OperationChanged();
        void LimitByteLengthTo(int maxByteLength);
        void Calculate();
        

        bool CreateSubWindows();
        void HideAllInputs();
        void HideDeathInput();
        void HideConstantInput();
        void HideMemoryInput();
        void ShowDeathInput();
        void ShowConstantInput();
        void ShowMemoryInput();

    private:

        s32 lastSelectedBitLength;

};

#endif