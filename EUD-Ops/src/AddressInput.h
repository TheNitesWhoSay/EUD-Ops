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

        ButtonControl buttonCalculate;

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
        void ShowDeathInput();
        void ShowConstantInput();

    private:

        s32 lastSelectedBitLength;

};

#endif