#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

int main() {
    // __debugbreak();
    while(true){
        cout << "input:" << endl;
        char str[10];
        cin >> str;
        if(strcmp(str, "1") == 0){
            MessageBoxA(0,"my name is miaoshaoxuan\n","test test\n",0);
        }
    }
    return 0;
}
