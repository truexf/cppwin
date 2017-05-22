#ifndef HPERHANDLEDATA_HPP
#define HPERHANDLEDATA_HPP

#include "hbuffer.hpp"

struct perhandledata
{    
    WSAOVERLAPPED ovlp;
    DWORD data;
};

inline void CreatePerhandledataBuf(HBuff **buf,unsigned long cnt)
{
    *buf = new HBuff(cnt,sizeof(perhandledata));
}

inline perhandledata* CreatePerhandledata(HBuff* hbuf)
{
    perhandledata *ret = (perhandledata*)(hbuf->GetBuf()); 
    return ret;
}
inline void FreePerhandledata(perhandledata *data,HBuff *hbuf)
{
    hbuf->FreeBuf((void*)data);
}

#endif