/*
常用函数
author fangyousong
create time 2013.7
history:


*/
#ifndef CSYSUTILS_H
#define CSYSUTILS_H

#include <Windows.h>
#include <string>
#include <vector>
#include <list>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

using namespace std;
#ifdef _WIN64
#define POINTER long long
#else
#define POINTER	long
#endif

#ifdef _DEBUG
#define DEBUGOUT(S) OutputDebugStringA(S)
#else
#define DEBUGOUT(S) //OutputDebugStringA(S)
#endif

struct Cfree
{
  void operator()(void *p){::free(p);}
};

#define FreeAndNULL(Ptr) void *p = Ptr; \
Ptr = NULL; \
free(p);

#define DeleteAndNULL(PtrObj) delete PtrObj; PtrObj = NULL;

void DeleteAndNULL_fn(void *ptr);

struct CharCmpI
{
  bool operator()(char a,char b){return (ChrCmpIA(a,b)==0);}
};

struct CharCmpIW
{
  bool operator()(wchar_t a,wchar_t b){return (ChrCmpIW(a,b)==0);}
};

//指针递增
inline void IncPtr(void **p, POINTER inc_bytes)
{
	*p = (void*) ((POINTER) *p + inc_bytes);
}

inline void *AddPtr(void *p, POINTER add_bytes)
{
	return (void*) ((POINTER) p + add_bytes);
}

//string转wstring
wstring string2wstring(const string &str,UINT codepage = 936);
//wstring转string
string wstring2string(const wstring &wstr,UINT codepage = 936);
//int转string
string IntToStr(const int i);
wstring IntToStrW(const int i);
//
std::string utf8toansi( const string &utf8 );
//
std::wstring utf8toutf16(const string &utf8);

//创建一个guid，以字符串形式返回
string CreateGUID();
wstring CreateGUIDW();

//去掉字符串str两端的空白字符
string trim(const string &str);
wstring trimW(const wstring &str);

//判断文件fn是否存在
bool FileExists(const string &fn);
bool FileExistsW(const wstring &fn);

//判断目录fn是否存在
bool DirectoryExists(const string &fn);
bool DirectoryExistsW(const wstring &fn);

//判断目录fn是否存在，若不存在则创建（会连其上层目录也一并递归创建）
bool ForceDirectory(const string &fn);
bool ForceDirectoryW(const wstring &fn);

//获取子目录列表
void GetSubDirs(const string &dir,vector<string> &lst,bool include_self);
void GetSubDirsW(const wstring &dir,vector<wstring> &lst,bool include_self);
//删除一个目录及其目录中的文件，ANoDeleFiles指示不删除的文件列表
void ClearDir(const string &ADir,const string &file_mask,  vector<string> *ANoDeleFiles,bool delete_dir,bool include_subdir=true);

//以yyyy-mm-dd hh:nn:ss的格式返回一个日期的字符串格式
string FormatDatetime(const SYSTEMTIME &st);
wstring FormatDatetimeW(const SYSTEMTIME &st);

//以yyyy-mm-dd的格式返回一个日期的字符串格式
string FormatDate(const SYSTEMTIME &st);
wstring FormatDateW(const SYSTEMTIME &st);

//以hh:nn:ss的格式返回一个日期的字符串格式
string FormatTime(const SYSTEMTIME &st);
wstring FormatTimeW(const SYSTEMTIME &st);

//YYYY-MM-DD HH:NN:SS
bool Str2Datetime(const string &str, SYSTEMTIME &ret);
bool IsLeapYear(WORD p_year);
bool IsValidateDate(SYSTEMTIME &dt);
bool IsValidateDateTime(SYSTEMTIME &dt);
int YearsBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
int MonthsBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
int DaysBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
int HoursBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
int MinutesBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
long long SecondsBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
void IncYear(SYSTEMTIME &dt,int years);
void IncMonth(SYSTEMTIME &dt,int months);
void IncDay(SYSTEMTIME &dt,int days);
void IncHour(SYSTEMTIME &dt,int hours);
void IncMinute(SYSTEMTIME &dt,int minutes);
void IncSecond(SYSTEMTIME &dt,int seconds);
int DayOfWeek(SYSTEMTIME &dt); //0 sunday 1 monday 2 tuesday ...6 saturday 
void IncWeek(SYSTEMTIME &dt,int weeks);
int WeeksBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then);
string NowStr();


//将字符串转换为大写形式
string UpperCase(const string &str);
wstring UpperCaseW(const wstring &str);

//将字符串转化为小写形式
string LowerCase(const string &str);
wstring LowerCaseW(const wstring &str);

//忽略大小写判断两个字符串是否相等
inline bool SameText(const string &a,const string &b){return (_strcmpi(a.c_str(),b.c_str()) == 0);}
inline bool SameTextW(const wstring &a,const wstring &b){return (StrCmpIW(a.c_str(),b.c_str()) == 0);}

//获取当前进程的命令行参数列表，其中lst[0]表示exe文件名，后续依次为各个参数
void GetCommandLineList(vector<string> &lst);

//获取当前进程命令行的参数个数，无参数是默认为1，也就是exe文件名本身
int ParamCount();

//获取当前进程的第i个参数，第0个参数是exe文件名
string ParamStr(size_t i);
wstring ParamStrW(size_t i);
bool ParamExists(const string &s);
bool ParamExists(const wstring &s);

//从全路径文件名filestr中分解出路径，IncludeBackslash表示是否在返回的路径后面加反斜杠\。
string ExtractFilePath(const string &filestr, bool IncludeBackslash);
wstring ExtractFilePathW(const wstring &filestr, bool IncludeBackslash);

//从全路径文件名filestr中分解出文件名
string ExtractFileName(const string &filestr);
wstring ExtractFileNameW(const wstring &filestr);

//给路径末尾加上反斜杠
string ExcludeTrailingPathDelimiter(const string &path);
wstring ExcludeTrailingPathDelimiterW(const wstring &path);

//去除路径末尾的反斜杠
string IncludeTrailingPathDelimiter(const string &path);
wstring IncludeTrailingPathDelimiterW(const wstring &path);

wstring ChangeFileExt(const wstring &file,const wstring &ext);

//分割字符串AString到字符串列表AStrings中，以ASplitStr作为分隔符
void SplitString(const string &AString,const string &ASplitStr,vector<string> &AStrings);
void SplitStringW(const wstring &AString,const wstring &ASplitStr,vector<wstring> &AStrings);
//分割字符串AString到ALeft和ARight中
void SplitString(const string &AString,const string &ASplitStr,string &ALeft,string &ARight);
void SplitStringW(const wstring &AString,const wstring &ASplitStr,wstring &ALeft,wstring &ARight);
//
string StringList2String(const vector<string> &AStringList,const string &AConnectStr);
wstring StringList2StringW(const vector<wstring> &AStringList,const wstring &AConnectStr);
string StringList2String(const list<string> &AStringList,const string &AConnectStr);
wstring StringList2StringW(const list<wstring> &AStringList,const wstring &AConnectStr);
//字符串替换
string& ReplaceString(string &s, const string &OldPattern,const string &NewPattern,bool bReplaceAll = true);
wstring& ReplaceStringW(wstring &s, const wstring &OldPattern,const wstring &NewPattern,bool bReplaceAll = true);
//字符串替换，忽略大小写
string& ReplaceStringI(string &s, const string &OldPattern,const string &NewPattern,bool bReplaceAll = true);
wstring& ReplaceStringIW(wstring &s, const wstring &OldPattern,const wstring &NewPattern,bool bReplaceAll = true);
string FillString(string s,size_t width,char fillChar,bool fillLeft);

//从单字节或utf8编码的文本文件AFilename中加载字符串
string LoadStringFromFile(const string &AFileName);
//从utf16编码格式文件加载文本
wstring LoadWStringFromFile(const wstring &AFileName);

//加载文件内容到buffer,调用者负责是否返回的内存
bool LoadFileToBuffer(const wstring &file,/*outer*/void **buf,/*outer*/DWORD &bufsize);
bool SaveBufferToFile(const wstring &file,const void *buf,DWORD bufsize);

//将字符串AString保存到单字节编码的文本文件AFilename
void SaveStringToFile(const string &AString, const string &AFileName, bool IncludeUTF8Head = false);
void SaveWStringToFile(const wstring &AString, const wstring &AFileName);

//从单字节编码的文本文件AFilename中加载字符串到AStrings中
void LoadStringsFromFile(vector<string> &AStrings, const string &AFileName);
void LoadStringsFromFileW(vector<wstring> &AStrings, const wstring &AFileName);

//将AStrings保存到单字节编码的文本文件AFilename中
void SaveStringsToFile(const vector<string> &AStrings, const string &AFileName);
void SaveStringsToFileW(const vector<wstring> &AStrings, const wstring &AFileName);

//獲取當前活動桌面的SessionID ！需要特權
DWORD GetActiveConsoleSessionId();
//获取当前活动桌面的Explorer进程的令牌
HANDLE GetCurrentExplorerToken(DWORD ADesiredAccess = TOKEN_QUERY);

//取得当前活动桌面用户名
string GetActiveConsoleUser(string &ADoMain);
wstring GetActiveConsoleUserW(wstring &ADoMain);

//取得当前活动桌面的环境变量%userprofile%
string GetActiveConsoleUserProfileDir();
wstring GetActiveConsoleUserProfileDirW();

wstring GetCurrentProcessProfileDirW();

//取得当前活动桌面的环境变量%appdata%
string GetActiveConsoleAppDataDir();
wstring GetActiveConsoleAppDataDirW();

//取得当前活动桌面的环境变量%localdata%
string GetActiveConsoleAppDataLocalDir();
wstring GetActiveConsoleAppDataLocalDirW();

string GetActiveConsoleAppDataLocalLowDir();
wstring GetActiveConsoleAppDataLocalLowDirW();

//取得当前活动桌面的用户SID
string GetCurrentUserSID();
wstring GetCurrentUserSIDW();


/*以当前活动桌面的用户创建进程
AExeFile: exe全路径文件名
AParams: 命令行参数
AHideConsole: 如果是控制台程序，是否隐藏控制台窗口
AWaitForEnd: 是否等待进程结束
AWaitTimeout: 若AWaitForEnd==true,那么AWaitTimeout设置等待超时时间，单位为毫秒 
*/
BOOL CreateProcessAsActiveConsoleUser(const string &AExeFile,const string &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut);
BOOL CreateProcessAsActiveConsoleUserW(const wstring &AExeFile,const wstring &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut);
BOOL CreateProcess1(const string &AExeFile,const string &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut);

//----------------------服务相关----------------------------
//服务控制类型：启动，停止，重新启动
enum TSvcCtrlCode{sccStart,sccStop,sccRestart};
//服务是否已经安装
bool SvcInstalled(const string &ASvcName);
//取得服务的Exe文件路径
string GetSvcFilePath(const string &ASvcName);
//服务的启动类型是否为自动启动
bool IsSvcAutoStart(const string &ASvcName);
//服务是否正在运行
bool IsSvcRunning(const string &ASvcName);
/*
安装/卸载服务
ASvcName: 服务名
ADisplayName: 显示名
AUninstall: 是否卸载服务
ASvcFile: 服务的exe文件路径
*/
void SvcInstall(const string &ASvcName,const string &ADisplayName,const string &ADescription, bool AUninstall,const string &ASvcFile);
/*
服务控制
ASvcName: 服务名
ACtrlCode: 控制码
ASvcManager: 服务管理器handle,可空
*/
void SvcControl(const string &ASvcName, TSvcCtrlCode ACtrlCode,SC_HANDLE ASvcManager = 0);
//设置服务的exe文件路径
void SetSvcFilePath(const string &ASvcName, const string &ASvcFileName);
//设置服务的启动类型为自动启动/手段启动
void SetSvcAutoStart(const string &ASvcName, bool AAutoStart);
//----------------------------------------------------------

/*
从Buffer中读取ini参数
AIniData: ini文件内容
ASection: 节点
AIdent: 参数名
ADefaultValue: 默认值
*/
string ReadBufferIni(const string &AIniData, const string &ASection, const string &AIdent, const string &ADefaultValue);

//判断字符串是否为UTF8编码格式
bool IsTextUTF8(const char *lpstrInputStream, int iLen);
//转换为UTF8
std::string ToUtf8(const wchar_t* buffer, int len);
std::string ToUtf8(const std::wstring& str);
//URL编码字符串
string URLEncoding(const string &msg);

void XorBorder(HWND AHwnd); //窗口画框线

//取窗口层级
int GetWindowLevel(HWND wnd); 

//根据进程id去进程exe路径
string GetProcessExeByPID(DWORD pid);

//取剪贴版的字符数据
string GetClipbrdDataAsString();
//设置剪贴板的值
void SetClipbrdDataAsString(const string &str);
//清除剪贴板
void ClearClipbrd();

//是否win64
BOOL IsWin64();

//取HOSTS文件的位置
string GetHOSTSFilePath();
wstring GetHOSTSFilePathW();

//进程是否存在
bool ProcessExists( DWORD pid,string &full_exe_path,string &proc_name );
bool ProcessExistsW( DWORD pid,wstring &full_exe_path,wstring &proc_name );

string FormatDataSize_(INT64 Bytes);

void ClearADSafeHosts(bool ACustomHost);

void ProcessMessages();

DWORD GetTrayIconDataSize();

string GetSysDisk(); //获取系统盘
void GetFixedDisks(vector<string> &dest); //获取逻辑盘列表
typedef void (*VoidFun)(void);

void GetActiveSessionProcess(vector<wstring> &procs);

int CompareVersion(const string &ver1,const string &ver2);
bool Simplematch(const string &afmt,const string& astr);

BOOL GetFileVersionEx_(const char *FileName, DWORD &MajorVer, DWORD &MinorVer, DWORD &ReleaseVer, DWORD &BuildVer );
string GetFileVersionEx_(const char *fn);
string Int2Hex( int val,int width);
bool GetGatewayAndIP(vector<string> &macs, vector<string> &ips,vector<string> &gates );
string GetRemoteMac(const string &remote_ip);
string GetMac();
string GetGateWayMac();
string GetMac(const string &split_str);
string GetGateWayMac(const string &split_str);
wstring GetMac(const wstring &split_str);
wstring GetGateWayMac(const wstring &split_str);

string FormatString(const char* str,...);
wstring FormatStringW(const wchar_t* str,...);
string FormatStringEx(size_t buf_size,const char* str,...);
wstring FormatStringWEx(size_t buf_size,const wchar_t* str,...);

string GetOSVersion();
bool IsWinVistaPlus();

//buffer转16进制
string BufToHex(unsigned char *buf,unsigned long bufsize);
//16进制转buffer
void HexToBuf(const string &strhex,
			 /*out*/unsigned char **buf,
			 /*out*/unsigned long &bufsize);

bool AppendBuf2File(const wstring file_name, const void *buf,size_t buf_size);

void CreateNULLDACL(SECURITY_ATTRIBUTES &lsa,SECURITY_DESCRIPTOR &lsd);

void rc4_crypt(unsigned char *key, unsigned long keyLen,unsigned char *data, unsigned long dataLen);
string rc4_string(string key,string data);

char *base64_encode(const char* data, int data_len,bool urlencode);
char *base64_decode(const char* data, int data_len,bool urlencode);
string base64_encode_string(string data,bool urlencode);
string base64_decode_string(string data,bool urlencode);

bool DnsResolve(string domain, vector<string> &ip);

class MemoryStream
{
public:
  enum SeekOrigin
  {
    soBegin,
    soEnd,
    soCurrent
  };
  MemoryStream();
  virtual ~MemoryStream();
  long Read(MemoryStream &dest, long bytes);
  long Read(void *dest, long bytes);
  bool Write(const MemoryStream &from,long bytes = -1);
  bool Write(const void *from,long bytes);
  long GetSize() const {return m_size;}
  long GetPos() const {return m_pos;}
  long GetCapacity() const {return m_capacity;}
  void* GetBuffer() const {return m_buffer;}
  void Seek(SeekOrigin so,long offset);
  void Clear();
  bool LoadFromFile(const wstring &file);
  bool SaveToFile(const wstring &file);  
  bool Expand(long new_capacity = -1);//-1 默认为当前容量的两倍
  void SetInitCapacity(long cc){m_init_capacity = cc;}
  void Empty();
  bool Shrink(long new_size); //
private:
  MemoryStream(const MemoryStream&);
  MemoryStream& operator=(const MemoryStream&);  
private:
  void *m_buffer;
  long m_size;
  long m_pos;
  long m_capacity;
  long m_init_capacity;
};

class ForwardBuffer
{
public:
    ForwardBuffer();
    virtual ~ForwardBuffer();

    POINTER Read(void *dest, POINTER bytes);
    POINTER ReadNP(void *dest, POINTER bytes);
    void Forward(POINTER bytes);
    bool Write(const void *from, POINTER bytes);
    POINTER GetSize(){
        return m_size;
    }
    POINTER GetCapacity(){
        return m_capacity;
    }
    void Grow(POINTER bytes);
    void Reset();
private:
    ForwardBuffer(const ForwardBuffer&);
    ForwardBuffer& operator=(const ForwardBuffer&);
private:
    void *m_buffer;
    POINTER m_size;
    POINTER m_pos;
    POINTER m_capacity;
    CRITICAL_SECTION *m_lock;
};


#endif