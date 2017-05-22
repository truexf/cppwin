#ifndef __SOCKETUTIL__
#define __SOCKETUTIL__

#include <WinSock2.h>
#include <Mstcpip.h>
#include <Windows.h>

inline void wsastartup()
{
	WSADATA wsa={0};
	WSAStartup(MAKEWORD(2,2),&wsa);	
}

inline void wsacleanup()
{
	WSACleanup();
}

inline SOCKET tcpsocket()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

inline SOCKET udpsocket()
{
	return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

inline void addr4(const char* ip,unsigned short port, sockaddr_in &ret)
{
	memset(&ret,0,sizeof(sockaddr_in));
	ret.sin_family = AF_INET;
	ret.sin_addr.S_un.S_addr = inet_addr(ip);
	ret.sin_port = htons(port);
	return;
}

inline int bindaddr4(SOCKET fd, const char* ip,unsigned short port)
{
	sockaddr_in addr;
	addr4(ip,port,addr);
	return bind(fd,(sockaddr*)&addr,sizeof(sockaddr_in));
}

inline int setlinger(SOCKET fd,unsigned short onoff,unsigned short seconds)
{
	linger lg;
	lg.l_onoff = onoff;
	lg.l_linger = seconds;
	return setsockopt(fd,SOL_SOCKET,SO_LINGER,(char*)&lg,sizeof(linger));
}

inline int setkeepalive(SOCKET fd, u_long onoff,u_long alive_milliseconds,u_long interval_milliseconds)
{
	tcp_keepalive kl;
	kl.onoff = onoff;
	kl.keepalivetime = alive_milliseconds;
	kl.keepaliveinterval = interval_milliseconds;
	return WSAIoctl(fd,SIO_KEEPALIVE_VALS,&kl,sizeof(tcp_keepalive),NULL,0,NULL,NULL,NULL);
}

inline int setrecvbuflen(SOCKET fd, int len)
{
	int l = len;
	return setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*)&l,sizeof(int));
}

inline int setsendbuflen(SOCKET fd, int len)
{
	int l = len;
	return setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char*)&l,sizeof(int));
}

int GetTcp4AddrInfo(const char *host, const char *port, sockaddr_in &ret);


#endif