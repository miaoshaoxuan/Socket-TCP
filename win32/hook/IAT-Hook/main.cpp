#include <iostream>
#include <Windows.h>
#include <string>
using namespace std;

DWORD methodAddress;

BOOL SetIATHook(DWORD methodAddress, DWORD hookAddress){
    BOOL bFlag = FALSE;
    DWORD dwImageBase = 0;
    PDWORD pFuncAddress = NULL;
    PIMAGE_NT_HEADERS pNTHeader = NULL;
    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
    DWORD dwOldProtect = 0;
    // 得到模块基址
    dwImageBase = (DWORD)::GetModuleHandle(NULL);
    pNTHeader = (PIMAGE_NT_HEADERS)(dwImageBase + ((PIMAGE_DOS_HEADER)dwImageBase)->e_lfanew);
    pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress);
    while(pImportDescriptor->FirstThunk != 0 && !bFlag){
        pFuncAddress = PDWORD(dwImageBase + pImportDescriptor->FirstThunk);
        // 获取 dll 名称
        char* name = (char *)(dwImageBase + pImportDescriptor->Name);
        cout << name << endl;
        while (*pFuncAddress){
            if(methodAddress == *pFuncAddress){
                char * funcName = PIMAGE_IMPORT_BY_NAME(dwImageBase + ((PIMAGE_THUNK_DATA32)(dwImageBase + pImportDescriptor->OriginalFirstThunk))->u1.AddressOfData)->Name;
                cout << methodAddress << ":this dll method address:" << pFuncAddress << ":" << funcName << endl;
                VirtualProtect((LPVOID)(pFuncAddress), sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
                *pFuncAddress = hookAddress;
                bFlag = TRUE;
                break;
            }
            pFuncAddress++;
        }
        pImportDescriptor++;
    }
    return TRUE;
}

int
WINAPI
NewMessageBoxA(
        _In_opt_ HWND hWnd,
        _In_opt_ LPCSTR lpText,
        _In_opt_ LPCSTR lpCaption,
        _In_ UINT uType){
    typedef int (WINAPI* BOX)(HWND hwnd, LPCSTR lpText, LPCSTR lpCaption,  UINT uType);
    ((BOX)methodAddress)(0, "IAT Hook", "New", 0);
    cout << "you are hooked!!" << endl;
    return 0;
}

int main() {
   methodAddress = (DWORD)GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");

    MessageBoxA(0,"text","old",0);
    SetIATHook(methodAddress, (DWORD) NewMessageBoxA);
    MessageBoxA(0,"text","old",0);
    SetIATHook((DWORD) NewMessageBoxA, methodAddress);
    MessageBoxA(0,"text","old",0);

    return 0;
}
