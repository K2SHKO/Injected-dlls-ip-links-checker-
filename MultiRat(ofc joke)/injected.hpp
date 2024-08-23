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



void ListInjectedDLLs() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create snapshot of processes." << std::endl;
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        std::cerr << "Failed to retrieve the first process." << std::endl;
        CloseHandle(hSnapshot);
        return;
    }

    srand(static_cast<unsigned int>(time(0)));

    do {
        HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
        if (hModuleSnapshot == INVALID_HANDLE_VALUE) {
            continue;
        }

        MODULEENTRY32 me32;
        me32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hModuleSnapshot, &me32)) {
            do {
                if (_tcsstr(me32.szExePath, _T(".dll")) != nullptr) {
                    WORD initialColor = GetRandomColor();
                    std::wcout << L"";
                    SetConsoleColor(initialColor);
                    std::wcout << pe32.szExeFile[0];
                    SetConsoleColor(ORANGE);
                    std::wcout << &pe32.szExeFile[1];
                    SetConsoleColor(DEFAULT);
                    std::wcout << L"\t ";
                    SetConsoleColor(CYAN);
                    std::wcout << me32.szExePath << std::endl;
                    SetConsoleColor(DEFAULT);
                }
            } while (Module32Next(hModuleSnapshot, &me32));
        }

        CloseHandle(hModuleSnapshot);
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

std::wstring ToLower(const std::wstring& str) {
    std::wstring result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

void ListInjectedDLLsForProcess(DWORD processID, std::wostream& out) {
    HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (hModuleSnapshot == INVALID_HANDLE_VALUE) {
        out << L"Failed to create snapshot of modules for process ID: " << processID << std::endl;
        return;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    WORD colorIndex = 0; // Index for color cycling
    static const std::vector<WORD> colors = {
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };

    if (Module32First(hModuleSnapshot, &me32)) {
        do {
            SetConsoleColor(colors[colorIndex % colors.size()]);
            out << L"    " << me32.szExePath << std::endl;
            colorIndex++;
        } while (Module32Next(hModuleSnapshot, &me32));
    }
    else {
        out << L"Failed to retrieve modules for process ID: " << processID << std::endl;
    }

    SetConsoleColor(DEFAULT);
    CloseHandle(hModuleSnapshot);
}


void SearchDLLConnections(const std::wstring& dllPath) {
    std::wifstream file(dllPath, std::ios::binary);
    if (!file) {
        std::wcerr << L"Could not open file: " << dllPath << std::endl;
        return;
    }

    std::wcout << L"Searching connections in: " << dllPath << std::endl;


    std::wstring buffer((std::istreambuf_iterator<wchar_t>(file)),
        std::istreambuf_iterator<wchar_t>());


    std::wregex urlRegex(LR"((https?://[^\s]+))");
    std::wregex ipRegex(LR"((\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b))");

    std::wstring lowerBuffer = ToLower(buffer);

    auto urls_begin = std::wsregex_iterator(lowerBuffer.begin(), lowerBuffer.end(), urlRegex);
    auto urls_end = std::wsregex_iterator();

    auto ips_begin = std::wsregex_iterator(lowerBuffer.begin(), lowerBuffer.end(), ipRegex);
    auto ips_end = std::wsregex_iterator();

    std::wcout << L"Connections found:" << std::endl;


    for (std::wsregex_iterator i = urls_begin; i != urls_end; ++i) {
        std::wsmatch match = *i;
        std::wstring match_str = match.str();
        std::wcout << L"---> URL: " << match_str << std::endl;
    }


    for (std::wsregex_iterator i = ips_begin; i != ips_end; ++i) {
        std::wsmatch match = *i;
        std::wstring match_str = match.str();
        std::wcout << L"---> IP: " << match_str << std::endl;
    }

    std::wcout << std::endl << std::endl << std::endl;
}


void SearchProcesses(const std::wstring& searchTerm) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create snapshot of processes." << std::endl;
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        std::cerr << "Failed to retrieve the first process." << std::endl;
        CloseHandle(hSnapshot);
        return;
    }

    std::wstring lowerSearchTerm = ToLower(searchTerm);

    std::wcout << L"Processes containing \"" << searchTerm << L"\" in their name:" << std::endl;

    do {
        std::wstring processName = pe32.szExeFile;
        std::wstring lowerProcessName = ToLower(processName);

        if (lowerProcessName.find(lowerSearchTerm) != std::wstring::npos) {
            WORD initialColor = GetRandomColor();
            SetConsoleColor(initialColor);
            std::wcout << std::setw(30) << std::left << processName << L" PID: " << pe32.th32ProcessID << std::endl;
            SetConsoleColor(DEFAULT);


            ListInjectedDLLsForProcess(pe32.th32ProcessID, std::wcout);
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}