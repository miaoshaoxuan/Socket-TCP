//
// Created by Miaoshaoxuan on 2022/6/30.
//

#include "examples.h"

void Mypcap_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char * pkt_data){
    // 5, 编写回调函数
    struct tm *ltime;
    char timestr[16] = {0};
    time_t local_tv_sec;

    // 将时间戳转换为可识别的格式
    local_tv_sec = pkt_header->ts.tv_sec;
    ltime = localtime(&local_tv_sec);
    strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);
    printf("time: %s, Caplen: %d, Len: %d \n", timestr, pkt_header->caplen, pkt_header->len);

    // 根据分析，我们发现数据包的六个字节为目的Mac地址，第七个字母到第12个字节则为源Mac地址，所以在这里输出
    u_char dstMac[6] = {0};
    for (int i = 0; i < 6; ++i) {
        dstMac[i] = *(pkt_data+i+0);
    }
    printf("Dst Mac: %02x:%02x:%02x:%02x:%02x:%02x \n", dstMac[0],dstMac[1],dstMac[2],dstMac[3],dstMac[4],dstMac[5]);

    u_char srcMac[6] = {0};
    for (int i = 0; i < 6; ++i) {
        srcMac[i] = *(pkt_data+i+6);
    }
    printf("Src Mac: %02x:%02x:%02x:%02x:%02x:%02x \n", srcMac[0],srcMac[1],srcMac[2],srcMac[3],srcMac[4],srcMac[5]);

    u_char type[2] = {0};
    for (int i = 0; i < 2; ++i) {
        type[i] = *(pkt_data + i + 12);
    }

    printf("Type: ");
    if(type[0] == 0x08){
        if(type[1] == 0x00){
            printf("IPv4\n");
            printf("\n ================IP===============\n");
            u_char versionAndHeaderLen = *(pkt_data + 14);
            u_char version = versionAndHeaderLen >> 4; // 版本
            u_char headerLen = versionAndHeaderLen << 4;
            headerLen = headerLen >> 4; // 首部长度
            u_char realHeaderLen = headerLen * 4;
            if(version == 0x04){
                printf("version : IPv4\n");
            }
            printf("HeaderLen: %d(%d Byte) \n", headerLen, realHeaderLen);

            u_char allLen[2] = {0};
            for (int i = 0; i < 2; ++i) {
                allLen[i] = *(pkt_data + i + 14 + 2);
            }
            // 手动字节转换
            u_short resLen = 0;
            u_char *tmpLen = (u_char*)&resLen;
            tmpLen[0] = allLen[1];
            tmpLen[1] = allLen[0];
            // 基于函数转换
            // u_short resLen = ntohs(*((u_short*)allLen));
            printf("AllLen: %d Byte \n", resLen);

            u_char protocol = *(pkt_data + 14 + 9);
            switch (protocol) {
                case 1:
                    printf("Protocol: ICMP \n");
                    break;
                case 2:
                    printf("Protocol: IGMP \n");
                    break;
                case 6:
                    printf("Protocol: TCP \n");
                    break;
                case 17:
                    printf("Protocol: UDP \n");
                    break;
                case 89:
                    printf("Protocol: OSPF \n");
                    break;
            }
            u_char srcIP[4] = {0};
            for (int i = 0; i < 4; ++i) {
                srcIP[i] = *(pkt_data + i + 14 + 12);
            }
            printf("Src IP: %d.%d.%d.%d\n", srcIP[0],srcIP[1],srcIP[2],srcIP[3]);
            u_char dstIP[4] = {0};
            for (int i = 0; i < 4; ++i) {
                dstIP[i] = *(pkt_data + i + 14 + 12 + 4);
            }
            printf("Dst IP: %d.%d.%d.%d\n", dstIP[0],dstIP[1],dstIP[2],dstIP[3]);

            printf("\n ================IP================\n\n");

            if(protocol == 6){
                printf("\n ================TCP================\n\n");
                u_char srcPort[2] = {0};
                for (int i = 0; i < 2; ++i) {
                    srcPort[i] = *(pkt_data + 14 + realHeaderLen + i);
                }
                u_short resSrcPort = ntohs(*((u_short*)srcPort));
                printf("Src Port: %d\n", resSrcPort);
                u_char dstPort[2] = {0};
                for (int i = 0; i < 2; ++i) {
                    dstPort[i] = *(pkt_data + 14 + 2 + realHeaderLen + i);
                }
                u_short resDstPort = ntohs(*((u_short*)dstPort));
                printf("Dst Port: %d\n", resDstPort);
                u_char offsetAndReserved = *(pkt_data+14+realHeaderLen+12);
                u_char offset = offsetAndReserved >> 4;
                printf("offset: %d \n", offset);
                printf("================TCP================\n");

            }

        } else if(type[1] == 0x06){
            printf("ARP\n");
        }
    }

    // pkt_data 给的存储数据的内存首地址，我们根据实际捕获的长度去遍历即可获取完整数据
    printf("\n Data: \n ----------------------\n");
    for (int i = 0; i < pkt_header->caplen; ++i) {
        printf("%x", pkt_data[i]);
    }
    printf("\n----------------------\n");

}


int examples::simple() {
    pcap_if_t *alldevsp = NULL;
    pcap_if_t *tempdevsp = NULL;
    char errbuf[PCAP_ERRBUF_SIZE] = {0};
    int iRet = pcap_findalldevs(&alldevsp, errbuf);

    if(iRet == 0){
        tempdevsp = alldevsp;
        tempdevsp = tempdevsp->next;
        do{
            printf("Name: %s\nDesc:%s\n", tempdevsp->name, tempdevsp -> description);
            pcap_addr *tempaddr = tempdevsp->addresses;
            do{
                sockaddr_in *ipaddr = (sockaddr_in*)tempaddr->addr;
                printf("IP: %s\n", inet_ntoa(ipaddr->sin_addr));
                tempaddr = tempaddr->next;
            }while(tempaddr);
            printf("==============================\n");
            tempdevsp = tempdevsp->next;
        }while(tempdevsp);
    } else{
        printf("pcap_findalldevs error: %s \n", errbuf);
    }
    printf("=================\n");

    pcap_freealldevs(alldevsp);
    return 0;
}

int examples::haveRemote() {
    pcap_if_t *alldevsp = NULL;
    pcap_if_t *tempdevsp = NULL;
    char errbuf[PCAP_ERRBUF_SIZE] = {0};
    // 1, 遍历适配器
    int iRet = pcap_findalldevs(&alldevsp, errbuf);
    if(iRet != 0){
        printf("pcap_findalldevs error: %s \n", errbuf);
        return 0;
    }
    tempdevsp = alldevsp;
    do{
        printf("Name: %s\nDesc:%s\n", tempdevsp->name, tempdevsp -> description);
        tempdevsp = tempdevsp->next;
    }while(tempdevsp);

    // 打开指定适配器
    pcap_t *pcap = pcap_open("\\Device\\NPF_{81C50322-61CA-400E-B703-40B7D25AB56C}", 65535, 1, NULL, NULL, errbuf);
    if(!pcap){
        printf("open pcap error: %s\n", errbuf);
        return 0;
    }
    // 3,关闭适配器
    pcap_freealldevs(alldevsp);

    // 4, 捕获数据包
    pcap_loop(pcap, 0, Mypcap_handler, NULL);

    return 0;
}

// 初始化开始时间和结束时间
// 这里使用static关键字是为了避免重复初始化
static timeval tBegin,tEnd = {0};
// 初始化数据长度
static DWORD dataLen = 0;

void Net_speed_pcap_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char * pkt_data) {
    // 5，编写回调函数
    // 结束为抓包时间
    tEnd = pkt_header -> ts;
    // 数据长度递增
    dataLen += pkt_header->caplen;
    // 计算开始时间减去结束时间
    DWORD fTime = tEnd.tv_sec - tBegin.tv_sec + (tEnd.tv_usec - tBegin.tv_usec) / 1000000;
    // 当时间大于1s ，计算网速
    if(fTime > 1){
        float speed = dataLen / fTime;
        printf("Speed: %f K/S \n", speed/1024);
        // 将开始时间移到结束时间，提供下一次计算
        tBegin = tEnd;
        // 初始化
        dataLen = 0;
    }
}

int examples::netSpeed() {
    pcap_if_t *alldevsp = NULL;
    pcap_if_t *tempdevsp = NULL;
    char errbuf[PCAP_ERRBUF_SIZE] = {0};
    // 1, 遍历适配器
    int iRet = pcap_findalldevs(&alldevsp, errbuf);
    if(iRet != 0){
        printf("pcap_findalldevs error: %s \n", errbuf);
        return 0;
    }
    tempdevsp = alldevsp;
    do{
        printf("Name: %s\nDesc:%s\n", tempdevsp->name, tempdevsp -> description);
        tempdevsp = tempdevsp->next;
    }while(tempdevsp);

    // 打开指定适配器
    pcap_t *pcap = pcap_open("\\Device\\NPF_{81C50322-61CA-400E-B703-40B7D25AB56C}", 65535, 1, NULL, NULL, errbuf);
    if(!pcap){
        printf("open pcap error: %s\n", errbuf);
        return 0;
    }
    // 3,关闭适配器
    pcap_freealldevs(alldevsp);

    // 4, 捕获数据包
    pcap_loop(pcap, 0, Net_speed_pcap_handler, NULL);

    return 0;
}


