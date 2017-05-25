/*  usage
void OnConnected(Client *c)
{
	printf("client connected\n");
}
void OnDisconnected(Client *c)
{
	printf("client disconnected\n");
}
void OnRecv(TcpClient *c,void *buf, size_t len)
{
	printf("%s\n",string((char*)buf,len).c_str());
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa={0};
    WSAStartup(MAKEWORD(2,2),&wsa);
	AutoFuncClass auto1(AutoFunc(WSACleanup));

	XLog *log = new XLog();
	log->Start();
	TcpServerManager *m = new TcpServerManager;
	m->Init(log,10);
	m->OnClientConnected = OnConnected;
	m->OnClientDisconnected = OnDisconnected;
	if (m->Go(8080, 100))
		printf("start ok");
	else 
		printf("start fail")
	return 0;
}
*/
#ifndef _TCPSERVER_H
#define _TCPSERVER_H
#include <WinSock2.h>
#include <Windows.h>
#include "xlog.h"
#include <string>
#include <deque>
#include <vector>
#include <map>
using std::string;
using std::deque;
using std::vector;
using std::map;
#include "TThread.h"
#include "hbuffer.hpp"
#include "hperhandledata.hpp"
#include "CSysUtils.h"

class Client;
class TcpServerManager;
class TcpServerWorker;
class TcpServerListener;

typedef void (*OnClientConnected)(Client *c);
typedef void (*OnClientDisconnected)(Client *c);
typedef void (*OnClientDataSent)(Client *c,void *buf, size_t len);
typedef void (*OnClientDataRecved)(Client *c,void *buf, size_t len);

struct ClientBuf
{
	Client *m_client;
	void *m_buf;
	size_t m_len;
};

struct ClientSendBuf
{
	void *m_buf;
	size_t m_len;
	size_t m_send_pos;
};

struct ClosedClient
{
	SYSTEMTIME m_close_time;
	Client *m_client;
};

class TcpServerManager: public TThread
{
	friend class Client;
	friend class TcpServerWorker;	
	friend class TcpServerListener;
private:
	HANDLE m_iocp;
	map<string, Client*> m_clients;
	RTL_CRITICAL_SECTION *m_clients_lock;
	deque<TcpServerWorker*> m_workers;
	RTL_CRITICAL_SECTION *m_workers_lock;
	XLog *m_log;
	HBuff *m_perhandledata_buf; 
	int m_worker_count;	
	map<string, ClosedClient> m_closed_clients;
	RTL_CRITICAL_SECTION *m_closed_clients_lock;
protected:
	void Run();
public:	
	OnClientConnected m_on_client_connected;
	OnClientDisconnected m_on_client_disconnected;

	void Init(XLog *log, int worker_count);
	bool Go(unsigned port, int backlog);
	void RemoveClient(Client *c);
};

class TcpServerListener: public TThread {
private:
	TcpServerManager *m_svr;
	SOCKET m_skt;
protected:
	void Run();
public:
	bool StartListen(TcpServerManager *svr, unsigned short port,int backlog);
};

class Client
{
	friend class TcpServerManager;
	friend class TcpServerWorker;
	friend class TcpServerListener;
private:
	SOCKET m_socket;
	string m_remote_ip;
	unsigned short m_remote_port;
	deque<ClientSendBuf> m_send_queue;
	RTL_CRITICAL_SECTION *m_send_queue_lock;
	TcpServerManager *m_mgr;
	bool m_can_send; 
	char m_recv_buf[8192];
	RTL_CRITICAL_SECTION *m_recv_buf_lock;

	void OnDataComplete(int complet_bytes,LPWSAOVERLAPPED ovlp);
	void Recv();
	void SetSocket(SOCKET s){m_socket = s;}
public:
	string m_uuid;
	void *m_data;

	OnClientDataRecved m_on_data_recved;
	OnClientDataSent m_on_data_sent;

	Client();
	~Client();
	void SetMgr(TcpServerManager *m){m_mgr = m;}
	void SetData(void *data){m_data = data;}	
	void Send(void *buf,size_t len);		
};

class TcpServerWorker: public TThread
{
private:
	TcpServerManager *m_mgr;
protected:
	void Run();
public:
	void SetMgr(TcpServerManager *m){m_mgr = m;}
};

#endif