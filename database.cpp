#include <Windows.h>
#include "database.h"
#include "TThread.h"
#include <list>
#include "sqlite3.h"
#include <string>

using namespace std;

enum db_oper
{
  doSet, //key/value
  doGet, //key/value
  doDelete, //key/value
  doLoad, //load wstring table,split by \r\n
  doExecuteSQL, //execute sql
  doGetBLOBBySQL, //return first col, first row
  doGetTextBySQL, //return first col, first row
  doGetIntBySQL, //return first col, first row
  doGetFloatBySQL //return first col, first row
};

struct db_oper_info
{
  db_oper oper;
  wstring table;
  void *key;
  unsigned long keysize;
  void *value;
  unsigned long valuesize;
  void **returned_data;
  unsigned long *returned_datasize;
  HANDLE waitevent;
  wstring sql;  

  db_oper_info():key(NULL),keysize(0),value(NULL),valuesize(0),waitevent(NULL),returned_data(NULL),
    returned_datasize(0){}
};

list<db_oper_info*> *db_oper_queue = NULL;
RTL_CRITICAL_SECTION *db_lock = NULL;
HANDLE db_oper_notify_event = NULL;
HANDLE db_thrd_terminated_event = NULL; 

void pushOper(db_oper_info &oper)
{
  AutoCriticalSection auto_cri(*db_lock);
  db_oper_info *l_oper = new(db_oper_info);
  l_oper->oper = oper.oper;
  l_oper->table = oper.table;
  l_oper->key = oper.key;
  l_oper->keysize = oper.keysize;
  l_oper->value = oper.value;
  l_oper->valuesize = oper.valuesize;
  l_oper->waitevent = oper.waitevent;
  l_oper->returned_data = oper.returned_data;
  l_oper->returned_datasize = oper.returned_datasize;
  l_oper->sql = oper.sql;
  db_oper_queue->push_back(l_oper);
  SetEvent(db_oper_notify_event);
}
db_oper_info* popOper()
{
  AutoCriticalSection auto_cri(*db_lock);
  if (db_oper_queue->empty())
  {
    ResetEvent(db_oper_notify_event);
    return NULL;
  }
  else
  {
    db_oper_info *ret = db_oper_queue->front();
    db_oper_queue->pop_front();
    return ret;
  }
}
class DBThread: public TThread
{
public:
  void SetDBFile(char *db_file);
protected:
  void Run();
private:
  sqlite3 *m_db;  
  string m_db_file;
  bool TableExists(wstring tablename);
  void CreateTable(wstring tablename);
  void DoGet(db_oper_info &oper);
  void DoSet(db_oper_info &oper);
  void DoLoad(db_oper_info &oper);
  void DoDelete(db_oper_info &oper);
  void DoExecuteSQL(db_oper_info &oper); 
  void DoGetBLOBBySQL(db_oper_info &oper);
  void DoGetTextBySQL(db_oper_info &oper);
  void DoGetIntBySQL(db_oper_info &oper);
  void DoGetFloatBySQL(db_oper_info &oper);
  void DoOper(db_oper_info &oper);
};

DBThread *db_thrd = NULL;

void db_init(char *dbfile)
{
  db_lock = new RTL_CRITICAL_SECTION();
  InitializeCriticalSection(db_lock);
  db_oper_notify_event = CreateEvent(NULL,true,false,NULL);
  db_oper_queue = new list<db_oper_info*>();
  db_thrd_terminated_event = CreateEvent(NULL,true,false,NULL);
  db_thrd = new DBThread();  
  db_thrd->SetDBFile(dbfile);
  db_thrd->Start();
}

void db_fina()
{
  db_thrd->SetTerminated(true);
  SetEvent(db_oper_notify_event);
  WaitForSingleObject(db_thrd_terminated_event,INFINITE);
  delete db_thrd;
  CloseHandle(db_thrd_terminated_event);
  for(list<db_oper_info*>::iterator it = db_oper_queue->begin(); it != db_oper_queue->end(); ++it)
  {
    if((*it)->waitevent)
      SetEvent((*it)->waitevent);
    delete (*it);
  }
  db_oper_queue->clear();
  delete db_oper_queue;
  CloseHandle(db_oper_notify_event);
  DeleteCriticalSection(db_lock);
  delete db_lock;  
}

void db_SetData( const wstring &tbl, const void *key, unsigned long keysize, const void *value, unsigned long valuesize,
  bool wait_until_return)
{
  if (tbl.empty() || key == NULL || keysize == 0)
    return;
  HANDLE hWait = NULL;
  if (wait_until_return)
    hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  info.oper = doSet;
  info.table = tbl;
  info.key = const_cast<void*>(key);
  info.keysize = keysize;
  info.value = const_cast<void*>(value);
  info.valuesize = valuesize;
  info.waitevent = hWait;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
}

void db_GetData(const wstring &tbl, const void *key, unsigned long keysize, /*out*/void **value,
                /*out*/unsigned long &valuesize,bool wait_until_return )
{
  if (tbl.empty() || key == NULL || keysize == 0 || value == NULL)
    return;
  HANDLE hWait = NULL;
  if (wait_until_return)
    hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  info.oper = doGet;
  info.table = tbl;
  info.key = const_cast<void*>(key);
  info.keysize = keysize;
  info.value = value;
  info.valuesize = valuesize;
  info.waitevent = hWait;
  info.returned_data = value;
  info.returned_datasize = &valuesize;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
}

void db_DeleteData( const wstring &tbl, const void *key, unsigned long keysize,
                   bool wait_until_return)
{
  if (tbl.empty())
    return;
  HANDLE hWait = NULL;
  if (wait_until_return)
    hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  info.oper = doDelete;
  info.table = tbl;
  info.key = const_cast<void*>(key);
  info.keysize = keysize;
  info.waitevent = hWait;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
}

void db_LoadData( const wstring &tbl, /*out*/wchar_t **data, /*out*/unsigned long &datasize, bool wait_until_return /*= true*/ )
{
  if (tbl.empty() || data == NULL)
    return;
  HANDLE hWait = NULL;
  if (wait_until_return)
    hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  info.oper = doLoad;
  info.table = tbl;
  info.returned_data = (void**)data;
  info.returned_datasize = &datasize;
  info.waitevent = hWait;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
}

std::string Buf2Hex( unsigned char *buf,unsigned long bufsize )
{
  if (NULL == buf || bufsize == 0)
    return "";
  char *ret = (char*)malloc(bufsize*2);
  if (!ret)
    return "";
  char m[17] = "0123456789ABCDEF";
  unsigned char L,R;
  int j = 0;
  for(unsigned long i=0;i!=bufsize;++i)
  {
    L = (unsigned char)buf[i];
    L >>= 4;
    R = (unsigned char)buf[i];
    R <<= 4;
    R >>= 4;
    ret[j] = m[L];
    ret[j+1] = m[R];
    j+=2;
  }
  string sret;
  sret.assign(ret,bufsize*2);
  free(ret);
  return sret;
}

std::wstring str2wstr( const string &str,UINT codepage )
{
  int iULen = MultiByteToWideChar(codepage,0,str.c_str(),-1,NULL,0);
  wchar_t *ustr = (wchar_t*)malloc((iULen)*sizeof(wchar_t));
  if (!ustr)
    return L"";
  memset(ustr,0,(iULen)*sizeof(wchar_t));
  MultiByteToWideChar(codepage,0,str.c_str(),-1,ustr,iULen);
  wstring ret;
  ret.assign(ustr);
  free(ustr);
  return ret;
}

std::string wstr2str( const wstring &wstr,UINT codepage )
{
  int iALen = WideCharToMultiByte(codepage,0,wstr.c_str(),-1,NULL,0,NULL,NULL);
  char *astr = (char*)malloc(iALen);
  if (!astr)
    return "";
  memset(astr,0,iALen);
  WideCharToMultiByte(codepage,0,wstr.c_str(),-1,astr,iALen,NULL,NULL);
  string ret;
  ret.assign(astr);
  free(astr);
  return ret;
}

void Hex2Buf( const string &strhex, /*out*/unsigned char **buf, /*out*/unsigned long &bufsize )
{

  if (strhex.length() < 2)
  {
    *buf = NULL;
    bufsize = 0;
    return;
  }
  const unsigned char tbl[256] =  
  {  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   255, 255,  
    255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 10,  11,  12,  13,  14,  15,  255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
    255, 255, 255, 255  
  };  
  *buf = (unsigned char*)malloc(strhex.length()/2);
  unsigned long j=0;
  for(unsigned long i=0; i < strhex.length()-1;i+=2)
  {
    unsigned char L,R;
    L = tbl[strhex[i]];
    L <<= 4;
    R = tbl[strhex[i+1]];
    (*buf)[j] = (L | R);
    ++j;
  }
  bufsize = strhex.length()/2;
}

void db_ExecuteSQL( const wstring &sql, bool wait_until_return /*= false*/ )
{
  if (sql.empty())
    return;
  HANDLE hWait = NULL;
  if (wait_until_return)
    hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  info.oper = doExecuteSQL;
  info.waitevent = hWait;
  info.sql = sql;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
}


void db_GetBlobBySQL( const wstring &sql, /*out*/void **data, /*out*/unsigned long &datasize, bool wait_until_return /*= true*/ )
{
  if (sql.empty())
    return;
  HANDLE hWait = NULL;
  if (wait_until_return)
    hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  info.oper = doGetBLOBBySQL;
  info.waitevent = hWait;
  info.returned_data = data;
  info.returned_datasize = &datasize;
  info.sql = sql;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
}

double db_GetFloatBySQL( const wstring &sql )
{
  if (sql.empty())
    return 0;
  HANDLE hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  double *data = NULL;
  unsigned long datasize = 0;
  info.oper = doGetFloatBySQL;
  info.waitevent = hWait;
  info.returned_data = (void**)(&data);
  info.returned_datasize = &datasize;
  info.sql = sql;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
  if (data)
  {
    double ret = *data;
    free(data);
    return ret;
  }
  else
    return 0;
}

long db_GetIntBySQL( const wstring &sql )
{
  if (sql.empty())
    return 0;
  HANDLE hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  long *data = NULL;
  unsigned long datasize = 0;
  info.oper = doGetIntBySQL;
  info.waitevent = hWait;
  info.returned_data = (void**)(&data);
  info.returned_datasize = &datasize;
  info.sql = sql;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
  if (data)
  {
    long ret = *data;
    free(data);
    return ret;
  }
  else
    return 0;
}

std::wstring db_GetTextBySQL( const wstring &sql )
{
  if (sql.empty())
    return L"";
  HANDLE hWait = CreateEvent(NULL,true,false,NULL);
  db_oper_info info;
  void *data = NULL;
  unsigned long datasize = 0;
  info.oper = doGetTextBySQL;
  info.waitevent = hWait;
  info.returned_data = &data;
  info.returned_datasize = &datasize;
  info.sql = sql;
  pushOper(info);
  if (hWait)
    WaitForSingleObject(hWait,INFINITE);
  if (data)
  {
    wstring ret;
    ret.assign((wchar_t*)data,datasize/2);
    free(data);
    return ret;
  }
  else
    return L"";
}


void DBThread::Run()
{
  int idbopen = sqlite3_open(m_db_file.c_str(),&m_db);
  if (SQLITE_OK != idbopen)
  {
    //error log
    return;
  }
  while (!GetTerminated())
  {
    db_oper_info *oper = popOper();
    if (NULL != oper)
    {
      DoOper(*oper);
      if (oper->waitevent)
        SetEvent(oper->waitevent);
      delete oper;
    }
    else
      WaitForSingleObject(db_oper_notify_event,INFINITE);
  }
  sqlite3_close(m_db);
  SetEvent(db_thrd_terminated_event);
}

void DBThread::DoGet(db_oper_info &oper)
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  CreateTable(oper.table);
  wstring sql;
  sql = sql + L"select fvalue from " + oper.table + L" where fkey = x\'"
    +str2wstr(Buf2Hex((unsigned char*)(oper.key),oper.keysize))
    + L"\'";
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  if (SQLITE_ROW == rc)
  {
    const void * blob = sqlite3_column_blob(stmt,0);
    int blobsize = sqlite3_column_bytes(stmt,0);
    if (blobsize >= 0)
      *(oper.returned_datasize) = (unsigned long)blobsize;
    if (blobsize > 0)
    {
      *(oper.returned_data) = malloc(blobsize);
      memcpy(*oper.returned_data,blob,*(oper.returned_datasize));
    }
  }
  sqlite3_finalize(stmt);
}
void DBThread::DoSet(db_oper_info &oper)
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  CreateTable(oper.table);
  wstring sql;
  sql = sql + L"select 1 from " + oper.table + L" where fkey = x\'"
    +str2wstr(Buf2Hex((unsigned char*)(oper.key),oper.keysize))
    + L"\'";
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  sql.clear();
  if (SQLITE_ROW == rc)
  {
    sql = sql + L"update " + oper.table + L" set fvalue = x\'"
      +str2wstr(Buf2Hex((unsigned char*)(oper.value),oper.valuesize))
      + L"\' where fkey = x\'"
      +str2wstr(Buf2Hex((unsigned char*)(oper.key),oper.keysize))
      + L"\'";
  }
  else
  {
    sql = sql + L"insert into " + oper.table + L" (fkey,fvalue) values (x\'"
      +str2wstr(Buf2Hex((unsigned char*)(oper.key),oper.keysize))
      +L"\',x\'"
      +str2wstr(Buf2Hex((unsigned char*)(oper.value),oper.valuesize))
      +L"\')";
  }
  rc = sqlite3_prepare16(m_db,sql.c_str(),sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}
void DBThread::DoLoad(db_oper_info &oper)
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  CreateTable(oper.table);
  wstring sql;    
  sql = sql + L"select fkey,fvalue from " + oper.table;
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  wstring ret;
  while (SQLITE_ROW == rc)
  {
    const void * blob = sqlite3_column_blob(stmt,0);
    int blobsize = sqlite3_column_bytes(stmt,0);      
    if (blobsize >= 0)
    {
      if (ret.empty())
        ret.assign((wchar_t*)blob,blobsize/2);
      else
      {
        ret.append(L"\r\n");
        ret.append((wchar_t*)blob,blobsize/2);
      }
    }
    rc = sqlite3_step(stmt);
  }
  sqlite3_finalize(stmt);
  if (ret.empty())
  {
    *oper.returned_data = NULL;
    *oper.returned_datasize = 0;
  }
  else
  {
    *oper.returned_data = malloc(ret.length()*2);
    memcpy(*oper.returned_data,ret.c_str(),ret.length()*2);
    *oper.returned_datasize = ret.length()*2;
  }    
    
}

void DBThread::DoDelete(db_oper_info &oper)
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  CreateTable(oper.table);
  wstring sql;
  sql = sql + L"delete from " + oper.table;
  if (oper.keysize > 0)
    sql = sql + L" where fkey = x\'"
    +str2wstr(Buf2Hex((unsigned char*)(oper.key),oper.keysize))
    + L"\'";
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}
void DBThread::DoExecuteSQL(db_oper_info &oper)
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  wstring sql;
  sql = oper.sql;
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);  
}

void DBThread::DoOper( db_oper_info &oper )
{
  if (doGet == oper.oper)
  {
    DoGet(oper);
  }
  else if (doSet == oper.oper)
  {
    DoSet(oper);
  }
  else if (doLoad == oper.oper)
  {
    DoLoad(oper);
  }
  else if (doDelete == oper.oper)
  {
    DoDelete(oper);
  }
  else if (doExecuteSQL == oper.oper)
  {
    DoExecuteSQL(oper);
  }
  else if (doGetBLOBBySQL == oper.oper)
  {
    DoGetBLOBBySQL(oper);
  }
  else if (doGetTextBySQL == oper.oper)
  {
    DoGetTextBySQL(oper);
  }
  else if (doGetIntBySQL == oper.oper)
  {
    DoGetIntBySQL(oper);
  }
  else if (doGetFloatBySQL == oper.oper)
  {
    DoGetFloatBySQL(oper);
  }
  return;
}

bool DBThread::TableExists( wstring tablename )
{  
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;
  wchar_t *sql = L"select name from sqlite_master where type = \'table\' and name = :tablename";
  rc = sqlite3_prepare16(m_db,sql,wcslen(sql)*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error;
    return false;
  }
  rc = sqlite3_bind_text16(stmt,1,tablename.c_str(),tablename.length()*sizeof(wchar_t),SQLITE_STATIC);
  if (SQLITE_OK != rc)
  {
    //log error;
    sqlite3_finalize(stmt);
    return false;
  }
  rc = sqlite3_step(stmt);
  bool ret = (rc == SQLITE_ROW);
  sqlite3_finalize(stmt);
  return ret;
}

void DBThread::CreateTable( wstring tablename )
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;
  if (!TableExists(tablename))
  {
    wstring s;
    s = s + L"create table " + tablename + L" (fkey blob primary key,fvalue blob)";
    rc = sqlite3_prepare16(m_db,s.c_str(),s.length()*2,&stmt,&tail);
    if (SQLITE_OK != rc)
    {
      //log error
      return;
    }
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return;
  }
}

void DBThread::SetDBFile( char *db_file )
{
  m_db_file.assign(db_file);
}

void DBThread::DoGetBLOBBySQL( db_oper_info &oper )
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  wstring sql;
  sql = oper.sql;
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  if (SQLITE_ROW == rc)
  {
    const void * blob = sqlite3_column_blob(stmt,0);
    int blobsize = sqlite3_column_bytes(stmt,0);
    if (blobsize >= 0)
      *(oper.returned_datasize) = (unsigned long)blobsize;
    if (blobsize > 0)
    {
      *(oper.returned_data) = malloc(blobsize);
      memcpy(*oper.returned_data,blob,*(oper.returned_datasize));
    }
  }
  sqlite3_finalize(stmt);
}

void DBThread::DoGetTextBySQL( db_oper_info &oper )
{
  int rc;
  sqlite3_stmt *stmt;
  const void *tail;  
  wstring sql;
  sql = oper.sql;
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  if (SQLITE_ROW == rc)
  {
    const void * blob = sqlite3_column_text16(stmt,0);
    int blobsize = sqlite3_column_bytes16(stmt,0);
    if (blobsize >= 0)
      *(oper.returned_datasize) = (unsigned long)blobsize;
    if (blobsize > 0)
    {
      *(oper.returned_data) = malloc(blobsize);
      memcpy(*oper.returned_data,blob,*(oper.returned_datasize));
    }
  }
  sqlite3_finalize(stmt);
}

void DBThread::DoGetIntBySQL( db_oper_info &oper )
{
  int rc;
  long ret = 0;
  sqlite3_stmt *stmt;
  const void *tail;  
  wstring sql;
  sql = oper.sql;
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  if (SQLITE_ROW == rc)
  {
    ret = sqlite3_column_int(stmt,0);    
    *(oper.returned_data) = malloc(sizeof(long));
    memcpy(*oper.returned_data,&ret,sizeof(long));    
  }
  sqlite3_finalize(stmt);
}

void DBThread::DoGetFloatBySQL( db_oper_info &oper )
{
  int rc;
  double ret = 0;
  sqlite3_stmt *stmt;
  const void *tail;  
  wstring sql;
  sql = oper.sql;
  rc = sqlite3_prepare16(m_db,(const void*)sql.c_str(),(int)sql.length()*2,&stmt,&tail);
  if (SQLITE_OK != rc)
  {
    //log error
    return;
  }
  rc = sqlite3_step(stmt);
  if (SQLITE_ROW == rc)
  {
    ret = sqlite3_column_int(stmt,0);    
    *(oper.returned_data) = malloc(sizeof(double));
    memcpy(*oper.returned_data,&ret,sizeof(double));    
  }
  sqlite3_finalize(stmt);
}




