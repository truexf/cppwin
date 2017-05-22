#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include <set>
#include <Windows.h>
using std::set;

class MemPool
{
private:
	MemPool(const MemPool&);
	MemPool& operator=(const MemPool&);
private:
	set<void*> m_data;
	RTL_CRITICAL_SECTION *m_lock;
	int m_block_size;
public:
	MemPool(int blockSize);
	virtual ~MemPool();

	void* Pull();
	void Push(void* p);
};

#endif // !_MEMPOOL_H_