#ifndef TCPPACKET_H
#define TCPPACKET_H

#include "xcrc32.h"
#include <memory>

typedef void (*pfnPacket)(void *buf,unsigned long buf_size,void *key);

class TCPPacket_SEND
{
public:
    static inline void IncPtr(void **p,int i)
    {  
        *p = (void*)((int)*p + i);
    }
    TCPPacket_SEND():m_packet(NULL),m_packet_size(0),m_pos(0),m_send_func(NULL),m_cmpted_func(NULL),m_key(NULL){}
    virtual ~TCPPacket_SEND(){}
    void SetSendFunc(pfnPacket send_func){m_send_func = send_func;}
    void SetCmptedFunc(pfnPacket cmpted_func){m_cmpted_func = cmpted_func;}
    void Setkey(void* key){m_key = key;}
    void StartSend(void *packet,unsigned long packet_size)
    {
        if (NULL == packet || 0==packet_size)
            return;
        m_packet = packet;
        m_packet_size = packet_size;
        m_pos = 0;
        if (m_send_func)
        {      
            m_send_func(m_packet,packet_size,m_key);
        }
    }
    void Sent(unsigned long sent_size)
    {
        if (NULL == m_packet || 0==m_packet_size)
            return;
        m_pos += sent_size;
        if (m_pos < m_packet_size)
        {
            if (m_send_func)
            {
                void *p = m_packet;
                IncPtr(&p,m_pos);
                m_send_func(p,m_packet_size - m_pos,m_key);
            }
        }
        else
        {
            if (m_cmpted_func)
            {
                m_cmpted_func(m_packet,m_packet_size,m_key);
            }
        }   
    }  
    void* GetPacket(){return m_packet;}
    unsigned long GetPacket_Size(){return m_packet_size;}
    unsigned long GetPacket_Pos(){return m_pos;}  
private:
    TCPPacket_SEND(const TCPPacket_SEND&);
    TCPPacket_SEND& operator=(const TCPPacket_SEND&);
    void *m_packet;
    unsigned long m_packet_size;
    unsigned long m_pos;
    pfnPacket m_send_func;
    pfnPacket m_cmpted_func;
    void *m_key;
};


class TCPPacket_RECV
{
public: 
    static inline void IncPtr(void **p,int i)
    {  
        *p = (void*)((int)*p + i);
    }
    TCPPacket_RECV(){Clear();}
    virtual ~TCPPacket_RECV(){if (m_packet) free(m_packet);};
    //when receiving a completed packet,call this function
    void SetPacketFunc(pfnPacket packet_func){m_packet_func = packet_func;}
    void Setkey(void* key){m_key = key;}
    //when data incoming,call this function
    void DataIncoming(void *data,size_t data_size)
    {    
        if (!data || !data_size)
            return;
        void *psrc = data;
        size_t iremain = data_size;
label1:
        if (!iremain)
            return;
        if (m_headsize < 8) //head incomplete
        {
            size_t iMin = min(8-m_headsize,data_size);
            memcpy(&(m_head[m_headsize]),psrc,iMin);
            IncPtr(&psrc,iMin);
            m_headsize += iMin;
            iremain -= iMin;
            goto label1;
        }
        if (m_headsize == 8) //head completed
        {
            if (0 == m_packet_size) //packet unallocated
            {
                m_packet_pos = 0;
                m_packet_size = *((unsigned long *)m_head);//MAKELONG(MAKEWORD(m_head[0],m_head[1]),MAKEWORD(m_head[2],m_head[3]));
                m_packet_size = ntohl(m_packet_size);
                if (m_packet)
                    free(m_packet);
                m_packet = malloc(m_packet_size+sizeof(m_head));
                memcpy(m_packet,m_head,sizeof(m_head));
                m_packet_pos += sizeof(m_head);
                void *pdest = m_packet;
                IncPtr(&pdest,sizeof(m_head));
                size_t iMin = min(m_packet_size,iremain);
                memcpy(pdest,psrc,iMin);
                IncPtr(&psrc,iMin);
                iremain -= iMin;
                m_packet_pos += iMin;
                goto label1;
            }
            else //packet allocated
            {
                if (m_packet_pos == m_packet_size) //packet complete
                {
                    if (ValidatePacket())
                        m_packet_func(m_packet,m_packet_size,m_key);            
                    else
                        m_packet_func(0,0,m_key);
                    free(m_packet);
                    Clear();
                    goto label1;
                }
                else //packet uncompleted
                {
                    size_t iMin = min(iremain,m_packet_size - (m_packet_pos+1));
                    void *pdest = m_packet;
                    IncPtr(&pdest,m_packet_pos+1);
                    memcpy(pdest,psrc,iMin);
                    IncPtr(&psrc,iMin);
                    iremain -= iMin;
                    m_packet_pos += iMin;
                    goto label1;
                }
            }
        }
        return;
    }
    void* GetPacket(){return m_packet;}
    size_t GetPacket_Size(){return m_packet_size;}
    size_t GetPacket_Pos(){return m_packet_pos;}
    static bool CreatePacket(void *data,unsigned long data_size,/*out*/void **packet,/*out*/unsigned long &packet_size)
    {
        if (!packet || !data || !data_size)
            return false;
        packet_size = data_size+8;
        *packet = malloc(packet_size);
        DWORD dwhead[2];
        dwhead[0] = htonl(data_size);
        dwhead[1] = htonl(GenerateCRC32((unsigned char *)data,data_size));
        void *p = *packet;
        memcpy(p,dwhead,sizeof(dwhead));
        IncPtr(&p,sizeof(dwhead));
        memcpy(p,data,data_size);
        return true;
    }
private:  
    TCPPacket_RECV(const TCPPacket_RECV&);  
    TCPPacket_RECV& operator=(const TCPPacket_RECV&);
    bool ValidatePacket()
    {
        if(NULL == m_packet || 0 == m_packet_size || m_packet_pos != m_packet_size || m_headsize != sizeof(m_head))
            return false;    
        DWORD crc32head = *((unsigned long *)(m_head+4));//MAKELONG(MAKEWORD(m_head[4],m_head[5]),MAKEWORD(m_head[6],m_head[7]));
        crc32head = ntohl(crc32head);
        void *p = m_packet;
        IncPtr(&p,sizeof(m_head));
        DWORD crc32data = GenerateCRC32((unsigned char*)p,m_packet_size);
        if (crc32head != crc32data)
            return false;
        return true;
    }
    void Clear()
    {
        memset(m_head,0,sizeof(m_head)),
            m_headsize = 0;
        m_packet=NULL;
        m_packet_size=0;
        m_packet_pos=0;
        m_packet_func=NULL;        
        m_key = NULL;
    }
private:
    unsigned char m_head[8];// data_size + crc32
    unsigned long m_headsize; //head size
    void *m_packet; //packet buffer
    unsigned long m_packet_size; //packet size
    unsigned long m_packet_pos;  //recved size;
    pfnPacket m_packet_func; //call this function when packet recving completed
    void *m_key;
};

#endif