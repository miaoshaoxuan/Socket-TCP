#include <iostream>
#include <WinSock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 0 初始化
    WORD wsVersion = MAKEWORD(2,2);
    WSADATA  wsaData = {0};
    WSAStartup(wsVersion, &wsaData);
    // 1 创建套接字
    SOCKET sSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(SOCKET_ERROR == sSocket){
        printf("socket create error!\n");
        return 0;
    } else {
        printf("socket create success!\n");
    }
    // 2 绑定套接字
    sockaddr_in socketAddressInfo = {0};
    socketAddressInfo.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    socketAddressInfo.sin_port = htons(2118);
    socketAddressInfo.sin_family = AF_INET;
    int bRes = bind(sSocket, (sockaddr*)&socketAddressInfo, sizeof(socketAddressInfo));
    if(SOCKET_ERROR == bRes){
        printf("bind error!\n");
        return 0;
    } else {
        printf("bind success!\n");
    }
    // 3 收发数据
    char recvBuf[100] = {0};
    while(true){
        recvfrom(sSocket, recvBuf, 100 ,0, NULL, NULL);
        if(strlen(recvBuf) != 0){
            printf("Recv Data: %s \n", recvBuf);
        }
        if(strcmp(recvBuf, "bey") == 0){
            break;
        }
        memset(recvBuf, 0, 100);
    }
    // 4 关闭套接字
    closesocket(sSocket);
    WSACleanup();
    return 0;
}
