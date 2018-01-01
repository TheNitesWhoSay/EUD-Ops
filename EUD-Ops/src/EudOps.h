#ifndef EUDOPS_H
#define EUDOPS_H
#include "WindowsUI/WindowsUI.h"
#include "CommonFiles/CommonFiles.h"
#include "AddressInput.h"
#include "MainPlot.h"

class EudOps : public ClassWindow
{
    public:
                        void OnLoadTest(); // Write testing code here
                        
    /* Main Windows */  WindowMenu mainMenu; // Main window's menu
                        StatusControl statusBar; // Main window's status bar
                        MainPlot mainPlot; // The primary window area with the majority of the functionality

    /* Constructors */  EudOps();

    /*   Startup    */  int Run(LPSTR lpCmdLine, int nCmdShow);

    /* Quick Access */  bool EditFocused();

    /*   UI Accel   */  void SetCurrDialog(HWND hDialog);
                        void SetEditFocused(bool editFocused);

    protected:

    /*    Setup     */  bool CreateThis();
                        void ParseCmdLine(LPSTR lpCmdLine);

    /*  Preprocess  */  bool DlgKeyListener(HWND hWnd, UINT &msg, WPARAM wParam, LPARAM lParam);
                        void KeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /*    Procs     */  LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
                        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:

    /*     Data     */  HWND currDialog;
                        bool editFocused;

    /* Priv Methods */  bool CreateSubWindows();
                        void MinimizeDialogs();
                        void RestoreDialogs();
                        void SizeSubWindows();
                        void OpenWebPage(const char* address);
                        void ComboEditChanged(HWND hCombo, u16 comboId);
                        void ComboSelChanged(HWND hCombo, u16 comboId);
};

#endif