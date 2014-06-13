
#pragma once


#include <ppl/config.h>
//#include <ppl/util/memory.h>
#include <ppl/util/buffer.h>
#include <boost/noncopyable.hpp>


#if defined(_PPL_PLATFORM_MSWIN)

#include <ppl/mswin/FileMapping.h>

#else

class TrivialFileMapping : private boost::noncopyable
{
public:
	TrivialFileMapping()
	{

	}
	bool Create(LPCTSTR name, DWORD size)
	{
		assert( size < 1 * 1024 * 1024 );
		m_buffer.resize(size);
		return true;
	}

	void Close();

	void* GetView()				{ return m_buffer.data(); }
	bool IsValid() const		{ return false == m_buffer.empty(); }
	bool IsMapped() const		{ return this->IsValid(); }


private:
	byte_buffer m_buffer;
};

class FileMapping : public TrivialFileMapping
{
};

#endif

