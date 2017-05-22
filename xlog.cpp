#include "xlog.h"
#include "CSysUtils.h"
#include "TThread.h"
#include <Windows.h>

const char* FormatLog(XLog *log,const char * str, ...)
{
	AutoCriticalSection auto1(*(log->m_logfmt_lock));
	if (str != NULL)
	{
		va_list vl;
		va_start(vl, str);
		int n = vsnprintf(log->m_logfmt, sizeof(log->m_logfmt), str, vl);
		va_end(vl);
		if (n > 0)
		{
			log->m_logfmt[n] = 0;
			return log->m_logfmt;
		}
	}
	return "";
}

XLog::XLog(const wchar_t *p_logfilenamepath,
           int p_logqueue_size,
           int p_flush_interval)
{
  if (!p_logfilenamepath)
    m_logfilepath.assign(ExtractFilePathW(ParamStrW(0),true));
  else
  {  
    m_logfilepath.assign(p_logfilenamepath);
    if (m_logfilepath.empty() || !DirectoryExistsW(m_logfilepath))
      m_logfilepath.assign(ExtractFilePathW(ParamStrW(0),true));
    else
      m_logfilepath.assign(IncludeTrailingPathDelimiterW(m_logfilepath));
  }
  m_logqueue_size = p_logqueue_size;
  if (m_logqueue_size <= 0)
    m_logqueue_size = 1 * 1024 * 1024; //1M;
  m_flush_interval = p_flush_interval;

  m_swaplock = new RTL_CRITICAL_SECTION();
  InitializeCriticalSection(m_swaplock);
  m_prequeuelock = new RTL_CRITICAL_SECTION();
  InitializeCriticalSection(m_prequeuelock);
  m_logfmt_lock = new RTL_CRITICAL_SECTION();
  InitializeCriticalSection(m_logfmt_lock);
  m_prequeue_real = malloc(m_logqueue_size);
  m_savingqueue_real = malloc(m_logqueue_size);
  m_prequeue_logical = m_prequeue_real;
  m_savingqueue_logical = m_savingqueue_real;
  m_prequeue_size = 0;
  m_savingqueue_size = 0;
  m_file_handle = NULL;
  m_file_date = NULL;
  m_file_date_tmp = NULL;
  DWORD dwComputerNameSize = (MAX_COMPUTERNAME_LENGTH + 1);
  m_computer_name = (wchar_t*)malloc(dwComputerNameSize*sizeof(wchar_t));
  memset(m_computer_name,0,dwComputerNameSize*sizeof(wchar_t));
  DWORD dwsize;
  GetComputerNameW(m_computer_name,&dwsize);
}

XLog::~XLog()
{
  if (m_savingqueue_size > 0)
      Flush();
  if (m_prequeue_size > 0)
  {
      SwitchQueue();
      Flush();
  }
  free(m_computer_name);
  if (m_file_date)
    delete m_file_date;
  if(m_file_date_tmp)
      delete m_file_date_tmp;
  free(m_prequeue_real);
  free(m_savingqueue_real);
  delete m_prequeuelock;
  delete m_swaplock;
}


void XLog::Run()
{
  while (!GetTerminated())
  {
    WaitForSingleObject(GetThreadHandle(),m_flush_interval);
    if (GetTerminated())
      break;
    SwitchQueue();
  }
}

void XLog::Flush()
{
  if (0 == m_savingqueue_size)
    return;
  m_file_date_tmp = new SYSTEMTIME();
  GetLocalTime(m_file_date_tmp);
  wstring sfile = m_logfilepath;
  sfile.append(m_computer_name);
  sfile.append(L"_");
  sfile.append(ExtractFileNameW(ParamStrW(0)));
  sfile.append(L"_");
  sfile.append(string2wstring(FormatDate(*m_file_date_tmp)));    
  sfile.append(L".log");      
  
  DWORD file_create_flag;
  if (FileExistsW(sfile.c_str()))
    file_create_flag = OPEN_EXISTING;
  else
    file_create_flag = CREATE_NEW;
  m_file_handle = CreateFileW(sfile.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ,NULL,
    file_create_flag,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (INVALID_HANDLE_VALUE == m_file_handle)
    m_file_handle = NULL;    
  if (m_file_handle)
  {
    SetFilePointer(m_file_handle,0,0,FILE_END);
    if(file_create_flag==CREATE_NEW)
    {
        unsigned char head[2] = {0xFF,0xFE};
        DWORD dw1;
        WriteFile(m_file_handle,head,2,&dw1,NULL);
    }
    DWORD dwWritted = 0;
    void *p = m_savingqueue_logical;
    do
    {            
      if (FALSE == WriteFile(m_file_handle,p,m_savingqueue_size,&dwWritted,NULL))
        break;
      if (dwWritted >= (DWORD)m_savingqueue_size)
        break;
      m_savingqueue_size -= dwWritted;      
      IncPtr(&p,dwWritted);
      dwWritted = 0;
    } while (true);
    CloseHandle(m_file_handle);
  }
  m_savingqueue_size = 0;
}

void XLog::Log(const void *data,int datasize )
{
  if (!data || datasize <= 0)
    return;
  AutoCriticalSection auto1(*m_prequeuelock);
  if (m_prequeue_size + datasize > m_logqueue_size)
    SwitchQueue();
  void *p = m_prequeue_logical;
  IncPtr(&p,m_prequeue_size);
  memcpy(p,data,datasize);
  m_prequeue_size += datasize;
}

void XLog::LogStr(const char *str)
{
	Log(str,strlen(str));
}

void XLog::SwitchQueue()
{
  EnterCriticalSection(m_swaplock);
  AutoFuncClass auto_1(AutoFunc(LeaveCriticalSection,m_swaplock));
  if (m_savingqueue_size >= 0)    
    Flush();
  void *tmp;
  tmp = m_prequeue_logical;
  m_prequeue_logical = m_savingqueue_logical;
  m_savingqueue_logical = tmp;
  m_savingqueue_size = m_prequeue_size;  
  m_prequeue_size = 0;
}

void XLog::LogWarn(const  wchar_t *data )
{
  //yyyy-mm-dd hh:nn:ss:zzz(utc) warn threadid logdata
  SYSTEMTIME st;
  GetLocalTime(&st);  
  wstring s = FormatDatetimeW(st) + L" warn " + IntToStrW(GetCurrentThreadId()) + L" " + wstring(data) + L"\r\n";
  Log(s.c_str(),s.length()*sizeof(wchar_t));
}

void XLog::LogError(const wchar_t *data )
{
  //yyyy-mm-dd hh:nn:ss:zzz(utc) error threadid logdata
  SYSTEMTIME st;
  GetLocalTime(&st);  
  wstring s = FormatDatetimeW(st) + L" error " + L" " + IntToStrW(GetCurrentThreadId()) + L" " + wstring(data) + L"\r\n";
  Log(s.c_str(),s.length()*sizeof(wchar_t));
}

void XLog::LogInfo(const  wchar_t *data )
{
  //yyyy-mm-dd hh:nn:ss:zzz(utc) warn threadid logdata
  SYSTEMTIME st;
  GetLocalTime(&st);  
  wstring s = FormatDatetimeW(st) + L" info " + L" " + IntToStrW(GetCurrentThreadId()) + L" " + wstring(data) + L"\r\n";
  Log(s.c_str(),s.length()*sizeof(wchar_t));
}

void XLog::LogDebug(const wchar_t *data )
{
  //yyyy-mm-dd hh:nn:ss:zzz(utc) warn threadid logdata
  SYSTEMTIME st;
  GetLocalTime(&st);  
  wstring s = FormatDatetimeW(st) + L" warn " + L" " + IntToStrW(GetCurrentThreadId()) + L" " + wstring(data) + L"\r\n";
  OutputDebugStringW(s.c_str());
}
