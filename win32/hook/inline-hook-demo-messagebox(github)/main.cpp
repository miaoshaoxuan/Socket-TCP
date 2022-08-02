#include <iostream>
#include <windows.h>
#define REPLACESIZE 5		//最少需要5字节替换，jmp 0x412345
using namespace std;

//最好全局变量 因为无需使用推栈  不担心栈平衡
DWORD dwParaX;
DWORD dwParaY;
char szBuffer[20] = { 0 };

//全局变量
DWORD g_RetAddr;
DWORD g_HookFlag;
LPVOID lpCodeBuffer;
DWORD g_JmpAddr;
DWORD g_FunAddr;
DWORD g_HookProc;
DWORD g_CodeSize;
DWORD g_Protect;

//Hook函数 裸函数
extern "C" _declspec(naked) void HookProc()
{
	//保存寄存器的值
	__asm
	{
		pushad
		pushfd
	}

	//获取数据
	__asm
	{
		mov eax, DWORD PTR SS : [esp + 0x28]
		mov dwParaX, eax
		mov eax, DWORD PTR SS : [esp + 0x2C]
		mov dwParaY, eax
	}
	//数字转字符
	// sprintf_s(szBuffer, "参数X: %d,参数Y: %d", dwParaX, dwParaY);
	MessageBoxA(NULL, szBuffer, "Hook", MB_OK);

	//恢复寄存器
	__asm
	{
		popfd
		popad
	}

	//执行被覆盖的硬编码  被覆盖的函数开始的硬编码  这里调用函数用的jmp作了一个中转，没有到函数里面
	//__asm
	//{
	//	push  ebp
	//	mov   ebp, esp
	//	sub   esp, 0C0h
	//}

	//跳回去
	__asm
	{
		jmp g_RetAddr
	}
}

//原来的函数
DWORD Plus(DWORD x, DWORD y)
{
	return x + y;
}

//安装Inline Hook
void SetInlineHook(DWORD FunAddr, DWORD HookProc, DWORD CodeSize)
{
	//参数校验
	if (FunAddr == 0 || HookProc == 0)
	{
		cout << "地址为0" << endl;
		return;
	}

	//判断要替换的硬编码长度是否合适
	if (CodeSize < REPLACESIZE)
	{
		cout << "CodeSize < 5" << endl;
		return;
	}

	//将要Hook的内存修改为可写
	DWORD i_OldProtect;
	//第一种：VirtualProtectEx(GetCurrentProcess(), (LPVOID)FunAddr, CodeSize, PAGE_EXECUTE_READWRITE, &OldProtect);
	//自己进程不需要VirtualProtectEx
	VirtualProtect((LPVOID)FunAddr, CodeSize, PAGE_EXECUTE_READWRITE, &i_OldProtect);

	//创建内存空间，存储替换前的硬编码
	lpCodeBuffer = malloc(CodeSize);
	if (lpCodeBuffer == NULL)
	{
		cout << "创建内存空间错误" << endl;
		return;
	}
	memcpy(lpCodeBuffer, (LPVOID)FunAddr, CodeSize);

	//得到要跳转的值  E9后面的地址 =  要跳转的地址 -  E9的地址 - 5
	g_JmpAddr = HookProc - FunAddr - 5;

	//将要Hook的内存全部初始化成NOP
	memset((LPVOID)FunAddr, 0x90, CodeSize);

	//修改要Hook的硬编码
	*(PBYTE)FunAddr = 0xE9;
	*(PDWORD)((PBYTE)FunAddr + 1) = g_JmpAddr;

	//修改hook状态
	g_HookFlag = 1;
	g_RetAddr = FunAddr + CodeSize;
	g_FunAddr = FunAddr;
	g_HookProc = HookProc;
	g_CodeSize = CodeSize;
	g_Protect = i_OldProtect;

	//还原属性
	VirtualProtect((LPVOID)FunAddr, CodeSize, i_OldProtect, &i_OldProtect);

}

void UnInlineHook()
{
	//判断是否需要卸载
	if (!g_HookFlag)
	{
		cout << "没安装，不需要卸载" << endl;
		return;
	}
	//内存修改为可写
	DWORD u_OldProtect;
	VirtualProtect((LPVOID)g_FunAddr, g_CodeSize, PAGE_EXECUTE_READWRITE, &u_OldProtect);

	//还原
	memcpy((LPVOID)g_FunAddr, lpCodeBuffer, g_CodeSize);
	VirtualProtect((LPVOID)g_FunAddr, g_CodeSize, g_Protect, &g_Protect);

	//修改hook状态
	g_HookFlag = 0;
	g_RetAddr = 0;
	g_FunAddr = 0;
	g_HookProc = 0;
}

//主函数
int main()
{
    Plus(1, 3);
	//安装Inline Hook
	SetInlineHook((DWORD)Plus, (DWORD)HookProc, 5);

	//执行函数
	system("pause");
	Plus(1, 3);

	//卸载Inline Hook
	UnInlineHook();

	//执行函数
	system("pause");
	Plus(1, 3);
}
