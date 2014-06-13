
#pragma once

#include <ppl/stl/stlutils.h>
#include <ppl/mswin/mutex.h>
#include <ppl/mswin/event.h>
#include <ppl/concurrent/lock.h>


template <typename T>
class synchronized_queue
{
public:
	typedef T element_type;
	typedef list<T> element_collection;

	synchronized_queue()
	{
	}
	~synchronized_queue()
	{
		AutoLightMutex lock(m_lock);
		ptr_containers::clear(m_elements);
	}

	void push(const T& elem)
	{
		AutoLightMutex lock(m_lock);
		m_elements.push_back(elem);
	}
	bool pop(T& elem)
	{
		AutoLightMutex lock(m_lock);
		if (m_elements.empty())
			return false;
		elem = m_elements.front();
		m_elements.pop_front();
		return true;
	}
	size_t size() const
	{
		AutoLightMutex lock(m_lock);
		return m_elements.size();
	}
	LightMutex& get_lock() const { return m_lock; }

private:
	element_collection m_elements;
	LightMutex m_lock;
};


const size_t MAX_MESSAGE_QUEUE_SIZE = 800;

template <typename T, size_t _MAX_POOL_SIZE = MAX_MESSAGE_QUEUE_SIZE>
class synchronized_object_queue
{
public:
	typedef T element_type;
	typedef list<T*> element_collection;

	synchronized_object_queue()
	{
	}
	~synchronized_object_queue()
	{
		this->clear();
	}

	void clear()
	{
		AutoLightMutex lock(m_lock);
		ptr_containers::clear(m_elements);
		ptr_containers::clear(m_pool);
	}

	void push(const T& elem)
	{
		T* obj = allocate();
		*obj = elem;
		this->do_push(obj);
	}
	bool pop(T& elem)
	{
		T* obj = this->do_pop();
		if (obj == NULL)
			return false;
		elem = *obj;
		return true;
	}

	template <typename PredT>
	size_t erase(PredT pred)
	{
		size_t count = 0;
		AutoLightMutex lock(m_lock);
		element_collection::iterator pos, end;
		for (element_collection::iterator iter = m_elements.begin(); iter != m_elements.end(); )
		{
			T* elem = *iter;
			if (pred(elem))
			{
				m_elements.erase(iter++);
				this->deallocate(elem);
				++count;
			}
			else
			{
				++iter;
			}
		}
		return count;
	}

	size_t size() const
	{
		AutoLightMutex lock(m_lock);
		return m_elements.size();
	}
	size_t pool_size() const
	{
		AutoLightMutex lock(m_lock);
		return m_pool.size();
	}
	size_t total_size() const
	{
		AutoLightMutex lock(m_lock);
		return m_elements.size() + m_pool.size();
	}

	LightMutex& get_lock() const { return m_lock; }

private:
	T* allocate()
	{
		AutoLightMutex lock(m_lock);
		if (m_pool.empty())
			return new T;
		T* msg = m_pool.front();
		m_pool.pop_front();
		return msg;
	}
	void deallocate(T* elem)
	{
		AutoLightMutex lock(m_lock);
		if (m_pool.size() < _MAX_POOL_SIZE)
		{
			m_pool.push_back(elem);
		}
		else
		{
			delete elem;
		}
	}
	void do_push(T* elem)
	{
		AutoLightMutex lock(m_lock);
		m_elements.push_back(elem);
	}
	T* do_pop()
	{
		AutoLightMutex lock(m_lock);
		if (m_elements.empty())
			return NULL;
		T* elem = m_elements.front();
		m_elements.pop_front();
		return elem;
	}

private:
	element_collection m_elements;
	element_collection m_pool;
	mutable LightMutex m_lock;
};


