
#pragma once

/**
 * @file
 * @brief 单向链表
 */



struct slist_node_base
{
	slist_node_base* next;
};

struct slist_header : public slist_node_base
{
	int size;
};


template <typename NodeT, typename NodePtrT, typename NodeRefT>
class slist_iterator_impl
{
public:
	typedef size_t               size_type;
	typedef ptrdiff_t            difference_type;
	typedef std::forward_iterator_tag iterator_category;

	explicit slist_iterator_impl(NodePtrT node = 0) : m_node(node) { }
	void check() const { assert(m_node != 0); }
	void inc() { check(); m_node = static_cast<NodePtrT>( m_node->next ); }
	bool operator==(const slist_iterator_impl& x) const { return m_node == x.m_node; }
	bool operator!=(const slist_iterator_impl& x) const { return m_node != x.m_node; }
	slist_iterator_impl& operator++() { this->inc(); return *this; }
	slist_iterator_impl operator++(int) { iterator res = *this; this->inc(); return res; }
	NodeRefT operator*() const { check(); return *m_node; }
	NodePtrT operator->() const { check(); return m_node; }

	NodePtrT m_node;
};



class slist_base
{
public:
	slist_base()
	{
		m_head.next = 0;
		m_head.size = 0;
	}
	~slist_base()
	{
		clear();
	}
	//typedef slist_node_base* iterator;
	//typedef const slist_node_base* const_iterator;

	typedef slist_iterator_impl<slist_node_base, slist_node_base*, slist_node_base&> iterator;
	typedef slist_iterator_impl<slist_node_base, const slist_node_base*, const slist_node_base&> const_iterator;

	iterator begin() { return iterator(m_head.next); }
	iterator end() { return iterator(0); }
	const_iterator begin() const { return const_iterator(m_head.next); }
	const_iterator end() const { return const_iterator(0); }

	iterator insert(iterator pos, slist_node_base* node)
	{
		do_insert_after(pos.m_node, node);
		return iterator(node);
	}
	/// 删除pos后面的节点
	slist_node_base* erase(iterator pos)
	{
		return do_erase_after(pos.m_node);
	}

	slist_node_base* front()
	{
		assert(!empty());
		return m_head.next;
	}

	void push(slist_node_base* node)
	{
		this->do_insert_after(&m_head, node);
	}
	slist_node_base* pop()
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return this->do_erase_after(&m_head);
	}
	slist_node_base* top()
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return this->m_head.next;
	}
	const slist_node_base* top() const
	{
		check();
		assert(!empty());
		assert(m_head.size > 0);
		return this->m_head.next;
	}
	bool empty() const
	{
		check();
		assert(m_head.size == 0 || m_head.next != 0);
		return m_head.next == 0;
	}
	void clear()
	{
		check();
		m_head.next = 0;
		m_head.size = 0;
	}

	int size() const
	{
		check();
		return m_head.size;
	}
	void check() const
	{
		assert(m_head.size >= 0);
	//	assert(m_head.next == m_next);
	//	size_t count = calc_size();
	//	assert(count == m_size);
	}
	int calc_size() const
	{
		int count = 0;
		for (slist_node_base* node = m_head.next; node != 0; node = node->next)
		{
			++count;
		}
		return count;
	}

	slist_header& head() { return m_head; }
	const slist_header& head() const { return m_head; }

protected:
	/// 将node插入到pos后
	void do_insert_after(slist_node_base* pos, slist_node_base* node)
	{
		check();
		assert(pos);
		node->next = pos->next;
		pos->next = node;
		++m_head.size;
	}
	/// 删除pos后面的节点
	slist_node_base* do_erase_after(slist_node_base* pos)
	{
		check();
		assert(pos);
		slist_node_base* node = 0;
		if (pos->next)
		{
			assert(!empty());
			assert(m_head.size > 0);
			node = pos->next;
			pos->next = node->next;
			--m_head.size;
		}
		return node;
	}

private:
	slist_header m_head;
};


template <typename T>
class sslist
{
public:

	typedef slist_iterator_impl<T, T*, T&> iterator;
	typedef slist_iterator_impl<T, const T*, const T&> const_iterator;

	iterator begin() { return iterator( static_cast<T*>( m_lists.head().next ) ); }
	iterator end() { return iterator(0); }
	const_iterator begin() const { return const_iterator( static_cast<T*>( m_lists.head().next ) ); }
	const_iterator end() const { return const_iterator(0); }

	void push(T* node)
	{
		m_lists.push(node);
	}
	T* pop()
	{
		return static_cast<T*>(m_lists.pop());
	}
	T* top()
	{
		return static_cast<T*>(m_lists.top());
	}
	const T* top() const
	{
		return static_cast<T*>(m_lists.top());
	}
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

	/// 将node插入到pos后
	void insert(T* pos, T* node)
	{
		return m_lists.insert(pos, node);
	}
	/// 删除pos后面的节点
	T* erase(T* pos)
	{
		return static_cast<T*>(m_lists.erase(pos));
	}

private:
	slist_base m_lists;
};


