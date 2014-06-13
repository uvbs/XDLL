
#pragma once

#include <boost/checked_delete.hpp>
#include <ppl/stl/strings.h>
#include <set>
#include <map>



template <typename T>
inline void delete_element(T* p)
{
	assert(p != NULL);
	boost::checked_delete(p);
}




/// 存储对象指针的容器
class ptr_containers
{
public:
	/// 清空容器
	template <typename ContainerT>
	static void clear(ContainerT& cont)
	{
		for (typename ContainerT::const_iterator iter = cont.begin(); iter != cont.end(); ++iter)
		{
			delete_element(*iter);
		}
		cont.clear();
	}
};



/// 值为指针的map操作类
class ptr_maps
{
public:
	template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
	static TypeT find(const std::map<KeyT, TypeT, PredT, AllocatorT>& cont, const KeyT& key)
	{
		typename std::map<KeyT, TypeT, PredT, AllocatorT>::const_iterator pos = cont.find(key);
		if (pos == cont.end())
			return TypeT();
		return pos->second;
	}

	template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
	static void clear(std::map<KeyT, TypeT, PredT, AllocatorT>& cont)
	{
		typedef std::map<KeyT, TypeT, PredT, AllocatorT> map_type;
		for (typename map_type::const_iterator iter = cont.begin(); iter != cont.end(); ++iter)
		{
			delete_element(iter->second);
		}
		cont.clear();
	}

	template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
	static bool erase(std::map<KeyT, TypeT, PredT, AllocatorT>& cont, 
			typename std::map<KeyT, TypeT, PredT, AllocatorT>::iterator iter)
	{
		if (iter == cont.end())
			return false;
		delete_element(iter->second);
		cont.erase(iter);
		return true;
	}

	template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
	static bool erase(std::map<KeyT, TypeT, PredT, AllocatorT>& cont, const KeyT& key)
	{
		typename std::map<KeyT, TypeT, PredT, AllocatorT>::iterator iter = cont.find(key);
		if (iter == cont.end())
			return false;
		delete_element(iter->second);
		cont.erase(iter);
		return true;
	}
};



/// 值为指针的集合操作类
class ptr_sets
{
public:
	template <typename KeyT, typename PredT, typename AllocatorT>
	static void clear(std::set<KeyT, PredT, AllocatorT>& cont)
	{
		ptr_containers::clear(cont);
	}

	template <typename KeyT, typename PredT, typename AllocatorT>
	static bool erase(std::set<KeyT, PredT, AllocatorT>& cont, 
			typename std::set<KeyT, PredT, AllocatorT>::iterator iter)
	{
		if (iter == cont.end())
			return false;
		delete_element(*iter);
		cont.erase(iter);
		return true;
	}

	template <typename KeyT, typename PredT, typename AllocatorT>
	static bool erase(std::set<KeyT, PredT, AllocatorT>& cont, const KeyT& key)
	{
		typename std::set<KeyT, PredT, AllocatorT>::iterator iter = cont.find(key);
		if (iter == cont.end())
			return false;
		delete_element(*iter);
		cont.erase(iter);
		return true;
	}
};



class containers
{
public:
	template <typename ContainerT>
	static bool contains(const ContainerT& container, const typename ContainerT::key_type& key)
	{
		return container.find(key) != container.end();
	}
};


class sequences
{
public:
	template <typename InputIteratorT, typename ValueT>
	static bool contains(InputIteratorT first, InputIteratorT last, const ValueT& val)
	{
		return std::find(first, last, val) != last;
	}
	template <typename ContainerT>
	static bool contains(const ContainerT& container, const typename ContainerT::value_type& val)
	{
		return sequences::contains(container.begin(), container.end(), val);
	}

	template <typename OutputIteratorT, typename InputIteratorT>
	static void copy(InputIteratorT first, size_t size, OutputIteratorT dest)
	{
		for (size_t index = 0; index < size; ++index)
		{
			*dest = *first;
			++first;
			++dest;
		}
	}

	template <typename ContainerT>
	static typename ContainerT::value_type pop(ContainerT& container)
	{
		assert(false == container.empty());
		typename ContainerT::value_type val = container.top();
		container.pop();
		return val;
	}

	template <typename ContainerT>
	static typename ContainerT::value_type pop_front(ContainerT& container)
	{
		assert(false == container.empty());
		typename ContainerT::value_type val = container.front();
		container.pop_front();
		return val;
	}
};


/// 数组等一些顺序表的工具类
class arrays
{
public:
	template <typename OutputIteratorT, typename RandomInputIteratorT>
	static void copy(RandomInputIteratorT src, size_t size, OutputIteratorT dest)
	{
		std::copy(src, src + size, dest);
	}

	template <typename RandomInputIteratorT, typename ValueT>
	static RandomInputIteratorT find(RandomInputIteratorT first, size_t size, const ValueT& val)
	{
		return std::find(first, first + size, val);
	}

	template <typename RandomInputIteratorT, typename ValueT>
	static bool contains(RandomInputIteratorT first, size_t size, const ValueT& val)
	{
		return sequences::contains(first, first + size, val);
	}

};


class vectors
{
public:
	template <typename ValueT, typename AllocatorT>
	static bool contains(const std::vector<ValueT, AllocatorT>& vec, const ValueT& val)
	{
		return sequences::contains(vec.begin(), vec.end(), val);
	}
};


/// multimap的工具函数类
class multimaps
{
public:
	/// 从multimap中找到给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static typename std::multimap<KeyT, ValueT, PredT, AllocatorT>::iterator 
		find(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
		typename collection_type::iterator iterUpperBound = coll.upper_bound(key);
		for (typename collection_type::iterator iter = coll.lower_bound(key); iter != iterUpperBound; ++iter)
		{
			if (iter->second == value)
				return iter;
		}
		return coll.end();
	}

	/// 从multimap中删除给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool erase(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
		typename collection_type::iterator iter = multimaps::find(coll, key, value);
		if (iter == coll.end())
			return false;
		coll.erase(iter);
		return true;
	}

	/// 从multimap中插入给定key和value的项，如果已存在，则跳过
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool insert(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
		typename collection_type::iterator iter = multimaps::find(coll, key, value);
		if (iter != coll.end())
			return false;
		coll.insert(make_pair(key, value));
		return true;
	}

	/// 向multimap中更新给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static void update(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& oldKey, const KeyT& newKey, const ValueT& value)
	{
		// 需要先删除可能处在的原有的项
		multimaps::erase(coll, oldKey, value);
		coll.insert(make_pair(newKey, value));
	}

	/// 向multimap中更新给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool update_if_exist(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& oldKey, const KeyT& newKey, const ValueT& value)
	{
		// 需要先删除可能处在的原有的项
		if (multimaps::erase(coll, oldKey, value))
		{
			coll.insert(make_pair(newKey, value));
			return true;
		}
		return false;
	}

	/// 获取第一项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool pop_front(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, ValueT& val)
	{
		typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
		if (coll.empty())
			return false;
		typename collection_type::iterator iter = coll.begin();
		assert(iter != coll.end());
		val = iter->second;
		coll.erase(iter);
		return true;
	}

	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static typename std::multimap<KeyT, ValueT, PredT, AllocatorT>::const_iterator 
		max_less_than_or_equal(const std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key)
	{
		typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
		typename collection_type::const_iterator lowerBound = coll.lower_bound(key);
		if (lowerBound == coll.end())
		{
			if (!coll.empty())
			{
				--lowerBound;
				assert(coll.key_comp()(lowerBound->first, key));
			}
		}
		else
		{
			if (lowerBound->first != key)
			{
				assert(coll.key_comp()(key, lowerBound->first));
				if (lowerBound == coll.begin())
					lowerBound = coll.end();
				else
				{
					--lowerBound;
					assert(coll.key_comp()(lowerBound->first, key));
				}
			}
		}
		return lowerBound;
	}
};

/// set的工具类
class sets
{
public:
	/// 获取第一项
	template <typename KeyT, typename PredT, typename AllocatorT>
	static bool pop_front(std::set<KeyT, PredT, AllocatorT>& coll, KeyT& key)
	{
		typedef std::set<KeyT, PredT, AllocatorT> collection_type;
		if (coll.empty())
			return false;
		typename collection_type::iterator iter = coll.begin();
		assert(iter != coll.end());
		key = *iter;
		coll.erase(iter);
		return true;
	}
};



class ptr_multimaps
{
public:
	/// 从multimap中找到给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static ValueT find(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
		typename collection_type::iterator iter = multimaps::find(coll, key, value);
		if (iter == coll.end())
			return ValueT();
		return iter->second;
	}

};


/// map/set/hash_map/hash_set相关的工具类
class mappings
{
public:
	template <typename ContainerT>
	static size_t count_range(const ContainerT& coll, const typename ContainerT::key_type& rangeMin, const typename ContainerT::key_type& rangeMax)
	{
		assert(!coll.key_comp()(rangeMax, rangeMin)); // rangeMin<=rangeMax，也就是!(rangeMax<rangeMin)
		return std::distance(coll.lower_bound(rangeMin), coll.upper_bound(rangeMax));
	}


	template <typename ContainerT>
	static typename ContainerT::mapped_type get_value(const ContainerT& coll, const typename ContainerT::key_type& key, const typename ContainerT::mapped_type& defaultVal = typename ContainerT::mapped_type() )
	{
		typename ContainerT::const_iterator iter = coll.find(key);
		if (iter != coll.end())
			return iter->second;
		return defaultVal;
	}

};



/// map相关的工具类
class maps
{
public:
	template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
	static size_t count_range(const std::map<KeyT, TypeT, PredT, AllocatorT>& cont, const KeyT& rangeMin, const KeyT& rangeMax)
	{
		assert(!cont.key_comp()(rangeMax, rangeMin)); // rangeMin<=rangeMax，也就是!(rangeMax<rangeMin)
		return std::distance(cont.lower_bound(rangeMin), cont.upper_bound(rangeMax));
	}


	/// 从map中找到给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static typename std::map<KeyT, ValueT, PredT, AllocatorT>::iterator 
		find(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		return coll.find(key);
	}

	/// 从map中删除给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool erase(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		return coll.erase(key) == 1;
	}

	/// 从map中插入给定key和value的项，如果已存在，则跳过
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool insert(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
	{
		typedef std::map<KeyT, ValueT, PredT, AllocatorT> collection_type;
		pair<typename collection_type::iterator, bool> res = coll.insert(make_pair(key, value));
		return res.second;
	}

	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static size_t erase_lower(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& upperBound)
	{
		size_t erasedCount = 0;
		while (!coll.empty())
		{
			if ( false == coll.key_comp()( coll.begin()->first, upperBound ) )
				break;
			coll.erase( coll.begin() );
			++erasedCount;
		}
		return erasedCount;
	}

	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static ValueT get_value(const std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key)
	{
		typedef std::map<KeyT, ValueT, PredT, AllocatorT> collection_type;
		typename collection_type::const_iterator iter = coll.find(key);
		if (iter != coll.end())
			return iter->second;
		return ValueT();
	}



/*

	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static void pop(std::map<KeyT, ValueT, PredT, AllocatorT>& coll)
	{
		assert(!coll.empty());
		coll.erase(coll.begin());
	}



	/// 向map中更新给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static void update(map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& oldKey, const KeyT& newKey, const ValueT& value)
	{
		// 需要先删除可能处在的原有的项
		maps::erase(coll, oldKey, value);
		coll.insert(make_pair(newKey, value));
	}

	/// 向map中更新给定key和value的项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool update_if_exist(map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& oldKey, const KeyT& newKey, const ValueT& value)
	{
		// 需要先删除可能处在的原有的项
		if (maps::erase(coll, oldKey, value))
		{
			coll.insert(make_pair(newKey, value));
			return true;
		}
		return false;
	}

	/// 获取第一项
	template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
	static bool pop_front(map<KeyT, ValueT, PredT, AllocatorT>& coll, ValueT& val)
	{
		typedef map<KeyT, ValueT, PredT, AllocatorT> collection_type;
		if (coll.empty())
			return false;
		typename collection_type::iterator iter = coll.begin();
		assert(iter != coll.end());
		val = iter->second;
		coll.erase(iter);
		return true;
	}
*/
};



typedef std::map<string, string> string_table;
typedef std::map<string, string> string_dict;
typedef std::map<tstring, tstring> tstring_table;
typedef std::map<tstring, tstring> tstring_dict;


class string_dicts
{
public:
	static tstring get_value(const tstring_dict& dict, const tstring& key)
	{
		return maps::get_value(dict, key);
	}
	static tstring get_value_ignore_case(const tstring_dict& dict, const tstring& key)
	{
		return get_value(dict, strings::lower(key));
	}
};

class lists
{
public:
	template <typename T>
	static void append( std::list<T>& target, const std::list<T>& src )
	{
		target.insert( target.end(), src.begin(), src.end() );
	}

	template <typename T>
	static void limit_max( std::list<T>& target, size_t maxCount )
	{
		if ( target.size() > maxCount )
		{
			target.resize( maxCount );
		}
	}
};



