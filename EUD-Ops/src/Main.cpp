#include "EudOps.h"
#include <Windows.h>
#include <fstream>

#ifdef SHOW_CLI
CLI cli; // Open a command line interface that works with cin and cout
#endif

EudOps eudOps; // The main instance of EudOps

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    std::ofstream outFile("test.txt", std::ofstream::out);
    u32 number = 0xFFEEDDCC;
    u8* numberPtr = (u8*)&number;
    std::cout << std::hex << (int)numberPtr[0] << ", " << (int)numberPtr[1] << ", " << (int)numberPtr[2] << ", " << (int)numberPtr[3] << std::endl;
    return eudOps.Run(lpCmdLine, nCmdShow); // Forward the run command line params and the show options
}
