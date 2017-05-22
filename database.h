/************************************
copyright ycmedia.cn
by fangys
����sqlite3��ͨ�ô洢����
************************************/
#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <Windows.h>

using namespace std;

void db_init(char *dbfile); //��ʼ��
void db_fina(); //��ֹ��

/*
����key-value��ϵ����ݱ���
tbl ��
key key
value ֵ
valuesize ֵ����(��λ�ֽ�)
wait_until_return �Ƿ�ͬ���ȴ������
*/
void db_SetData(const wstring &tbl,
               const void *key,
               unsigned long keysize, 
               const void *value, 
               unsigned long valuesize,
               bool wait_until_return = false);
/*
����key-value��ϵ����ݻ�ȡ
tbl ��
key key
value ֵ,��Ҫ�õ�����free(value)
valuesize ֵ����(��λ�ֽ�)
wait_until_return �Ƿ�ͬ���ȴ������
*/
void db_GetData(const wstring &tbl,
               const void *key,
               unsigned long keysize,
               /*out*/void **value,
               /*out*/unsigned long &valuesize,
               bool wait_until_return = true);
/*
����key-value��ϵ�����ɾ��
tbl ��
key key
wait_until_return �Ƿ�ͬ���ȴ������
*/
void db_DeleteData(const wstring &tbl,
                  const void *key,
                  unsigned long keysize,
                  bool wait_until_return = false);
/*
����key-value��ϵ�����ȫ�����ݻ�ȡ��Ҫ��unicode�ַ��������ص�����ÿ��֮����\r\n�ָ���
tbl ��
data ֵ
datasize ֵ����(��λ�ֽ�)
wait_until_return �Ƿ�ͬ���ȴ������
*/
void db_LoadData(const wstring &tbl, 
                 /*out*/wchar_t **data,
                 /*out*/unsigned long &datasize,
                 bool wait_until_return = true);

/*
ִ��һ��sql���
sql  sql
wait_until_return �Ƿ�ͬ���ȴ������
*/
void db_ExecuteSQL(const wstring &sql,
                   bool wait_until_return = false);
/*
����sql��ȡBLOB�ֶε�����,ȡsql�������ݵĵ�1�е�1�е�����
sql  sql
data ��������
datasize ���ص����ݳ��ȣ���Ҫ�õ�����free(data)
wait_until_return �Ƿ�ͬ���ȴ������
*/
void db_GetBlobBySQL(const wstring &sql,
                 /*out*/void **data,
                 /*out*/unsigned long &datasize,
                 bool wait_until_return = true);
//����sql��ȡFloat�ֶε�����,ȡsql�������ݵĵ�1�е�1�е�����
double db_GetFloatBySQL(const wstring &sql);
//����sql��ȡInt�ֶε�����,ȡsql�������ݵĵ�1�е�1�е�����
long db_GetIntBySQL(const wstring &sql);
//����sql��ȡText�ֶε�����,ȡsql�������ݵĵ�1�е�1�е�����
wstring db_GetTextBySQL(const wstring &sql);

//bufferת16����
string Buf2Hex(unsigned char *buf,unsigned long bufsize);
//16����תbuffer
void Hex2Buf(const string &strhex,
             /*out*/unsigned char **buf,
             /*out*/unsigned long &bufsize);
//���ֽ�תutf16
wstring str2wstr(const string &str,UINT codepage = CP_ACP);
//utf16ת���ֽ�
string wstr2str(const wstring &str,UINT codepage = CP_ACP);
#endif