#include <iostream>
#include <WinSock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 初始化
    WORD wsVersion = MAKEWORD(2, 2);
    WSADATA wsaData = {0};
    WSAStartup(wsVersion, &wsaData);
    // 创建套接字
    SOCKET cSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(SOCKET_ERROR == cSocket){
        printf("socket create error!\n");
        return 0;
    }else {
        printf("socket create success!\n");
    }
    printf("Input: ");
    char sendBuf[100] = {0};
    scanf("%s", sendBuf);
    sockaddr_in sockAddressIn = {0};
    sockAddressIn.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddressIn.sin_port = htons(2118);
    sockAddressIn.sin_family = AF_INET;
    sendto(cSocket, sendBuf, 100 , 0, (sockaddr*)&sockAddressIn, sizeof(sockAddressIn));
    // 关闭套接字
    closesocket(cSocket);
    WSACleanup();
    return 0;
}
