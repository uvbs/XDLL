
#pragma once

#include <ppl/util/slist.h>
#include <ppl/mswin/mutex.h>
#include <ppl/concurrent/lock.h>

#include <ppl/util/alloc.h>
#include <ppl/util/TimeCounter.h>



struct alloc_header
{
	size_t size;
};


class MemoryPool
{
public:
	MemoryPool()
	{
		size_t itemSize = 64;
		size_t itemCount = 64;
		int i = 0;
		for (i = 0; i < MAX_FIXED_POOL_COUNT; ++i)
		{
			assert(itemSize >= 64 && itemSize <= 512);
			m_fixedPools[i].init(itemSize, itemCount);
			itemSize *= 2;
			assert(itemCount > 2);
			itemCount /= 2;
		}
		assert(itemSize == 1024);
		itemSize = 1024;
		for (i = 0; i < MAX_RECYCLED_POOL_COUNT; ++i)
		{
			m_recycledPools[i].init(itemSize);
			itemSize += 1024;
		}
		assert(itemSize == 17 * 1024);
	}


	void* allocate(size_t size)
	{
		AutoLightMutex lock(m_lock);
		if (m_lastCheckTime.GetElapsed() > 30 * 1000)
		{
			print_status();
			m_lastCheckTime.Sync();
		}

		assert(size > 0);
		void* p = do_allocate(size + sizeof(alloc_header));
		assert(p != 0);
		alloc_header* header = static_cast<alloc_header*>(p);
		header->size = size;
		return header + 1;
	}
	void deallocate(void* p)
	{
		AutoLightMutex lock(m_lock);

		alloc_header* header = static_cast<alloc_header*>(p) - 1;
		size_t size = header->size;
		assert(size > 0);
		do_deallocate(header);
	}
	void clear()
	{
		AutoLightMutex lock(m_lock);
		print_status();

		int i = 0;
		for (i = 0; i < MAX_FIXED_POOL_COUNT; ++i)
		{
			m_fixedPools[i].clear();
		}
		for (i = 0; i < MAX_RECYCLED_POOL_COUNT; ++i)
		{
			m_recycledPools[i].clear();
		}
	}

	void print_status()
	{
		size_t totalSize = 0;
		int i = 0;
		for (i = 0; i < MAX_FIXED_POOL_COUNT; ++i)
		{
			const fixed_alloc& alloc = m_fixedPools[i];
			totalSize += alloc.total_count() * alloc.item_size();
			double usage = alloc.calc_usage();
			LOGCORE_EVENT("fixed_alloc usage " << make_tuple(i, alloc.item_size(), usage) << " " << make_tuple(alloc.used_count(), alloc.total_count()));
		}
		for (i = 0; i < MAX_RECYCLED_POOL_COUNT; ++i)
		{
			const recycled_alloc& alloc = m_recycledPools[i];
			totalSize += alloc.total_count() * alloc.item_size();
			double usage = alloc.calc_usage();
			LOGCORE_EVENT("recycled_alloc usage " << make_tuple(i, alloc.item_size(), usage) << " " << make_tuple(alloc.used_count(), alloc.total_count()));
		}
		LOGCORE_EVENT("memory total size is " << totalSize);
	}

	size_t total_size() const
	{
		size_t totalSize = 0;
		int i = 0;
		for (i = 0; i < MAX_FIXED_POOL_COUNT; ++i)
		{
			const fixed_alloc& alloc = m_fixedPools[i];
			totalSize += alloc.total_count() * alloc.item_size();
		}
		for (i = 0; i < MAX_RECYCLED_POOL_COUNT; ++i)
		{
			const recycled_alloc& alloc = m_recycledPools[i];
			totalSize += alloc.total_count() * alloc.item_size();
		}
		return totalSize;
	}

protected:
	void* do_allocate(size_t size)
	{
		assert(size > 0);
		int index = 0;
		int allocType = get_alloc_info(size, index);
		if (allocType == 0)
		{
			return malloc(size);
		}
		if (allocType == 1)
		{
			assert(index < MAX_RECYCLED_POOL_COUNT);
			return m_recycledPools[index].allocate();
		}
		assert(index < MAX_FIXED_POOL_COUNT);
		return m_fixedPools[index].allocate();
	}
	void do_deallocate(alloc_header* node)
	{
		size_t size = node->size;
		assert(size > 0);
		size += sizeof(alloc_header);
		int index = 0;
		int allocType = get_alloc_info(size, index);
		if (allocType == 0)
		{
			free(node);
			return;
		}
		if (allocType == 1)
		{
			assert(index < MAX_RECYCLED_POOL_COUNT);
			m_recycledPools[index].deallocate(node);
			return;
		}
		assert(index < MAX_FIXED_POOL_COUNT);
		m_fixedPools[index].deallocate(node);
	}

	int get_alloc_info(size_t size, int& index)
	{
		if (size > 16 * 1024)
		{
			return 0;
		}
		if (size > 512)
		{
			index = (size - 1) / 1024;
			assert(index < MAX_RECYCLED_POOL_COUNT);
			return 1;
		}
		index = log2_div(size - 1, 64);
		assert(index < MAX_FIXED_POOL_COUNT);
		return 2;
	}


private:
	enum { MAX_FIXED_POOL_COUNT = 4, MAX_RECYCLED_POOL_COUNT = 16, };
	fixed_alloc m_fixedPools[MAX_FIXED_POOL_COUNT];
	recycled_alloc m_recycledPools[MAX_RECYCLED_POOL_COUNT];
	LightMutex m_lock;
	TimeCounter m_lastCheckTime;
};


typedef debug_alloc<MemoryPool> DebugMemoryPool;
#ifdef _DEBUG
static DebugMemoryPool s_pool;
#else
static MemoryPool s_pool;
#endif



template <typename PoolT>
void CheckPool(PoolT& pool, size_t size)
{
	vector<void*> ptrs;
	ptrs.resize(111);
	int i = 0;
	for (i = 0; i < ptrs.size(); ++i)
	{
		ptrs[i] = pool.allocate(size);
	//	CheckMemGuard(ptrs[i], size);
	}
	for (i = 0; i < ptrs.size(); ++i)
	{
	//	CheckMemGuard(ptrs[i], size);
		pool.deallocate(ptrs[i]);
	}
}

template <typename PoolT>
void CheckSinglePool(PoolT& pool)
{
	vector<void*> ptrs;
	ptrs.resize(111);
	int i = 0;
	for (i = 0; i < ptrs.size(); ++i)
	{
		ptrs[i] = pool.allocate();
	}
	for (i = 0; i < ptrs.size(); ++i)
	{
		pool.deallocate(ptrs[i]);
	}
}

void CheckPool(size_t size)
{
	CheckPool(s_pool, size);
}

void DoRunPeer()
{
	TRACE(TEXT("RunPeer\n"));

	assert(log2(1) == 0);
	assert(log2(2) == 1);
	assert(log2(3) == 1);
	assert(log2(4) == 2);
	assert(log2(5) == 2);
	assert(log2(6) == 2);
	assert(log2(7) == 2);
	assert(log2(8) == 3);
	assert(log2(9) == 3);
	assert(log2(15) == 3);
	assert(log2(16) == 4);
	assert(log2(17) == 4);

	assert(log2_div(63, 64) == 0);
	assert(log2_div(64, 64) == 0);
	assert(log2_div(65, 64) == 1);
	assert(log2_div(127, 64) == 1);
	assert(log2_div(128, 64) == 1);
	assert(log2_div(129, 64) == 2);
	assert(log2_div(255, 64) == 2);
	assert(log2_div(256, 64) == 2);
	assert(log2_div(257, 64) == 3);
	assert(log2_div(511, 64) == 3);
	assert(log2_div(512, 64) == 3);
	assert(log2_div(513, 64) == 4);
	assert(log2_div(1000, 64) == 4);
	assert(log2_div(1024, 64) == 4);
	assert(log2_div(1025, 64) == 5);

	void* p = s_pool.allocate(100);
	s_pool.deallocate(p);

	recycled_alloc ra;
	ra.init(100);
	p = ra.allocate();
	void* p2 = ra.allocate();
	memset(p, 10, 100);
	memset(p2, 11, 100);
	ra.deallocate(p);
	ra.deallocate(p2);
	ra.clear();
	CheckSinglePool(ra);
	ra.clear();

	fixed_alloc fa;
	fa.init(100);
	p = fa.allocate();
	p2 = fa.allocate();
	memset(p, 10, 100);
	memset(p2, 11, 100);
	fa.deallocate(p);
	fa.deallocate(p2);
	fa.clear();
	CheckSinglePool(fa);
	fa.clear();

	CheckPool(10);
	CheckPool(100);
	CheckPool(1000);
	CheckPool(2000);
	CheckPool(3000);
	CheckPool(4000);
	CheckPool(8100);
	CheckPool(18100);
	CheckPool(31100);
	CheckPool(60100);
	CheckPool(118100);

	s_pool.clear();
}

