
#pragma once

#include <utility>


/// 使用malloc/free的内存分配器
class malloc_alloc
{
public:
	static void* allocate(size_t size)
	{
		assert(size < 0x100000);
		void* p = malloc(size);
		assert(p);
		return p;
	}
	static void deallocate(void* p, size_t size = 0)
	{
		//assert(p != NULL);
		if (p)
		{
			free(p);
		}
	}
};


/*
typedef pool_object_impl<malloc_alloc> pool_object;
typedef malloc_alloc pool_alloc;



template <typename T>
class pool_allocator : public std::allocator<T>
{ };


class noncopyable_pool_object : public pool_object, private noncopyable
{
};
*/

