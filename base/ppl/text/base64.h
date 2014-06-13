
#pragma once

#include <boost/static_assert.hpp>
#include <ppl/util/macro.h>


class HexEncoding
{
public:
	static string Encode(const char* data, size_t size)
	{
		string result;
		result.resize(size * 2);
		DoEncode(&result[0], data, data + size);
		return result;
	}
	static string Encode(const string& str)
	{
		string result;
		result.resize(str.size() * 2);
		DoEncode(&result[0], str.data(), str.data() + str.size());
		return result;
	}

	static void DoEncode(char* dest, const char* src, const char* srcEnd)
	{
		for (const char* pch = src; pch != srcEnd; ++pch)
		{
			unsigned char ch = *pch;
			unsigned char highBit = (ch >> 4);
			unsigned char lowBit = (ch & 0x0F);
			*dest++ = EncodeBit(highBit);
			*dest++ = EncodeBit(lowBit);
		}
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






class Base64Encoding
{
public:
	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::basic_string<CharT, TraitsT, AllocatorT> Encode(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		string_type result;
		size_t size = calcEnoughEncodedLength(str.size());
		if (size > 0)
		{
			result.resize(size);
			size_t len = DoEncode(str.data(), str.size(), &result[0]);
			result.resize(len);
		}
		return result;
	}

	static string Encode( const char* data, size_t size )
	{
		string result;
		size_t maybeResultSize = calcEnoughEncodedLength(size);
		if (maybeResultSize > 0)
		{
			result.resize(maybeResultSize);
			size_t len = DoEncode(data, size, &result[0]);
			result.resize(len);
		}
		return result;
	}

	template <typename CharT, typename TraitsT, typename AllocatorT>
	static std::basic_string<CharT, TraitsT, AllocatorT> Decode(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
	{
		typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
		string_type result;
		size_t size = calcEnoughDecodedLength(str.size());
		if (size > 0)
		{
			result.resize(size);
			size_t len = DoDecode(str.data(), str.size(), &result[0]);
			result.resize(len);
		}
		return result;
	}


	static string& GetDelimiter()
	{
		//static string delimiter = "\r\n";
		static string delimiter = "";
		return delimiter;
	}
	static const char* GetEnBase64Table()
	{
		static const char EnBase64Table[] = 
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		BOOST_STATIC_ASSERT(SIZEOF_ARRAY(EnBase64Table) == 64 + 1);
		return EnBase64Table;
	}
	static const char* GetDeBase64Table()
	{
		static const char DeBase64Table[256] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			62,        // '+'
			0, 0, 0,
			63,        // '/'
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61,        // '0'-'9'
			0, 0, 0, 0, 0, 0, 0,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,        // 'A'-'Z'
			0, 0, 0, 0, 0, 0,
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,        // 'a'-'z'
		};
		BOOST_STATIC_ASSERT(SIZEOF_ARRAY(DeBase64Table) == 256);
		return DeBase64Table;
	}


public:
// Encode
	enum { _MAX_LINE_LENGTH = 72 };

	template <typename InputIteratorT, typename OutputIteratorT>
	static size_t DoEncode(InputIteratorT pSrc, size_t nSrcLen, OutputIteratorT pDst)
	{
		// 将来也可以考虑扩充到unicode版
		const size_t nMaxLineLen = _MAX_LINE_LENGTH;
		unsigned char c1, c2, c3;    // 输入缓冲区读出3个字节
		size_t nDstLen = 0;             // 输出的字符计数
		size_t nLineLen = 0;            // 输出的行长度计数
		size_t nDiv = nSrcLen / 3;      // 输入数据长度除以3得到的倍数
		size_t nMod = nSrcLen % 3;      // 输入数据长度除以3得到的余数
		const char* EnBase64Tab = GetEnBase64Table();
		// 每次取3个字节，编码成4个字符
		for (size_t i = 0; i < nDiv; i ++)
		{
			// 取3个字节
			c1 = *pSrc++;
			c2 = *pSrc++;
			c3 = *pSrc++;
			// 编码成4个字符
			*pDst++ = EnBase64Tab[c1 >> 2];
			*pDst++ = EnBase64Tab[((c1 << 4) | (c2 >> 4)) & 0x3f];
			*pDst++ = EnBase64Tab[((c2 << 2) | (c3 >> 6)) & 0x3f];
			*pDst++ = EnBase64Tab[c3 & 0x3f];
			nLineLen += 4;
			nDstLen += 4;
			// 输出换行？
			if (nLineLen > nMaxLineLen - 4)
			{
				const string& delimiter = GetDelimiter();
				std::copy(delimiter.begin(), delimiter.end(), pDst);
				nLineLen = 0;
				nDstLen += delimiter.size();
			}
		}
		// 编码余下的字节
		if (nMod == 1)
		{
			c1 = *pSrc++;
			*pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
			*pDst++ = EnBase64Tab[((c1 & 0x03) << 4)];
			*pDst++ = '=';
			*pDst++ = '=';
			nLineLen += 4;
			nDstLen += 4;
		}
		else if (nMod == 2)
		{
			c1 = *pSrc++;
			c2 = *pSrc++;
			*pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
			*pDst++ = EnBase64Tab[((c1 & 0x03) << 4) | ((c2 & 0xf0) >> 4)];
			*pDst++ = EnBase64Tab[((c2 & 0x0f) << 2)];
			*pDst++ = '=';
			nDstLen += 4;
		}
		return nDstLen;
	}

	template <typename InputIteratorT, typename OutputIteratorT>
	static size_t DoDecode(InputIteratorT pSrc, size_t nSrcLen, OutputIteratorT pDst)
	{
		size_t nValue;             // 解码用到的长整数
		size_t nDstLen = 0;
		size_t i = 0;
		// 取4个字符，解码到一个长整数，再经过移位得到3个字节
		const char* DeBase64Tab = GetDeBase64Table();
		while (i < nSrcLen)
		{
			if (*pSrc != '\r' && *pSrc != '\n')
			{
				nValue = DeBase64Tab[*pSrc++] << 18;
				nValue += DeBase64Tab[*pSrc++] << 12;
				*pDst++ = static_cast<char>( (nValue & 0x00ff0000) >> 16 );
				nDstLen++;
				if (*pSrc != '=')
				{
					nValue += DeBase64Tab[*pSrc++] << 6;
					*pDst++ = static_cast<char>( (nValue & 0x0000ff00) >> 8 );
					nDstLen++;
					if (*pSrc != '=')
					{
						nValue += DeBase64Tab[*pSrc++];
						*pDst++ = static_cast<char>( nValue & 0x000000ff );
						nDstLen++;
					}
				}
				i += 4;
			}
			else        // 回车换行，跳过
			{
				pSrc++;
				i++;
			}
		}
		return nDstLen;
	}

private:
	static size_t calcEnoughEncodedLength(size_t length)
	{
		size_t len = length / 3;
		if (length % 3 > 0)
			++len;
		len *= 4;
		size_t line = len / _MAX_LINE_LENGTH;
		if (len % _MAX_LINE_LENGTH > 0)
			++line;
		len += line * 2;
		return len;
	}

	static size_t calcEnoughDecodedLength(size_t length)
	{
		return length;
	}

private:
	static unsigned char getFirstSixBits(unsigned char val)
	{
		return (val & 0xFC) >> 2;
	}
	static unsigned char getLastSixBits(unsigned char val)
	{
		return val & 0x3F;
	}
};





