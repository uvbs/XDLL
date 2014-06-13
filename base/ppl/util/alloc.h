
#pragma once

#include <ppl/util/slist.h>
#include <ppl/mswin/mutex.h>
#include <ppl/concurrent/lock.h>
#if !defined(_WIN32_WCE)
#include <crtdbg.h>
#endif

/// 自动安装和卸载crash处理模块
//CrashHandlerInstaller g_crashHandlerInstaller;


inline void CalcRoundSize(size_t size, size_t& pos, size_t& chunkSize, size_t granularity)
{
	assert(granularity > 0);
	pos = (size + granularity - 1) / granularity;
	chunkSize = pos * granularity;
	assert(chunkSize >= size);
}


// 实际好像应该为128 + 8 + 254，不过可以稍微多留一些
const size_t POOL_SIZE = 128 + 8 + 256;



class fixed_alloc
{
public:
	explicit fixed_alloc() : m_itemSize(0), m_itemCount(0), m_realItemSize(0), m_usedCount(0)
	{
	}
	~fixed_alloc()
	{
		clear();
	}

	void init(size_t itemSize, size_t itemCount = 10)
	{
		m_itemSize = itemSize;
		m_itemCount = itemCount;
		m_realItemSize = m_itemSize + sizeof(slist_node_base);
		assert(m_itemCount > 0);
		assert(m_itemSize > 0);
//		assert(m_itemSize == 32 || m_itemSize == 64 || m_itemSize == 128 || m_itemSize == 256 || m_itemSize == 512 || m_itemSize == 1024 || m_itemSize == 2048);
	}

	size_t item_size() const
	{
		return m_itemSize;
	}

	void clear()
	{
//		AutoLightMutex lock(m_lock);
		while (!m_blocks.empty())
		{
			slist_node_base* node = m_blocks.pop();
			free(node);
		}
		m_blocks.clear();
		m_free.clear();
		m_usedCount = 0;
	}

	void* allocate()
	{
//		AutoLightMutex lock(m_lock);
		//APP_EVENT("fixed_alloc allocate " << make_tuple(m_itemSize, m_itemCount, m_blocks.size(), m_usedCount));
		++m_usedCount;
		if (!m_blocks.empty())
		{
#if !defined(_WIN32_WCE)
			assert(_CrtIsValidHeapPointer(&*m_blocks.begin()));
#endif
		}
		assert(m_itemSize > 0);
		if (m_free.empty())
		{
			return refill();
		}
		assert(!m_free.empty());
		slist_node_base* itemNode = m_free.pop();
		assert(itemNode != 0);
		//APP_EVENT("fixed_alloc real allocate " << make_tuple(m_itemSize, m_itemCount, m_blocks.size(), m_usedCount));
		return itemNode + 1;
	}
	void deallocate(void* p)
	{
//		AutoLightMutex lock(m_lock);
		--m_usedCount;
		slist_node_base* itemNode = static_cast<slist_node_base*>(p);
		--itemNode;
		m_free.push(itemNode);
		//APP_EVENT("fixed_alloc deallocate " << make_tuple(m_itemSize, m_itemCount, m_blocks.size(), m_usedCount));
	}

	size_t total_count() const
	{
		return m_blocks.size() * m_itemCount;
	}
	size_t used_count() const
	{
		return m_usedCount;
	}
	double calc_usage() const
	{
		if (m_blocks.empty())
			return 1.0;
//		assert(m_usedCount >= 0);
		double totalItems = total_count();
		return m_usedCount / totalItems;
	}

private:
	void* refill()
	{
		size_t totalSize = m_realItemSize * m_itemCount + sizeof(slist_node_base);
		slist_node_base* blockNode = static_cast<slist_node_base*>(malloc(totalSize));
		assert(blockNode != 0);
		blockNode->next = 0;
		m_blocks.push(blockNode);
		char* buf = reinterpret_cast<char*>(blockNode + 1);
		slist_node_base* itemNode = reinterpret_cast<slist_node_base*>(buf);
		itemNode->next = 0;
		for (size_t i = 1; i < m_itemCount; ++i)
		{
			buf += m_realItemSize;
			slist_node_base* itemNode = reinterpret_cast<slist_node_base*>(buf);
			itemNode->next = 0;
			m_free.push(itemNode);
		}
		return reinterpret_cast<char*>(blockNode) + sizeof(slist_node_base) + sizeof(slist_node_base);
	}

private:
	size_t m_itemSize;
	size_t m_itemCount;
	size_t m_realItemSize;
	int m_usedCount;

	slist_base m_blocks;
	slist_base m_free;
//	LightMutex m_lock;
};


class recycled_alloc
{
public:
	explicit recycled_alloc() : m_itemSize(0), m_realItemSize(0), m_totalItems(0)
	{
	}
	~recycled_alloc()
	{
		clear();
	}

	size_t item_size() const
	{
		return m_itemSize;
	}

	void init(size_t itemSize)
	{
		m_itemSize = itemSize;
		m_realItemSize = m_itemSize + sizeof(slist_node_base);
	//	assert(m_itemSize == 32 || m_itemSize == 64 || m_itemSize == 128 || m_itemSize == 256 || m_itemSize == 512 || m_itemSize == 1024 || m_itemSize == 2048);
	}

	void clear()
	{
//		AutoLightMutex lock(m_lock);
		check();
		while (!m_free.empty())
		{
			slist_node_base* node = m_free.pop();
			free(node);
		}
		m_free.clear();
		//m_free.clear();
		m_totalItems = 0;
		check();
	}

	void* allocate()
	{
//		AutoLightMutex lock(m_lock);
		check();
		slist_node_base* node;
		if (m_free.empty())
		{
			node = allocate_node();
			//m_free.push(node);
		}
		else
		{
			node = m_free.pop();
		}
		check();
		return node + 1;
	}
	void deallocate(void* p)
	{
//		AutoLightMutex lock(m_lock);
		check();
		slist_node_base* node = static_cast<slist_node_base*>(p) - 1;
		if (m_free.size() > 100)
		{
			--m_totalItems;
			free(node);
		}
		else
		{
			m_free.push(node);
		}
		check();
	}

	void check() const
	{
		if (m_totalItems < m_free.size())
		{
			assert(false);
		}
		assert(m_totalItems >= 0);
		//assert(m_totalItems >= m_free.size());
	}

	double calc_usage() const
	{
		check();
		if (m_totalItems == 0)
			return 1.0;
		double usedItems = m_totalItems - m_free.size();
		return usedItems / m_totalItems;
	}
	size_t total_count() const
	{
		return m_totalItems;
	}
	size_t used_count() const
	{
		check();
		return m_totalItems - m_free.size();
	}

protected:
	slist_node_base* allocate_node()
	{
		++m_totalItems;
		slist_node_base* node = static_cast<slist_node_base*>(malloc(m_realItemSize));
		return node;
	}

private:
//	slist_base m_total;
	slist_base m_free;

	size_t m_itemSize;
	size_t m_realItemSize;
	int m_totalItems;
//	LightMutex m_lock;
};


inline size_t log(size_t val, size_t val2)
{
	assert(val > 0);
	size_t result = 0;
	while (val > val2)
	{
		val /= val2;
		++result;
	}
	return result;
}

inline size_t log2(size_t val)
{
	assert(val > 0);
	size_t result = 0;
	while (val > 1)
	{
		val /= 2;
		++result;
	}
	return result;
}

inline size_t log2_div(size_t val, size_t val2)
{
	assert(val > 0);
	if (val <= val2)
		return 0;
	size_t val3 = (val + val2 - 1) / val2;
	return log2(val3 - 1) + 1;
}

const unsigned long MEM_TAG = 0X12345678;
const unsigned long MEM_CLEAR_TAG = 0X9ABCDEF0;

const unsigned long MEM_GUARD = 0xFAFAFAFA;
const unsigned char MEM_UNINITIALIZED = 0xFB;
const unsigned char MEM_FREED = 0xF9;

void CheckMemGuard(void* p, size_t size)
{
	assert(memchr(p, MEM_UNINITIALIZED, size) == 0);
}

template <typename AllocT>
class debug_alloc
{
public:
	struct debug_header
	{
		size_t size;
		unsigned long tag;
		unsigned long alloc_time;
		const char* filename;
		int line;
		unsigned long front_guard;
	};
	void clear()
	{
		m_impl.clear();
	}

	void* allocate(size_t size, const char* filename = NULL, int line = 0)
	{
		debug_header* header = static_cast<debug_header*>(m_impl.allocate(size + sizeof(debug_header) + 8));
		header->size = size;
		header->alloc_time = ::GetTickCount();
		header->filename = filename;
		header->line = line;
		assert(header->tag != MEM_TAG);
		header->tag = MEM_TAG;
		header->front_guard = MEM_GUARD;
		char* buf = reinterpret_cast<char*>(header + 1);
		memset(buf, MEM_UNINITIALIZED, size);
		WRITE_MEMORY(buf + size, MEM_GUARD, unsigned long);
		return buf;
	}
	void deallocate(void* p, const char* filename = NULL, int line = 0)
	{
		debug_header* header = static_cast<debug_header*>(p);
		--header;
		if (header->tag != MEM_TAG)
		{
			APP_EVENT("alloc: deallcate error " << p);
			assert(false);
			return;
		}
		assert(header->tag == MEM_TAG);
		assert(header->front_guard == MEM_GUARD);
		const char* buf = reinterpret_cast<const char*>(p);
		assert(READ_MEMORY(buf + header->size, unsigned long) == MEM_GUARD);
		header->tag = MEM_CLEAR_TAG;
		memset(p, MEM_FREED, header->size);
		m_impl.deallocate(header);
	}
	void print_status()
	{
		m_impl.print_status();
	}

private:
	AllocT m_impl;
};


