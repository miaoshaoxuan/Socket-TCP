#include "pch.h"
#include "Inline_hook.h"
#include <iostream>
using namespace std;

#define PATCH_LENGTH 5
DWORD dwHookAddress = 0;
DWORD dwRetAddress = 0;
char szNewText[] = "InlineHook!";


void MessageBoxProc(HWND hWnd, LPSTR lpText, LPSTR lpCaption, UINT uType) {
    // ���������ôд�����ԣ�������
    char szBuffer[256];
    memset(szBuffer, 0, sizeof(szBuffer));
    sprintf_s(szBuffer, "%u %s %s %u", hWnd, lpText, lpCaption, uType);\
    OutputDebugStringA(szBuffer);
    OutputDebugString(TEXT("���Բ���"));
}

__declspec(naked) void NewMessageBox() {
    __asm {
        // 1������Ĵ���
        pushad
        pushfd
        // 2���޸����ݣ�esp + 8
 /*       LEA EAX, DWORD PTR DS : [szNewText]
        MOV DWORD PTR SS : [esp + 0x24 + 8] , EAX*/
        // �Ż�֮��
        PUSH DWORD PTR SS:[ESP+0x28]
        PUSH DWORD PTR SS:[ESP+0x30]
        PUSH DWORD PTR SS:[ESP+0x38]
        PUSH DWORD PTR SS:[ESP+0x40]
        call MessageBoxProc
        add esp,0x10
        // 3���ָ��Ĵ���ֵ
        popfd
        popad
        // 4��ִ�и��ǵĴ���
        MOV EDI, EDI
        PUSH EBP
        MOV EBP, ESP
        ADD EBP, 0x4
        ADD ESP, 0x4
        // ִ�з���
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

    // ��ʼ��byJmpCode
    memset(&byJmpCode[1], 0x90, PATCH_LENGTH - 1);
    // �洢��ת��ַ
    *(DWORD*)&byJmpCode[1] = (DWORD)NewMessageBox - (DWORD)dwHookAddress - 5;
    // ���ݱ����ǵ�Code
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

    // ��װ����ж��Hook
    HookMessageBox(TRUE);
}
