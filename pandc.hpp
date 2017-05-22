/*
生产者/消费者通用模板

*/
#ifndef PANDC_H
#define PANDC_H

#include <vector>
#include <deque>
#include <Windows.h>
#include <limits.h>


using namespace std;

enum QueueType
{
  qtFIFO,
  qtFILO
};
//T生产的对象
template<typename T>
class Pandc
{
public:
  Pandc(unsigned long QUEUE_COUNT,QueueType QT);
  ~Pandc();
  void P(T obj);
  bool C(/*out*/T &ret);
  long ItemCount(){return m_item_count;}
  long QueueItemCount(unsigned long queue_id);
private:
  Pandc(const Pandc&);
  Pandc& operator=(const Pandc&);
private:
  volatile LONG m_queue_id; //当前队列ID;
  vector<deque<T>* > m_queues; //队列
  vector<RTL_CRITICAL_SECTION*> m_queuelocks;   //队列锁
  HANDLE *m_semaphores; //队列信号量
  unsigned long m_queue_count; //队列数
  QueueType m_queue_type; //进出队列方式 FIFO/FILO
  volatile LONG m_item_count; //队列中的总条目数
};

template<typename T>
long Pandc<T>::QueueItemCount( unsigned long queue_id )
{
  if (queue_id >= m_queue_count)
    return 0;
  return (m_queues[queue_id])->size();
}

template<typename T>
bool Pandc<T>::C(/*out*/T &ret)
{
  DWORD wait_ret = WaitForMultipleObjects(m_queue_count,m_semaphores,false,INFINITE);
  if (WAIT_FAILED == wait_ret)
    return false;

  size_t i = wait_ret - WAIT_OBJECT_0;
  EnterCriticalSection(m_queuelocks[i]);
  if (qtFIFO == m_queue_type)
  {  
    ret = m_queues[i]->front();
    m_queues[i]->pop_front();
  }
  else
  {
    ret = m_queues[i]->back();
    m_queues[i]->pop_back();
  }
  LeaveCriticalSection(m_queuelocks[i]);
  InterlockedDecrement(&m_item_count);
  return true;
}

template<typename T>
void Pandc<T>::P( T obj )
{  
  if (InterlockedIncrement(&m_queue_id) > 1024 * 1024)
    InterlockedExchange(&m_queue_id,0);
  size_t i = (m_queue_id % m_queue_count);  
  EnterCriticalSection(m_queuelocks[i]);  
  m_queues[i]->push_back(obj);
  ReleaseSemaphore(m_semaphores[i],1,NULL);
  LeaveCriticalSection(m_queuelocks[i]);
  InterlockedIncrement(&m_item_count);
}

template<typename T>
Pandc<T>::~Pandc()
{
  for(vector<deque<T> * >::iterator it = m_queues.begin(); it!=m_queues.end();++it)
  { 
    delete (*it);    
  }
  for(vector<RTL_CRITICAL_SECTION*>::iterator it = m_queuelocks.begin(); it!=m_queuelocks.end();++it)
  {
    DeleteCriticalSection(*it);
    delete (*it);
  }
  for(size_t i = 0; i<m_queue_count; ++i)
  {
    CloseHandle(m_semaphores[i]);
  }  
}

template<typename T>
Pandc<T>::Pandc(unsigned long QUEUE_COUNT,QueueType QT)
{
  m_queue_id = 0;
  m_queue_type = QT;
  m_queue_count = QUEUE_COUNT;
  m_item_count = 0;
  m_semaphores = (HANDLE*)malloc(sizeof(HANDLE)*QUEUE_COUNT);
  memset(m_semaphores,0,sizeof(HANDLE)*QUEUE_COUNT);
  for(size_t i = 0; i< m_queue_count; ++i)
  {
    deque<T> *q = new deque<T>;
    m_queues.push_back(q);
    RTL_CRITICAL_SECTION *lock = new RTL_CRITICAL_SECTION;
    InitializeCriticalSection(lock);
    m_queuelocks.push_back(lock);
    HANDLE sp = CreateSemaphoreA(NULL,0,LONG_MAX,"");
    m_semaphores[i] = sp;
  }
}

#endif