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


void SpeakText(const std::wstring& text) {
    ISpVoice* pVoice = NULL;
    if (FAILED(::CoInitialize(NULL))) {
        std::wcerr << L"Failed to initialize COM library." << std::endl;
        return;
    }

    if (FAILED(::CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice))) {
        std::wcerr << L"Failed to create SAPI voice instance." << std::endl;
        ::CoUninitialize();
        return;
    }

    pVoice->Speak(text.c_str(), SPF_IS_XML, NULL);
    pVoice->Release();
    ::CoUninitialize();
}