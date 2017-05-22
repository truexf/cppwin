#include "hzp_routetable.h"
#include "shared_mem.h"

#define ROUTE_TABLE_NAME  "6EB165BD-20BC-4bc7-9BEA-0956EA9BB2DC"
#define ROUTE_TABLE_LOCK  "8360BDA6-DE2B-44a1-9CF0-E25B8950D555"
#define ROUTE_TABLE_ITEMCOUNT 1024

HANDLE G_mem_handle = NULL;
HANDLE G_mem_lock = NULL;

inline void IncPtr(void **p,int i)
{  
    *p = (void*)((int)*p + i);
}

HANDLE HzpCreateRouteTable()
{
    if(!G_mem_handle)
    {
        G_mem_handle = SM_Create(ROUTE_TABLE_ITEMCOUNT * sizeof(RouteItem),ROUTE_TABLE_NAME);
    }    
    if(!G_mem_lock)
    {
        SECURITY_ATTRIBUTES lsa;
        SECURITY_DESCRIPTOR lsd;
        InitializeSecurityDescriptor(&lsd,SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&lsd,TRUE,NULL,FALSE);
        lsa.nLength = sizeof(lsa);
        lsa.lpSecurityDescriptor = &lsd;
        lsa.bInheritHandle = true;
        G_mem_lock = CreateEventA(&lsa,FALSE,TRUE,ROUTE_TABLE_LOCK);
    }
    return G_mem_handle;
}

bool HzpFindRemoteAddr(unsigned short port,void *addr)
{
    if(!G_mem_handle)
        HzpCreateRouteTable();
    if(!G_mem_handle)
        return false;
    if(!G_mem_lock)
        return false;
    if(WAIT_OBJECT_0 != WaitForSingleObject(G_mem_lock,10000))
        return false;
    void *buf = MapViewOfFile(G_mem_handle,FILE_MAP_ALL_ACCESS,0,0,sizeof(RouteItem)*ROUTE_TABLE_ITEMCOUNT);
    if(!buf)
        return false;
    void *p = buf;
    bool ret = false;
    for(int i=0;i<ROUTE_TABLE_ITEMCOUNT;++i)
    {
        RouteItem *item = (RouteItem*)p;
        if(item->use && item->port == port)
        {            
            memcpy(addr,&(item->addr),16);
            item->use = false;
            ret = true;
            break;
        }
        IncPtr(&p,sizeof(RouteItem));
    }
    UnmapViewOfFile(G_mem_handle);
    SetEvent(G_mem_lock);
    return ret;
}

bool AddRouteItem(unsigned short port,SOCKADDR_IN *addr)
{
    if(!G_mem_handle)
        HzpCreateRouteTable();
    if(!G_mem_handle)
        return false;
    if(!G_mem_lock)
        return false;
    if(WAIT_OBJECT_0 != WaitForSingleObject(G_mem_lock,10000))
        return false;
    void *buf = MapViewOfFile(G_mem_handle,FILE_MAP_ALL_ACCESS,0,0,sizeof(RouteItem)*ROUTE_TABLE_ITEMCOUNT);
    if(!buf)
        return false;
    void *p = buf;
    bool ret = false;
    for(int i=0;i<ROUTE_TABLE_ITEMCOUNT;++i)
    {
        RouteItem *item = (RouteItem*)p;
        if(!(item->use))
        {          
            item->port = port;
            memcpy(&(item->addr),addr,16);
            item->use = true;
            ret = true;
            break;
        }
        IncPtr(&p,sizeof(RouteItem));
    }
    UnmapViewOfFile(G_mem_handle);
    SetEvent(G_mem_lock);
    return ret;
}


