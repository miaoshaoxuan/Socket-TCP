//
// Created by Miaoshaoxuan on 2022/6/30.
//

#ifndef WINPCAPDEMO_EXAMPLES_H
#define WINPCAPDEMO_EXAMPLES_H

#define HAVE_REMOTE
#include <WinSock2.h>
#include <pcap.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wpcap.lib")

class examples {
public:
    static int simple();
    static int haveRemote();
    static int netSpeed();
};


#endif //WINPCAPDEMO_EXAMPLES_H
