
#pragma once




namespace ppl {


template <typename T>
static T* construct(void* p)
{
	return new(p) T();
}

template <typename T>
static T* construct(void* p, const T& val)
{
	return new(p) T(val);
}

template <typename T>
static void destruct(T* obj)
{
	obj->~T();
}


}

