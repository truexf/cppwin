#ifndef SHARED_MEM_H
#define SHARED_MEM_H
#include <WinSock2.h>
#include <Windows.h>

HANDLE SM_Create(size_t mem_size,const char* mem_name);
bool SM_Read(HANDLE mem_handle,size_t offset,size_t read_size,void *out_buf);
bool SM_Write(HANDLE mem_handle,size_t offset,size_t write_size,void *data_buf);
HANDLE SM_Open(const char* mem_name);
void SM_Close(HANDLE mem_handle);

#endif