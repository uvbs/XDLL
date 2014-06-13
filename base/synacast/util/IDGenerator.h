
#pragma once

#include <ppl/util/random.h>

#include <boost/noncopyable.hpp>


template <typename ValueT>
class IDGeneratorImpl : private boost::noncopyable
{
public:
	explicit IDGeneratorImpl(ValueT id) : m_id(id)
	{
	}

	ValueT Get(ValueT id)
	{
		return (id> 0) ? id : this->New();
	}

	ValueT New()
	{
		++m_id;
		if ( 0 == m_id )
		{
			++m_id;
		}
		return m_id;
	}

	ValueT Current() const
	{
		return m_id;
	}
private:
	ValueT m_id;
};


class IDGenerator : public IDGeneratorImpl<UINT>
{
public:
	explicit IDGenerator(UINT id = 0) : IDGeneratorImpl<UINT>( id != 0 ? id : RandomGenerator().NextDWord() )
	{
	}

};


