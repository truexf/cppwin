/************************************
copyright ycmedia.cn
by fangys
基于sqlite3的通用存储功能
************************************/
#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <Windows.h>

using namespace std;

void db_init(char *dbfile); //初始化
void db_fina(); //终止化

/*
基于key-value组合的数据保存
tbl 表
key key
value 值
valuesize 值长度(单位字节)
wait_until_return 是否同步等待其完成
*/
void db_SetData(const wstring &tbl,
               const void *key,
               unsigned long keysize, 
               const void *value, 
               unsigned long valuesize,
               bool wait_until_return = false);
/*
基于key-value组合的数据获取
tbl 表
key key
value 值,需要用调用者free(value)
valuesize 值长度(单位字节)
wait_until_return 是否同步等待其完成
*/
void db_GetData(const wstring &tbl,
               const void *key,
               unsigned long keysize,
               /*out*/void **value,
               /*out*/unsigned long &valuesize,
               bool wait_until_return = true);
/*
基于key-value组合的数据删除
tbl 表
key key
wait_until_return 是否同步等待其完成
*/
void db_DeleteData(const wstring &tbl,
                  const void *key,
                  unsigned long keysize,
                  bool wait_until_return = false);
/*
基于key-value组合的数据全表数据获取（要求unicode字符串，返回的数据每行之间用\r\n分隔）
tbl 表
data 值
datasize 值长度(单位字节)
wait_until_return 是否同步等待其完成
*/
void db_LoadData(const wstring &tbl, 
                 /*out*/wchar_t **data,
                 /*out*/unsigned long &datasize,
                 bool wait_until_return = true);

/*
执行一条sql语句
sql  sql
wait_until_return 是否同步等待其完成
*/
void db_ExecuteSQL(const wstring &sql,
                   bool wait_until_return = false);
/*
根据sql获取BLOB字段的数据,取sql返回数据的第1行第1列的数据
sql  sql
data 返回数据
datasize 返回的数据长度，需要用调用者free(data)
wait_until_return 是否同步等待其完成
*/
void db_GetBlobBySQL(const wstring &sql,
                 /*out*/void **data,
                 /*out*/unsigned long &datasize,
                 bool wait_until_return = true);
//根据sql获取Float字段的数据,取sql返回数据的第1行第1列的数据
double db_GetFloatBySQL(const wstring &sql);
//根据sql获取Int字段的数据,取sql返回数据的第1行第1列的数据
long db_GetIntBySQL(const wstring &sql);
//根据sql获取Text字段的数据,取sql返回数据的第1行第1列的数据
wstring db_GetTextBySQL(const wstring &sql);

//buffer转16进制
string Buf2Hex(unsigned char *buf,unsigned long bufsize);
//16进制转buffer
void Hex2Buf(const string &strhex,
             /*out*/unsigned char **buf,
             /*out*/unsigned long &bufsize);
//单字节转utf16
wstring str2wstr(const string &str,UINT codepage = CP_ACP);
//utf16转单字节
string wstr2str(const wstring &str,UINT codepage = CP_ACP);
#endif