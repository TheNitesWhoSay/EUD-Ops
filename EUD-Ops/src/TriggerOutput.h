#ifndef TRIGGEROUTPUT_H
#define TRIGGEROUTPUT_H
#include "WindowsUI/WindowsUI.h"

class TriggerOutput : public ClassWindow
{
    public:
        EditControl outputTriggers;

        bool CreateThis(HWND hParent, int x, int y, int width, int height);
        void SetOutput(const std::string &output);

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        bool CreateSubWindows();
        void SizeSubWindows();
};

#endif