#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

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

    fd_set fdSocket; // 定义
    FD_ZERO(&fdSocket); // 初始化
    FD_SET(sSocket, &fdSocket); // 将当前服务器创建的socket放入集合中

    while(true){
        // 可读集合
        fd_set readfds = fdSocket;
        // 可写集合
        fd_set writefds = fdSocket;

        int iRes = select(0, &readfds, &writefds, NULL, NULL);
        if(iRes > 0){
            // 遍历可读集合，给每个socket发送Hello
            for (int i = 0; i < writefds.fd_count; ++i) {
                send(readfds.fd_array[i], "Hello", 6, 0);
            }
            // 遍历可写集合
            for (int i = 0; i < readfds.fd_count; ++i) {
                // 如何socket为当前服务器创建的socket则进入accpet等待消息
                if(readfds.fd_array[i] == sSocket){
                    sockaddr_in acceptSockAddressInfo = {0};
                    int acceptSockAddressInfoLen = sizeof(acceptSockAddressInfo);
                    SOCKET aSocket = accept(sSocket, (sockaddr*)&acceptSockAddressInfo, &acceptSockAddressInfoLen);
                    if(INVALID_SOCKET == aSocket){
                        continue;
                    }
                    FD_SET(aSocket, &fdSocket);
                    // inet_ntoa 获取IP
                    printf("Accept Client IP : %s \n", inet_ntoa(acceptSockAddressInfo.sin_addr));
                } else {
                    // 如果不是则进入接收消息
                    char buf[100] = {0};
                    int iRecv = recv(readfds.fd_array[i], buf, sizeof(buf), 0);
                    // 判断接收消息的返回值，大于0则表示接收成功
                    if(iRecv > 0){
                        printf("Recv: %s \n", buf);
                    } else {
                        // 否则关闭连接，关闭套接字
                        SOCKET tSocket = readfds.fd_array[i];
                        FD_CLR(tSocket, &fdSocket);
                        shutdown(tSocket, SD_BOTH);
                        closesocket(tSocket);
                    }
                }
            }
        } else{
            continue;
        }
    }


    closesocket(sSocket);

    WSACleanup();
    return 0;
}
