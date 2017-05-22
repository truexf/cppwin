#ifndef WINHTTPWRAPPER_H
#define WINHTTPWRAPPER_H

#include <string>
using std::wstring;
#include "TThread.h"
class TThread;
typedef void (*OnHttpGetRet)(int retCode);


int winhttp_get(const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,bool follow_location=true,const wchar_t *user_agent = NULL);
int winhttp_post(const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,void *postdata,int postdata_size,bool follow_location=true,const wchar_t *user_agent = NULL);
void async_winhttp_get(OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,bool follow_location=true,const wchar_t *user_agent = NULL);
void async_winhttp_post(OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,void *postdata,int postdata_size,bool follow_location=true,const wchar_t *user_agent = NULL);




class Winhttp_get: public TThread
{
public:
	void Set(OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,bool follow_location,const wchar_t *user_agent);
protected:
	void Run(); //override;
private:
	wstring m_url;
	wstring m_headers_crlf;
	void** m_response;
	int *m_response_size;
	wstring *m_response_header;
	bool m_follow_location;  
	wstring m_user_agent;
	OnHttpGetRet m_complete;
};

class Winhttp_post: public TThread
{
public:
	void Set(OnHttpGetRet on_complete,const wchar_t *url,const wchar_t *headers_crlf,void **response,int &response_size,wstring *response_header,void *postdata,int postdata_size,bool follow_location,const wchar_t *user_agent);
protected:
	void Run(); //override;
private:
	wstring m_url;
	wstring m_headers_crlf;
	void** m_response;
	int *m_response_size;
	wstring *m_response_header;
	bool m_follow_location;  
	wstring m_user_agent;
	OnHttpGetRet m_complete;
	void *m_postdata;
	int m_postdata_size;
};

#endif