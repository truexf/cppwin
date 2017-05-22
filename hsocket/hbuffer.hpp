/*
                �����ڴ滺���
���ص�����:
���ڴ��������ͷŲ���ʹ������ֻ���õ�΢�ĵ�interlockedincrement��interlockeddecrement����
���ڴ��ܴ�С�ͳ����ڴ��Ĵ�С�ǹ̶����ȵģ�ͨ�����캯���Ĳ���ָ����
��������ĳ��ڴ��Ǿ���memsetȷ�ϵ������ڴ棻
�ش�С�ǹ̶���,�����п����ڴ治�����ṩ���裬����ڴ���亯������malloc��ϵͳ�������룬����������ڴ治������У�
ͨ��GetBuf()��FreeBuf�����������̳��ڴ��������ͷţ�
*/
#ifndef HBUFFER_HPP
#define HBUFFER_HPP

#include <deque>
#include <algorithm>
using std::deque;

class HBuff
{
public:
    inline void IncPtr(void **p,int i)
    {  
        *p = (void*)((int)*p + i);
    }
    HBuff(long buf_count,long buf_size)
        :m_buf_count(buf_count),m_buf_size(buf_size)
    {
        m_lock = new RTL_CRITICAL_SECTION;
        InitializeCriticalSection(m_lock);
        m_buf = malloc(buf_count * buf_size);
        memset(m_buf,0,buf_count * buf_size);
        m_buf_end = (unsigned long)m_buf + (buf_count *  buf_size);
        m_buf_free = new void*[buf_count];        
        void *p = m_buf;
        for(int i=0;i<buf_count;++i)
        {
            m_buf_free[i] = p;
            IncPtr(&p,buf_size);
        }
        m_buf_free_pos = buf_count - 1;       
    }
    ~HBuff()
    {
        ::free(m_buf);
        delete [] m_buf_free;   
        DeleteCriticalSection(m_lock);
        delete m_lock;
    }
    void *GetBuf()
    {
        AutoCriticalSection auto1(*m_lock);        
        --m_buf_free_pos;
        if(m_buf_free_pos < -1)
        {
            ++m_buf_free_pos;
            void *ret = malloc(m_buf_size);
            memset(ret,0,m_buf_size);
            m_buf_busy.push_back(ret);
            return ret;
        }
        else
        {
            void* ret = m_buf_free[m_buf_free_pos+1];
            m_buf_busy.push_back(ret);
            return ret;
        }
    }
    void FreeBuf(void *buf)
    {
        AutoCriticalSection auto1(*m_lock);
        deque<void*>::iterator it = std::find(m_buf_busy.begin(),m_buf_busy.end(),buf);
        if(m_buf_busy.end() == it)
            return;
        m_buf_busy.erase(it);
        if((unsigned long)buf >= m_buf_end || (unsigned long)buf < (unsigned long)m_buf)
        {
            ::free(buf);
        }
        else
        {
            ++m_buf_free_pos;
            m_buf_free[m_buf_free_pos] = buf;            
        }
    }
private:
    HBuff(const HBuff&);
    HBuff& operator=(const HBuff&);
private:
    long    m_buf_count;
    long    m_buf_size;
    unsigned long m_buf_end;
    void*   m_buf;
    void**  m_buf_free;
    deque<void*>  m_buf_busy;
    long volatile m_buf_free_pos;   
    RTL_CRITICAL_SECTION *m_lock;
};

#endif