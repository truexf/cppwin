#include "tcpclient.h"
#include "xlog.h"
#include "CSysUtils.h"

void TcpClientManager::Init(XLog *log, int worker_count)
{
	m_worker_count = worker_count;	
	m_log = log;
}

bool TcpClientManager::Go()
{
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
	if (!m_iocp)
	{
		LOG_ERR(m_log,FormatString("create iocp fail,errno %d",GetLastError()).c_str());
		return false;
	}	
	for (int i = 0; i < m_worker_count; i++)
	{
		TcpClientWorker *w = new TcpClientWorker;
		w->SetMgr(this);
		w->Start();
		m_workers.push_back(w);
	}
	CreatePerhandledataBuf(&m_perhandledata_buf,256);
	m_clients_lock = new RTL_CRITICAL_SECTION;
	InitializeCriticalSection(m_clients_lock);
	m_workers_lock = new RTL_CRITICAL_SECTION;
	InitializeCriticalSection(m_workers_lock);
	m_closed_clients_lock = new RTL_CRITICAL_SECTION;
	InitializeCriticalSection(m_closed_clients_lock);
	Start();
	return true;
}

TcpClient * TcpClientManager::ConnectRemote(string ip, unsigned short port, OnTcpClientDataRecved onRecved, OnTcpClientDataSent onSent)
{
	SOCKET skt = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == skt)
	{
		LOG_ERR(m_log,FormatString("create socket fail,%d",WSAGetLastError()).c_str());
		return NULL;
	}
	sockaddr_in addr;
	memset(&addr,0,sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);
	if (SOCKET_ERROR == connect(skt,(sockaddr*)&addr,sizeof(sockaddr_in)))
	{
		closesocket(skt);
		LOG_WARN(m_log,FormatString("connect to %s:%d fail,%d",ip.c_str(),port,WSAGetLastError()).c_str());
		return NULL;
	}
	
	TcpClient *c = new TcpClient();
	if (NULL == CreateIoCompletionPort((HANDLE)skt,m_iocp,(ULONG_PTR)c,0))
	{
		closesocket(skt);
		delete c;
		LOG_ERR(m_log,FormatString("bind skt to iocp fail,%d",GetLastError()).c_str());
		return NULL;
	}
	c->m_uuid = CreateGUID();
	c->SetMgr(this);
	c->SetSocket(skt);
	{
		AutoCriticalSection auto1(*m_clients_lock);
		m_clients[c->m_uuid] = c;
	}
	c->m_on_data_recved = onRecved;
	c->m_on_data_sent = onSent;
	c->Recv();
	return c;	
}

void TcpClientManager::RemoveClient(TcpClient *c)
{
	if (!c)
		return;
	AutoCriticalSection auto1(*m_closed_clients_lock);
	AutoCriticalSection auto2(*m_clients_lock);
	map<string, ClosedTcpClient>::iterator it = m_closed_clients.find(c->m_uuid);
	if (it == m_closed_clients.end())
		return;	
	map<string, TcpClient*>::iterator it2 = m_clients.find(c->m_uuid);
	if (it2 != m_clients.end())
	{
		m_clients.erase(it2);
	}
	closesocket(c->m_socket);
	ClosedTcpClient cc;
	GetLocalTime(&cc.m_close_time);
	cc.m_client = c;
	m_closed_clients[c->m_uuid] = cc;
	return;
}

void TcpClientManager::Run()
{
	while (true)
	{
		if (WaitforTerminate(1000))
			break;
		{
			vector<string> cc_tmp;
			AutoCriticalSection auto1(*m_closed_clients_lock);
			for (map<string, ClosedTcpClient>::iterator it = m_closed_clients.begin(); it != m_closed_clients.end(); it++)
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				if (it->second.m_client && SecondsBetween(it->second.m_close_time,st) > 10)
				{
					delete it->second.m_client;
					cc_tmp.push_back(it->first);
				}
			}
			for (size_t i = 0; i < cc_tmp.size(); i++)
			{
				m_closed_clients.erase(cc_tmp[i]);
			}
		}
	}
}

TcpClient::TcpClient(): m_data(NULL),m_mgr(NULL),m_can_send(true)
{
	m_on_data_recved = NULL;
	m_on_data_sent = NULL;
	m_send_queue_lock = new RTL_CRITICAL_SECTION;	
	InitializeCriticalSection(m_send_queue_lock);
	m_recv_buf_lock = new RTL_CRITICAL_SECTION;	
	InitializeCriticalSection(m_recv_buf_lock);
	memset(&m_recv_buf,0,sizeof(m_recv_buf));
}

TcpClient::~TcpClient()
{
	if (m_on_data_sent)
	{
		for (deque<SendBuf>::iterator it = m_send_queue.begin(); it != m_send_queue.end(); it++)
		{
			m_on_data_sent(this,it->m_buf,it->m_len);
		}
	}
	DeleteCriticalSection(m_send_queue_lock);
	delete m_send_queue_lock;
	DeleteCriticalSection(m_recv_buf_lock);
	delete m_recv_buf_lock;
}

void TcpClient::Send(void *buf,size_t len)
{	
	AutoCriticalSection auto1(*m_send_queue_lock);
	if (buf && len>0)
	{
		SendBuf sb;
		sb.m_buf = buf;
		sb.m_len = len;
		sb.m_send_pos = 0;
		m_send_queue.push_back(sb);
	}
	if (m_can_send)
	{
		if (m_send_queue.empty())
			return;
		SendBuf sb = m_send_queue.front();
		char *send_buf = (char*)AddPtr(sb.m_buf,sb.m_send_pos);
		WSABUF buf;
		buf.buf = send_buf;
		buf.len = sb.m_len - sb.m_send_pos;
		if (buf.len > 8192)
			buf.len = 8192;
		DWORD send_bytes = 0;
		perhandledata *ovlp = CreatePerhandledata(m_mgr->m_perhandledata_buf);
		ovlp->data = 1; //1 send, 0 recv
		if (SOCKET_ERROR == WSASend(m_socket,&buf,1,&send_bytes,0,(LPWSAOVERLAPPED)ovlp,NULL))
		{
            int err = WSAGetLastError();
            if(WSA_IO_PENDING != err)
			{
				LOG_ERR(m_mgr->m_log,FormatString("wsasend fail,%d",WSAGetLastError()).c_str());
				m_mgr->RemoveClient(this);
				return;
			}
		}
		m_can_send = false;
	}

	return;
}

void TcpClient::Recv()
{
	AutoCriticalSection auto1(*m_recv_buf_lock);
	perhandledata *ovlp = CreatePerhandledata(m_mgr->m_perhandledata_buf);
	ovlp->data = 0; //1 send, 0 recv
	WSABUF send_buf;
	send_buf.buf = m_recv_buf;
	send_buf.len = sizeof(m_recv_buf);
	DWORD recv_bytes = 0;
	DWORD recv_flag = 0;	
	if (SOCKET_ERROR == WSARecv(m_socket,&send_buf,1,&recv_bytes,&recv_flag,(LPWSAOVERLAPPED)ovlp,NULL))
	{
        int err = WSAGetLastError();
        if(WSA_IO_PENDING != err)
		{
			LOG_ERR(m_mgr->m_log,FormatString("wsarecv fail,%d",WSAGetLastError()).c_str());
			m_mgr->RemoveClient(this);
			return;
		}
	}
}

void TcpClient::OnDataComplete(int complete_bytes,LPWSAOVERLAPPED ovlp)
{
	perhandledata *pd = (perhandledata*)ovlp;
	if (1 == pd->data) //send compeleted
	{
		AutoCriticalSection auto1(*m_send_queue_lock);
		m_can_send = true;
		SendBuf sb = m_send_queue.front();
		sb.m_send_pos += complete_bytes;
		if (sb.m_send_pos >= sb.m_len)
		{
			m_send_queue.pop_front();
			if (m_on_data_sent)
				m_on_data_sent(this,sb.m_buf,sb.m_len);			
		}
		else
		{
			m_send_queue.pop_front();
			m_send_queue.push_front(sb);			
		}		
		Send(NULL,0);
	}
	else //recv completed
	{
		if (complete_bytes > 0)
		{
			if (m_on_data_recved)
			{
				m_on_data_recved(this,m_recv_buf,complete_bytes);				
			}
		}
		Recv();
	}
}

void TcpClientWorker::Run()
{
    DWORD complete_bytes = 0;
    ULONG_PTR complete_key = NULL;
    TcpClient* skt = NULL;
    LPOVERLAPPED ovlp = NULL;
    while (!GetTerminated())
    {
        if(ovlp)
			FreePerhandledata((perhandledata*)ovlp,m_mgr->m_perhandledata_buf);
        complete_key = NULL;
        skt = NULL;        
        complete_bytes = 0;
        ovlp = NULL;
		if (FALSE == GetQueuedCompletionStatus(m_mgr->m_iocp,&complete_bytes,&complete_key,&ovlp,INFINITE))
        {
            DWORD dwErr = GetLastError();
            if (WAIT_TIMEOUT == dwErr) //wait timeout,continue wait
            {
                continue;
            }
            else if (NULL == ovlp) //no overlapped,none of my business
            {
                //LOGSYSERR(m_server->m_log,L"HSWorker::Run()",L"GetQueuedCompletionStatus");
                break;
            }
            else if (complete_key == NULL) //no key,none of my business
            {
                break;
            }
            else if (ERROR_ABANDONED_WAIT_0 == dwErr) //invalid iocp_handle
            {
                break;
            }
            else if (complete_bytes == 0) //io fail,close socket
            {                
                skt = (TcpClient*)(complete_key);
				m_mgr->RemoveClient(skt);
                continue;
            }
            else 
            {
                skt = (TcpClient*)(complete_key);
				skt->OnDataComplete(complete_bytes,ovlp);
                continue;        
            }
        }
        else//io successful
        {
            skt = (TcpClient*)(complete_key);
            if(0 == complete_bytes)
            {                
				//remote shutdown
				m_mgr->RemoveClient(skt);
            }
            else 
			{
				skt->OnDataComplete(complete_bytes,ovlp);
			}
        }    
    }

}
