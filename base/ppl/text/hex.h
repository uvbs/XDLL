
#pragma once



class HexEncoding
{
public:
	static string Encode(const string& str)
	{
		string result;
		result.resize(str.size() * 2);
		Encode( &result[0], str.data(), str.data() + str.size() );
		return result;
	}

	static string Encode(const void* data, size_t size)
	{
		const char* buf = static_cast<const char*>(data);
		string result;
		result.resize(size * 2);
		Encode( &result[0], buf, buf + size );
		return result;
	}


	static void Encode(char* dest, const unsigned char* src, const unsigned char* srcEnd)
	{
		for (const unsigned char* pch = src; pch != srcEnd; ++pch)
		{
			unsigned char ch = *pch;
			unsigned char highBit = (ch >> 4);
			unsigned char lowBit = (ch & 0x0F);
			*dest++ = EncodeBit(highBit);
			*dest++ = EncodeBit(lowBit);
		}
	}

	static void Encode(char* dest, const char* src, const char* srcEnd)
	{
		return Encode( dest, reinterpret_cast<const unsigned char*>(src), reinterpret_cast<const unsigned char*>(srcEnd) );
	}

	static char EncodeBit(unsigned char bit)
	{
		assert(bit < 16);
		char ch;
		if (bit < 10)
		{
			ch = '0' + bit;
		}
		else
		{
			ch = 'A' + bit - 10;
		}
		return ch;
	}
};



