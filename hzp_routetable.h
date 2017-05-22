#ifndef HZP_ROUTETABLE_H_
#define HZP_ROUTETABLE_H_
#include <WinSock2.h>
#include <Windows.h>
#pragma pack(push)
#pragma pack(1)
struct RouteItem
{
    unsigned short port;
    unsigned char addr[16];
    bool use;
};
#pragma pack(pop)

HANDLE HzpCreateRouteTable();
bool HzpFindRemoteAddr(unsigned short port,void *addr);
bool AddRouteItem(unsigned short port,SOCKADDR_IN *addr);


#endif