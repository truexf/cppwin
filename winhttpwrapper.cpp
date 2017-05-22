#include "winhttpwrapper.h"
#include <Windows.h>
#include <winhttp.h>
#include "CSysUtils.h"
#pragma comment(lib,"winhttp.lib")


bool ParseURL(const wchar_t *url,wstring &host,wstring &path,wstring &obj,INTERNET_PORT &port)
{
	wchar_t szHostName[2048] = L"";
	wchar_t szURLPath[2048] = L"";
	wchar_t szURLExtra[2048] = L"";
	URL_COMPONENTS urlComp;
	memset(&urlComp, 0, sizeof(urlComp));
	urlComp.dwStructSize = sizeof(urlComp);
	urlComp.lpszHostName = szHostName;
	urlComp.dwHostNameLength = 2048;
	urlComp.lpszUrlPath = szURLPath;
	urlComp.dwUrlPathLength = 2048;
	urlComp.lpszExtraInfo = szURLExtra;
	urlComp.dwExtraInfoLength = 2048;
	urlComp.dwSchemeLength = -1;

	if (::WinHttpCrackUrl(url, wcslen(url), 0, &urlComp))
	{
		host.assign(szHostName);
		path.assign(szURLPath);
		obj.assign(szURLExtra);
		port = urlComp.nPort;
		return true;
	}

	return false;
}

int winhttp_get(const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,bool follow_location,const wchar_t *user_agent )
{    
	wstring host,path,obj;
	WORD wdPort = 0;
	if (!ParseURL(url,host,path,obj,wdPort))
		return -1;
	HINTERNET hSession = NULL,hConnect = NULL,hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD dwSize = 0,dwDownLoaded = 0;
	MemoryStream memResponse;
	void *buf = NULL;
	/*proxy*/
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG prxy;
	memset(&prxy,0,sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	WinHttpGetIEProxyConfigForCurrentUser(&prxy);
	DWORD prxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
	LPCTSTR prxy_name = WINHTTP_NO_PROXY_NAME;
	LPCTSTR prxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	if (prxy.lpszProxy)
	{
		prxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		prxy_name = prxy.lpszProxy;
		prxy_bypass = prxy.lpszProxyBypass;
	}    
	wstring uagent = L"Mozilla/5.0 (compatible)";
	if(user_agent)
		uagent.assign(user_agent);
	hSession = WinHttpOpen(uagent.c_str(),
		prxy_type,
		prxy_name,
		prxy_bypass,
		0);
	if (prxy.lpszAutoConfigUrl)
		GlobalFree(prxy.lpszAutoConfigUrl);
	if (prxy.lpszProxy)
		GlobalFree(prxy.lpszProxy);
	if (prxy.lpszProxyBypass)
		GlobalFree(prxy.lpszProxyBypass);

	if (hSession)
		hConnect = WinHttpConnect(hSession,host.c_str(),wdPort,0);
	DWORD openflag = WINHTTP_FLAG_REFRESH;
	if(wdPort == 443)
		openflag |= WINHTTP_FLAG_SECURE;
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect,L"GET",(path+obj).c_str(),NULL,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,openflag);
	if (hRequest && headers_crlf)
		bResults = WinHttpAddRequestHeaders(hRequest,headers_crlf,-1,WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	if(bResults && !follow_location)
	{
		DWORD d1 = WINHTTP_DISABLE_REDIRECTS;
		WinHttpSetOption(hRequest,WINHTTP_OPTION_DISABLE_FEATURE ,&d1,4);
	}    
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,NULL,0,0,NULL);
	if (bResults)
		bResults = WinHttpReceiveResponse( hRequest, NULL );
	
	if (bResults && response_header)
	{
		dwSize = 0;
		WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_RAW_HEADERS_CRLF,WINHTTP_HEADER_NAME_BY_INDEX,NULL,&dwSize,WINHTTP_NO_HEADER_INDEX);
		if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			wchar_t *head = new wchar_t[dwSize/sizeof(wchar_t)];
			bResults = WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_RAW_HEADERS_CRLF,WINHTTP_HEADER_NAME_BY_INDEX,head,&dwSize,WINHTTP_NO_HEADER_INDEX);
			if (bResults)
				response_header->assign(head);
			delete [] head;
		}
	}
	
	dwSize = 0;

	if (bResults)
	{

		do 
		{
			if (!WinHttpQueryDataAvailable(hRequest,&dwSize))
				break;
			if (dwSize == 0)
				break;            
			buf = malloc(dwSize);
			if (NULL==buf)
				break;
			if (!WinHttpReadData(hRequest,buf,dwSize,&dwDownLoaded))            
				break;
			memResponse.Write(buf,dwDownLoaded);
			free(buf);
			buf = NULL;
		} while (dwSize>0);
		if (buf)
			free(buf);
		if (memResponse.GetSize() > 0)
		{
			*response = malloc(memResponse.GetSize());
			if (*response)
			{
				memResponse.Seek(MemoryStream::soBegin,0);
				memResponse.Read(*response,memResponse.GetSize());
				response_size = memResponse.GetSize();
			}
		}
	}


	int reterr = GetLastError();    
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);


	if (bResults)
		return 0;
	else
		return reterr;    

}

int winhttp_post(const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,void *postdata,int postdata_size,bool follow_location,const wchar_t *user_agent)
{
	wstring host,path,obj;
	WORD wdPort = 0;
	if (!ParseURL(url,host,path,obj,wdPort))
		return -1;
	HINTERNET hSession = NULL,hConnect = NULL,hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD dwSize = 0,dwDownLoaded = 0,dwUpLoaded = 0,dwRemain = 0;
	MemoryStream memResponse;
	void *buf = NULL;
	void *buftmp = NULL;
	/*proxy*/
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG prxy;
	memset(&prxy,0,sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	WinHttpGetIEProxyConfigForCurrentUser(&prxy);
	DWORD prxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
	LPCTSTR prxy_name = WINHTTP_NO_PROXY_NAME;
	LPCTSTR prxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	if (prxy.lpszProxy)
	{
		prxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		prxy_name = prxy.lpszProxy;
		prxy_bypass = prxy.lpszProxyBypass;
	}
	hSession = WinHttpOpen(L"Mozilla/5.0 (compatible)",
		prxy_type,
		prxy_name,
		prxy_bypass,
		0);
	if (prxy.lpszAutoConfigUrl)
		GlobalFree(prxy.lpszAutoConfigUrl);
	if (prxy.lpszProxy)
		GlobalFree(prxy.lpszProxy);
	if (prxy.lpszProxyBypass)
		GlobalFree(prxy.lpszProxyBypass);

	if (hSession)
		hConnect = WinHttpConnect(hSession,host.c_str(),wdPort,0);
	DWORD openflag = WINHTTP_FLAG_REFRESH;
	if(wdPort == 443)
		openflag |= WINHTTP_FLAG_SECURE;
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect,L"POST",(path+obj).c_str(),NULL,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,openflag);
	if (hRequest && headers_crlf)
		bResults = WinHttpAddRequestHeaders(hRequest,headers_crlf,-1,WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	if(bResults && !follow_location)
	{
		DWORD d1 = WINHTTP_DISABLE_REDIRECTS;
		WinHttpSetOption(hRequest,WINHTTP_OPTION_DISABLE_FEATURE ,&d1,4);
	}    
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,NULL,0,postdata_size,NULL);
	if (bResults && postdata && postdata_size > 0)
	{
		dwRemain = postdata_size;
		buftmp = postdata;
		do
		{
			bResults = WinHttpWriteData(hRequest,buftmp,dwRemain,&dwUpLoaded);
			if (bResults)
			{
				dwRemain -= dwUpLoaded;
				IncPtr(&buftmp,dwUpLoaded);
				dwUpLoaded = 0;
				if (0==dwRemain)
					break;
			}
			else
			{
				DWORD i = GetLastError();                
				break;
			}
		}while(1);
	}
	if (bResults)
		bResults = WinHttpReceiveResponse( hRequest, NULL );
	if (bResults && response_header)
	{
		dwSize = 0;
		WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_RAW_HEADERS_CRLF,WINHTTP_HEADER_NAME_BY_INDEX,NULL,&dwSize,WINHTTP_NO_HEADER_INDEX);
		if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			wchar_t *head = new wchar_t[dwSize/sizeof(wchar_t)];
			bResults = WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_RAW_HEADERS_CRLF,WINHTTP_HEADER_NAME_BY_INDEX,head,&dwSize,WINHTTP_NO_HEADER_INDEX);
			if (bResults)
				response_header->assign(head);
			delete [] head;
		}
	}
	dwSize = 0;
	if (bResults)
	{
		do 
		{
			if (!WinHttpQueryDataAvailable(hRequest,&dwSize))
				break;
			if (dwSize == 0)
				break;            
			buf = malloc(dwSize);
			if (NULL==buf)
				break;
			if (!WinHttpReadData(hRequest,buf,dwSize,&dwDownLoaded))            
				break;
			memResponse.Write(buf,dwDownLoaded);
			free(buf);
			buf = NULL;
		} while (dwSize>0);
		if (buf)
			free(buf);
		if (memResponse.GetSize() > 0)
		{
			*response = malloc(memResponse.GetSize());
			if (*response)
			{
				memResponse.Seek(MemoryStream::soBegin,0);
				memResponse.Read(*response,memResponse.GetSize());
				response_size = memResponse.GetSize();
			}
		}
	}

	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	if (bResults)
	{
		return 0;
	}
	else
		return GetLastError();    
}

void async_winhttp_get( OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,bool follow_location,const wchar_t *user_agent)
{
	Winhttp_get *thrd = new Winhttp_get;
	thrd->Set(on_complete,url,headers_crlf,response,response_size,response_header,follow_location,user_agent);
	thrd->Start();
}

void async_winhttp_post(OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,void *postdata,int postdata_size,bool follow_location/*=true*/,const wchar_t *user_agent /*= NULL*/)
{
	Winhttp_post *thrd = new Winhttp_post;
	thrd->Set(on_complete,url,headers_crlf,response,response_size,response_header,postdata,postdata_size,follow_location,user_agent);
	thrd->Start();
}


void Winhttp_get::Run()
{
	SetFreeOnTerminate(true);
	wchar_t *req_head;
	if(m_headers_crlf.empty())
		req_head = NULL;
	else
		req_head = (wchar_t*)m_headers_crlf.c_str();    
	int ret = winhttp_get(m_url.c_str(),req_head,m_response,*m_response_size,m_response_header,m_follow_location,m_user_agent.c_str());
	if(m_complete)
		m_complete(ret);
}

void Winhttp_get::Set( OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,bool follow_location,const wchar_t *user_agent )
{
	m_complete = on_complete;
	m_url = url;
	if(headers_crlf)
		m_headers_crlf = headers_crlf;
	else
		m_headers_crlf.clear();
	m_response = response;
	m_response_size = &response_size;
	m_response_header = response_header;
	m_follow_location = follow_location;
    if(user_agent)
    	m_user_agent.assign(user_agent);
    else
        m_user_agent.clear();
}

void Winhttp_post::Run()
{
	SetFreeOnTerminate(true);
	wchar_t *req_head;
	if(m_headers_crlf.empty())
		req_head = NULL;
	else
		req_head = (wchar_t*)m_headers_crlf.c_str();    
	int ret = winhttp_post(m_url.c_str(),
							req_head,
							m_response,
							*m_response_size,
							m_response_header,
							m_postdata,
							m_postdata_size,
							m_follow_location,
							m_user_agent.c_str());
	if(m_complete)
		m_complete(ret);
}

void Winhttp_post::Set(OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,void *postdata,int postdata_size,bool follow_location,const wchar_t *user_agent)
{
	m_complete = on_complete;
	m_url.assign(url);
    if(headers_crlf)
	    m_headers_crlf.assign(headers_crlf);
    else
        m_headers_crlf.clear();
	m_response = response;
	m_response_size = &response_size;
	m_response_header = response_header;
	m_postdata = postdata;
	m_postdata_size = postdata_size;
	m_follow_location = follow_location;
    if(user_agent)
	    m_user_agent.assign(user_agent);
    else
        m_user_agent.clear();
}
