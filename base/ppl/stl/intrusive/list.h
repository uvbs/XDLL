
#pragma once

/**
 * @file
 * @brief 单向链表
 */


namespace ppl {
namespace stl {
namespace intrusive {

struct list_node_base
{
	list_node_base* next;
	list_node_base* prev;

	bool is_valid() const
	{
		return this->next != 0 && this->prev != 0;
	}
};

struct list_header : public list_node_base
{
	int size;

};


template <typename NodeStorageT, typename NodePtrT, typename NodeRefT>
class list_iterator_impl
{
	typedef list_iterator_impl<NodeStorageT, NodePtrT, NodeRefT> this_type;
public:
	typedef std::size_t               size_type;
	typedef std::ptrdiff_t            difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;


	explicit list_iterator_impl(NodeStorageT node) : m_node(node)
	{
		assert(m_node != 0 && m_node->is_valid());
	}

	void inc()
	{
		m_node = m_node->next;
	}
	void dec()
	{
		m_node = m_node->prev;
	}
 	bool operator==(const list_iterator_impl& x) const
 	{
 		return m_node == x.m_node;
 	}
 	bool operator!=(const list_iterator_impl& x) const
 	{
		return m_node != x.m_node;
 	}
	list_iterator_impl& operator++()
	{
		this->inc();
		return *this;
	}
	list_iterator_impl operator++(int)
	{
		this_type res = *this;
		this->inc();
		return res;
	}
	list_iterator_impl& operator--()
	{
		this->dec();
		return *this;
	}
	list_iterator_impl operator--(int)
	{
		this_type res = *this;
		this->dec();
		return res;
	}
	NodeRefT operator*() const
	{
		return static_cast<NodeRefT>( *(m_node) );
	}
	NodePtrT operator->() const
	{
		return static_cast<NodePtrT>( m_node );
	}

	NodeStorageT m_node;
};



class list_base
{
public:
	list_base()
	{
		m_end = &m_head;
		this->init();
		this->check();
	}
	~list_base()
	{
		clear();
	}

	list_node_base* begin() { return m_head.next; }
	const list_node_base* begin() const { return m_head.next; }
	list_node_base* end() { return m_end; }
	const list_node_base* end() const { return m_end; }

	typedef list_node_base* value_type;


	void push_front(list_node_base* node)
	{
		this->insert(m_head.next, node);
	}
	void pop_front()
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		this->erase(m_head.next);
	}
	list_node_base* front()
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return this->m_head.next;
	}
	const list_node_base* front() const
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return this->m_head.next;
	}

	void push_back(list_node_base* node)
	{
		this->insert(m_end, node);
	}
	void pop_back()
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		this->erase(m_end->prev);
	}
	list_node_base* back()
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return m_end->prev;
	}
	const list_node_base* back() const
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return m_end->prev;
	}

	bool empty() const
	{
		check();
		return m_head.next == m_end;
	}
	void clear()
	{
		check();
		this->init();
		check();
	}

	int size() const
	{
		check();
		return m_head.size;
	}
	void check() const
	{
#ifdef _DEBUG
		assert(m_head.size >= 0);
	//	assert(m_head.next == m_next);
		size_t count = calc_size();
		assert(count == m_head.size);
#endif
	}
	int calc_size() const
	{
		int count = 0;
		for (list_node_base* node = m_head.next; node != m_end; node = node->next)
		{
			++count;
		}
		return count;
	}

	/// 将node插入到pos前
	void insert(list_node_base* pos, list_node_base* node)
	{
		check();
		assert(pos && node);
		assert(pos->is_valid());

		node->prev = pos->prev;
		node->next = pos;

		pos->prev->next = node;
		pos->prev = node;

		assert(pos->is_valid() && node->is_valid());
		++m_head.size;
		check();
	}
	/// 删除pos对应的节点
	void erase(list_node_base* pos)
	{
		check();
		assert(pos && pos->is_valid());

		pos->next->prev = pos->prev;
		pos->prev->next = pos->next;
		--m_head.size;
		check();
	}

protected:
	void init()
	{
		m_head.next = m_end;
		m_head.prev = m_end;
		m_head.size = 0;
	}

private:
	list_header m_head;
	list_node_base* m_end;
};


template <typename T>
class list
{
public:
	typedef T* value_type;

	typedef list_iterator_impl<list_node_base*, T*, T&> iterator;
	typedef list_iterator_impl<const list_node_base*, const T*, const T&> const_iterator;

	iterator begin() { return iterator( m_lists.begin() ); }
	iterator end() { return iterator( m_lists.end() ); }
	const_iterator begin() const { return const_iterator( m_lists.begin() ); }
	const_iterator end() const { return const_iterator( m_lists.end() ); }

	void push_front(T* node) { m_lists.push_front(node); }
	void push_back(T* node) { m_lists.push_back(node); }
	void pop_front() { m_lists.pop_front(); }
	void pop_back() { m_lists.pop_back(); }

	T* front() { return static_cast<T*>(m_lists.front()); }
	const T* front() const { return static_cast<const T*>(m_lists.front()); }
	T* back() { return static_cast<T*>(m_lists.back()); }
	const T* back() const { return static_cast<const T*>(m_lists.back()); }

	bool empty() const
	{
		return m_lists.empty();
	}
	void clear()
	{
		m_lists.clear();
	}
	int size() const
	{
		return m_lists.size();
	}

	/// 将node插入到pos前
	void insert(iterator pos, T* node)
	{
		return m_lists.insert(pos.m_node, node);
	}
	/// 删除pos对应的节点
	void erase(iterator pos)
	{
		m_lists.erase(pos.m_node);
	}

private:
	list_base m_lists;
};

}
}
}


