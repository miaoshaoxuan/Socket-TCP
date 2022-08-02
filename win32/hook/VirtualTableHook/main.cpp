#include <iostream>
#include <Windows.h>
using namespace std;

class Base {
public:
    virtual void printf1(){
        cout << "Base printf1..." << endl;
    };
    virtual void printf(){
        cout << "Base printf..." << endl;
    };
};

void  MyPrintf(){
    cout << "you are Hooked" << endl;
};

int main() {
    Base *base = new Base();

    DWORD *pVtAddress = (DWORD *)*(DWORD *)base;
//    DWORD *oldPVtAddress = (DWORD *)*(DWORD *)base;
    DWORD dwOldProc = 0;
    VirtualProtect(pVtAddress, sizeof(DWORD), PAGE_READWRITE, &dwOldProc);
    DWORD oldFuncPtr = pVtAddress[1];
    pVtAddress[1] = (DWORD)MyPrintf;
    base->printf1();
    base->printf();
    // 还原hook
    pVtAddress[1] = (DWORD)oldFuncPtr;
    // 恢复内存页属性
//    *pVtAddress = *oldPVtAddress;
//    VirtualProtect(oldPVtAddress, sizeof(DWORD), dwOldProc, 0);
    cout << "----------------" << endl;
    base->printf1();
    base->printf();

//    cout << "----------------" << endl;
//    oldPVtAddress[1] = (DWORD)MyPrintf;
//    base->printf1();
//    base->printf();

    delete base;

    return 0;
}
