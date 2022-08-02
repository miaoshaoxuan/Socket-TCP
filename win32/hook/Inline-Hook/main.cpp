#include <iostream>
#include <Windows.h>
using namespace std;


void SetMessageBoxHook();
#define PATCH_LENGTH 6
DWORD dwHookAddress = 0;
DWORD dwRetAddress = 0;
char szNewText[] = "InlineHook!";

int add (int a,int b){
    return a + b;
}

/**
 * 原执行流程
 * 00B114AD      55            push ebp
   00B114AE      8BEC          mov ebp,esp
   00B114C3   > \8B45 08       mov eax,dword ptr ss:[ebp+0x8]
    00B114C6   .  0345 0C       add eax,dword ptr ss:[ebp+0xC]
    00B114C9   .  5D            pop ebp
    00B114CA   .  C3            retn
   ——————————————————————————————————————————————————————————
   修改后
00B114C0 > >^\EB DB         jmp XInline_H.00B1149D
00B114C2      90            nop
00B114C3   >  8B45 08       mov eax,dword ptr ss:[ebp+0x8]
00B114C6   .  0345 0C       add eax,dword ptr ss:[ebp+0xC]
00B114C9   .  5D            pop ebp
00B114CA   .  C3            retn

00B1149D      C74424 04 020>mov dword ptr ss:[esp+0x4],0x2
00B114A5      C74424 08 050>mov dword ptr ss:[esp+0x8],0x5
00B114AD      55            push ebp
00B114AE      8BEC          mov ebp,esp
00B114B0      EB 11         jmp XInline_H.00B114C3


需要硬编码 书写



 * @return
 */


// JMP 跳转Code = 带跳转地址 - 下一行地址
// 下一行地址 = 当前地址 + 5

void __declspec(naked) NewMessageBox(){
    __asm{
        // 1，保存寄存器
      pushad
      pushfd
      // 2，修改数据，esp + 8
      LEA EAX,DWORD PTR DS:[szNewText]
      MOV DWORD PTR SS:[esp + 0x24+8],EAX
      // 3，恢复寄存器值
      popfd
      popad
      // 4，执行覆盖的代码
      MOV EDI,EDI
      PUSH EBP
      JMP dwRetAddress
      retn
    };
}

BOOL HookMessageBox(BOOL bOpen){
    BOOL bRet = FALSE;
    BYTE byJmpCode[PATCH_LENGTH] = {0xE9};
    DWORD dwOldProtect = 0;

    static BYTE byOriginalCode[PATCH_LENGTH] = {0};
    static BOOL bHookFlag = FALSE;

    // 初始化byJmpCode
    memset(&byJmpCode[1], 0x90, PATCH_LENGTH - 1);
    // 存储跳转地址
    *(DWORD *)&byJmpCode[1] = (DWORD)NewMessageBox - (DWORD)dwHookAddress - 5;
    // 备份杯覆盖的Code
    memcpy(byOriginalCode, (LPVOID)NewMessageBox, PATCH_LENGTH);
    if(bOpen){
        if(!bHookFlag){
            VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            memcpy((LPVOID)dwHookAddress,byJmpCode, PATCH_LENGTH);
            VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, dwOldProtect, 0);
            bHookFlag = TRUE;
            bRet = TRUE;
        }else {
            if(bHookFlag){
                VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, PAGE_EXECUTE_READWRITE, &dwOldProtect);
                memcpy((LPVOID)dwHookAddress,byOriginalCode, PATCH_LENGTH);
                VirtualProtect((LPVOID)dwHookAddress, PATCH_LENGTH, dwOldProtect, 0);
                bHookFlag = FALSE;
                bRet = TRUE;
            }
        }
    }
    return bRet;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter){
    SetMessageBoxHook();
    return 0;
}

void SetMessageBoxHook() {
    dwHookAddress = (DWORD)GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");
    dwHookAddress = dwHookAddress + PATCH_LENGTH;
    // 安装或者卸载Hook
    HookMessageBox(TRUE);
}


int main() {
//    cout << add(1, 2) << endl;
        // 测试调用汇编
//    __asm{
//    nop
//    };

    return 0;
}
