#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

DWORD WINAPI ThreadProc(LPVOID lpParameter){
    int ret = 1;
    SOCKET aSocket = (SOCKET)lpParameter;
    char buf[100] = {0};
    do{
        ret = recv(aSocket, buf, 100, 0);
        if(strcmp(buf, "close") == 0){
            break;
        }
        printf("Recv data:%s\n", buf);
        send(aSocket, buf, strlen(buf) + 1, 0);
        memset(buf, 0, 100);
    } while (ret != 0);

    // 断开连接
    shutdown(aSocket, SD_BOTH);
    closesocket(aSocket);
    return 0;
}

int main() {
    WORD wsVersion = MAKEWORD(2, 2);
    WSADATA  wsadata = {0};

    WSAStartup(wsVersion, &wsadata);

    // 创建套接字
    SOCKET sSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(SOCKET_ERROR == sSocket){
        printf("socket create error!\n");
    } else {
        printf("socket create success!\n");
    }
    // 绑定套接字
    sockaddr_in sockAddressInfo = {0};
    sockAddressInfo.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddressInfo.sin_port = htons(9999);
    sockAddressInfo.sin_family = AF_INET;
    int bRes = bind(sSocket, (sockaddr*)&sockAddressInfo, sizeof(sockAddressInfo));

    if(SOCKET_ERROR == bRes){
        printf("bind error!\n");
        return 0;
    } else {
        printf("bind success!\n");
    }
    // 监听套接字
    int lRes = listen(sSocket, 1);
    if(SOCKET_ERROR == lRes){
        printf("listen error!\n");
        return 0;
    } else {
        printf("listen success!\n");
    }
    while(true){
        // 等待连接
        sockaddr_in acceptSockAddressInfo = {0};
        int acceptSockAddressInfoLen = sizeof(acceptSockAddressInfo);
        SOCKET aSocket = accept(sSocket, (sockaddr*)&acceptSockAddressInfo, &acceptSockAddressInfoLen);
        if(INVALID_SOCKET != aSocket){
            HANDLE hThread = CreateThread(NULL, NULL, ThreadProc, (LPVOID)aSocket, 0, NULL);
            CloseHandle(hThread);
        }
    }


    // 关闭套接字
    closesocket(sSocket);

    WSACleanup();
    return 0;
}
