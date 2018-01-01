#include "EudOps.h"
#include <Windows.h>

#ifdef SHOW_CLI
CLI cli; // Open a command line interface that works with cin and cout
#endif

EudOps eudOps; // The main instance of EudOps

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return eudOps.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
}
