#ifndef MAINPLOT_H
#define MAINPLOT_H
#include "WindowsUI/WindowsUI.h"
#include "AddressInput.h"
#include "TriggerOutput.h"

class MainPlot : public ClassWindow
{
public:

    AddressInput addressInput;
    TriggerOutput triggerOutput;

    bool CreateThis(HWND hParent);


protected:

    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool CreateSubWindows();
    void SizeSubWindows();
};

#endif