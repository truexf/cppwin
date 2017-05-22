#include "mempool.h"
#include "TThread.h"

MemPool::MemPool(int blockSize)
{
	m_block_size = blockSize;
	m_lock = new RTL_CRITICAL_SECTION;
	InitializeCriticalSection(m_lock);
}

MemPool::~MemPool()
{
	{
		AutoCriticalSection ac(*m_lock);
		for (set<void*>::iterator it = m_data.begin();it != m_data.end();it++)
		{
			free(*it);
		}
	}
	DeleteCriticalSection(m_lock);
	delete m_lock;
}

void* MemPool::Pull()
{
	AutoCriticalSection ac(*m_lock);
	if (m_data.empty())
	{
		void *ret = malloc(m_block_size);
		return ret;		
	}
	else
	{
		void *ret = *m_data.begin();
		m_data.erase(m_data.begin());
		return ret;
	}
}

void MemPool::Push(void *p)
{
	AutoCriticalSection ac(*m_lock);
	if (m_data.find(p) == m_data.end())
		m_data.insert(p);
}