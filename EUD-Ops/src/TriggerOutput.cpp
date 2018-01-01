#include "TriggerOutput.h"
#include <iostream>

bool TriggerOutput::CreateThis(HWND hParent, int x, int y, int width, int height)
{
    if (!ClassWindow::WindowClassIsRegistered("wcTriggerOutput"))
    {
        DWORD classStyle = 0;
        HICON hIcon = NULL;
        HICON hIconSmall = NULL;
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = NULL;
        LPCTSTR menu = NULL;
        LPCTSTR wcName = "wcTriggerOutput";
        if (!ClassWindow::RegisterWindowClass(classStyle, hIcon, hCursor, hBackground, menu, wcName, hIconSmall, false))
            return false;
    }

    if (ClassWindow::CreateClassWindow(0, "wcTriggerOutput", WS_CHILD, x, y, width, height, hParent, NULL) &&
        CreateSubWindows())
    {
        ClassWindow::ShowNormal();
        return true;
    }
    return false;
}

void TriggerOutput::SetOutput(const std::string &output)
{
    outputTriggers.SetText(output.c_str());
}

LRESULT TriggerOutput::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
    case WM_SIZING:
        SizeSubWindows();
        RedrawThis();
        break;
    default:
        return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
        break;
    }
    return 0;
}

bool TriggerOutput::CreateSubWindows()
{
    bool success = outputTriggers.CreateThis(ClassWindow::getHandle(), 0, 0, 50, 50, true, 0);
    outputTriggers.MaximizeTextLimit();
    return success;
}

void TriggerOutput::SizeSubWindows()
{
    RECT rcMain;

    GetClientRect(getHandle(), &rcMain);

    SetWindowPos(outputTriggers.getHandle(), NULL, 0, rcMain.top,
        rcMain.right - rcMain.left,
        rcMain.bottom - rcMain.top,
        SWP_NOZORDER | SWP_NOACTIVATE);
}
