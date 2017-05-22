#ifndef ADSREGISTR_H
#define ADSREGISTR_H

#include <Windows.h>
#include <string>
#include <vector>

/*
sample:
subkey = "Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Mappings";
if (!RegEnumKeys(HKEY_USERS,subkey,appids,values))
return;
*/

bool RegKeyExists(HKEY key,wstring subkey);
bool RegNewKey(HKEY key,wstring subkey);
bool RegValueExists(HKEY key,wstring subkey,wstring valuname);
DWORD RegGetValueSize(HKEY key,wstring subkey,wstring valuename);

wstring RegReadStringValue(HKEY key,wstring subkey,wstring valuename,wstring defaultret);
bool RegReadBinaryValue(HKEY key,wstring subkey,wstring valuename,/*in/out*/unsigned char *ret,DWORD &retsize);
DWORD RegReadDWORDValue(HKEY key,wstring subkey,wstring valuename,DWORD defaultret);

bool RegWriteStringValue(HKEY key,wstring subkey,wstring valuename,wstring valuedata);
bool RegWriteBinaryValue(HKEY key,wstring subkey,wstring valuename,unsigned char *valuedata,DWORD datasize);
bool RegWriteDWORDValue(HKEY key,wstring subkey,wstring valuename,DWORD valuedata);

bool RegRemoveValue(HKEY key,wstring subkey,wstring valuename);
bool RegRemoveKey(HKEY key,wstring subkey);

bool RegEnumKeys(HKEY key,wstring subkey,/*out*/vector<wstring> &keys,/*out*/vector<wstring> &values);



#endif