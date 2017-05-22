#include "shared_mem.h"

HANDLE SM_Create( size_t mem_size,const char* mem_name )
{
    SECURITY_ATTRIBUTES lsa;
    SECURITY_DESCRIPTOR lsd;
    InitializeSecurityDescriptor(&lsd,SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&lsd,TRUE,NULL,FALSE);
    lsa.nLength = sizeof(lsa);
    lsa.lpSecurityDescriptor = &lsd;
    lsa.bInheritHandle = true;
    return CreateFileMappingA(INVALID_HANDLE_VALUE,&lsa,PAGE_READWRITE,0,mem_size,mem_name);
}

bool SM_Read( HANDLE mem_handle,size_t offset,size_t read_size,void *out_buf )
{
    if(!out_buf)
        return false;
    void *buf = MapViewOfFile(mem_handle,FILE_MAP_READ,0,offset,read_size);
    if (!buf)
        return false;
    memcpy(out_buf,buf,read_size);
    UnmapViewOfFile(buf);
    return true;
}

bool SM_Write( HANDLE mem_handle,size_t offset,size_t write_size,void *data_buf )
{
    if(!data_buf)
        return false;
    void *buf = MapViewOfFile(mem_handle,FILE_MAP_ALL_ACCESS,0,offset,write_size);
    if (!buf)
        return false;
    memcpy(buf,data_buf,write_size);
    UnmapViewOfFile(buf);
    return true;
}

HANDLE SM_Open( const char* mem_name )
{
    return OpenFileMappingA(FILE_MAP_ALL_ACCESS,false,mem_name);
}

void SM_Close( HANDLE mem_handle )
{
    CloseHandle(mem_handle);
}
