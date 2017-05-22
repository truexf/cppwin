/*  usage
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
	TcpClientManager *m = new TcpClientManager;
	m->Init(log,10);
	m->Go();
	TcpClient *c = m->ConnectRemote("172.18.188.175",8888);
	if (c)
	{
		c->m_on_data_recved = OnRecv;
		string s;
		while(true)
		{
			getline(cin,s);
			if (SameText(s,"exit"))
				return 0;
			c->Send((void*)(s.c_str()),s.length());
		}
	}
	else
	{
		printf("start fail.");
	}
	return 0;
}
*/
#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
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

class TcpClient;
class TcpClientManager;
class TcpClientWorker;

typedef void (*OnTcpClientConnected)(TcpClient *c);
typedef void (*OnTcpClientDisconnected)(TcpClient *c);
typedef void (*OnTcpClientDataSent)(TcpClient *c,void *buf, size_t len);
typedef void (*OnTcpClientDataRecved)(TcpClient *c,void *buf, size_t len);

struct TcpClientBuf
{
	TcpClient *m_client;
	void *m_buf;
	size_t m_len;
};

struct SendBuf
{
	void *m_buf;
	size_t m_len;
	size_t m_send_pos;
};

struct ClosedTcpClient
{
	SYSTEMTIME m_close_time;
	TcpClient *m_client;
};

class TcpClientManager: public TThread
{
	friend class TcpClient;
	friend class TcpClientWorker;	
private:
	HANDLE m_iocp;
	map<string, TcpClient*> m_clients;
	RTL_CRITICAL_SECTION *m_clients_lock;
	deque<TcpClientWorker*> m_workers;
	RTL_CRITICAL_SECTION *m_workers_lock;
	RTL_CRITICAL_SECTION *m_consumers_lock;
	XLog *m_log;
	HBuff *m_perhandledata_buf; 
	int m_worker_count;	
	map<string, ClosedTcpClient> m_closed_clients;
	RTL_CRITICAL_SECTION *m_closed_clients_lock;
protected:
	void Run();
public:	
	OnTcpClientConnected m_on_client_connected;
	OnTcpClientDisconnected m_on_client_disconnected;

	void Init(XLog *log, int worker_count);
	bool Go();
	TcpClient *ConnectRemote(string ip, unsigned short port, OnTcpClientDataRecved onRecved, OnTcpClientDataSent onSent);
	void RemoveClient(TcpClient *c);
};

class TcpClient
{
	friend class TcpClientManager;
	friend class TcpClientWorker;
private:
	SOCKET m_socket;
	string m_remote_ip;
	unsigned short m_remote_port;
	deque<SendBuf> m_send_queue;
	RTL_CRITICAL_SECTION *m_send_queue_lock;
	TcpClientManager *m_mgr;
	bool m_can_send; 
	char m_recv_buf[8192];
	RTL_CRITICAL_SECTION *m_recv_buf_lock;

	void OnDataComplete(int complet_bytes,LPWSAOVERLAPPED ovlp);
	void Recv();
	void SetSocket(SOCKET s){m_socket = s;}
public:
	string m_uuid;
	void *m_data;

	OnTcpClientDataRecved m_on_data_recved;
	OnTcpClientDataSent m_on_data_sent;

	TcpClient();
	~TcpClient();
	void SetMgr(TcpClientManager *m){m_mgr = m;}
	void SetData(void *data){m_data = data;}	
	void Send(void *buf,size_t len);		
};

class TcpClientWorker: public TThread
{
private:
	TcpClientManager *m_mgr;
protected:
	void Run();
public:
	void SetMgr(TcpClientManager *m){m_mgr = m;}
};

#endif