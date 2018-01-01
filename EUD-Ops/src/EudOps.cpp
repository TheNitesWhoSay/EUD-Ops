#include "EUDOps.h"
#include <thread>
#include <chrono>

enum MainWindow {
    IDR_MAIN_STATUS = ID_FIRST
};

void EudOps::OnLoadTest()
{

}

EudOps::EudOps()
{

}

int EudOps::Run(LPSTR lpCmdLine, int nCmdShow)
{
    if (!CreateThis())
        return 1;

    InitCommonControls();
    ShowWindow(getHandle(), nCmdShow);
    UpdateWindow();
    ParseCmdLine(lpCmdLine);
    this->OnLoadTest();

    MSG msg = {};
    bool keepRunning = true;
    do
    {
        while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
        {
            if (msg.message == WM_QUIT)
                keepRunning = false;
            else
            {
                bool isDlgKey = DlgKeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                if (::IsDialogMessage(currDialog, &msg) == FALSE)
                {
                    ::TranslateMessage(&msg);
                    if (!isDlgKey)
                        KeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                    ::DispatchMessage(&msg);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid consuming a core

    } while (keepRunning);

    return msg.wParam;
}

bool EudOps::CreateThis()
{
    if (!ClassWindow::WindowClassIsRegistered("wcEudOps"))
    {
        DWORD classStyle = 0;
        HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 32, 32, 0);
        HICON hIconSmall = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
        LPCTSTR menu = MAKEINTRESOURCE(IDR_MAIN_MENU);
        LPCTSTR wcName = "wcEudOps";
        if (!ClassWindow::RegisterWindowClass(classStyle, hIcon, hCursor, hBackground, menu, wcName, hIconSmall, false))
        {
            DestroyIcon(hIcon);
            DestroyCursor(hCursor);
            DestroyIcon(hIconSmall);
        }
    }

    std::string sWindowName(std::string("EUD Ops ") + GetFullVersionString());
    static char cWindowName[200] = {};
    strncpy(cWindowName, sWindowName.c_str(), sWindowName.size());

    DWORD exStyle = 0;
    LPCSTR windowName = cWindowName;
    DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    int windowX = CW_USEDEFAULT,
        windowY = CW_USEDEFAULT,
        windowWidth = CW_USEDEFAULT,
        windowHeight = CW_USEDEFAULT;
    HMENU id = NULL;

    return ClassWindow::CreateClassWindow(exStyle, windowName, style, windowX, windowY, windowWidth, windowHeight, NULL, id) &&
        EudOps::CreateSubWindows();
}

LRESULT EudOps::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
        // File
    case ID_FILE_NEW1: break;
    case ID_FILE_OPEN1: break;
    case ID_FILE_CLOSE1: break;
    case ID_FILE_SAVE1: break;
    case ID_FILE_SAVEAS1: break;
    case ID_FILE_EXIT: PostQuitMessage(0); break;

        // Edit
    case ID_EDIT_UNDO1: break;
    case ID_EDIT_REDO1: break;
    case ID_EDIT_CUT1: break;
    case ID_EDIT_COPY1: break;
    case ID_EDIT_PASTE1: break;
    case ID_EDIT_SELECTALL: break;
    case ID_EDIT_DELETE: break;
    case ID_EDIT_PROPERTIES: break;

        // Editor

        // Tools

        // Windows

        // Help
    case ID_HELP_STARCRAFT_WIKI: OpenWebPage("http://www.staredit.net/starcraft/Main_Page"); break;
    case ID_HELP_SUPPORT_FORUM: OpenWebPage("http://www.staredit.net/forums/"); break;
    case ID_HELP_CHKDRAFTGITHUB: OpenWebPage("https://github.com/jjf28/EUD-Ops/"); break;

    default:
        switch (HIWORD(wParam))
        {
        case CBN_SETFOCUS: editFocused = true; break;
        case CBN_KILLFOCUS: editFocused = false; break;
        case CBN_EDITCHANGE: ComboEditChanged((HWND)lParam, LOWORD(wParam)); SetFocus(getHandle()); break;
        case CBN_SELCHANGE: ComboSelChanged((HWND)lParam, LOWORD(wParam)); SetFocus(getHandle()); break;
        default: return ClassWindow::Command(hWnd, wParam, lParam); break;
        }
        break;
    }
    return 0;
}

LRESULT EudOps::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_MENUCHAR:
        return MAKELPARAM(0, MNC_CLOSE);
        break;

    case WM_SIZE:
    case WM_SIZING:
        SizeSubWindows();
        break;

    case WM_SHOWWINDOW:
    {
        LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
        if (wParam == TRUE)
            ShowWindow(currDialog, SW_SHOW);

        return result;
    }
    break;

    case WM_SYSCOMMAND:
        if (wParam == SC_RESTORE)
        {
            LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            RestoreDialogs();
            return result;
        }
        else if (wParam == SC_MINIMIZE)
            MinimizeDialogs();
        return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
        break;

    case WM_CLOSE:
        return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
        break;
    }
    return 0;
}

bool EudOps::CreateSubWindows()
{
    HWND hWnd = getHandle();
    if (hWnd != NULL)
    {
        int statusWidths[] = { 130, 205, 350, 450, 600, -1 };

        return mainMenu.FindThis(hWnd) &&
            statusBar.CreateThis(sizeof(statusWidths) / sizeof(int), statusWidths, 0,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, hWnd, (HMENU)IDR_MAIN_STATUS) &&
            mainPlot.CreateThis(hWnd);
    }
    else
        return false;
}

void EudOps::ParseCmdLine(LPSTR lpCmdLine)
{

}

bool EudOps::EditFocused()
{
    return editFocused;
}

void EudOps::SetCurrDialog(HWND hDialog)
{
    if (currDialog != hDialog)
        currDialog = hDialog;
}

void EudOps::SetEditFocused(bool editFocused)
{
    this->editFocused = editFocused;
}

bool EudOps::DlgKeyListener(HWND hWnd, UINT &msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_TAB:
            msg = WM_NULL; // Dirty fix to prevent tabs from being focused
            return true;
            break;
        case VK_RETURN:
            break;
        case VK_DELETE:
            break;
        case 'Z': case 'Y': case 'X': case 'C': case 'V':
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                KeyListener(hWnd, msg, wParam, lParam);
                return true;
            }
            break;
        }
    }
    break;
    case WM_KEYUP:
        return true;
        break;
    }
    return false;
}

void EudOps::KeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_SPACE: return; break;
        case VK_DELETE: return; break;
        case VK_ESCAPE: return; break;
        case VK_RETURN: return; break;
        }

        if (GetKeyState(VK_CONTROL) & 0x8000) // Control is down
        {
            if (GetKeyState(VK_SHIFT) & 0x8000) // Shift is down
            {
                switch (wParam)
                {
                case 'S': return; break;
                }
            }
            else // Only control
            {
                switch (wParam)
                {
                case 'A': case 'C': case 'X': case 'V':
                {
                    switch (wParam)
                    {
                    case 'A': return; break;
                    case 'C': return; break;
                    case 'X': return; break;
                    case 'V': return; break;
                    }
                }
                break;
                case 'D': return; break;
                case 'E': FindLeaks(); return; break;
                case 'F': return; break;
                case 'L': return; break;
                case 'N': return; break;
                case 'O': return; break;
                case 'R': return; break;
                case 'S': return; break;
                case 'T': return; break;
                case 'U': return; break;
                case 'Y': return; break;
                case 'Z': return; break;
                case VK_OEM_PLUS: return; break;
                case VK_OEM_MINUS: return; break;
                case VK_F4: return; break;
                case VK_F6: return; break;
                }
            }
        }
    }
    break;

    case WM_SYSKEYDOWN: // Sent insted of WM_KEYUP when alt is pressed
    {
        switch (wParam)
        {
        case 'U': return; break;
        case 'F': return; break;
        case 'G': return; break;
        case 'L': return; break;
        case 'E': return; break;
        }
    }
    break;

    case WM_KEYUP:
    {
        switch (wParam)
        {
        case VK_SPACE:
            return; break;
        }
    }
    break;
    }
}

void EudOps::MinimizeDialogs()
{

}

void EudOps::RestoreDialogs()
{

}

void EudOps::SizeSubWindows()
{
    RECT rcMain, rcStatus;

    statusBar.AutoSize();

    // Get the size of the client area, toolbar, status bar, and left bar
    GetClientRect(getHandle(), &rcMain);
    GetWindowRect(statusBar.getHandle(), &rcStatus);

    int xBorder = GetSystemMetrics(SM_CXSIZEFRAME) - 1,
        yBorder = GetSystemMetrics(SM_CYSIZEFRAME) - 1;

    // Fit plot to the area between the menu and statusbar
    SetWindowPos(mainPlot.getHandle(), NULL, 0, rcMain.top,
        rcMain.right - rcMain.left,
        rcMain.bottom - rcMain.top - rcMain.top - (rcStatus.bottom - rcStatus.top),
        SWP_NOZORDER | SWP_NOACTIVATE);

    RedrawWindow(statusBar.getHandle(), NULL, NULL, RDW_INVALIDATE);
    RedrawThis();
}

void EudOps::OpenWebPage(const char* address)
{
    ShellExecute(NULL, "open", address, NULL, NULL, SW_SHOWNORMAL);
}

void EudOps::ComboEditChanged(HWND hCombo, u16 comboId)
{
    
}

void EudOps::ComboSelChanged(HWND hCombo, u16 comboId)
{
    
}
