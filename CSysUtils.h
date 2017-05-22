/*
���ú���
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

//ָ�����
inline void IncPtr(void **p, POINTER inc_bytes)
{
	*p = (void*) ((POINTER) *p + inc_bytes);
}

inline void *AddPtr(void *p, POINTER add_bytes)
{
	return (void*) ((POINTER) p + add_bytes);
}

//stringתwstring
wstring string2wstring(const string &str,UINT codepage = 936);
//wstringתstring
string wstring2string(const wstring &wstr,UINT codepage = 936);
//intתstring
string IntToStr(const int i);
wstring IntToStrW(const int i);
//
std::string utf8toansi( const string &utf8 );
//
std::wstring utf8toutf16(const string &utf8);

//����һ��guid�����ַ�����ʽ����
string CreateGUID();
wstring CreateGUIDW();

//ȥ���ַ���str���˵Ŀհ��ַ�
string trim(const string &str);
wstring trimW(const wstring &str);

//�ж��ļ�fn�Ƿ����
bool FileExists(const string &fn);
bool FileExistsW(const wstring &fn);

//�ж�Ŀ¼fn�Ƿ����
bool DirectoryExists(const string &fn);
bool DirectoryExistsW(const wstring &fn);

//�ж�Ŀ¼fn�Ƿ���ڣ����������򴴽����������ϲ�Ŀ¼Ҳһ���ݹ鴴����
bool ForceDirectory(const string &fn);
bool ForceDirectoryW(const wstring &fn);

//��ȡ��Ŀ¼�б�
void GetSubDirs(const string &dir,vector<string> &lst,bool include_self);
void GetSubDirsW(const wstring &dir,vector<wstring> &lst,bool include_self);
//ɾ��һ��Ŀ¼����Ŀ¼�е��ļ���ANoDeleFilesָʾ��ɾ�����ļ��б�
void ClearDir(const string &ADir,const string &file_mask,  vector<string> *ANoDeleFiles,bool delete_dir,bool include_subdir=true);

//��yyyy-mm-dd hh:nn:ss�ĸ�ʽ����һ�����ڵ��ַ�����ʽ
string FormatDatetime(const SYSTEMTIME &st);
wstring FormatDatetimeW(const SYSTEMTIME &st);

//��yyyy-mm-dd�ĸ�ʽ����һ�����ڵ��ַ�����ʽ
string FormatDate(const SYSTEMTIME &st);
wstring FormatDateW(const SYSTEMTIME &st);

//��hh:nn:ss�ĸ�ʽ����һ�����ڵ��ַ�����ʽ
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


//���ַ���ת��Ϊ��д��ʽ
string UpperCase(const string &str);
wstring UpperCaseW(const wstring &str);

//���ַ���ת��ΪСд��ʽ
string LowerCase(const string &str);
wstring LowerCaseW(const wstring &str);

//���Դ�Сд�ж������ַ����Ƿ����
inline bool SameText(const string &a,const string &b){return (_strcmpi(a.c_str(),b.c_str()) == 0);}
inline bool SameTextW(const wstring &a,const wstring &b){return (StrCmpIW(a.c_str(),b.c_str()) == 0);}

//��ȡ��ǰ���̵������в����б�����lst[0]��ʾexe�ļ�������������Ϊ��������
void GetCommandLineList(vector<string> &lst);

//��ȡ��ǰ���������еĲ����������޲�����Ĭ��Ϊ1��Ҳ����exe�ļ�������
int ParamCount();

//��ȡ��ǰ���̵ĵ�i����������0��������exe�ļ���
string ParamStr(size_t i);
wstring ParamStrW(size_t i);
bool ParamExists(const string &s);
bool ParamExists(const wstring &s);

//��ȫ·���ļ���filestr�зֽ��·����IncludeBackslash��ʾ�Ƿ��ڷ��ص�·������ӷ�б��\��
string ExtractFilePath(const string &filestr, bool IncludeBackslash);
wstring ExtractFilePathW(const wstring &filestr, bool IncludeBackslash);

//��ȫ·���ļ���filestr�зֽ���ļ���
string ExtractFileName(const string &filestr);
wstring ExtractFileNameW(const wstring &filestr);

//��·��ĩβ���Ϸ�б��
string ExcludeTrailingPathDelimiter(const string &path);
wstring ExcludeTrailingPathDelimiterW(const wstring &path);

//ȥ��·��ĩβ�ķ�б��
string IncludeTrailingPathDelimiter(const string &path);
wstring IncludeTrailingPathDelimiterW(const wstring &path);

wstring ChangeFileExt(const wstring &file,const wstring &ext);

//�ָ��ַ���AString���ַ����б�AStrings�У���ASplitStr��Ϊ�ָ���
void SplitString(const string &AString,const string &ASplitStr,vector<string> &AStrings);
void SplitStringW(const wstring &AString,const wstring &ASplitStr,vector<wstring> &AStrings);
//�ָ��ַ���AString��ALeft��ARight��
void SplitString(const string &AString,const string &ASplitStr,string &ALeft,string &ARight);
void SplitStringW(const wstring &AString,const wstring &ASplitStr,wstring &ALeft,wstring &ARight);
//
string StringList2String(const vector<string> &AStringList,const string &AConnectStr);
wstring StringList2StringW(const vector<wstring> &AStringList,const wstring &AConnectStr);
string StringList2String(const list<string> &AStringList,const string &AConnectStr);
wstring StringList2StringW(const list<wstring> &AStringList,const wstring &AConnectStr);
//�ַ����滻
string& ReplaceString(string &s, const string &OldPattern,const string &NewPattern,bool bReplaceAll = true);
wstring& ReplaceStringW(wstring &s, const wstring &OldPattern,const wstring &NewPattern,bool bReplaceAll = true);
//�ַ����滻�����Դ�Сд
string& ReplaceStringI(string &s, const string &OldPattern,const string &NewPattern,bool bReplaceAll = true);
wstring& ReplaceStringIW(wstring &s, const wstring &OldPattern,const wstring &NewPattern,bool bReplaceAll = true);
string FillString(string s,size_t width,char fillChar,bool fillLeft);

//�ӵ��ֽڻ�utf8������ı��ļ�AFilename�м����ַ���
string LoadStringFromFile(const string &AFileName);
//��utf16�����ʽ�ļ������ı�
wstring LoadWStringFromFile(const wstring &AFileName);

//�����ļ����ݵ�buffer,�����߸����Ƿ񷵻ص��ڴ�
bool LoadFileToBuffer(const wstring &file,/*outer*/void **buf,/*outer*/DWORD &bufsize);
bool SaveBufferToFile(const wstring &file,const void *buf,DWORD bufsize);

//���ַ���AString���浽���ֽڱ�����ı��ļ�AFilename
void SaveStringToFile(const string &AString, const string &AFileName, bool IncludeUTF8Head = false);
void SaveWStringToFile(const wstring &AString, const wstring &AFileName);

//�ӵ��ֽڱ�����ı��ļ�AFilename�м����ַ�����AStrings��
void LoadStringsFromFile(vector<string> &AStrings, const string &AFileName);
void LoadStringsFromFileW(vector<wstring> &AStrings, const wstring &AFileName);

//��AStrings���浽���ֽڱ�����ı��ļ�AFilename��
void SaveStringsToFile(const vector<string> &AStrings, const string &AFileName);
void SaveStringsToFileW(const vector<wstring> &AStrings, const wstring &AFileName);

//�@ȡ��ǰ��������SessionID ����Ҫ�ؙ�
DWORD GetActiveConsoleSessionId();
//��ȡ��ǰ������Explorer���̵�����
HANDLE GetCurrentExplorerToken(DWORD ADesiredAccess = TOKEN_QUERY);

//ȡ�õ�ǰ������û���
string GetActiveConsoleUser(string &ADoMain);
wstring GetActiveConsoleUserW(wstring &ADoMain);

//ȡ�õ�ǰ�����Ļ�������%userprofile%
string GetActiveConsoleUserProfileDir();
wstring GetActiveConsoleUserProfileDirW();

wstring GetCurrentProcessProfileDirW();

//ȡ�õ�ǰ�����Ļ�������%appdata%
string GetActiveConsoleAppDataDir();
wstring GetActiveConsoleAppDataDirW();

//ȡ�õ�ǰ�����Ļ�������%localdata%
string GetActiveConsoleAppDataLocalDir();
wstring GetActiveConsoleAppDataLocalDirW();

string GetActiveConsoleAppDataLocalLowDir();
wstring GetActiveConsoleAppDataLocalLowDirW();

//ȡ�õ�ǰ�������û�SID
string GetCurrentUserSID();
wstring GetCurrentUserSIDW();


/*�Ե�ǰ�������û���������
AExeFile: exeȫ·���ļ���
AParams: �����в���
AHideConsole: ����ǿ���̨�����Ƿ����ؿ���̨����
AWaitForEnd: �Ƿ�ȴ����̽���
AWaitTimeout: ��AWaitForEnd==true,��ôAWaitTimeout���õȴ���ʱʱ�䣬��λΪ���� 
*/
BOOL CreateProcessAsActiveConsoleUser(const string &AExeFile,const string &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut);
BOOL CreateProcessAsActiveConsoleUserW(const wstring &AExeFile,const wstring &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut);
BOOL CreateProcess1(const string &AExeFile,const string &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut);

//----------------------�������----------------------------
//����������ͣ�������ֹͣ����������
enum TSvcCtrlCode{sccStart,sccStop,sccRestart};
//�����Ƿ��Ѿ���װ
bool SvcInstalled(const string &ASvcName);
//ȡ�÷����Exe�ļ�·��
string GetSvcFilePath(const string &ASvcName);
//��������������Ƿ�Ϊ�Զ�����
bool IsSvcAutoStart(const string &ASvcName);
//�����Ƿ���������
bool IsSvcRunning(const string &ASvcName);
/*
��װ/ж�ط���
ASvcName: ������
ADisplayName: ��ʾ��
AUninstall: �Ƿ�ж�ط���
ASvcFile: �����exe�ļ�·��
*/
void SvcInstall(const string &ASvcName,const string &ADisplayName,const string &ADescription, bool AUninstall,const string &ASvcFile);
/*
�������
ASvcName: ������
ACtrlCode: ������
ASvcManager: ���������handle,�ɿ�
*/
void SvcControl(const string &ASvcName, TSvcCtrlCode ACtrlCode,SC_HANDLE ASvcManager = 0);
//���÷����exe�ļ�·��
void SetSvcFilePath(const string &ASvcName, const string &ASvcFileName);
//���÷������������Ϊ�Զ�����/�ֶ�����
void SetSvcAutoStart(const string &ASvcName, bool AAutoStart);
//----------------------------------------------------------

/*
��Buffer�ж�ȡini����
AIniData: ini�ļ�����
ASection: �ڵ�
AIdent: ������
ADefaultValue: Ĭ��ֵ
*/
string ReadBufferIni(const string &AIniData, const string &ASection, const string &AIdent, const string &ADefaultValue);

//�ж��ַ����Ƿ�ΪUTF8�����ʽ
bool IsTextUTF8(const char *lpstrInputStream, int iLen);
//ת��ΪUTF8
std::string ToUtf8(const wchar_t* buffer, int len);
std::string ToUtf8(const std::wstring& str);
//URL�����ַ���
string URLEncoding(const string &msg);

void XorBorder(HWND AHwnd); //���ڻ�����

//ȡ���ڲ㼶
int GetWindowLevel(HWND wnd); 

//���ݽ���idȥ����exe·��
string GetProcessExeByPID(DWORD pid);

//ȡ��������ַ�����
string GetClipbrdDataAsString();
//���ü������ֵ
void SetClipbrdDataAsString(const string &str);
//���������
void ClearClipbrd();

//�Ƿ�win64
BOOL IsWin64();

//ȡHOSTS�ļ���λ��
string GetHOSTSFilePath();
wstring GetHOSTSFilePathW();

//�����Ƿ����
bool ProcessExists( DWORD pid,string &full_exe_path,string &proc_name );
bool ProcessExistsW( DWORD pid,wstring &full_exe_path,wstring &proc_name );

string FormatDataSize_(INT64 Bytes);

void ClearADSafeHosts(bool ACustomHost);

void ProcessMessages();

DWORD GetTrayIconDataSize();

string GetSysDisk(); //��ȡϵͳ��
void GetFixedDisks(vector<string> &dest); //��ȡ�߼����б�
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

//bufferת16����
string BufToHex(unsigned char *buf,unsigned long bufsize);
//16����תbuffer
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
  bool Expand(long new_capacity = -1);//-1 Ĭ��Ϊ��ǰ����������
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