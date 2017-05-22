/*
* objectpool.hpp
*
*  Created on: Apr 3, 2015
*      Author: root
*/

#ifndef HELLOLINUX_FYSLIB_OBJECTPOOL_HPP_
#define HELLOLINUX_FYSLIB_OBJECTPOOL_HPP_

#include <vector>
#include "tthread.h"

using std::vector;


	template<typename T>
	class ObjectPool
	{
	private:
		vector<T*> m_pool;
		CRITICAL_SECTION m_lock;
		int m_capacity;
	public:
		ObjectPool()
		{
			InitializeCriticalSection(&m_lock);
			m_capacity = 1024;
		}
		~ObjectPool()
		{
			{
				AutoCriticalSection auto1(m_lock);
				for (size_t i = 0;i<m_pool.size();++i)
				{
					delete (T*)(m_pool[i]);
				}
				m_pool.clear();
			}
			DeleteCriticalSection(&m_lock);
		}
		void SetCapacity(int v)
		{
			AutoCriticalSection auto1(m_lock);
			if (v > 0)
				m_capacity = v;
		}
		void PushObject(T* obj)
		{
			AutoCriticalSection auto1(m_lock);
			if (m_pool.size() < (size_t)m_capacity)
				m_pool.push_back(obj);
			else
				delete obj;
		}
		T* PullObject()
		{
			AutoCriticalSection auto1(m_lock);
			if (m_pool.empty())
				m_pool.push_back(new T);
			T* ret = m_pool.back();
			m_pool.pop_back();
			return ret;
		}
		T* PullObjectNoCreate()
		{
			AutoCriticalSection auto1(m_lock);
			if (m_pool.empty())
				return NULL;
		}
	private:
		ObjectPool(const ObjectPool&);
		ObjectPool& operator=(const ObjectPool&);
	};



#endif /* HELLOLINUX_FYSLIB_OBJECTPOOL_HPP_ */
