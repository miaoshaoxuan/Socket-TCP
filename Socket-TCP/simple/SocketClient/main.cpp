#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WORD wsVersion = MAKEWORD(2, 2);
    WSADATA  wsadata = {0};

    WSAStartup(wsVersion, &wsadata);

    // 创建套接字
    SOCKET cSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(SOCKET_ERROR == cSocket){
        printf("socket create error!\n");
    } else {
        printf("socket create success!\n");
    }
    // 绑定套接字
    sockaddr_in sockAddressInfo = {0};
    sockAddressInfo.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddressInfo.sin_port = htons(9996);
    sockAddressInfo.sin_family = AF_INET;
    int bRes = bind(cSocket, (sockaddr*)&sockAddressInfo, sizeof(sockAddressInfo));

    if(SOCKET_ERROR == bRes){
        printf("bind error!\n");
        return 0;
    } else {
        printf("bind success!\n");
    }

    // 连接服务器
    sockaddr_in serverSockAddressInfo = {0};
    serverSockAddressInfo.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverSockAddressInfo.sin_port = htons(9999);
    serverSockAddressInfo.sin_family = AF_INET;
    int cRes = connect(cSocket, (sockaddr*)&serverSockAddressInfo, sizeof(serverSockAddressInfo) );
    if(SOCKET_ERROR == cRes){
        printf("server link error!\n");
        return 0;
    } else {
        printf("server link success!\n");
    }

    // 收发数据
    printf("Input: ");
    char sendData[100];
    while (true){
        scanf("%s", sendData);
        send(cSocket, sendData, strlen(sendData) + 1, 0);
        char buf[100] = {0};
        recv(cSocket, buf, 100, 0);
        printf("Recv data:%s \n", buf);
        if(strcmp(sendData, "close") == 0){
            break;
        }
        memset(sendData, 0, 100);
    }

    // 断开连接
    shutdown(cSocket, SD_SEND);
    // 关闭套接字
    closesocket(cSocket);

    WSACleanup();
    return 0;
}
