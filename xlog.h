#ifndef XLOG_H
#define XLOG_H

#include "TThread.h"
#include <string.h>
#include <Windows.h>
#include <string>

using namespace std;

class XLog: public TThread
{
public:
  XLog(const wchar_t *p_logfilenamepath=NULL,int p_logqueue_size = 1024*1024,int p_flush_interval = 1000);
  virtual ~XLog();
  void Log(const void *data,int datasize);
  void LogStr(const char *str);
  void LogWarn(const wchar_t *data);
  void LogError(const wchar_t *data);
  void LogInfo(const wchar_t *data);
  void LogDebug(const wchar_t *data); 
protected:
  void Flush();
  void SwitchQueue();  
  void Run(); //override;    
private:
  //cannot clone
  XLog(const XLog &){};
  XLog& operator=(const XLog &) {};

  //structures
  void *m_prequeue_logical;
  void *m_savingqueue_logical;
  void *m_prequeue_real;
  void *m_savingqueue_real;
  RTL_CRITICAL_SECTION * m_swaplock;
  RTL_CRITICAL_SECTION * m_prequeuelock;
  int m_prequeue_size;
  int m_savingqueue_size;
  HANDLE m_file_handle;  
  PSYSTEMTIME m_file_date;
  PSYSTEMTIME m_file_date_tmp;
  wchar_t *m_computer_name;

  //parameters
  wstring m_logfilepath;
  int m_logqueue_size;
  int m_flush_interval;
public:
  char m_logfmt[8192];
  RTL_CRITICAL_SECTION *m_logfmt_lock;
};


const char* FormatLog(XLog *log,const char * str, ...);
#ifdef _DEBUG
#define LOG_ERR(logger,s) printf("ERR %s %s,%d %s\n",NowStr().c_str(),__FILE__,__LINE__,s);
#define LOG_WARN(logger,s) printf("WARN %s %s,%d %s\n",NowStr().c_str(),__FILE__,__LINE__,s);
#define LOG_INFO(logger,s) printf("INFO %s %s,%d %s\n",NowStr().c_str(),__FILE__,__LINE__,s);
#else
#define LOG_ERR(logger,s) if (logger) logger->LogStr(FormatLog(logger,"ERR %s %s,%d %s\n",NowStr().c_str(),__FILE__,__LINE__,s));
#define LOG_WARN(logger,s) if (logger) logger->LogStr(FormatLog(logger,"WARN %s %s,%d %s\n",NowStr().c_str(),__FILE__,__LINE__,s));
#define LOG_INFO(logger,s) if (logger) logger->LogStr(FormatLog(logger,"INFO %s %s,%d %s\n",NowStr().c_str(),__FILE__,__LINE__,s));
#endif

#endif