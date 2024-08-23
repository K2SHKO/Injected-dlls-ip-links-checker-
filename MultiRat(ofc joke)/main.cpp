#include "includes.hpp"

int main() {
    //std::wcout << L"Welcome to Cipix's PC checker!" << std::endl;

    std::wstring welcomeText = L"hey";
    SpeakText(welcomeText);

    ListInjectedDLLs();

    while (true) {
        std::wcout << L"Choose an option:\n";
        std::wcout << L"1. Search DLL connections\n";
        std::wcout << L"2. Search for processes\n";
        std::wcout << L"Option: ";

        int choice;
        std::wcin >> choice;
        std::wcin.ignore(); 

        if (choice == 3) {
            break;
        }

        if (choice == 1) {
            std::wstring dllPath;
            std::wcout << L"Enter DLL path: ";
            std::getline(std::wcin, dllPath);
            if (!dllPath.empty()) {
                SearchDLLConnections(dllPath);
            }
        }
        else if (choice == 2) {
            std::wstring searchTerm;
            std::wcout << L"Enter process name to search for: ";
            std::getline(std::wcin, searchTerm);
            if (!searchTerm.empty()) {
                SearchProcesses(searchTerm);
            }
        }
        else {
            std::wcout << L"Invalid option. Please try again." << std::endl;
        }
    }

    std::wcout << L"Press any key to exit..." << std::endl;
    std::cin.get();
    return 0;
}
