#include "MainPlot.h"
#include "CommonFiles\CommonFiles.h"

bool MainPlot::CreateThis(HWND hParent)
{
    if (!ClassWindow::WindowClassIsRegistered("wcAddressInput"))
    {
        DWORD classStyle = 0;
        HICON hIcon = NULL;
        HICON hIconSmall = NULL;
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = (HBRUSH)(COLOR_WINDOW);
        LPCTSTR menu = NULL;
        LPCTSTR wcName = "wcAddressInput";
        if (!ClassWindow::RegisterWindowClass(classStyle, hIcon, hCursor, hBackground, menu, wcName, hIconSmall, false))
            return false;
    }

    if (ClassWindow::CreateClassWindow(0, "wcAddressInput", WS_CHILD | WS_BORDER, 0, 0, 50, 50, hParent, NULL) &&
        CreateSubWindows())
    {
        ReplaceChildFonts(defaultFont);
        ClassWindow::ShowNormal();
        return true;
    }
    return false;
}

bool MainPlot::CreateSubWindows()
{
    HWND hWnd = getHandle();
    if ( hWnd != NULL )
    {
        return addressInput.CreateThis(hWnd, 0, 0, 250, 250)
            && triggerOutput.CreateThis(hWnd, 0, 250, 250, 250);
    }
    return false;
}

void MainPlot::SizeSubWindows()
{
    RECT rcMain;

    // Get the size of the client area, toolbar, status bar, and left bar
    GetClientRect(getHandle(), &rcMain);

    SetWindowPos(addressInput.getHandle(), NULL, 0, rcMain.top,
        rcMain.right - rcMain.left,
        165,
        SWP_NOZORDER | SWP_NOACTIVATE);

    int remainingLength = rcMain.bottom - addressInput.Bottom();
    SetWindowPos(triggerOutput.getHandle(), NULL, 0, addressInput.Bottom(),
        rcMain.right - rcMain.left,
        remainingLength > 40 ? remainingLength : 40,
        SWP_NOZORDER | SWP_NOACTIVATE);

    RedrawThis();
}

LRESULT MainPlot::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
