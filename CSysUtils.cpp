/*
常用函数
author fangyousong
create time 2013.7
history:


*/

//#include "stdafx.h"

#include "CSysUtils.h"
#include <Shlwapi.h>
#include <Windows.h>
#include <WinBase.h>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <time.h>
#include <ObjBase.h>
#include <map>
#include <vector>
#include <list>
#include <TlHelp32.h>
#include <UserEnv.h>
#include <Sddl.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <WtsApi32.h>
#include "TThread.h"
#include "Iphlpapi.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "version.lib")
using namespace std;

wstring string2wstring(const string &str,UINT codepage)
{
  if (str.empty())
    return L"";
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

string wstring2string(const wstring &wstr,UINT codepage)
{
  if (wstr.empty())
    return "";
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

string IntToStr(const int i)
{
  char buf[16]={0};
  _itoa_s(i,buf,10);
  return string(buf);
}

string CreateGUID()
{
  GUID guid;
  CoCreateGuid(&guid);
  char sguid[64] = {0};
  sprintf_s(sguid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
    guid.Data1, guid.Data2, guid.Data3,
    guid.Data4[0], guid.Data4[1],
    guid.Data4[2], guid.Data4[3],
    guid.Data4[4], guid.Data4[5],
    guid.Data4[6], guid.Data4[7]);
  return string(sguid);
}

#pragma warning( push )
#pragma warning(disable:4018)
string trim(const string &str) 
{
  if (str=="")
    return str;
  string::size_type i=-1,j=-1;
  int k;
  for(k=0;k<str.length();++k)
  {
    if (unsigned(str[k])>unsigned(' '))
    {
      i = k;
      break;
    }    
  }
  for(k=str.length()-1;k>=0;--k)
  {
    if (unsigned(str[k])>unsigned(' '))
    {
      j = k;
      break;
    }    
  }
  i = (i==-1?0:i);
  j = (j==-1?str.length()-1:j);
  return str.substr(i,j-i+1);
}
#pragma warning( pop )

bool FileExists(const string &fn)
{
  WIN32_FIND_DATAA fd;
  HANDLE hFile = FindFirstFileA(fn.c_str(),&fd);
  return (hFile != INVALID_HANDLE_VALUE);
}

bool FileExistsW(const wstring &fn)
{
  WIN32_FIND_DATAW fd;
  HANDLE hFile = FindFirstFileW(fn.c_str(),&fd);
  return (hFile != INVALID_HANDLE_VALUE);
}

bool DirectoryExists(const string &fn)
{
//  return PathFileExistsA(fn.c_str());
  DWORD Code = GetFileAttributesA(fn.c_str());
  return (Code != INVALID_FILE_ATTRIBUTES) && ((FILE_ATTRIBUTE_DIRECTORY & Code) != 0);
}

bool DirectoryExistsW(const wstring &fn)
{
  //  return PathFileExistsA(fn.c_str());
  DWORD Code = GetFileAttributesW(fn.c_str());
  return (Code != INVALID_FILE_ATTRIBUTES) && ((FILE_ATTRIBUTE_DIRECTORY & Code) != 0);
}

bool ForceDirectory(const string &fn)
{
  bool ret = true;
  if (fn.empty())
    return false;
  string sdir = ExcludeTrailingPathDelimiter(fn);
  if (sdir.length() < 3 || DirectoryExists(sdir) || (ExtractFilePath(sdir,true) == sdir))
    return ret;
  return ForceDirectory(ExtractFilePath(sdir,true)) && CreateDirectoryA(sdir.c_str(), NULL);
}

bool ForceDirectoryW(const wstring &fn)
{
  bool ret = true;
  if (fn.empty())
    return false;
  wstring sdir = ExcludeTrailingPathDelimiterW(fn);
  if (sdir.length() < 3 || DirectoryExistsW(sdir) || (ExtractFilePathW(sdir,true) == sdir))
    return ret;
  return ForceDirectoryW(ExtractFilePathW(sdir,true)) && CreateDirectoryW(sdir.c_str(), NULL);
}

string FormatDatetime(const SYSTEMTIME &st)
{
  char cst[30]={0};  
  sprintf_s(cst,"%4d-%02d-%02d %02d:%02d:%02d:%03d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
  return string(cst);
}

string FormatDate(const SYSTEMTIME &st)
{
  char cst[30]={0};  
  sprintf_s(cst,"%4d-%02d-%02d",st.wYear,st.wMonth,st.wDay);
  return string(cst);
}

string FormatTime(const SYSTEMTIME &st)
{
  char cst[30]={0};  
  sprintf_s(cst,"%02d:%02d:%02d:%03d",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
  return string(cst);
}

std::wstring FormatDatetimeW( const SYSTEMTIME &st )
{
  wchar_t cst[30]={0};  
  swprintf_s(cst,L"%4d-%02d-%02d %02d:%02d:%02d:%03d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
  return wstring(cst);
}

std::wstring FormatDateW( const SYSTEMTIME &st )
{
  wchar_t cst[30]={0};  
  swprintf_s(cst,L"%4d-%02d-%02d",st.wYear,st.wMonth,st.wDay);
  return wstring(cst);
}

std::wstring FormatTimeW( const SYSTEMTIME &st )
{
  wchar_t cst[30]={0};  
  swprintf_s(cst,L"%02d:%02d:%02d:%03d",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
  return wstring(cst);
}

string UpperCase(const string &str)
{
  string ret(str);
  if (ret.empty())
    return ret;
  transform(ret.begin(),ret.end(),ret.begin(),toupper);
  return ret;
}

wstring UpperCaseW(const wstring &str)
{
  wstring ret(str);
  if (ret.empty())
    return ret;
  transform(ret.begin(),ret.end(),ret.begin(),toupper);
  return ret;
}

string LowerCase(const string &str)
{
  string ret(str);
  if (ret.empty())
    return ret;
  transform(ret.begin(),ret.end(),ret.begin(),tolower);
  return ret;
}

wstring LowerCaseW(const wstring &str)
{
  wstring ret(str);
  if (ret.empty())
    return ret;
  transform(ret.begin(),ret.end(),ret.begin(),tolower);
  return ret;
}

void GetCommandLineList(vector<string> &lst)
{
  LPCSTR cstr = GetCommandLineA();    
  string s(cstr);  
  bool bSub = false; //是否参数内部引号
  string sSub = "";
  lst.clear();
  for(int i=0; i!=s.length(); ++i)
  {
    if ('\"' == s[i])
    {
      if (!bSub)
      {
        bSub = true;
        if (sSub!="")
        {
          lst.push_back(sSub);
          sSub = "";
        }
        continue;
      }
      else
      {
        bSub = false;
        lst.push_back(sSub);
        sSub = "";
      }
    }
    else if (' ' == s[i])
    {
      if (bSub)
      {
        sSub += s[i];
        continue;
      }
      else //not bsub
      {
        if (sSub!="")        
        {
          lst.push_back(sSub);                 
          sSub = "";
        }
        else
          continue;        
      }
    }
    else
      sSub += s[i];
  }
  if (sSub != "")
    lst.push_back(sSub);
}

int ParamCount()
{
  vector<string> lstCmdline;
  GetCommandLineList(lstCmdline);
  return lstCmdline.size();
}
string ParamStr(size_t i)
{
  vector<string> lstCmdline;
  GetCommandLineList(lstCmdline);
  if (i >= lstCmdline.size() || i<0)
    return "";
  else
    return lstCmdline[i];

  return "";  
}

wstring ParamStrW(size_t i)
{
  return string2wstring(ParamStr(i));
}

string ExtractFilePath(const string &filestr, bool IncludeBackslash)
{
  if (filestr=="")
    return "";
  for(int i = filestr.length()-1; i>=0; --i)
  {
    if (filestr[i] == '\\')
    {
      if (IncludeBackslash) 
        return filestr.substr(0,i+1);
      else
        return filestr.substr(0,i);
    }
  }
  return "";
}

wstring ExtractFilePathW(const wstring &filestr, bool IncludeBackslash)
{
  if (filestr.empty())
    return L"";
  for(int i = filestr.length()-1; i>=0; --i)
  {
    if (filestr[i] == L'\\')
    {
      if (IncludeBackslash) 
        return filestr.substr(0,i+1);
      else
        return filestr.substr(0,i);
    }
  }
  return L"";
}

string ExtractFileName(const string &filestr)
{
  if (filestr =="")
    return "";
  for(int i = filestr.length()-1; i>=0; --i)
  {
    if (filestr[i] == '\\')
    {
      return filestr.substr(i+1);
    }
  }
  return filestr;
}

wstring ExtractFileNameW(const wstring &filestr)
{
  if (filestr.empty())
    return L"";
  for(int i = filestr.length()-1; i>=0; --i)
  {
    if (filestr[i] == L'\\')
    {
      return filestr.substr(i+1);
    }
  }
  return L"";
}

string ExcludeTrailingPathDelimiter(const string &path)
{
  string s = trim(path);
  if (s.empty())
    return s;
  if (s[s.length()-1] == '\\')
    return s.substr(0,s.length()-1);
  else
    return s;
}

wstring ExcludeTrailingPathDelimiterW(const wstring &path)
{
  wstring s = trimW(path);
  if (s.empty())
    return s;
  if (s[s.length()-1] == L'\\')
    return s.substr(0,s.length()-1);
  else
    return s;
}

string IncludeTrailingPathDelimiter(const string &path)
{  
  string s = trim(path);
  if (s.empty())
    return s;
  if (s[s.length()-1] != '\\')
    return s+"\\";
  else
    return s;  
}

wstring IncludeTrailingPathDelimiterW(const wstring &path)
{
  wstring s = trimW(path);
  if (s.empty())
    return s;
  if (s[s.length()-1] != L'\\')
    return s+L"\\";
  else
    return s;  
}

void GetSubDirs(const string &dir,vector<string> &lst,bool include_self)
{
  string sDir,sFind,sFile;
  WIN32_FIND_DATAA fdata;
  HANDLE fhwnd;

  sDir = IncludeTrailingPathDelimiter(trim(dir));
  if (! DirectoryExists(sDir))
    return;
  if (include_self)
    lst.push_back(dir);
  sFind = sDir + "*.*";    
  memset(&fdata,0,sizeof(WIN32_FIND_DATAA));
  fhwnd = FindFirstFileA(sFind.c_str(),&fdata);
  if (fhwnd != INVALID_HANDLE_VALUE)
  {
    do
    {
      sFile.assign(fdata.cFileName,strlen(fdata.cFileName));
      if (!((sFile == ".") || (sFile == "..")))
      {
        sFile = sDir + sFile;
        if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
          lst.push_back(sFile);
          GetSubDirs(sFile,lst,false);            
        }
        else
        {          
        }
      }
      memset(&fdata,0,sizeof(WIN32_FIND_DATAA));
    } while (FindNextFileA(fhwnd,&fdata));
    FindClose(fhwnd);    
  }
  else
  {
  }

}

void GetSubDirsW(const wstring &dir,vector<wstring> &lst,bool include_self)
{
  wstring sDir,sFind,sFile;
  WIN32_FIND_DATAW fdata;
  HANDLE fhwnd;

  sDir = IncludeTrailingPathDelimiterW(trimW(dir));
  if (! DirectoryExistsW(sDir))
    return;
  if (include_self)
    lst.push_back(dir);
  sFind = sDir + L"*.*";    
  memset(&fdata,0,sizeof(WIN32_FIND_DATAW));
  fhwnd = FindFirstFileW(sFind.c_str(),&fdata);
  if (fhwnd != INVALID_HANDLE_VALUE)
  {
    do
    {
      sFile.assign(fdata.cFileName,wcslen(fdata.cFileName));
      if (!((sFile == L".") || (sFile == L"..")))
      {
        sFile = sDir + sFile;
        if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
          lst.push_back(sFile);
          GetSubDirsW(sFile,lst,false);            
        }
        else
        {          
        }
      }
      memset(&fdata,0,sizeof(WIN32_FIND_DATAA));
    } while (FindNextFileW(fhwnd,&fdata));
    FindClose(fhwnd);    
  }
  else
  {
  }

}

bool strcmp_1(const string &s1,const string &s2)
{
    return SameText(s1,s2);
}

void ClearDir(const string &ADir,const string &file_mask, vector<string> *ANoDeleFiles,bool delete_dir,bool include_subdir)
{	
  string sDir,sFind,sFile;
  WIN32_FIND_DATAA fdata;
  HANDLE fhwnd;

  sDir = IncludeTrailingPathDelimiter(trim(ADir));
  if(SameText(ADir,"c:\\windows\\")
	  || SameText(ADir,"c:\\windows\\system32\\")
    || SameText(ADir,"C:\\windows\\SysWOW64\\")
	  || SameText(ADir,"c:\\program files\\")
	  || SameText(ADir,"c:\\Program Files (x86)\\")
	  || SameText(ADir,"c:\\")
	  || SameText(ADir,"d:\\")
	  || SameText(ADir,"e:\\")
	  || SameText(ADir,"f:\\")
	  || SameText(ADir,"g:\\")
	  || SameText(ADir,"h:\\")
	  || SameText(ADir,"i:\\")
	  || SameText(ADir,"j:\\"))
  {
    if (delete_dir)
	    return;
  }
  if (! DirectoryExists(sDir))
    return;
  sFind = sDir + file_mask;    
  memset(&fdata,0,sizeof(WIN32_FIND_DATAA));
  fhwnd = FindFirstFileA(sFind.c_str(),&fdata);
  if (fhwnd != INVALID_HANDLE_VALUE)
  {
    do
    {
      sFile.assign(fdata.cFileName,strlen(fdata.cFileName));
      if (!((sFile == ".") || (sFile == "..")))
      {
        sFile = sDir + sFile;
        if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
          if (include_subdir)
            ClearDir(sFile,file_mask,ANoDeleFiles,delete_dir);
          if (delete_dir)
            RemoveDirectoryA(sFile.c_str());
        }
        else
        {
          if (ANoDeleFiles)
          {
          //  if (find_if(ANoDeleFiles->begin(),ANoDeleFiles->end(),sFile) == ANoDeleFiles->end())
              bool bretain = false;
              for(vector<string>::iterator it = ANoDeleFiles->begin(); it!=ANoDeleFiles->end(); ++it)
              {
                  if (SameText(*it,sFile))
                  {
                      bretain = true;
                      break;
                  }
              }
              if (!bretain)
                DeleteFileA(sFile.c_str());
          }
          else
            DeleteFileA(sFile.c_str());
        }
      }
      memset(&fdata,0,sizeof(WIN32_FIND_DATAA));
    } while (FindNextFileA(fhwnd,&fdata));
    FindClose(fhwnd);

    if (delete_dir)
      RemoveDirectoryA(sDir.c_str());
  }
  else
  {
  }
  
}

void SplitString(const string &AString,const string &ASplitStr,vector<string> &AStrings)
{
    if (ASplitStr.empty())
    {
        AStrings.clear();
        vector<string> v1,v2,v3;
        SplitString(AString,"\r\n",v1);
        for(vector<string>::iterator it = v1.begin(); it!=v1.end(); ++it)
        {
            v2.clear();
            SplitString(*it,"\r",v2);
            v3.insert(v3.end(),v2.begin(),v2.end());
        }
        for(vector<string>::iterator it = v3.begin(); it!=v3.end(); ++it)
        {
            v2.clear();
            SplitString(*it,"\n",v2);
            AStrings.insert(AStrings.end(),v2.begin(),v2.end());
        }
        return;
    }
    else
    {
        const char *sdata = AString.c_str();
        const char *ssplit = ASplitStr.c_str();
        DWORD i = 0,j = 0,iSizeA = AString.length(),iSizeB = ASplitStr.length();  
        AStrings.clear();
        while(i < iSizeA)
        {     
            if (i + iSizeB > iSizeA) 
            {
                AStrings.push_back(AString.substr(j));
                j = i;
                break;
            }
            if (memcmp(&(sdata[i]),ssplit,iSizeB) == 0)
            {
                //if (j < i)
                AStrings.push_back(AString.substr(j,i-j));        
                j = i+iSizeB;
                i += iSizeB;      
            }
            else
                ++i;    
        }
        if (j < i)
            AStrings.push_back(AString.substr(j));
        return;
    }
}


void SplitStringW(const wstring &str,const wstring &splitstr,vector<wstring> &dest)
{
    if (splitstr.empty())
    {
        dest.clear();
        vector<wstring> v1,v2,v3;
        SplitStringW(str,L"\r\n",v1);
        for(vector<wstring>::iterator it = v1.begin();it != v1.end(); ++it)
        {
            v2.clear();
            SplitStringW(*it,L"\r",v2);
            v3.insert(v3.end(),v2.begin(),v2.end());
        }
        for(vector<wstring>::iterator it = v3.begin();it != v3.end(); ++it)
        {
            v2.clear();
            SplitStringW(*it,L"\r",v2);
            dest.insert(dest.end(),v2.begin(),v2.end());
        }
    }
    else
    {    
        wstring::size_type pos = wstring::npos;
        wstring::size_type offset = 0;
        dest.clear();
        while(1)
        {
            pos = str.find(splitstr.c_str(),offset);
            if (wstring::npos == pos)
                break;
            dest.push_back(str.substr(offset,pos-offset));
            offset = splitstr.length() + pos;

        }
        if (offset != str.length())
            dest.push_back(str.substr(offset));
    }
}

void SplitString(const string &AString,const string &ASplitStr,string &ALeft,string &ARight)
{
  vector<string> vs;
  SplitString(AString,ASplitStr,vs);
  if(vs.empty())
      return;
  ALeft = vs[0];
  ARight = "";
  if (vs.size() > 1)
  {
    for (size_t i = 1; i!=vs.size(); ++i)
    {
      if (ARight.empty())
        ARight = vs[i];
      else
        ARight = ARight + ASplitStr + vs[i];
    }    
  }
  else
    ARight = "";
  return;
}

void SplitStringW(const wstring &AString,const wstring &ASplitStr,wstring &ALeft,wstring &ARight)
{
  vector<wstring> vs;
  SplitStringW(AString,ASplitStr,vs);
  ALeft = vs[0];
  if (vs.size() > 1)
  {
    for (size_t i = 1; i!=vs.size(); ++i)
    {
      if (ARight.empty())
        ARight = vs[i];
      else
        ARight = ARight + ASplitStr + vs[i];
    }    
  }
  else
    ARight = L"";
  return;
}

string& ReplaceString(string &s, const string &OldPattern,const string &NewPattern,bool bReplaceAll)
{
  if (OldPattern==NewPattern)
    return s;
  string::size_type i;
  while (true)
  {
    i = s.find(OldPattern);
    if(i==string::npos)
      break;
    else
    {
      s = s.replace(i,OldPattern.length(),NewPattern);
      if(!bReplaceAll)
          break;
    }
  }
  return s;
}

wstring& ReplaceStringW(wstring &s, const wstring &OldPattern,const wstring &NewPattern,bool bReplaceAll)
{
  if (OldPattern==NewPattern)
    return s;
  wstring::size_type i;
  while (true)
  {
    i = s.find(OldPattern);
    if(i==wstring::npos)
      break;
    else
    {
      s = s.replace(i,OldPattern.length(),NewPattern);
      if(!bReplaceAll)
          break;
    }
  }
  return s;
}

string& ReplaceStringI(string &s, const string &OldPattern,const string &NewPattern,bool bReplaceAll)
{
  if (!lstrcmpiA(OldPattern.c_str(),NewPattern.c_str()))
    return s;
  string::iterator it,itend;
  do{    
    it = search(s.begin(),s.end(),OldPattern.begin(),OldPattern.end(),CharCmpI());
    if (it != s.end())
    {
      itend = it+OldPattern.length();
      s = s.replace(it,itend,NewPattern);
      if(!bReplaceAll)
          break;
    }
    else
      break;
  }
  while (true);
  return s;
}

wstring& ReplaceStringIW(wstring &s, const wstring &OldPattern,const wstring &NewPattern,bool bReplaceAll)
{
  if (!lstrcmpiW(OldPattern.c_str(),NewPattern.c_str()))
    return s;
  wstring::iterator it,itend;
  do{    
    it = search(s.begin(),s.end(),OldPattern.begin(),OldPattern.end(),CharCmpIW());
    if (it != s.end())
    {
      itend = it+OldPattern.length();
      s = s.replace(it,itend,NewPattern);
      if(!bReplaceAll)
          break;
    }
    else
      break;
  }
  while (true);
  return s;
}

void SaveStringToFile(const string &AString, const string &AFileName, bool IncludeUTF8Head)
{
  HANDLE h = CreateFileA(AFileName.c_str(),GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
  if (h)
  {
    DWORD dwWrited;
    if (IncludeUTF8Head)
    {
      unsigned char utf8head[3] = {0xEF,0xBB,0xBF};
      WriteFile(h,utf8head,3,&dwWrited,NULL);
    }
    WriteFile(h,AString.c_str(),AString.length(),&dwWrited,NULL);
    CloseHandle(h);
  }
  return;
}

void SaveWStringToFile(const wstring &AString, const wstring &AFileName)
{
  if (FileExistsW(AFileName))
    DeleteFileW(AFileName.c_str());
  HANDLE h = CreateFileW(AFileName.c_str(),GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
  if (h)
  {
    DWORD dwWrited;
    unsigned char head[2] = {0xFF,0xFE};
    SetFilePointer(h,0,0,FILE_BEGIN);
    WriteFile(h,head,2,&dwWrited,NULL);
    SetFilePointer(h,2,0,FILE_BEGIN);
    WriteFile(h,AString.c_str(),AString.length()*sizeof(wchar_t),&dwWrited,NULL);
    CloseHandle(h);
  }
  return;
}

void SaveStringsToFile(const vector<string> &AStrings, const string &AFileName)
{
  string data = "";
  for(vector<string>::const_iterator it = AStrings.begin(); it != AStrings.end(); ++it)
  {
    if (data.empty())
      data += *it;
    else
    {
      data += "\r\n";
      data += *it;
    }      
  }
  SaveStringToFile(data,AFileName);
  return;
}

void SaveStringsToFileW(const vector<wstring> &AStrings, const wstring &AFileName)
{
  wstring data = L"";
  for(vector<wstring>::const_iterator it = AStrings.begin(); it != AStrings.end(); ++it)
  {
    if (data.empty())
      data.append(*it);
    else
    {
      data.append(L"\r\n");
      data.append(*it);
    }      
  }
  SaveWStringToFile(data,AFileName);
  return;
}

string LoadStringFromFile(const string &AFileName)
{
  string ret = "";
  if (!FileExists(AFileName))
    return ret;
  HANDLE h = CreateFileA(AFileName.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if (h!=INVALID_HANDLE_VALUE)
  {
    DWORD iSize = GetFileSize(h,NULL);
    if (iSize!=INVALID_FILE_SIZE)
    {      
      SetFilePointer(h,0,NULL,FILE_BEGIN);
      char *data = (char*)malloc(iSize);
      DWORD iReaded;
      if (ReadFile(h,data,iSize,&iReaded,NULL))      
      {
        byte head[3]={0};
        memcpy(head,data,3);
        if(head[0]==0xEF && head[1]==0xBB && head[2]==0xBF) //处理UTF8文件头
        {          
          IncPtr((void**)(&data),3);          
          ret.assign(data,iSize-3);
          IncPtr((void**)(&data),-3);          
        }
        else if (head[0]==0xFF && head[1]==0xFE)
        {
          free(data);
          CloseHandle(h);
          return wstring2string(LoadWStringFromFile(string2wstring(AFileName)));
        }
        else
          ret.assign(data,iSize);
      }
      free(data);
    }       
    CloseHandle(h);
  }
  return ret;
}

void LoadStringsFromFile(vector<string> &AStrings, const string &AFileName)
{
  string s = LoadStringFromFile(AFileName);
  vector<string> sl1,sl2,sl3;
  SplitString(s,"\r\n",sl1);
  AStrings.clear();
  sl3.clear();
  for(size_t i = 0; i!=sl1.size();++i)
  {
    sl2.clear();
    SplitString(sl1[i],"\r",sl2);
    sl3.insert(sl3.end(),sl2.begin(),sl2.end());
  }
  for(size_t i = 0; i!=sl3.size();++i)
  {
    sl2.clear();
    SplitString(sl3[i],"\n",sl2);
    AStrings.insert(AStrings.end(),sl2.begin(),sl2.end());
  }
}

void LoadStringsFromFileW(vector<wstring> &AStrings, const wstring &AFileName)
{
  wstring s = LoadWStringFromFile(AFileName);
  vector<wstring> sl1,sl2,sl3;
  SplitStringW(s,L"\r\n",sl1);
  AStrings.clear();
  sl3.clear();
  for(size_t i = 0; i!=sl1.size();++i)
  {
    sl2.clear();
    SplitStringW(sl1[i],L"\r",sl2);
    sl3.insert(sl3.end(),sl2.begin(),sl2.end());
  }
  for(size_t i = 0; i!=sl3.size();++i)
  {
    sl2.clear();
    SplitStringW(sl3[i],L"\n",sl2);
    AStrings.insert(AStrings.end(),sl2.begin(),sl2.end());
  }
}

HANDLE GetCurrentExplorerToken(DWORD ADesiredAccess)
{
  HANDLE hToken,hProcess,h;
  PROCESSENTRY32 pe32;
  wstring s;
  DWORD iCurrentSession,iSession;
  HANDLE iRet = 0;
  PULONG prealsessionid = 0;
  iCurrentSession = GetActiveConsoleSessionId();
  DWORD dwBytes = 0;
  if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,WTS_CURRENT_SESSION,WTSSessionId,
    (LPWSTR*)(&prealsessionid),&dwBytes))
  {
    iCurrentSession = *prealsessionid;
    WTSFreeMemory(prealsessionid);
  }

  h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  if (h != INVALID_HANDLE_VALUE)
  {
    pe32.dwSize = sizeof(PROCESSENTRY32);
    memset(pe32.szExeFile,0,sizeof(pe32.szExeFile));
    if (Process32First(h,&pe32))
    {
      do
      {
        s.assign(pe32.szExeFile,wcslen(pe32.szExeFile));
        if (! StrCmpI(s.c_str(),L"explorer.exe"))
        {
          if (ProcessIdToSessionId(pe32.th32ProcessID,&iSession))
          {
            if (iSession == iCurrentSession)
            {
              hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,false,pe32.th32ProcessID);
              if (hProcess)
              {
                if (OpenProcessToken(hProcess,ADesiredAccess,&hToken))
                {
                  iRet = hToken;
                  CloseHandle(hProcess);
                  break;
                }
                CloseHandle(hProcess);
              }
            }
          }
        }
      } while (Process32Next(h,&pe32));
    }
  }  
  return iRet;
}

string GetActiveConsoleUser(string &ADoMain)
{
  HANDLE hToken;
  DWORD isizeptiUser,cbti;
  PTOKEN_USER ptiUser;
  DWORD iUserlen,iDomainLen;
  SID_NAME_USE nameuse;

  string sret = "";
  hToken = GetCurrentExplorerToken();
  if (!hToken)
    return sret;

  cbti = 0;
  GetTokenInformation(hToken,TokenUser,NULL,0,&cbti);
  if (!cbti)
  {
    CloseHandle(hToken);
    return sret;
  }
  ptiUser = (PTOKEN_USER)malloc(cbti);
  isizeptiUser = cbti;
  memset(ptiUser,0,cbti);
  if (! GetTokenInformation(hToken,TokenUser,ptiUser,cbti,&cbti))
  {
    CloseHandle(hToken);
    free(ptiUser);
    return sret;
  }    
  iUserlen = 128;
  iDomainLen = iUserlen;
  char sUser[128] = {0};
  char sDomain[128] = {0};
  if (! LookupAccountSidA(NULL,ptiUser->User.Sid,sUser,&iUserlen,sDomain,&iDomainLen,&nameuse))
  {
    CloseHandle(hToken);
    free(ptiUser);
    return sret;
  }
  sret = trim(sUser);
  ADoMain = trim(sDomain);
  CloseHandle(hToken);
  free(ptiUser);
  return sret;
}

wstring GetActiveConsoleUserW(wstring &ADoMain)
{
  HANDLE hToken;
  DWORD isizeptiUser,cbti;
  PTOKEN_USER ptiUser;
  DWORD iUserlen,iDomainLen;
  SID_NAME_USE nameuse;

  wstring sret = L"";
  hToken = GetCurrentExplorerToken();
  if (!hToken)
    return sret;

  cbti = 0;
  GetTokenInformation(hToken,TokenUser,NULL,0,&cbti);
  if (!cbti)
  {
    CloseHandle(hToken);
    return sret;
  }
  ptiUser = (PTOKEN_USER)malloc(cbti);
  isizeptiUser = cbti;
  memset(ptiUser,0,cbti);
  if (! GetTokenInformation(hToken,TokenUser,ptiUser,cbti,&cbti))
  {
    CloseHandle(hToken);
    free(ptiUser);
    return sret;
  }    
  iUserlen = 128;
  iDomainLen = iUserlen;
  wchar_t sUser[128] = {0};
  wchar_t sDomain[128] = {0};
  if (! LookupAccountSidW(NULL,ptiUser->User.Sid,sUser,&iUserlen,sDomain,&iDomainLen,&nameuse))
  {
    CloseHandle(hToken);
    free(ptiUser);
    return sret;
  }
  sret = trimW(sUser);
  ADoMain = trimW(sDomain);
  CloseHandle(hToken);
  free(ptiUser);
  return sret;
}

string GetActiveConsoleUserProfileDir()
{
  HANDLE hToken = GetCurrentExplorerToken();
  if (! hToken)
    return "";
  string sret = "";
  char sDir[MAX_PATH] = {0};
  DWORD iDirSize = MAX_PATH;
  if (GetUserProfileDirectoryA(hToken,sDir,&iDirSize))
    sret = trim(sDir);
  CloseHandle(hToken);
  return sret;
}

wstring GetActiveConsoleUserProfileDirW()
{
  HANDLE hToken = GetCurrentExplorerToken();
  if (! hToken)
    return L"";
  wstring sret = L"";
  wchar_t sDir[MAX_PATH] = {0};
  DWORD iDirSize = MAX_PATH;
  if (GetUserProfileDirectoryW(hToken,sDir,&iDirSize))
    sret = trimW(sDir);
  CloseHandle(hToken);
  return sret;
}

wstring GetCurrentProcessProfileDirW()
{
    HANDLE hToken;
    if(FALSE == OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken))
        return L"";
    wstring sret = L"";
    wchar_t sDir[MAX_PATH] = {0};
    DWORD iDirSize = MAX_PATH;
    if (GetUserProfileDirectoryW(hToken,sDir,&iDirSize))
        sret = trimW(sDir);
    CloseHandle(hToken);
    return sret;
}

string GetActiveConsoleAppDataDir()
{
  OSVERSIONINFO sp;
  string sUserProfileDir = GetActiveConsoleUserProfileDir();
  sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&sp);
  if (sp.dwMajorVersion >= 6)
    return ExcludeTrailingPathDelimiter(sUserProfileDir) + "\\AppData\\Roaming";
  else
    return ExcludeTrailingPathDelimiter(sUserProfileDir) + "\\Application Data";
}

wstring GetActiveConsoleAppDataDirW()
{
  OSVERSIONINFO sp;
  wstring sUserProfileDir = GetActiveConsoleUserProfileDirW();
  sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&sp);
  if (sp.dwMajorVersion >= 6)
    return ExcludeTrailingPathDelimiterW(sUserProfileDir) + L"\\AppData\\Roaming";
  else
    return ExcludeTrailingPathDelimiterW(sUserProfileDir) + L"\\Application Data";
}

string GetActiveConsoleAppDataLocalDir()
{
  OSVERSIONINFO sp;
  string sUserProfileDir = GetActiveConsoleUserProfileDir();
  sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&sp);
  if (sp.dwMajorVersion >= 6)
    return ExcludeTrailingPathDelimiter(sUserProfileDir) + "\\AppData\\Local";
  else
    return ExcludeTrailingPathDelimiter(sUserProfileDir) + "\\Local Settings\\Application Data";
}

string GetActiveConsoleAppDataLocalLowDir()
{
    OSVERSIONINFO sp;
    string sUserProfileDir = wstring2string(GetCurrentProcessProfileDirW());
    sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&sp);
    if (sp.dwMajorVersion >= 6)
        return ExcludeTrailingPathDelimiter(sUserProfileDir) + "\\AppData\\LocalLow";
    else
        return ExcludeTrailingPathDelimiter(sUserProfileDir) + "\\Local Settings\\Application Data";
}

wstring GetActiveConsoleAppDataLocalDirW()
{
  OSVERSIONINFO sp;
  wstring sUserProfileDir = GetActiveConsoleUserProfileDirW();
  sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&sp);
  if (sp.dwMajorVersion >= 6)
    return ExcludeTrailingPathDelimiterW(sUserProfileDir) + L"\\AppData\\Local";
  else
    return ExcludeTrailingPathDelimiterW(sUserProfileDir) + L"\\Local Settings\\Application Data";
}

wstring GetActiveConsoleAppDataLocalLowDirW()
{
    OSVERSIONINFO sp;
    wstring sUserProfileDir = GetCurrentProcessProfileDirW();
    sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&sp);
    if (sp.dwMajorVersion >= 6)
        return ExcludeTrailingPathDelimiterW(sUserProfileDir) + L"\\AppData\\LocalLow";
    else
        return ExcludeTrailingPathDelimiterW(sUserProfileDir) + L"\\Local Settings\\Application Data";
}

string GetCurrentUserSID()
{
  string sret = "";
  HANDLE hToken = GetCurrentExplorerToken();
  if (!hToken)
    return sret;

  DWORD dwLen,dwRet;
  char *pRet;

  GetTokenInformation(hToken,TokenUser,NULL,0,&dwRet);
  if (dwRet)
  {
    dwLen = dwRet;
    void *puserToken = malloc(dwLen);
    if (GetTokenInformation(hToken,TokenUser,puserToken,dwLen,&dwRet))
    {
      if (ConvertSidToStringSidA(((PTOKEN_USER)puserToken)->User.Sid,&pRet))
      {
        sret.assign(pRet,strlen(pRet));
        LocalFree(pRet);
      }
    }
    free(puserToken);
  }
  CloseHandle(hToken);
  return sret;
}

wstring GetCurrentUserSIDW()
{
  return string2wstring(GetCurrentUserSID());
}

BOOL CreateProcessAsActiveConsoleUser(const string &AExeFile,const string &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut)
{
  HANDLE hExplorerToken,hToken_Dup;
  LPVOID pEnv;
  DWORD dwCreateFlags;
  STARTUPINFOA startupInfo;
  PROCESS_INFORMATION procInfo;
  BOOL ret = false;
  if (! FileExists(AExeFile))
    return ret;
  hExplorerToken = GetCurrentExplorerToken(MAXIMUM_ALLOWED);
  if (! hExplorerToken)
    return ret;
  if (! DuplicateTokenEx(hExplorerToken,MAXIMUM_ALLOWED,NULL,SecurityImpersonation,TokenPrimary,&hToken_Dup))
  {
    CloseHandle(hExplorerToken);
    return ret;
  }
  dwCreateFlags = NORMAL_PRIORITY_CLASS;
  if (AHideConsole)
    dwCreateFlags = dwCreateFlags | CREATE_NO_WINDOW;
  pEnv = NULL;
  if (CreateEnvironmentBlock(&pEnv,hToken_Dup,true))
    dwCreateFlags |= CREATE_UNICODE_ENVIRONMENT;
  memset(&startupInfo,0,sizeof(STARTUPINFO));
  startupInfo.cb = sizeof(STARTUPINFO);
  startupInfo.lpReserved = NULL;
  startupInfo.lpDesktop = "winsta0\\default";
  startupInfo.dwFlags = 0;
  memset(&procInfo,0,sizeof(PROCESS_INFORMATION));
  string scmd = "\""+AExeFile+"\" "+AParams;
  char cmds[32768] = {0};  
  memcpy(cmds,scmd.c_str(),min(scmd.length(),32767));
  ret = CreateProcessAsUserA(hToken_Dup,
    AExeFile.c_str(),
    cmds,
    NULL,
    NULL,
    false,
    dwCreateFlags,
    pEnv,
    NULL,
    &startupInfo,
    &procInfo);
  if (ret && AWaitForEnd)
    WaitForSingleObject(procInfo.hProcess,AWaitTimeOut);
  if (pEnv != NULL)
    DestroyEnvironmentBlock(pEnv);
  CloseHandle(hToken_Dup);
  CloseHandle(hExplorerToken);
  if (ret)
    CloseHandle(procInfo.hProcess);
  return ret;
}

BOOL CreateProcessAsActiveConsoleUserW(const wstring &AExeFile,const wstring &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut)
{
  HANDLE hExplorerToken,hToken_Dup;
  LPVOID pEnv;
  DWORD dwCreateFlags;
  STARTUPINFOW startupInfo;
  PROCESS_INFORMATION procInfo;
  BOOL ret = false;
  if (! FileExistsW(AExeFile))
    return ret;
  hExplorerToken = GetCurrentExplorerToken(MAXIMUM_ALLOWED);
  if (! hExplorerToken)
    return ret;
  if (! DuplicateTokenEx(hExplorerToken,MAXIMUM_ALLOWED,NULL,SecurityImpersonation,TokenPrimary,&hToken_Dup))
  {
    CloseHandle(hExplorerToken);
    return ret;
  }
  dwCreateFlags = NORMAL_PRIORITY_CLASS;
  if (AHideConsole)
    dwCreateFlags = dwCreateFlags | CREATE_NO_WINDOW;
  pEnv = NULL;
  if (CreateEnvironmentBlock(&pEnv,hToken_Dup,true))
    dwCreateFlags |= CREATE_UNICODE_ENVIRONMENT;
  memset(&startupInfo,0,sizeof(STARTUPINFO));
  startupInfo.cb = sizeof(STARTUPINFO);
  startupInfo.lpReserved = NULL;
  startupInfo.lpDesktop = L"winsta0\\default";
  startupInfo.dwFlags = 0;
  memset(&procInfo,0,sizeof(PROCESS_INFORMATION));
  wstring scmd = L"\""+AExeFile+L"\" "+AParams;
  wchar_t cmds[32768] = {0};  
  memcpy(cmds,scmd.c_str(),min(scmd.length()*sizeof(wchar_t),32767));
  ret = CreateProcessAsUserW(hToken_Dup,
    AExeFile.c_str(),
    cmds,
    NULL,
    NULL,
    false,
    dwCreateFlags,
    pEnv,
    NULL,
    &startupInfo,
    &procInfo);
  if (ret && AWaitForEnd)
    WaitForSingleObject(procInfo.hProcess,AWaitTimeOut);
  if (pEnv != NULL)
    DestroyEnvironmentBlock(pEnv);
  CloseHandle(hToken_Dup);
  CloseHandle(hExplorerToken);
  if (ret)
    CloseHandle(procInfo.hProcess);
  return ret;
}

bool SvcInstalled(const string &ASvcName)
{
  SC_HANDLE hMgr,hSvc;
  bool ret = false;
  hMgr = OpenSCManagerA(NULL,NULL,GENERIC_READ);
  if (hMgr > 0)
  {
    hSvc = OpenServiceA(hMgr,ASvcName.c_str(),SERVICE_QUERY_CONFIG);
    ret = (hSvc > 0);
    if (ret)
      CloseServiceHandle(hSvc);
    CloseServiceHandle(hMgr);
  }
  return ret;
}
string GetSvcFilePath(const string &ASvcName)
{
  SC_HANDLE hMgr,hSvc;
  LPQUERY_SERVICE_CONFIGA svcConf;
  DWORD i;
  string ret = "";
  hMgr = OpenSCManagerA(NULL,NULL,GENERIC_READ);
  if (hMgr > 0)
  {
    hSvc = OpenServiceA(hMgr,ASvcName.c_str(),SERVICE_QUERY_CONFIG);
    if (hSvc > 0)
    {
      svcConf = (LPQUERY_SERVICE_CONFIGA)malloc(4096);
      if (QueryServiceConfigA(hSvc,svcConf,4096,&i))
        ret = svcConf->lpBinaryPathName;
      free(svcConf);
      CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hMgr);
  }  
  return ret;
}
bool IsSvcAutoStart(const string &ASvcName)
{
  SC_HANDLE hMgr,hSvc;
  LPQUERY_SERVICE_CONFIGA svcConf;
  DWORD i;
  bool ret = false;
  hMgr = OpenSCManagerA(NULL,NULL,GENERIC_READ);
  if (hMgr > 0)
  {
    hSvc = OpenServiceA(hMgr,ASvcName.c_str(),SERVICE_QUERY_CONFIG);
    if (hSvc > 0)
    {
      svcConf = (LPQUERY_SERVICE_CONFIGA)malloc(4096);
      if (QueryServiceConfigA(hSvc,svcConf,4096,&i))
        ret = (svcConf->dwStartType == SERVICE_AUTO_START);
      free(svcConf);
      CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hMgr);
  }  
  return ret;
}
bool IsSvcRunning(const string &ASvcName)
{
  SC_HANDLE hMgr,hSvc;
  SERVICE_STATUS svcStat;
  bool ret = false;
  hMgr = OpenSCManagerA(NULL,NULL,GENERIC_READ);
  if (hMgr > 0)
  {
    hSvc = OpenServiceA(hMgr,ASvcName.c_str(),SERVICE_QUERY_STATUS);
    if (hSvc > 0)
    {
      memset(&svcStat,0,sizeof(SERVICE_STATUS));
      if (QueryServiceStatus(hSvc,&svcStat))
        ret = (svcStat.dwCurrentState == SERVICE_RUNNING);
      CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hMgr);
  }  
  return ret;
}
void SvcInstall(const string &ASvcName,const string &ADisplayName,const string &ADescription, bool AUninstall,const string &ASvcFile)
{
  SC_HANDLE schSCManager,schService = 0;
  schSCManager = OpenSCManagerA(
    NULL, // local computer
    NULL, // ServicesActive database
    SC_MANAGER_ALL_ACCESS); // full access rights  
  if (schSCManager) 
  {
    if (! AUninstall)
    {
      schService = CreateServiceA(
        schSCManager, // SCM database
        ASvcName.c_str(), // name of service
        ADisplayName.c_str(), // service name to display
        SERVICE_ALL_ACCESS, // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_AUTO_START, // start type
        SERVICE_ERROR_NORMAL, // error control type
        ASvcFile.c_str(), // path to service's binary
        NULL, // no load ordering group
        NULL, // no tag identifier
        NULL, // no dependencies
        NULL, // LocalSystem account
        NULL); // no password
      if (schService)
      {
        SERVICE_DESCRIPTIONA _SVC_DESC;
        _SVC_DESC.lpDescription = (char*)ADescription.c_str();
        if (ChangeServiceConfig2A(schService, SERVICE_CONFIG_DESCRIPTION, &_SVC_DESC))
        {}
        else
        {}
      }
      else
      {}
    }
    else
    {
      schService = OpenServiceA(schSCManager, ASvcName.c_str(), SERVICE_ALL_ACCESS);
      if (schService)
        DeleteService(schService);
    }
  }
  else
  {}
  if (schService)
    CloseServiceHandle(schService);
  if (schSCManager)
    CloseServiceHandle(schSCManager);
}
void SvcControl(const string &ASvcName, TSvcCtrlCode ACtrlCode,SC_HANDLE ASvcManager)
{
  SC_HANDLE schSCManager,schService;
  SERVICE_STATUS schStatu, schStatu1;
  if (!ASvcManager)
    schSCManager = OpenSCManagerA(
    NULL, // local computer
    NULL, // ServicesActive database
    SC_MANAGER_ALL_ACCESS); // full access rights
  else
    schSCManager = ASvcManager;
  schService = 0;
  if (schSCManager)
  {
    schService = OpenServiceA(schSCManager, ASvcName.c_str(), SERVICE_ALL_ACCESS);
    if (schService)
    {
      if (QueryServiceStatus(schService, &schStatu))
      {
        switch (schStatu.dwCurrentState)
        {
        case SERVICE_RUNNING:
          {
            switch (ACtrlCode)
            {
            case sccStart: break;
            case sccStop:
              {
                if (ControlService(schService, SERVICE_CONTROL_STOP, &schStatu1))
                {}
                else
                {}
                break;
              }
            case sccRestart:
              {
                if (ControlService(schService, SERVICE_CONTROL_STOP, &schStatu1))
                {}
                else
                {}
                if (StartServiceA(schService, 0, NULL))
                {}
                else
                {}
                break;
              }                  
            }
            break;
          }        
        case SERVICE_STOPPED:
          {
            switch (ACtrlCode)
            {
            case sccStart:              
              {
                if (StartServiceA(schService, 0, NULL))
                {}
                else
                {}
                break;
              }
            case sccStop: break;
            case sccRestart:
              {
                if (StartServiceA(schService, 0,NULL))
                {}
                else
                {}
                break;
              }              
            }
            break;
          }
        }
      }
      else
      {}
    }
    else
    {}
  }
  else
  {}
  if (schService)
    CloseServiceHandle(schService);
  if ((ASvcManager == 0) && (schSCManager != 0))
    CloseServiceHandle(schSCManager);
}
void SetSvcFilePath(const string &ASvcName, const string &ASvcFileName)
{
  SC_HANDLE hMgr, hSvc;
  DWORD dwStartType;

  if (IsSvcAutoStart(ASvcName.c_str()))
    dwStartType = SERVICE_AUTO_START;
  else
    dwStartType = SERVICE_DEMAND_START;

  hMgr = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (hMgr > 0)
  {
    hSvc = OpenServiceA(hMgr, ASvcName.c_str(), SERVICE_ALL_ACCESS);
    if (hSvc > 0)
    {
      ChangeServiceConfigA(hSvc, SERVICE_WIN32_OWN_PROCESS, dwStartType, SERVICE_ERROR_NORMAL,
        ASvcFileName.c_str(), NULL, NULL, NULL, NULL, NULL, NULL);
      CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hMgr);
  }
}
void SetSvcAutoStart(const string &ASvcName, bool AAutoStart)
{
  SC_HANDLE hMgr, hSvc;
  DWORD dwStartType;

  if (AAutoStart)
    dwStartType = SERVICE_AUTO_START;
  else
    dwStartType = SERVICE_DEMAND_START;

  hMgr = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (hMgr > 0)
  {
    hSvc = OpenServiceA(hMgr, ASvcName.c_str(), SERVICE_ALL_ACCESS);
    if (hSvc > 0)
    {
      ChangeServiceConfigA(hSvc, SERVICE_WIN32_OWN_PROCESS, dwStartType, SERVICE_ERROR_NORMAL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hMgr);
  }
}

string ReadBufferIni(const string &AIniData, const string &ASection, const string &AIdent, const string &ADefaultValue)
{
  vector<string> sl;
  string sLeft, sRight;
  string ret = ADefaultValue;
  SplitString(AIniData,"",sl);

  for(vector<string>::size_type i = 0; i != sl.size(); ++i)
  {
    if (SameText("["+ASection+"]",sl[i]))
    {
      for(vector<string>::size_type j = i + 1; j != sl.size(); ++j)
      {
        if (sl[j][1] == '[')
          return ret;
        else
        {
          SplitString(sl[j],"=",sLeft,sRight);
          if (SameText(sLeft,AIdent))
            return sRight;
        }
      }
      return ret;
    }    
  }
  return ret;
}

bool IsTextUTF8(const char *lpstrInputStream, int iLen)
{
  int i;
  DWORD cOctets; // octets to go in this UTF-8 encoded character
  UCHAR chr;
  bool bAllAscii;

  cOctets = 0;
  bAllAscii = true;
  for (i = 0; i < iLen; ++i)
  {
    chr = lpstrInputStream[i];

    if ((chr & 0x80) != 0)
      bAllAscii = false;

    if (! cOctets)
    {
      //
      // 7 bit ascii after 7 bit ascii is just fine. Handle start of encoding case.
      //
      if (chr >= 0x80)
      {
        //
        // count of the leading 1 bits is the number of characters encoded
        //
        chr = chr * 2;
        cOctets = cOctets + 1;
        while ((chr & 0x80) != 0)
        {
          chr = chr * 2;
          cOctets = cOctets + 1;
        }

        cOctets = cOctets - 1; // count includes this character
        if (cOctets == 0)
        {
          return false;
        }
      }
    }
    else
    {
      // non-leading bytes must start as 10xxxxxx
      if ((chr & 0xC0) != 0x80)
      {
        return false;        
      }
      cOctets = cOctets - 1; // processed another octet in encoding
    }
  }

  //
  // End of text. Check for consistency.
  //

  if (cOctets) // anything left over at the end is an error
  {
    return false;
  }

  if (bAllAscii) // Not utf-8 if all ascii. Forces caller to use code pages for conversion
  {
    return false;
  }

  return true;
}

string URLEncoding(const string &msg)
{
  string::size_type i;
  string ret = "";
  for (i = 0; i < msg.length(); ++i)
  {
    if (msg[i] == ' ')
      ret += '+';
    else if ((msg[i] >= 'a' && msg[i] <= 'z') || (msg[i] >= 'Z' && msg[i] <= 'Z') || (msg[i] >= '0' && msg[i] <= '9'))
      ret += msg[i];
    else
    {
      char s[3] = {0};
      sprintf_s(s,3,"%02X",msg[i]);
      ret += "%";
      ret.append(s,2);
    }
  }
  return ret;
}

string StringList2String( const vector<string> &AStringList,const string &AConnectStr )
{
  string ret;
  for(vector<string>::const_iterator it = AStringList.begin(); it != AStringList.end();++it)
  {
    if (ret.empty())
      ret = *it;
    else
      ret = ret + AConnectStr + *it;
  }
  return ret;
}

wstring StringList2StringW( const vector<wstring> &AStringList,const wstring &AConnectStr )
{
  wstring ret;
  for(vector<wstring>::const_iterator it = AStringList.begin(); it != AStringList.end();++it)
  {
    if (ret.empty())
      ret = *it;
    else
      ret = ret + AConnectStr + *it;
  }
  return ret;
}

std::string StringList2String( const list<string> &AStringList,const string &AConnectStr )
{
  string ret;
  for(list<string>::const_iterator it = AStringList.begin(); it != AStringList.end();++it)
  {
    if (ret.empty())
      ret = *it;
    else
      ret = ret + AConnectStr + *it;
  }
  return ret;
}

std::string ToUtf8( const wchar_t* buffer, int len )
{
  int nChars = ::WideCharToMultiByte(
    CP_UTF8,
    0,
    buffer,
    len,
    NULL,
    0,
    NULL,
    NULL);
  if (nChars == 0) return "";

  string newbuffer;
  newbuffer.resize(nChars) ;
  ::WideCharToMultiByte(
    CP_UTF8,
    0,
    buffer,
    len,
    const_cast< char* >(newbuffer.c_str()),
    nChars,
    NULL,
    NULL); 

  return newbuffer;
}

std::string ToUtf8( const std::wstring& str )
{
  return ToUtf8(str.c_str(), (int)str.size());
}

void XorBorder(HWND AHwnd) //窗口画框线
{
  HPEN vhPen,vhOldPen;
  HDC vhdc;
  RECT vRect;
  if (AHwnd && IsWindow(AHwnd) && IsWindowVisible(AHwnd))
  {
    GetWindowRect(AHwnd, &vRect);
    InflateRect(&vRect, 1, 1);
    vhdc = GetWindowDC(GetDesktopWindow());
    vhPen = CreatePen(PS_SOLID, 3, 0x00000000);
    vhOldPen = (HPEN)SelectObject(vhdc, vhPen);
    SelectObject(vhdc, GetStockObject(NULL_BRUSH));
    SetROP2(vhdc, R2_NOT);
    Rectangle(vhdc, vRect.left, vRect.top, vRect.right, vRect.bottom);
    SelectObject(vhdc, vhOldPen);
    DeleteObject(vhPen);
    ReleaseDC(AHwnd, vhdc);
  };
}

int GetWindowLevel(HWND wnd)
{
  HWND hParent;
  int ret = -1;
  hParent = wnd;
  while (hParent != 0)
  {
    ++ret;
    hParent = GetAncestor(hParent, GA_PARENT);
  }
  return ret;
}


string GetProcessExeByPID(DWORD pid)
{
  HANDLE hProcess;
  char Buff[MAX_PATH] = {0};
  string ret = "";  
  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (hProcess)
  { 
    memset(Buff, 0,MAX_PATH);
    if (GetModuleFileNameExA(hProcess,NULL,Buff,MAX_PATH))
      //GetProcessImageFileNameA(hProcess, Buff, MAX_PATH);  
      ret.assign(Buff);
  }
  return ret;
}

string GetClipbrdDataAsString()
{
  HANDLE Data;
  if (!OpenClipboard(NULL))
    return "";
  string ret;
  Data = GetClipboardData(CF_TEXT);
  try
  {
    if (Data)
      ret = (char*)(GlobalLock(Data));
    else
      ret = "";
  }
  catch(...)
  {
    if (Data)
      GlobalUnlock(Data);
    CloseClipboard();
    return "";
  }  
  CloseClipboard();
  return ret;
}

void ClearClipbrd()
{
  if (!OpenClipboard(NULL))
    return;
  EmptyClipboard();
  CloseClipboard();
}

void SetClipbrdBuffer(WORD Format, void *Buffer, int Size)
{
  HANDLE Data;
  void * DataPtr;
  if (!OpenClipboard(NULL))
    return;
  try
  {
    Data = GlobalAlloc(GMEM_MOVEABLE+GMEM_DDESHARE, Size);
    try
    {
      DataPtr = GlobalLock(Data);
      try
      {
        memcpy(DataPtr,Buffer, Size);
        //ClearClipbrd();
        SetClipboardData(Format, Data);
        GlobalUnlock(Data);
      }
      catch(...)
      {
        GlobalUnlock(Data);
      }
      GlobalFree(Data);
    }  
    catch(...)
    {
      GlobalFree(Data);
    }
    CloseClipboard();
  }
  catch(...)
  {
    CloseClipboard();
  }
}
void SetClipbrdDataAsString( const string &str )
{
  SetClipbrdBuffer(CF_TEXT, (void*)(str.c_str()), str.length());
}

typedef BOOL (__stdcall *T_IsWow64Process)(HANDLE Handle, BOOL &res);
typedef void (__stdcall *T_GetNativeSystemInfo)(SYSTEM_INFO &lpSystemInfo);
BOOL IsWin64()
{  
  BOOL ret = FALSE;
  HMODULE Kernel32Handle;
  T_IsWow64Process IsWow64Process;
  T_GetNativeSystemInfo GetNativeSystemInfo;  
  BOOL isWoW64;
  SYSTEM_INFO SystemInfo;
  Kernel32Handle = GetModuleHandleA("KERNEL32.DLL");
  if (!Kernel32Handle)
    Kernel32Handle = LoadLibraryA("KERNEL32.DLL");
  if (Kernel32Handle)
  {
    IsWow64Process = (T_IsWow64Process)(GetProcAddress(Kernel32Handle, "IsWow64Process"));
    GetNativeSystemInfo = (T_GetNativeSystemInfo)(GetProcAddress(Kernel32Handle, "GetNativeSystemInfo"));
    if (IsWow64Process)
    {
      IsWow64Process(GetCurrentProcess(), isWoW64);
      ret = isWoW64 && GetNativeSystemInfo;
      if (ret) 
      {
        GetNativeSystemInfo(SystemInfo);
        ret = (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ||
          (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64);
      }
    }
  }
  return ret;
}

BOOL CreateProcess1( const string &AExeFile,const string &AParams,bool AHideConsole,bool AWaitForEnd,DWORD AWaitTimeOut )
{
  LPVOID pEnv;
  DWORD dwCreateFlags;
  STARTUPINFOA startupInfo;
  PROCESS_INFORMATION procInfo;
  BOOL ret = false;
  if (! FileExists(AExeFile))
    return ret;

  dwCreateFlags = NORMAL_PRIORITY_CLASS;
  if (AHideConsole)
    dwCreateFlags = dwCreateFlags | CREATE_NO_WINDOW;
  pEnv = NULL;
  dwCreateFlags |= CREATE_UNICODE_ENVIRONMENT;
  memset(&startupInfo,0,sizeof(STARTUPINFO));
  startupInfo.cb = sizeof(STARTUPINFO);
  startupInfo.lpReserved = NULL;
  startupInfo.lpDesktop = "winsta0\\default";
  startupInfo.dwFlags = 0;
  memset(&procInfo,0,sizeof(PROCESS_INFORMATION));
  string scmd = "\""+AExeFile+"\" "+AParams;
  char cmds[32768] = {0};  
  memcpy(cmds,scmd.c_str(),min(scmd.length(),32767));
  ret = CreateProcessA(
    AExeFile.c_str(),
    cmds,
    NULL,
    NULL,
    false,
    dwCreateFlags,
    pEnv,
    NULL,
    &startupInfo,
    &procInfo);
  if (ret && AWaitForEnd)
    WaitForSingleObject(procInfo.hProcess,AWaitTimeOut);
  if (pEnv != NULL)
    DestroyEnvironmentBlock(pEnv);
  if (ret)
    CloseHandle(procInfo.hProcess);
  return ret;
}

std::string GetHOSTSFilePath()
{
  string ret = "";
  HKEY hKey;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters",0,KEY_READ,&hKey) == ERROR_SUCCESS)
  {
    char rv[100] ={0};
    DWORD lptype = REG_SZ;
    DWORD lpsize = 100;
    if (ERROR_SUCCESS == RegQueryValueExA(hKey,"DataBasePath",0,&lptype,(LPBYTE)rv,&lpsize))
    {
      ret.assign(rv);
      char ev[100] = {0};      
      if (GetEnvironmentVariableA("systemroot",ev,100))
      {
        ret = ReplaceStringI(ret,"%systemroot%",ev);
      }

    }
    RegCloseKey(hKey);
  }
  return ret;
}

std::wstring GetHOSTSFilePathW()
{
  wstring ret = L"";
  HKEY hKey;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters",0,KEY_READ,&hKey) == ERROR_SUCCESS)
  {
    wchar_t rv[100] ={0};
    DWORD lptype = REG_SZ;
    DWORD lpsize = 100;
    if (ERROR_SUCCESS == RegQueryValueEx(hKey,L"DataBasePath",0,&lptype,(LPBYTE)rv,&lpsize))
    {
      ret.assign(rv);
      wchar_t ev[100] = {0};      
      if (GetEnvironmentVariableW(L"systemroot",ev,100))
      {
        ret = ReplaceStringIW(ret,L"%systemroot%",ev);
      }

    }
    RegCloseKey(hKey);
  }
  return ret;
}

bool GetIPAndDomain(string sHostItem, string &AIP, string &ADomain, string &AComment,char ACommentChar)
{
  int i, j;

  AIP = "";
  ADomain = "";
  AComment = "";
  for (i = 0; i!= sHostItem.length();++i)
  {  
    if (sHostItem[i] == ACommentChar)
    {
      for (j = i + 1; j!= sHostItem.length();++j)
        AComment += sHostItem[j];
      break;
    }
    if ((ADomain != "") || (sHostItem[i] != ' '))
      ADomain += sHostItem[i];
    else
      AIP += sHostItem[i];
  }
  AIP = trim(AIP);
  ADomain = trim(ADomain);
  AComment = LowerCase(trim(AComment));
  return (ADomain != "");
}

void ClearADSafeHosts(bool ACustomHost)
{
  string k, sip2, sdomain2, sComment, sDir, sHosts;
  vector<string> slHosts;
  int j;
  string sCmt;
  bool bModified = false;

  if (ACustomHost)
    sCmt = "adcust";
  else
    sCmt = "adsafe";
  sDir = GetHOSTSFilePath();
  if (!DirectoryExists(sDir))
    return;
  sHosts = IncludeTrailingPathDelimiter(sDir) + "HOSTS";
  if (FileExists(sHosts))
  {
    LoadStringsFromFile(slHosts,sHosts);

    for (j = slHosts.size()-1; j>=0;--j)
    {
      if (slHosts.size() == 0)
        break;
      k = trim(slHosts[j]);
      if ((k == "") || (k[1] == '#'))
        slHosts.erase(slHosts.begin()+j);
      if (! GetIPAndDomain(k, sip2, sdomain2, sComment, '#'))
        continue;
      if (sComment == sCmt)
      {
        slHosts.erase(slHosts.begin()+j);
        bModified = true;
      }
    }
    if (bModified)
      SaveStringsToFile(slHosts,sHosts);
  }
}

std::string utf8toansi( const string &utf8 )
{
  int iULen = MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,NULL,0);
  wchar_t *ustr = (wchar_t*)malloc((iULen)*sizeof(wchar_t));
  if (!ustr)
    return "";
  memset(ustr,0,(iULen)*sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,ustr,iULen);
  wstring ret;
  ret.assign(ustr);
  free(ustr);
  return wstring2string(ret);  
}

bool ProcessExists( DWORD pid,string &full_exe_path,string &proc_name )
{
  HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
  PROCESSENTRY32 pe32;
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;
  wstring procname,fullexepath;
  full_exe_path = "";
  proc_name = "";


  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  if (INVALID_HANDLE_VALUE == hProcessSnap)
    return false;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );        
    return( false );
  }

  do
  {
    if (pe32.th32ProcessID == pid)
    {      
      procname.assign(pe32.szExeFile);
      proc_name = wstring2string(procname);
      hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
      if (INVALID_HANDLE_VALUE == hModuleSnap)
      {
        full_exe_path = "";
        return true;
      }
      me32.dwSize = sizeof(MODULEENTRY32);
      if (!Module32First(hModuleSnap,&me32))
      {
        full_exe_path = "";
        CloseHandle(hModuleSnap);
        CloseHandle(hProcessSnap);
        return true;
      }
      do 
      {
        fullexepath.assign(me32.szExePath);
        full_exe_path = wstring2string(fullexepath);
        CloseHandle(hModuleSnap);
        CloseHandle(hProcessSnap);
        return true;
      } while (Module32Next(hModuleSnap,&me32));
    }
  }
  while(Process32Next(hProcessSnap,&pe32));
  CloseHandle(hProcessSnap);

	return false;
}

bool ProcessExistsW( DWORD pid,wstring &full_exe_path,wstring &proc_name )
{
  HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
  PROCESSENTRY32 pe32;
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;
  wstring procname,fullexepath;
  full_exe_path = L"";
  proc_name = L"";


  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  if (INVALID_HANDLE_VALUE == hProcessSnap)
    return false;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );        
    return( false );
  }

  do
  {
    if (pe32.th32ProcessID == pid)
    {      
      procname.assign(pe32.szExeFile);
      proc_name = procname;
      hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
      if (INVALID_HANDLE_VALUE == hModuleSnap)
      {
        full_exe_path = L"";
        return true;
      }
      me32.dwSize = sizeof(MODULEENTRY32);
      if (!Module32First(hModuleSnap,&me32))
      {
        full_exe_path = L"";
        CloseHandle(hModuleSnap);
        CloseHandle(hProcessSnap);
        return true;
      }
      do 
      {
        fullexepath.assign(me32.szExePath);
        full_exe_path = fullexepath;
        CloseHandle(hModuleSnap);
        CloseHandle(hProcessSnap);
        return true;
      } while (Module32Next(hModuleSnap,&me32));
    }
  }
  while(Process32Next(hProcessSnap,&pe32));
  CloseHandle(hProcessSnap);

  return false;
}

std::string FormatDataSize_( INT64 Bytes )
{
  INT64 k,m,g,b;
  if(Bytes == 0)
    return "0 Byte";
  char ret[100] = {0};
  string sret;  
  g = (Bytes / 1024 / 1024 / 1024);
  if (g)
  {
    m = (Bytes - 1024 * 1024 * 1024 * g);    
    m = (INT64)(m * 10.24L / 1024 / 1024 /1024);
    if (m>9)
    {
      ++g;
      m=0;
    }
    if (m>9) m = 9;
    sprintf_s(ret,100,"%lld.%lld GB",g,m);    
    //sret.assign(ret,100);
//    return trim(sret);
  }
  else 
  {
    m = Bytes / 1024 / 1024;
    if (m)
    {    
      k = (INT64)((Bytes - 1024 * 1024 * m) * 10.24L / 1024 /1024);
      if (k>9)
      {
        ++m;
        k=0;
      }
      if (k>9) k = 9;
      sprintf_s(ret,100,"%lld.%lld MB",m,k);  
      //sret.assign(ret,100);
      //return trim(sret);
    }
    else
    {
      k = Bytes / 1024;
      if (k)
      {      
        b = (INT64)((Bytes - 1024 * k) * 10.24L / 1024);
        if (b>9)
        {
          ++k;
          b=0;
        }
        if (b>9) b = 9;
        sprintf_s(ret,100,"%lld.%lld KB",k,b);
        //sret.assign(ret,100);
        //return trim(sret);
      }
      else
      {
        sprintf_s(ret,100,"%lld Bytes",Bytes);
        //sret.assign(ret,100);
        //return trim(sret);
      } 
    }
  }  
  sret.assign(ret);
  return sret;
}

std::wstring LoadWStringFromFile( const wstring &AFileName )
{
  wstring ret = L"";
  if (!FileExistsW(AFileName))
    return ret;
  HANDLE h = CreateFileW(AFileName.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if (h!=INVALID_HANDLE_VALUE)
  {
    DWORD iSize = GetFileSize(h,NULL);
    if (iSize!=INVALID_FILE_SIZE)
    {      
      SetFilePointer(h,0,NULL,FILE_BEGIN);
      wchar_t *data = (wchar_t*)malloc(iSize);
      DWORD iReaded;
      if (ReadFile(h,data,iSize,&iReaded,NULL))      
      {
        unsigned char head[2]={0};
        memcpy(head,data,2);
        if(head[0]==0xFF && head[1]==0xFE)//处理utf16文件头
        {          
          IncPtr((void**)(&data),2);
          ret.assign(data,(iSize-2)/2);
          IncPtr((void**)(&data),-2);
        }
        else
          ret.assign(data,iSize/2);
      }
      free(data);
    }       
    CloseHandle(h);
  }
  return ret;
}

void ProcessMessages()
{
  
  bool Unicode;
  bool Handled;
  bool MsgExists;
  MSG Msg;
  
  do 
  {
    if (PeekMessageA(&Msg, 0, 0, 0, PM_NOREMOVE))
    {
      Unicode = ((Msg.hwnd != 0) && IsWindowUnicode(Msg.hwnd));
      if (Unicode)
        MsgExists = (PeekMessageW(&Msg, 0, 0, 0, PM_REMOVE)==TRUE);
      else
        MsgExists = (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE)==TRUE);
      if (!MsgExists)
        return;
      if (Msg.message != WM_QUIT)
      {      
        Handled = false;        
        TranslateMessage(&Msg);
        if (Unicode)
          DispatchMessageW(&Msg);
        else
          DispatchMessage(&Msg);
      }
      else
      {
        PostMessageA(Msg.hwnd,Msg.message,Msg.wParam,Msg.lParam);
        return;
      }      
    }
    else
    {    
      return;
    }
  }while(1);
  
}

std::wstring utf8toutf16( const string &utf8 )
{
  int iULen = MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,NULL,0);
  wchar_t *ustr = (wchar_t*)malloc((iULen)*sizeof(wchar_t));
  if (!ustr)
    return L"";
  memset(ustr,0,(iULen)*sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,ustr,iULen);
  wstring ret;
  ret.assign(ustr);
  free(ustr);
  return ret;
}

std::wstring CreateGUIDW()
{
  return string2wstring(CreateGUID());
}

#pragma warning( push )
#pragma warning(disable:4018)
std::wstring trimW( const wstring &str )
{
  if (str.empty())
    return str;  
  string::size_type i=-1,j=-1;
  int k;
  for(k=0;k<str.length();++k)
  {
    if (unsigned(str[k])>unsigned(L' '))
    {
      i = k;
      break;
    }    
  }
  for(k=str.length()-1;k>=0;--k)
  {
    if (unsigned(str[k])>unsigned(L' '))
    {
      j = k;
      break;
    }    
  }
  i = (i==-1?0:i);
  j = (j==-1?str.length()-1:j);
  return str.substr(i,j-i+1);  
}
#pragma warning( pop )

DWORD GetTrayIconDataSize()
{
  HINSTANCE hinstDll;  
  DWORD ret = sizeof(NOTIFYICONDATA);

  /* For security purposes, LoadLibrary should be provided with a fully-qualified  
  path to the DLL. The lpszDllName variable should be tested to ensure that it  
  is a fully qualified path before it is used. */  
  hinstDll = LoadLibraryA("Shell32.dll");  

  if(hinstDll)  
  {  
    DLLGETVERSIONPROC pDllGetVersion;  
    pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");  

    /* Because some DLLs might not implement this function, you must test for  
    it explicitly. Depending on the particular DLL, the lack of a DllGetVersion  
    function can be a useful indicator of the version. */  

    if(pDllGetVersion)  
    {  
      DLLVERSIONINFO dvi;  
      HRESULT hr;  

      ZeroMemory(&dvi, sizeof(dvi));              
      dvi.cbSize = sizeof(dvi);  

      hr = (*pDllGetVersion)(&dvi);  
      if(SUCCEEDED(hr)) 
      {
        char ver[25] = {0};
        sprintf_s(ver,"%d.%d.%d",dvi.dwMajorVersion,dvi.dwMinorVersion,dvi.dwBuildNumber);
        string sver(ver);              
        if (sver >= "6.0.6")
          ret = sizeof(NOTIFYICONDATA);
        else if (sver >= "6.0")
          ret = NOTIFYICONDATA_V3_SIZE;
        else if (sver >= "5.0")
          ret = NOTIFYICONDATA_V2_SIZE;              
      }

    }  
    FreeLibrary(hinstDll);  
  }    
  return ret;
}

DWORD GetActiveConsoleSessionId()
{
  PWTS_SESSION_INFOA sinfo1,sinfo;
  DWORD dw = 0;
  DWORD ret = WTSGetActiveConsoleSessionId();
  if (WTSEnumerateSessionsA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sinfo1, &dw))
  {  
    sinfo = sinfo1;
    for (int i = 0; i!=dw;++i)
    {    
      if (sinfo->State = WTSActive)
      {
        ret = sinfo->SessionId;
        WTSFreeMemory(sinfo1);
        return ret;
      }
      IncPtr((void**)(&sinfo),sizeof(WTS_SESSION_INFOA));

    }
    WTSFreeMemory(sinfo1);
  }
  return ret;
}

std::string GetSysDisk()
{
  char buf[128] = {0};
  if (0==GetWindowsDirectoryA(buf,128))
    return "";
  else
  {
    for(size_t i = 0;i<128;++i)
    {
      if (0==buf[i])
        return "";
      if (':'==buf[i])
      {
        string s;
        s.assign(buf,i+1);
        return s;
      }
    }
  }
  return "";
}

void GetFixedDisks( vector<string> &dest )
{
  dest.clear();
  DWORD dwDriveStrLen = 0;
  dwDriveStrLen= GetLogicalDriveStrings(0,NULL);
  char *szDriveName = new char[dwDriveStrLen];
  char *pDriveName=NULL;
  if (GetLogicalDriveStringsA(dwDriveStrLen,szDriveName))
  {
    pDriveName = szDriveName;
    while (*pDriveName!=NULL)
    {      
      switch (GetDriveTypeA(pDriveName))
      {
      case DRIVE_UNKNOWN :
        break;
      case DRIVE_NO_ROOT_DIR:
        break;
      case DRIVE_REMOVABLE :
        break;
      case DRIVE_FIXED :
        dest.push_back(ExcludeTrailingPathDelimiter(pDriveName));
        break;
      case DRIVE_REMOTE:
        break;
      case DRIVE_CDROM:
        break;
      case DRIVE_RAMDISK:
        break;
      default:
        break;
      }

      pDriveName+=strlen(pDriveName)+1;
    }
  }
  delete [] szDriveName;
}

std::wstring IntToStrW( const int i )
{
  wchar_t buf[16]={0};
  _itow_s(i,buf,10);
  return wstring(buf);
}

std::wstring StringList2StringW( const list<wstring> &AStringList,const wstring &AConnectStr )
{
  wstring ret;
  for(list<wstring>::const_iterator it = AStringList.begin(); it != AStringList.end();++it)
  {
    if (ret.empty())
      ret = *it;
    else
      ret = ret + AConnectStr + *it;
  }
  return ret;
}

bool LoadFileToBuffer( const wstring &file,/*outer*/void **buf,/*outer*/DWORD &bufsize )
{
  if (!FileExistsW(file))
    return false;
  HANDLE h = CreateFileW(file.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if (h!=INVALID_HANDLE_VALUE)
  {
    AutoFuncClass auto_1(AutoFunc(CloseHandle,h));
    DWORD iSize = GetFileSize(h,NULL);
    if (iSize!=INVALID_FILE_SIZE)
    {      
      SetFilePointer(h,0,NULL,FILE_BEGIN);
      *buf = malloc(iSize);
      if (NULL == *buf)
        return false;
      DWORD iReaded = 0;
      if (ReadFile(h,*buf,iSize,&iReaded,NULL))      
      {
        bufsize = iSize;
        return true;        
      }
      else
        iReaded += 0;
    }           
  }
  return false;
}

bool SaveBufferToFile(const wstring &file,const void *buf,DWORD bufsize)
{
  if (NULL == buf || bufsize == 0)
    return false;
  if(FileExistsW(file))
      DeleteFileW(file.c_str());
  HANDLE h = CreateFileW(file.c_str(),GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
  if (h!=INVALID_HANDLE_VALUE)
  {
    AutoFuncClass auto_1(AutoFunc(CloseHandle,h));
    SetFilePointer(h,0,NULL,FILE_BEGIN);
    DWORD wted = 0;
    return (TRUE == WriteFile(h,buf,bufsize,&wted,NULL));
  }
  return false;
}

bool Str2Datetime( const string &str, SYSTEMTIME &ret )
{
  memset(&ret,0,sizeof(SYSTEMTIME));
  //YYYY-MM-DD HH:NN:SS:ZZZ
  if (str.length() < strlen("YYYY-MM-DD"))
    return false;
  string sY,sM,sD,sH,sN,sS,sZ;
  int iY=0,iM=0,iD=0,iH=0,iN=0,iS=0,iZ=0;
  sY = str.substr(0,4);
  iY = atoi(sY.c_str());
  if (0==iY)
    return false;
  sM = str.substr(5,2);
  iM = atoi(sM.c_str());
  if (0==iM || 12<iM)
    return false;
  sD = str.substr(8,2);
  iD = atoi(sD.c_str());
  if (0==iD || 31<iD)
    return false;
  if (str.length()>=13)
    sH = str.substr(11,2);  
  if (str.length()>=16)
    sN = str.substr(14,2);
  if (str.length()>=19)
    sS = str.substr(17,2);
  if (str.length()>=23)
    sZ = str.substr(20,3);
  iH = atoi(sH.c_str());
  if (iH>23) 
    iH = 0;
  iN = atoi(sN.c_str());
  if (iN>59)
    iN = 0;
  iS = atoi(sS.c_str());
  if (iS>59)
    iS = 0;
  iZ = atoi(sZ.c_str());
  if (iZ > 999)
    iZ = 0;

  ret.wYear = (WORD)iY;
  ret.wMonth = (WORD)iM;
  ret.wDay = (WORD)iD;
  ret.wHour = (WORD)iH;
  ret.wMinute = (WORD)iN;
  ret.wSecond = (WORD)iS;
  ret.wMilliseconds = (WORD)iZ;
  
  return true;
}

int DaysBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{  
  if (!IsValidateDate(p_now) || !IsValidateDate(p_then))
    return 0;
  SYSTEMTIME dtNow,dtThen;
  int x;
  if (p_now.wYear < p_then.wYear
    || (p_now.wYear == p_then.wYear && p_now.wMonth < p_then.wMonth)
    || (p_now.wYear == p_then.wYear && p_now.wMonth == p_then.wMonth && p_now.wDay < p_then.wDay)
    )
  {
    dtNow = p_now;
    dtThen = p_then;
    x = 1;
  }
  else
  {
    dtNow = p_then;
    dtThen = p_now;
    x = -1;
  }
  int ret = 0;
  while(true)
  {
    if (dtNow.wYear == dtThen.wYear && dtNow.wMonth == dtThen.wMonth && dtNow.wDay == dtThen.wDay)
      break;
    ++ret;
    ++(dtNow.wDay);

    if (dtNow.wMonth == 2)
    {
      if (dtNow.wDay>28 && !IsLeapYear(dtNow.wYear))
      {
        dtNow.wMonth = 3;
        dtNow.wDay = 1;
      }
      else if (dtNow.wDay>29)
      {
        dtNow.wMonth = 3;
        dtNow.wDay = 1;
      }
    }
    else if (dtNow.wDay > 30 && (dtNow.wMonth == 4 || dtNow.wMonth == 6 || dtNow.wMonth == 9 || dtNow.wMonth == 11))
    {
      ++(dtNow.wMonth);
      dtNow.wDay = 1;
    }
    else if (dtNow.wDay > 31 && (dtNow.wMonth == 1 || dtNow.wMonth == 3 || dtNow.wMonth == 5 || dtNow.wMonth == 7 || dtNow.wMonth == 8 || dtNow.wMonth == 10 || dtNow.wMonth == 12))
    {
      ++(dtNow.wMonth);
      dtNow.wDay = 1;
      if (dtNow.wMonth > 12)
      {
        ++(dtNow.wYear);
        dtNow.wMonth = 1;
      }
    }
  }
    
  return ret*x;
}

inline bool IsLeapYear( WORD p_year )
{
  return ((p_year % 400 == 0)||(p_year % 4 == 0))&&(p_year % 100 != 0);
}

inline int YearsBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{
  return p_then.wYear - p_now.wYear;
}

inline int MonthsBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{
  if (! IsValidateDate(p_now) || ! IsValidateDate(p_then))
    return 0;
  return YearsBetween(p_now,p_then)*12+(p_then.wMonth - p_now.wMonth);
}

inline bool IsValidateDate( SYSTEMTIME &dt )
{
 if (dt.wMonth > 12 || dt.wMonth == 0 
   || dt.wDay > 31 || dt.wDay == 0
   || (dt.wMonth == 2 && IsLeapYear(dt.wYear) && dt.wDay>29)
   || (dt.wMonth == 2 && (!IsLeapYear(dt.wYear)) && dt.wDay>28)
   || ((dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11) && dt.wDay > 30)
   )
   return false;
 return true;
}

int HoursBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{
  if (! IsValidateDateTime(p_now) || ! IsValidateDateTime(p_then))
    return 0;
  return DaysBetween(p_now,p_then)*24+(p_then.wHour-p_now.wHour);
}

int MinutesBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{
  if (! IsValidateDateTime(p_now) || ! IsValidateDateTime(p_then))
    return 0;
  return HoursBetween(p_now,p_then)*60+(p_then.wMinute-p_now.wMinute);
}

long long SecondsBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{
	return MinutesBetween(p_now,p_then)*60+(p_then.wSecond-p_now.wSecond);
}

inline void IncYear( SYSTEMTIME &dt,int years )
{
  dt.wYear += years;
}

//just internal use for IncMonth
void IncMonth_( SYSTEMTIME &dt,int months )
{
  int y = months / 12;
  int m = months % 12;
  IncYear(dt,y);
  while (m>0)
  {
    --m;
    ++(dt.wMonth);
    if (dt.wMonth > 12)
    {
      ++(dt.wYear);
      dt.wMonth = 1;
    }
  }
  if (dt.wMonth == 2)
  {
    if (dt.wDay > 29 && IsLeapYear(dt.wYear))
      dt.wDay = 29;
    if (dt.wDay > 28 && !IsLeapYear(dt.wYear))
      dt.wDay = 28;
  }
  else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
  {
    if (dt.wDay > 30)
      dt.wDay = 30;
  }
  
}

//just internal use for IncMonth
void DecMonth( SYSTEMTIME &dt,int months )
{  
  int ms = months * -1;
  int y = ms / 12;
  int m = ms % 12;
  IncYear(dt,y * -1);
  while (m>0)
  {
    --m;    
    --(dt.wMonth);    
    if (dt.wMonth < 1)
    {
      --(dt.wYear);
      dt.wMonth = 12;
    }
  }
  if (dt.wMonth == 2)
  {
    if (dt.wDay > 29 && IsLeapYear(dt.wYear))
      dt.wDay = 29;
    if (dt.wDay > 28 && !IsLeapYear(dt.wYear))
      dt.wDay = 28;
  }
  else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
  {
    if (dt.wDay > 30)
      dt.wDay = 30;
  }

}

void IncMonth( SYSTEMTIME &dt,int months )
{
  if (! IsValidateDate(dt))
    return;
  if (months>=0)
    IncMonth_(dt,months);
  else
    DecMonth(dt,months);
}

//just internal use for IncDay
void IncDay_( SYSTEMTIME &dt,int days )
{
  int d = days;
  while (d>0)
  {
    --d;
    ++(dt.wDay);
    if (dt.wMonth == 2)
    {
      if (dt.wDay > 29 && IsLeapYear(dt.wYear))
      {
        dt.wDay = 1;
        dt.wMonth = 3;
      }
      if (dt.wDay > 28 && !IsLeapYear(dt.wYear))
      {
        dt.wDay = 1;
        dt.wMonth = 3;
      }
    }
    else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
    {
      if (dt.wDay > 30)
      {
        dt.wDay = 1;
        ++(dt.wMonth);
      }
    }
    else
    {
      if (dt.wDay > 31)
      {
        dt.wDay = 1;
        ++(dt.wMonth);
      }
    }
    if (dt.wMonth > 12)
    {
      dt.wMonth = 1;
      ++(dt.wYear);
    }
  }
}

//just internal use for IncDay
void DecDay( SYSTEMTIME &dt,int days )
{
  int d = days;
  while (d<0)
  {
    ++d;    
    --(dt.wDay);    
    if (dt.wDay < 1)
    {
      --(dt.wMonth);      
      if (dt.wMonth < 1)
      {
        dt.wMonth = 12;
        --(dt.wYear);
      }
      if (dt.wMonth == 2)
      {
        if (IsLeapYear(dt.wYear))
          dt.wDay = 29;
        else
          dt.wDay = 28;
      }
      else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
      {
        dt.wDay = 30;
      }
      else
      {
        dt.wDay = 31;
      }
    }
  }
}

void IncDay( SYSTEMTIME &dt,int days )
{
  if (! IsValidateDate(dt))
    return;
  if (days >= 0)
    IncDay_(dt,days);
  else
    DecDay(dt,days);
}

void IncHour( SYSTEMTIME &dt,int hours )
{
  if (! IsValidateDateTime(dt))
    return;
  int d = hours / 24;
  int h = hours % 24;
  IncDay(dt,d);
  if (hours >= 0)
  {    
    if (dt.wHour+h > 23)
    {
      IncDay(dt,1);
      dt.wHour = (dt.wHour + h - 24);
    }
    else
      dt.wHour += h;
  }
  else
  {
    int i = dt.wHour;
    i += h;
    if (i < 0)
    {
      IncDay(dt,-1);
      dt.wHour = 24 + h;
    }
    else
      dt.wHour = i;
  }
}

void IncMinute( SYSTEMTIME &dt,int minutes )
{
  if (! IsValidateDateTime(dt))
    return;
  int h = minutes / 60;
  int m = minutes % 60;
  IncHour(dt,h);
  if (minutes >= 0)
  {
    if (dt.wMinute+m > 59)
    {
      IncHour(dt,1);
      dt.wMinute = (dt.wMinute + m - 60);
    }
    else
      dt.wMinute = dt.wMinute + m;
  }
  else
  {
    int i = dt.wMinute;
    i += m;
    if (i < 0)
    {
      IncHour(dt,-1);
      dt.wMinute = 60 + m;
    }
    else
      dt.wMinute = i;
  }
}

string NowStr()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return FormatDatetime(st);
}

void IncSecond( SYSTEMTIME &dt,int seconds )
{
  if (! IsValidateDateTime(dt))
    return;
  int m = seconds / 60;
  int s = seconds % 60;
  IncMinute(dt,m);
  if (seconds >= 0)
  {
    if (dt.wSecond+s > 59)
    {
      IncMinute(dt,1);
      dt.wSecond = (dt.wSecond + s - 60);
    }
    else
      dt.wSecond = dt.wSecond + s;
  }
  else
  {
    int i = dt.wSecond;
    i += s;
    if (i < 0)
    {
      IncHour(dt,-1);
      dt.wSecond = 60 + s;
    }
    else
      dt.wSecond = i;
  }
}

int DayOfWeek( SYSTEMTIME &dt )
{
  if (! IsValidateDate(dt))
   return -1;
  SYSTEMTIME st;
  GetLocalTime(&st);
  int ispan = DaysBetween(st,dt);
  int ispan1 = ispan % 7;
  if (ispan > 0)
  {
    dt.wDayOfWeek = st.wDayOfWeek + ispan1;
    if (dt.wDayOfWeek > 6)
      dt.wDayOfWeek -= 7;
  }
  else
  {
    int wd = st.wDayOfWeek;
    wd += ispan1;
    if (wd < 0)
      dt.wDayOfWeek = 7 + wd;
    else
      dt.wDayOfWeek = wd;    
  }
  return dt.wDayOfWeek;
}

void IncWeek( SYSTEMTIME &dt,int weeks )
{
  if(! IsValidateDate(dt))
    return;
  IncDay(dt,weeks*7);
}

int WeeksBetween( SYSTEMTIME &p_now, SYSTEMTIME &p_then )
{
  return DaysBetween(p_now,p_then) / 7;
}

bool IsValidateDateTime( SYSTEMTIME &dt )
{
  if (!IsValidateDate(dt))
    return false;
  return (dt.wHour >= 0 && dt.wHour<24 && dt.wMinute>=0 && dt.wMinute<60 && dt.wSecond>=0 && dt.wSecond<60 && dt.wMilliseconds>=0 && dt.wMilliseconds<1000);
}

void GetActiveSessionProcess( vector<wstring> &procs )
{
  procs.clear();
  HANDLE h;
  PROCESSENTRY32 pe32;
  wstring s;
  DWORD iCurrentSession,iSession;
  PULONG prealsessionid = 0;
  iCurrentSession = GetActiveConsoleSessionId();
  DWORD dwBytes = 0;
  if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,WTS_CURRENT_SESSION,WTSSessionId,
    (LPWSTR*)(&prealsessionid),&dwBytes))
  {
    iCurrentSession = *prealsessionid;
    WTSFreeMemory(prealsessionid);
  }

  h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  if (h != INVALID_HANDLE_VALUE)
  {
    AutoFuncClass auto1(AutoFunc(CloseHandle,h));
    pe32.dwSize = sizeof(PROCESSENTRY32);
    memset(pe32.szExeFile,0,sizeof(pe32.szExeFile));
    if (Process32First(h,&pe32))
    {
      do
      {        
        if (ProcessIdToSessionId(pe32.th32ProcessID,&iSession))
        {
          if (iSession == iCurrentSession)
          {
            s.assign(pe32.szExeFile,wcslen(pe32.szExeFile));
            procs.push_back(pe32.szExeFile);
          }
        }        
      } while (Process32Next(h,&pe32));
    }
  }    
}

std::wstring ChangeFileExt( const wstring &file,const wstring &ext )
{
  if (file.empty() || ext.empty())
    return file;
  for(int i = file.length()-1;i>=0;--i)
  {
    if(L'.' == file[i])
    {
      if (L'.' != ext[0])
        return file.substr(0,i)+L"."+ext;
      else
        return file.substr(0,i)+ext;
    }
  }
  if (L'.' != ext[0])
    return file + L"."+ext;
  else
    return file + ext;
}

void DeleteAndNULL_fn( void *ptr )
{
    delete ptr;
    ptr = NULL;
}

int CompareVersion( const string &ver1,const string &ver2 )
{
    if (ver1.empty() && ver2.empty())
        return 0;
    if (ver1.empty() && !ver2.empty())
        return -1;
    if (!ver1.empty() && ver2.empty())
        return 1;

    vector<string> v1,v2;
    SplitString(ver1,".",v1);
    SplitString(ver2,".",v2);
    for(size_t i = 0; i < v1.size(); ++i)
    {
        if(i>= v2.size())
            return 1;
        if(StrToIntA(v1[i].c_str()) < StrToIntA(v2[i].c_str()))
            return -1;
        if(StrToIntA(v1[i].c_str()) > StrToIntA(v2[i].c_str()))
            return 1;
    }
    if(v1.size()<v2.size())
        return -1;
    return 0;
}

//测试字符串匹配，只支持通配符*,表示0个或多个字符，忽略大小写
bool Simplematch(const string &afmt,const string& astr)
{
    string sfmt = LowerCase(afmt);
    string sstr = LowerCase(astr);
    if(sfmt == sstr)
        return true;
    const char* fmt = sfmt.c_str();
    const char* str = sstr.c_str();
    int fmt_len = sfmt.length(),str_len = sstr.length();
    int i = 0,j = 0;
    int ib,ie;
    bool bmatched;
    while(i<fmt_len && j < str_len)
    {
        if(fmt[i] == '*') //遇到*，取*后面的那段进行匹配
        {
            while(i<fmt_len && fmt[i] == '*')
                ++i;
            ib = i;
            while(i<fmt_len && fmt[i] != '*' )
                ++i;
            ie = i;
            if (fmt[ib] == fmt[ie])
                return true;
            i = ib;
            if (j+ie-ib >= str_len)
                return false;

            bmatched = false;
            while(str_len-j >= ie - ib)
            {
                if(memcmp(&fmt[ib],&str[j],ie-ib) == 0)
                {
                    j += ie-ib;
                    i = ie;
                    bmatched = true;
                    break;
                }
                else
                    ++j;                
            }
            if (!bmatched)
                return false;
        }
        else
        {
            if (fmt[i] != str[j])
                return false;
            else
            {
                ++i;
                ++j;
            }
        }
    }

    if(i<fmt_len && j>=str_len) //这种情况下判断fmt的末尾是否为*，若是，则匹配成功
    {
        bool b = false;
        for(int k = i;k<fmt_len;++k)
        {
            if (fmt[k] != '*')
            {
                b = true;
                break;
            }
        }
        if (!b)
            return true;
    }
    if (i<fmt_len || j<str_len)
        return false;
    return true;
}

BOOL GetFileVersionEx_( const char *FileName, DWORD &MajorVer, DWORD &MinorVer, DWORD &ReleaseVer, DWORD &BuildVer )
{
    DWORD InfoSize,Wnd;
    void *VerBuf;
    VS_FIXEDFILEINFO *VerInfo;
    BOOL ret = FALSE;
    InfoSize = GetFileVersionInfoSizeA(FileName,&Wnd);
    if(InfoSize)
    {
        VerBuf = malloc(InfoSize);
        if(GetFileVersionInfoA(FileName,Wnd,InfoSize,VerBuf))
        {
            VerInfo = NULL;
            VerQueryValueA(VerBuf,"\\",(LPVOID*)&VerInfo,(PUINT)&Wnd);
            if(VerInfo)
            {
                MajorVer = VerInfo->dwFileVersionMS >> 16;
                MinorVer = VerInfo->dwFileVersionMS & 0x0000FFFF;
                ReleaseVer = VerInfo->dwFileVersionLS >> 16;
                BuildVer =  VerInfo->dwFileVersionLS & 0x0000FFFF;
                ret = TRUE;
            }            
        }
        free(VerBuf);
    }
    return ret;
}

string GetFileVersionEx_( const char *fn )
{
    char ret[1024] = {0};
    DWORD a,b,c,d;
    GetFileVersionEx_(fn,a,b,c,d);
    sprintf_s(ret,1024,"%d.%d.%d.%d",a,b,c,d);
    return ret;
}

string Int2Hex( int val,int width)
{
    int imax = max(9,width);
    char *buf = (char*)malloc(imax);
    AutoFuncClass auto1(AutoFunc(::free,buf));
    memset(buf,0,imax);
    if(0 != _itoa_s(val,buf,imax,16))
        return "";
    string ret(buf);
    if ((size_t)width <= ret.length())
        return ret;
    else
    {        
        memset(buf,0,imax);
        string fmt;
        char cw[8] = {0};
        fmt = fmt + "%0" + _itoa(width,cw,10) + "s";
        sprintf_s(buf,imax,fmt.c_str(),ret.c_str());
        ret.assign(buf);
        return ret;
    }        
}

bool GetGatewayAndIP(vector<string> &macs, vector<string> &ips,vector<string> &gates )
{
    PIP_ADAPTER_INFO adapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD ret;
    DWORD bufsize = 0;
    ret = GetAdaptersInfo(NULL,&bufsize);
    if (ERROR_BUFFER_OVERFLOW != ret)
        return false;
    adapterInfo = (PIP_ADAPTER_INFO)malloc(bufsize);
    AutoFuncClass auto1(AutoFunc(::free,adapterInfo));
    ret = GetAdaptersInfo(adapterInfo,&bufsize);
    if(ERROR_SUCCESS != ret)
        return false;
    pAdapter = adapterInfo;
    ips.clear();
    gates.clear();
    char addr[8] = {0};
    string saddr;
    while(pAdapter)
    {        
        ips.push_back(string((pAdapter->IpAddressList).IpAddress.String,16));
        gates.push_back(string((pAdapter->GatewayList).IpAddress.String,16));
        saddr.clear();
        for (size_t i = 0; i < pAdapter->AddressLength; i++) 
        {
            if (i == (pAdapter->AddressLength - 1))
            {
                sprintf_s(addr,8,"%.2X",(int)(pAdapter->Address[i]));
                saddr.append(addr,2);
            }
            else
            {
                sprintf_s(addr,8,"%.2X-",(int)(pAdapter->Address[i]));
                saddr.append(addr,3);
            }
        }
        macs.push_back(saddr);

        pAdapter = pAdapter->Next;
    }
    return true;
}

string GetRemoteMac( const string &remote_ip )
{
    DWORD dwIP = inet_addr(remote_ip.c_str());
    DWORD retmac[2] = {0};
    DWORD macsize = 6;
    DWORD retcode = SendARP(dwIP,0,retmac,&macsize);
    if (NO_ERROR != retcode)
        return "";
    char addr[20] = {0};
    string saddr;
    byte *bmac = (byte*)retmac;
    for(size_t i = 0; i<6; ++i)    
    {
        if(i==5)
        {
            sprintf_s(addr,20,"%.2X",(int)bmac[i]);
            saddr.append(addr,2);
        }
        else
        {
            sprintf_s(addr,20,"%.2X-",(int)bmac[i]);
            saddr.append(addr,3);
        }
    }
    return saddr;
}

string GetGateWayMac()
{
	return GetGateWayMac("");
}

string GetGateWayMac( const string &split_str )
{
	vector<string> macs,ips,gates;    
	string ret;
	if(GetGatewayAndIP(macs,ips,gates) && !gates.empty())
	{
		for(size_t i = 0; i<gates.size(); ++i)
		{
			ret =  trim(GetRemoteMac(gates[i]));
			if (!ret.empty())
				break;
		}

	}
	ReplaceString(ret,"-",split_str);
	return ret;
}

bool DnsResolve(string domain, vector<string> &ip)
{
	struct hostent *remoteHost;
	remoteHost = gethostbyname(domain.c_str());
	if (NULL == remoteHost)
		return false;
	ip.clear();
	if (remoteHost->h_addrtype == AF_INET)
	{
		int i = 0;
		in_addr addr;
		while (remoteHost->h_addr_list[i] != 0) {
			addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
			ip.push_back(string(inet_ntoa(addr)));
		}
		return true;
	}
	return false;
}

wstring GetMac(const wstring &split_str)
{return string2wstring(GetMac(wstring2string(split_str)));}
wstring GetGateWayMac(const wstring &split_str)
{return string2wstring(GetGateWayMac(wstring2string(split_str)));}


void GetHtmlTable( const wstring &html_str, wstring &table_str,const wstring &table_pre )
{
	if (html_str.empty())
	{
		table_str.clear();
		return;
	}
	bool bStart = false;
	size_t iBegin = 0,iEnd = 0;
	size_t i = 0;
	while(i < html_str.length())
	{
		if(!bStart && !SameTextW(html_str.substr(i,table_pre.length()),table_pre))
		{
			++i;
			continue;
		}
		if(!bStart)
		{
			bStart = true;
			iBegin = i;
			i += table_pre.length();
			continue;
		}
		if (bStart && SameTextW(html_str.substr(i,strlen("</table>")),L"</table>"))
		{
			iEnd = i + strlen("</table>");
			break;
		}
		++i;        
	}
	if (iEnd > iBegin)
	{
		table_str = html_str.substr(iBegin,iEnd-iBegin);
		return;
	}
	table_str.clear();
	return;
}

string GetMac()
{
	return GetMac("");
}

string GetMac( const string &split_str )
{
	vector<string> macs,ips,gates;
	string ret;
	if (GetGatewayAndIP(macs,ips,gates))    
		ret = macs[0];    
	ReplaceString(ret,"-",split_str);
	return ret;
}

string FormatString( const char * str,... )
{
	string ret = str;
	if (ret != "")
	{
		char vret[1024] = {0};
		va_list vl;
		va_start(vl,str);
		vsprintf_s(vret,1024,ret.c_str(),vl);    
		va_end(vl);
		ret.assign(vret,lstrlenA(vret));
		return ret;
	}
	return "";
}

wstring FormatStringW(const wchar_t *str,... )
{
	wstring ret = str;
	if (ret != L"")
	{
		wchar_t vret[1024] = {0};
		va_list vl;
		va_start(vl,str);
		vswprintf(vret,1024,ret.c_str(),vl);    
		va_end(vl);
		ret.assign(vret,lstrlenW(vret));
		return ret;
	}
	return L"";
}

bool ParamExists( const string &s )
{
	vector<string> vcmdline;
	GetCommandLineList(vcmdline);
	bool bshowwnd = true;
	for(size_t i = 0;i<vcmdline.size();++i)
	{
		if(SameText(trim(vcmdline[i]),s))
		{
			return true;            
		}
	}
	return false;
}

bool ParamExists( const wstring &s )
{
	return ParamExists(wstring2string(s));
}

bool Str2DatetimeW( const wstring &str, SYSTEMTIME &ret )
{
	return Str2Datetime(wstring2string(str),ret);
}

string GetOSVersion()
{
	OSVERSIONINFOA sp;
	sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&sp);
	char ret[512] = {0};
	sprintf_s(ret,100,"%d.%d.%d.%d.%s",sp.dwMajorVersion,sp.dwMinorVersion,sp.dwBuildNumber,sp.dwPlatformId,sp.szCSDVersion);
	return ret;
}

bool IsWinVistaPlus()
{
	OSVERSIONINFO sp;
	sp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&sp);
	return (sp.dwMajorVersion >= 6);
}

string FormatStringEx( size_t buf_size,const char* str,... )
{
	string ret = str;
	if (ret != "")
	{
		char *vret = (char*)malloc(buf_size);
		memset(vret,0,buf_size);
		va_list vl;
		va_start(vl,str);
		vsprintf_s(vret,buf_size,ret.c_str(),vl);    
		va_end(vl);
		ret.assign(vret,lstrlenA(vret));
		::free(vret);
		return ret;
	}
	return "";
}

wstring FormatStringWEx( size_t buf_size,const wchar_t* str,... )
{
	wstring ret = str;
	if (ret != L"")
	{
		wchar_t *vret = (wchar_t*)malloc(buf_size * sizeof(wchar_t));
		memset(vret,0,buf_size * sizeof(wchar_t));
		va_list vl;
		va_start(vl,str);
		vswprintf(vret,buf_size,ret.c_str(),vl);    
		va_end(vl);
		ret.assign(vret,lstrlenW(vret));
		::free(vret);
		return ret;
	}
	return L"";
}

string BufToHex(unsigned char *buf,unsigned long bufsize)
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

void HexToBuf(const string &strhex, /*out*/unsigned char **buf, /*out*/unsigned long &bufsize)
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

bool AppendBuf2File(const wstring file_name, const void *buf,size_t buf_size)
{
	DWORD dwCreateFlag;
	if(FileExistsW(file_name))
		dwCreateFlag = OPEN_EXISTING;
	else
		dwCreateFlag = CREATE_ALWAYS;
	HANDLE hlog = CreateFileW(file_name.c_str(),GENERIC_READ | GENERIC_WRITE,0,NULL,dwCreateFlag,FILE_ATTRIBUTE_NORMAL,NULL);
	AutoFuncClass auto2(AutoFunc(::CloseHandle,hlog));
	if(INVALID_HANDLE_VALUE == hlog)
		return false;
	if(INVALID_SET_FILE_POINTER == SetFilePointer(hlog,buf_size,NULL,FILE_END))
		return false;
	if(FALSE == SetEndOfFile(hlog))
		return false;
	SetFilePointer(hlog,buf_size * -1,NULL,FILE_END);
	DWORD dwWritted = 0;
	return (TRUE == WriteFile(hlog,buf,buf_size,&dwWritted,NULL));
}

void CreateNULLDACL(SECURITY_ATTRIBUTES &lsa,SECURITY_DESCRIPTOR &lsd)
{
	InitializeSecurityDescriptor(&lsd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&lsd,TRUE,NULL,FALSE);
	lsa.nLength = sizeof(lsa);
	lsa.lpSecurityDescriptor = &lsd;
	lsa.bInheritHandle = true;
}

string FillString(string s,size_t width,char fillChar,bool fillLeft)
{
    if (width <= s.length())
        return s;
    vector<char> v(width,fillChar);
    if (fillLeft)
    {
        size_t j = 0;
        for(size_t i=width-s.length();i<width;++i)
        {
            v[i] = s[j];
            ++j;
        }
    }
    else
    {
        for(size_t i = 0;i<s.length();++i)
        {
            v[i] = s[i];
        }
    }
	char *ret = (char*)malloc(width);
	for(size_t i=0;i<width;i++)
	{
		ret[i] = v[i];
	}
    string sret(ret,width);
	free(ret);
	return sret;
}


void _rc4_init(unsigned char *s, unsigned char *key, unsigned long Len) //初始化函数
{
    int i =0, j = 0;
    char k[256] = {0};
    unsigned char tmp = 0;
    for (i=0;i<256;i++) {
        s[i] = i;
        k[i] = key[i%Len];
    }
    for (i=0; i<256; i++) {
        j=(j+s[i]+k[i])%256;
        tmp = s[i];
        s[i] = s[j]; //交换s[i]和s[j]
        s[j] = tmp;
    }
}

void _rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len) //加解密
{
    int i = 0, j = 0, t = 0;
    unsigned long k = 0;
    unsigned char tmp;
    for(k=0;k<Len;k++) {
        i=(i+1)%256;
        j=(j+s[i])%256;
        tmp = s[i];
        s[i] = s[j]; //交换s[x]和s[y]
        s[j] = tmp;
        t=(s[i]+s[j])%256;
        Data[k] ^= s[t];
    }
}

void rc4_crypt(unsigned char *key, unsigned long keyLen,unsigned char *data, unsigned long dataLen)
{
    unsigned char s[256] = {0}; //S-box
    _rc4_init(s,key,keyLen);
    _rc4_crypt(s,data,dataLen);
}

string rc4_string(string key,string data)
{
    unsigned char *ret = (unsigned char*)malloc(data.length());
    memcpy(ret,data.c_str(),data.length());
    rc4_crypt((unsigned char*)key.c_str(),key.length(),ret,data.length());
	string sret((char*)ret, data.length());
	free(ret);
	return sret;
}

const char base_trad[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const char base_url[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";
char *base64_encode(const char* data, int data_len,bool urlencode) {
    char *base;
    if (!urlencode)
        base = (char*)base_trad;
    else
        base = (char*)base_url;
    int prepare = 0;
    int ret_len;
    int temp = 0;
    char *ret = NULL;
    char *f = NULL;
    int tmp = 0;
    char changed[4];
    int i = 0;
    ret_len = data_len / 3;
    temp = data_len % 3;
    if (temp > 0) {
        ret_len += 1;
    }
    ret_len = ret_len * 4 + 1;
    ret = (char *) malloc(ret_len);
    if (ret == NULL) {
        printf("No enough memory.\n");
        exit(0);
    }
    memset(ret, 0, ret_len);
    f = ret;
    while (tmp < data_len) {
        temp = 0;
        prepare = 0;
        memset(changed, '\0', 4);
        while (temp < 3) {
            //printf("tmp = %d\n", tmp);
            if (tmp >= data_len) {
                break;
            }
            prepare = ((prepare << 8) | (data[tmp] & 0xFF));
            tmp++;
            temp++;
        }
        prepare = (prepare << ((3 - temp) * 8));
        //printf("before for : temp = %d, prepare = %d\n", temp, prepare);
        for (i = 0; i < 4; i++) {
            if (temp < i) {
                changed[i] = 0x40;
            } else {
                changed[i] = (prepare >> ((3 - i) * 6)) & 0x3F;
            }
            *f = base[changed[i]];
            //printf("%.2X", changed[i]);
            f++;
        }
    }
    *f = '\0';
    return ret;
}
/* */
static char find_pos(char ch,bool urlencode) {
    char *base;
    if (!urlencode)
        base = (char*)base_trad;
    else
        base = (char*)base_url;
    char *ptr = (char*) strrchr(base, ch); //the last position (the only) in base[]
    return (ptr - base);
}
/* */
char *base64_decode(const char *data, int data_len,bool urlencode) {
    char *base;
    if (!urlencode)
        base = (char*)base_trad;
    else
        base = (char*)base_url;
    int ret_len = (data_len / 4) * 3;
    int equal_count = 0;
    char *ret = NULL;
    char *f = NULL;
    int tmp = 0;
    int temp = 0;
    char need[3];
    int prepare = 0;
    int i = 0;
    if (*(data + data_len - 1) == '=') {
        equal_count += 1;
    }
    if (*(data + data_len - 2) == '=') {
        equal_count += 1;
    }
    if (*(data + data_len - 3) == '=') { //seems impossible
        equal_count += 1;
    }
    switch (equal_count) {
    case 0:
        ret_len += 4; //3 + 1 [1 for NULL]
        break;
    case 1:
        ret_len += 4; //Ceil((6*3)/8)+1
        break;
    case 2:
        ret_len += 3; //Ceil((6*2)/8)+1
        break;
    case 3:
        ret_len += 2; //Ceil((6*1)/8)+1
        break;
    }
    ret = (char *) malloc(ret_len);
    if (ret == NULL) {
        printf("No enough memory.\n");
        exit(0);
    }
    memset(ret, 0, ret_len);
    f = ret;
    while (tmp < (data_len - equal_count)) {
        temp = 0;
        prepare = 0;
        memset(need, 0, 4);
        while (temp < 4) {
            if (tmp >= (data_len - equal_count)) {
                break;
            }
            prepare = (prepare << 6) | (find_pos(data[tmp],urlencode));
            temp++;
            tmp++;
        }
        prepare = prepare << ((4 - temp) * 6);
        for (i = 0; i < 3; i++) {
            if (i == temp) {
                break;
            }
            *f = (char) ((prepare >> ((2 - i) * 8)) & 0xFF);
            f++;
        }
    }
    *f = '\0';
    return ret;
}

string base64_encode_string(string data,bool urlencode)
{
    char *ret = base64_encode(data.c_str(),data.length(),urlencode);
    string sret(ret);
    free(ret);
    return sret;
}
string base64_decode_string(string data,bool urlencode)
{
    char *ret = base64_decode(data.c_str(),data.length(),urlencode);
    string sret(ret);
    free(ret);
    return sret;
}

MemoryStream::MemoryStream()
:m_buffer(NULL),m_size(0),m_pos(0),m_capacity(0),m_init_capacity(1024)
{

}

MemoryStream::~MemoryStream()
{
  Clear();
}

long MemoryStream::Read( MemoryStream &dest, long bytes)
{
  long new_bytes = min(m_size - m_pos,bytes);  
  if (new_bytes <= 0)
    return 0;
  void *p = m_buffer;
  IncPtr(&p,m_pos);
  dest.Write(p,new_bytes);
  m_pos += new_bytes;
  return new_bytes;
}

long MemoryStream::Read( void *dest, long bytes)
{
  if (bytes <= 0 )
    return 0 ;
  long ret = min(bytes,m_size - m_pos);
  if (ret <= 0)
    return 0;
  void *p = m_buffer;
  IncPtr(&p,m_pos);
  memcpy(dest,p,ret);
  m_pos += ret;
  return ret;
}

bool MemoryStream::Write( const MemoryStream &from,long bytes /*= -1*/ )
{
  if (bytes < -1 || bytes == 0 || bytes > from.GetSize())
    return false;
  long lsize = 0;
  if (-1 == bytes)
    lsize = from.GetSize();
  else
    lsize = bytes;
  long add_size = lsize - (m_capacity - m_pos);
  while (add_size > 0)
  {
    if (!Expand())
      return false;
    add_size = lsize - (m_capacity - m_pos);
  }
  void *p = m_buffer;
  IncPtr(&p,m_pos);
  memcpy(p,from.GetBuffer(),lsize);
  m_pos += lsize;
  m_size = max(m_size,m_pos);
  return true;
}

bool MemoryStream::Write( const void *from,long bytes )
{
  if (bytes <= 0)
    return false;
  long add_size = bytes - (m_capacity - m_pos);
  while (add_size > 0)
  {
    if (!Expand())
      return false;
    add_size = bytes - (m_capacity - m_pos);
  }
  void *p = m_buffer;
  IncPtr(&p,m_pos);
  memcpy(p,from,bytes);
  m_pos += bytes;
  m_size = max(m_size,m_pos);
  return true;  
}

void MemoryStream::Seek( SeekOrigin so,long offset )
{
  if (m_size <= 0)
    return;
  long os = 0;
  if (so == soBegin)
    os = offset;
  else if (so == soEnd)
    os = m_size - 1 + offset;
  else if (so == soCurrent)
    os = m_pos + offset;
  else
    return;
  if (os < 0)
    os = 0;
  else if (os >= m_size)
    os = m_size-1;
  m_pos = os;
}

void MemoryStream::Clear()
{
  free(m_buffer);
  m_buffer = NULL;
  m_pos = 0;
  m_size = 0;    
  m_capacity = 0;
}

bool MemoryStream::LoadFromFile( const wstring &file )
{
  void *buf = NULL;
  DWORD bufsize = 0;
  if (LoadFileToBuffer(file,&buf,bufsize))
  {
    Clear();
    Write(buf,bufsize);
    free(buf);
    return true;
  }
  return false;
}

bool MemoryStream::SaveToFile( const wstring &file )
{
  return SaveBufferToFile(file,m_buffer,m_size);
}

bool MemoryStream::Expand( long new_capacity /*= -1*/ )
{  
  long newp = m_capacity * 2;
  if (0 == newp)
    newp = m_init_capacity;
  if (new_capacity != -1)
    newp = max(new_capacity,newp);
  if (newp <= m_capacity)
    return false;
  void *new_buf = NULL;
  if (m_buffer == NULL)
    new_buf = malloc(newp);
  else
   new_buf = realloc(m_buffer,newp);
  if (NULL == new_buf)
    return false;
  m_buffer = new_buf;
  m_capacity = newp;
  return true;
}

void MemoryStream::Empty()
{
    m_pos = 0;
    m_size = 0;    
}

bool MemoryStream::Shrink(long new_size)
{
    if(new_size < 0 || new_size >= m_size)
        return false;
    m_size = new_size;
    if(m_pos >= m_size )
        m_pos = m_size-1;
    return true;
}

ForwardBuffer::ForwardBuffer(){
    m_buffer = NULL;
    m_size = 0;
    m_pos = 0;
    m_capacity = 0;
    m_lock = new CRITICAL_SECTION;
    InitializeCriticalSection(m_lock);
}
ForwardBuffer::~ForwardBuffer(){
    if (m_buffer)
        free(m_buffer);
    DeleteCriticalSection(m_lock);
    delete m_lock;
}

POINTER ForwardBuffer::Read(void *dest, POINTER bytes){
    AutoCriticalSection auto1(*m_lock);
    if (m_buffer == NULL || m_size <= 0)
        return 0;
    long n = bytes;
    if (m_size < bytes){
        n = m_size;
    }
    memcpy(dest,AddPtr(m_buffer,m_pos),n);
    m_pos += n;
    m_size -= n;
    return n;
}
bool ForwardBuffer::Write(const void *from, POINTER bytes){
    AutoCriticalSection auto1(*m_lock);
    POINTER n = m_capacity - m_pos - m_size;
    if (n < bytes){
        Grow(bytes);
    }
    memcpy(AddPtr(m_buffer,m_pos + m_size),from,bytes);
    m_size += bytes;
    return true;
}
void ForwardBuffer::Grow(POINTER bytes){
    AutoCriticalSection auto1(*m_lock);
    POINTER n = m_capacity - m_pos + bytes;
    void *mem = malloc((size_t)n);
    if (m_buffer && m_size > 0){
        memcpy(mem,AddPtr(m_buffer,m_pos),m_size);
        free(m_buffer);
    }
    m_pos = 0;
    m_capacity = n;
    m_buffer = mem;
}
void ForwardBuffer::Reset(){
    m_size = 0;
    m_pos = 0;
}

POINTER ForwardBuffer::ReadNP(void *dest, POINTER bytes)
{
    AutoCriticalSection auto1(*m_lock);
    POINTER ret = Read(dest,bytes);
    m_pos -= ret;
    m_size += ret;
    return ret;
}

void ForwardBuffer::Forward(POINTER bytes)
{
    AutoCriticalSection auto1(*m_lock);
    POINTER m = bytes;
    if (m > m_size)
        m=m_size;
    m_pos += m;
    m_size -= m;
}
