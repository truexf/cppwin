#include <Windows.h>
#include <string>
#include <vector>
#include "CSysUtils.h"
#include "TThread.h"
#include <strsafe.h>

using namespace std;

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383


bool RegEnumKeys( HKEY key,wstring subkey,/*out*/vector<wstring> &keys,/*out*/vector<wstring> &values )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ,
    &hKey) == ERROR_SUCCESS
    )
  {
    keys.clear();
    values.clear();
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i, retCode; 

    TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 

    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
      hKey,                    // key handle 
      achClass,                // buffer for class name 
      &cchClassName,           // size of class string 
      NULL,                    // reserved 
      &cSubKeys,               // number of subkeys 
      &cbMaxSubKey,            // longest subkey size 
      &cchMaxClass,            // longest class string 
      &cValues,                // number of values for this key 
      &cchMaxValue,            // longest value name 
      &cbMaxValueData,         // longest value data 
      &cbSecurityDescriptor,   // security descriptor 
      &ftLastWriteTime);       // last write time 

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    if (cSubKeys)
    {
      //printf( "\nNumber of subkeys: %d\n", cSubKeys);

      for (i=0; i<cSubKeys; i++) 
      { 
        cbName = MAX_KEY_LENGTH;
        retCode = RegEnumKeyEx(hKey, i,
          achKey, 
          &cbName, 
          NULL, 
          NULL, 
          NULL, 
          &ftLastWriteTime); 
        if (retCode == ERROR_SUCCESS) 
        {
          //_tprintf(TEXT("(%d) %s\n"), i+1, achKey);
          keys.push_back(achKey);
        }
      }
    } 

    // Enumerate the key values. 

    if (cValues) 
    {
     // printf( "\nNumber of values: %d\n", cValues);

      for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
      { 
        cchValue = MAX_VALUE_NAME; 
        achValue[0] = '\0'; 
        retCode = RegEnumValue(hKey, i, 
          achValue, 
          &cchValue, 
          NULL, 
          NULL,
          NULL,
          NULL);

        if (retCode == ERROR_SUCCESS ) 
        { 
          //_tprintf(TEXT("(%d) %s\n"), i+1, achValue); 
          values.push_back(achValue);
        } 
      }
    }
    
    RegCloseKey(hKey);
  }  
  else
    return false;

  return true;
}

bool RegKeyExists( HKEY key,wstring subkey )
{
  HKEY hKey;
  bool ret = ( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ,
    &hKey) == ERROR_SUCCESS
    );
  if (ret)
    RegCloseKey(hKey);
  return ret;
}

bool RegValueExists( HKEY key,wstring subkey,wstring valuname )
{
  vector<wstring> keys,values;
  RegEnumKeys(key,subkey,keys,values);
  for(size_t i = 0; i!=values.size();++i)
  {
    if (SameTextW(valuname,values[i]))
      return true;
  }
  return false;
}

DWORD RegGetValueSize( HKEY key,wstring subkey,wstring valuename )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ,
    &hKey) != ERROR_SUCCESS
    )
    return 0;
  DWORD rtype,rsize;
  AutoFuncClass auto_func(AutoFunc(RegCloseKey,hKey));
  if (ERROR_SUCCESS == RegQueryValueExW(hKey,valuename.c_str(),0,&rtype,NULL,&rsize))
    return rsize;
  else
    return 0;
}

std::wstring RegReadStringValue( HKEY key,wstring subkey,wstring valuename,wstring defaultret )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ,
    &hKey) != ERROR_SUCCESS
    )
    return defaultret;
  DWORD rtype,rsize;
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));
  if (ERROR_SUCCESS == RegQueryValueExW(hKey,valuename.c_str(),0,&rtype,NULL,&rsize))
  {
    if (REG_SZ != rtype)
      return defaultret;
    wchar_t *ret = (wchar_t*)malloc(rsize+2);
    memset(ret,0,rsize+2);
    AutoFuncClass auto_func(AutoFunc(::free,ret));
    if (ERROR_SUCCESS == RegQueryValueExW(hKey,valuename.c_str(),0,&rtype,(LPBYTE)ret,&rsize))
    {     
      wstring sret;
      sret.assign(ret);
      return sret;
    }
  }  

  return defaultret;
}

bool RegReadBinaryValue( HKEY key,wstring subkey,wstring valuename,/*in/out*/unsigned char *ret,DWORD &retsize )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ,
    &hKey) != ERROR_SUCCESS
    )
    return false;  
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));
  DWORD rsize = retsize;
  return (ERROR_SUCCESS == RegQueryValueExW(hKey,valuename.c_str(),0,NULL,(LPBYTE)ret,&rsize));
}

DWORD RegReadDWORDValue( HKEY key,wstring subkey,wstring valuename,DWORD defaultret )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ,
    &hKey) != ERROR_SUCCESS
    )
    return defaultret;
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));
  DWORD rtype,rsize,ret; 
  if (ERROR_SUCCESS == RegQueryValueExW(hKey,valuename.c_str(),0,&rtype,NULL,&rsize))
  {
    if (REG_DWORD != rtype)
      return defaultret;
    if (ERROR_SUCCESS == RegQueryValueExW(hKey,valuename.c_str(),0,&rtype,(LPBYTE)(&ret),&rsize))
    {     
      return ret;
    }
  }  

  return defaultret;
}

bool RegWriteStringValue( HKEY key,wstring subkey,wstring valuename,wstring valuedata )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ | KEY_WRITE,
    &hKey) != ERROR_SUCCESS
    )
    return false;
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));
  return (ERROR_SUCCESS ==
    RegSetValueExW(hKey,valuename.c_str(),0,REG_SZ,(const BYTE*)valuedata.c_str(),valuedata.length()*2+4));
}

bool RegWriteBinaryValue( HKEY key,wstring subkey,wstring valuename,unsigned char *valuedata,DWORD datasize )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ | KEY_WRITE,
    &hKey) != ERROR_SUCCESS
    )
    return false;
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));  
  return (ERROR_SUCCESS ==
    RegSetValueExW(hKey,valuename.c_str(),0,REG_BINARY,valuedata,datasize));
}

bool RegWriteDWORDValue( HKEY key,wstring subkey,wstring valuename,DWORD valuedata )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ | KEY_WRITE,
    &hKey) != ERROR_SUCCESS
    )
    return false;
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));  
  return (ERROR_SUCCESS ==
    RegSetValueExW(hKey,valuename.c_str(),0,REG_DWORD,(const BYTE*)(&valuedata),sizeof(DWORD)));
}

bool RegRemoveValue( HKEY key,wstring subkey,wstring valuename )
{
  HKEY hKey;
  if( RegOpenKeyExW( key,
    subkey.c_str(),
    0,
    KEY_READ | KEY_WRITE,
    &hKey) != ERROR_SUCCESS
    )
    return false;
  AutoFuncClass auto_func(AutoFunc(::RegCloseKey,hKey));  
  return (ERROR_SUCCESS == RegDeleteValueW(hKey,valuename.c_str()));
}

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnodeRecurse (HKEY hKeyRoot, LPTSTR lpSubKey)
{
    LPTSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    TCHAR szName[MAX_PATH];
    HKEY hKey;
    FILETIME ftWrite;

    // First, see if we can delete the key without having
    // to recurse.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    lResult = RegOpenKeyEx (hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

    if (lResult != ERROR_SUCCESS) 
    {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found.\n");
            return TRUE;
        } 
        else {
            printf("Error opening key.\n");
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\')) 
    {
        *lpEnd =  TEXT('\\');
        lpEnd++;
        *lpEnd =  TEXT('\0');
    }

    // Enumerate the keys

    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                           NULL, NULL, &ftWrite);

    if (lResult == ERROR_SUCCESS) 
    {
        do {

            StringCchCopy (lpEnd, MAX_PATH*2, szName);

            if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
                break;
            }

            dwSize = MAX_PATH;

            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                                   NULL, NULL, &ftWrite);

        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey (hKey);

    // Try again to delete the key.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    return FALSE;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnode (HKEY hKeyRoot, LPTSTR lpSubKey)
{
    TCHAR szDelKey[2 * MAX_PATH];

    StringCchCopy (szDelKey, MAX_PATH*2, lpSubKey);
    return RegDelnodeRecurse(hKeyRoot, szDelKey);

}

bool RegRemoveKey( HKEY key,wstring subkey )
{
#ifdef UNICODE
  return (TRUE == RegDelnode(key,(LPTSTR)(subkey.c_str())));
#else
  string s = wstring2string(subkey);
  return (TRUE == RegDelnode(key,s.c_str()));
#endif
  
}

bool RegNewKey( HKEY key,wstring subkey )
{
  HKEY ret;
  bool bret = (ERROR_SUCCESS == RegCreateKeyExW(key,subkey.c_str(),0,0,0,0,0,&ret,0));
  if (bret)
    RegCloseKey(ret);
  return bret;
}

