
#pragma once


//class InputStream;
class MemoryOutputStream;
class DataInputStream;
class DataOutputStream;


class Serializable
{
public:
	virtual ~Serializable() { }

	virtual bool ReadObject( DataInputStream& is )
	{
		assert( false );
		return false;
	}

	virtual void WriteObject( DataOutputStream& os ) const
	{
		assert(false);
	}

	virtual size_t GetObjectSize() const = 0;

	virtual void WriteObject( MemoryOutputStream& os ) const
	{
		assert(false);
	}

};

inline DataInputStream& operator>>( DataInputStream& is, Serializable& obj )
{
	obj.ReadObject( is );
	return is;
}

inline DataOutputStream& operator<<( DataOutputStream& os, const Serializable& obj )
{
	obj.WriteObject( os );
	return os;
}



