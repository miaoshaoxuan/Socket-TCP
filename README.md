# C++ 编程小Demo 

## 前言

Socket 编程 - Demo

用于记录学习cpp一点一滴，更好的帮助自己成长



## Socket-TCP

1. SocketClient，SocketServer： 简单的服务与客户端通讯
2. SocketClient，SocketServer-Thread：针对多个客户端与服务通讯简单Demo
3. SocketClient，SocketServer-Select： 客户端和多路复用形式服务端进行通讯简单demo

## Socket-UDP

1. Client，Server： UPD简单服务端通讯
2. UDP-P2P：UPD端到端形式的Demo

## WinPcapDemo

发送找不到packet.dll和wpcap.dll的情况请安装**WinPcap_4_1_3.exe**

网卡抓包形式小Demo

1. examples::simple()  遍历网卡
2. examples::haveRemote(); 实现远程抓包IP，TCP，UDP，ARP形式包，并解析二进制数据
3. examples::netSpeed();   计算网络速率