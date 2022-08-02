#include "pch.h"
#include "Inline_hook.h"
#include <iostream>
using namespace std;

#define PATCH_LENGTH 5
DWORD dwHookAddress = 0;
DWORD dwRetAddress = 0;
char szNewText[] = "InlineHook!";


void MessageBoxProc(HWND hWnd, LPSTR lpText, LPSTR lpCaption, UINT uType) {
    // 这个函数怎么写都可以，无限制
    char szBuffer[256];
    memset(szBuffer, 0, sizeof(szBuffer));
    sprintf_s(szBuffer, "%u %s %s %u", hWnd, lpText, lpCaption, uType);\
    OutputDebugStringA(szBuffer);
    OutputDebugString(TEXT("测试测试"));
}

__declspec(naked) void NewMessageBox() {
    __asm {
        // 1，保存寄存器
        pushad
        pushfd
        // 2，修改数据，esp + 8
 /*       LEA EAX, DWORD PTR DS : [szNewText]
        MOV DWORD PTR SS : [esp + 0x24 + 8] , EAX*/
        // 优化之后
        PUSH DWORD PTR SS:[ESP+0x28]
        PUSH DWORD PTR SS:[ESP+0x30]
        PUSH DWORD PTR SS:[ESP+0x38]
        PUSH DWORD PTR SS:[ESP+0x40]
        call MessageBoxProc
        add esp,0x10
        // 3，恢复寄存器值
        popfd
        popad
        // 4，执行覆盖的代码
        MOV EDI, EDI
        PUSH EBP
        MOV EBP, ESP
        ADD EBP, 0x4
        ADD ESP, 0x4
        // 执行返回
        push dwRetAddress
        //add DWORD PTR ds:[esp], PATCH_LENGTH
        retn
    }
}

BOOL HookMessageBox(BOOL bOpen) {
    BOOL bRet = FALSE;
    BYTE byJmpCode[PATCH_LENGTH] = { 0xE8 };
    DWORD dwOldProtect = 0;

    static BYTE byOriginalCode[PATCH_LENGTH] = { 0 };
    static BOOL bHookFlag = FALSE;

    // 初始化byJmpCode
    memset(&byJmpCode[1], 0x90, PATCH_LENGTH - 1);
    // 存储跳转地址
    *(DWORD*)&byJmpCode[1] = (DWORD)NewMessageBox - (DWORD)dwHookAddress - 5;
    // 备份杯覆盖的Code
    memcpy(byOriginalCode, (LPVOID)NewMessageBox, PATCH_LENGTH);
    if (bOpen) {
        if (!bHookFlag) {
            cout << "entry hook !" << endl;
            VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            memcpy((LPVOID)dwHookAddress, byJmpCode, PATCH_LENGTH);
            VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, dwOldProtect, 0);
            bHookFlag = TRUE;
            bRet = TRUE;
        }
    }
    else {
        if (bHookFlag) {
            VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            memcpy((LPVOID)dwHookAddress, byOriginalCode, PATCH_LENGTH);
            VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, dwOldProtect, 0);
            bHookFlag = FALSE;
            bRet = TRUE;
        }
    }
    return bRet;
}


void Inline_hook::SetMessageBoxHook()
{
    dwHookAddress = (DWORD)GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");
    dwRetAddress = dwHookAddress + PATCH_LENGTH;

    // 安装或者卸载Hook
    HookMessageBox(TRUE);
}
