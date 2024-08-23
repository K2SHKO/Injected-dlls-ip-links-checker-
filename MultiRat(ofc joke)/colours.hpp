#include <iostream>
#include <windows.h>
#include <sapi.h> 
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <ctime>
#include <tchar.h>
#include <regex>
#include <fstream>
#include <algorithm> 
#include <iomanip>   


#define CYAN (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define ORANGE (FOREGROUND_RED | FOREGROUND_GREEN)
#define DEFAULT (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)


WORD GetRandomColor() {
    static const std::vector<WORD> colors = {
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };
    return colors[rand() % colors.size()];
}

void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}