
#pragma once

#pragma warning(disable: 4127)

//template<typename T, size_t N> inline size_t count_of( T (&)[N] ) { return N; }


#ifndef SIZEOF_ARRAY
	/// ��ȡC����ĳߴ�
    #define SIZEOF_ARRAY(ar)        (sizeof(ar)/sizeof((ar)[0]))
#endif // !defined(SIZEOF_ARRAY)


#define FillZero(obj, type) memset(&(obj), 0, sizeof(type))

#define GetMemoryValue(buf, type) (*reinterpret_cast<const type*>(buf))
#define SetMemoryValue(buf, val, type) do { *reinterpret_cast<type*>(buf) = (val); } while (false)

#define LimitMin(val, minVal) do { if ((val) < (minVal)) (val) = (minVal) } while (false)

#define LimitMax(val, maxVal) do { if ((val) > (maxVal)) (val) = (maxVal) } while (false)

#define LimitMinMax(val, minVal, maxVal) \
	do { \
		if ((val) > (maxVal)) (val) = (maxVal); \
		if ((val) < (minVal)) (val) = (minVal); \
	} while (false)



/// �Խṹ������
#define FILL_ZERO(obj) memset(&(obj), 0, sizeof(obj))


/** 
 * @brief ���ڴ���ȡ����ָ�����͵�����
 * @param buf �ڴ���ַ
 * @param type ��������
 */
#define READ_MEMORY(buf, type) (*reinterpret_cast<const type*>(buf))

/** 
 * @brief ��ָ�����͵�����д���ڴ��
 * @param buf �ڴ���ַ
 * @param val ����
 * @param type ��������
 */
#define WRITE_MEMORY(buf, val, type) do { *reinterpret_cast<type*>(buf) = (val); } while (false)

/// ����ָ����������Сֵ
#define LIMIT_MIN(val, minVal) do { if ((val) < (minVal)) (val) = (minVal); assert((val) >= (minVal)); } while (false)

/// ����ָ�����������ֵ
#define LIMIT_MAX(val, maxVal) do { if ((val) > (maxVal)) (val) = (maxVal); assert((val) <= (maxVal)); } while (false)

/// ����ָ����������Сֵ�����ֵ
#define LIMIT_MIN_MAX(val, minVal, maxVal) \
	do { \
		if ((val) > (maxVal)) (val) = (maxVal); \
		if ((val) < (minVal)) (val) = (minVal); \
		assert((val) >= (minVal)); \
		assert((val) <= (maxVal)); \
	} while (false)



/// ������32λ��������һ��64λ����
#define MAKE_ULONGLONG(low, high)		(ULONGLONG) ( ( ((ULONGLONG)(high)) << 32 ) | ((DWORD)(low)) )


#define MAKE_DWORD(low, high)      ((DWORD)(((WORD)(low)) | ((DWORD)((WORD)(high))) << 16))


/// ����stl������(non-const��ʽ)
#define STL_FOR_EACH(containerType, container, iter)	\
	for ( containerType::iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )

/// ����stl������(const��ʽ)
#define STL_FOR_EACH_CONST(containerType, container, iter)	\
	for ( containerType::const_iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )




/// ����2������
#define  CPPUNIT_JOIN(symbol1, symbol2)   _CPPUNIT_DO_JOIN( symbol1, symbol2 ) 
#define  _CPPUNIT_DO_JOIN(symbol1, symbol2)   _CPPUNIT_DO_JOIN2( symbol1, symbol2 ) 
#define  _CPPUNIT_DO_JOIN2(symbol1, symbol2)   symbol1##symbol2 

/// ����Ψһ������
#define  CPPUNIT_MAKE_UNIQUE_NAME(prefix)   CPPUNIT_JOIN( prefix, __LINE__ ) 


#ifdef _PPL_RELEASE_LOG
#  ifdef _DEBUG
#    define PPL_LIB_FILE(x)		CPPUNIT_JOIN(x, "DLog.lib")
#  else
#    define PPL_LIB_FILE(x)		CPPUNIT_JOIN(x, "Log.lib")
#  endif

#else

#  ifdef _DEBUG
#    define PPL_LIB_FILE(x)		CPPUNIT_JOIN(x, "D.lib")
#  else
#    define PPL_LIB_FILE(x)		CPPUNIT_JOIN(x, ".lib")
#  endif

#endif





#ifndef SAFE_RELEASE
/// ��ȫ���ͷ�COMָ��
#define SAFE_RELEASE( x )	\
    if( NULL != x )			\
    {						\
		x->Release( );		\
		x = NULL;			\
    }

#endif

#ifndef SAFE_DELETE
/// ��ȫ��ɾ������ָ��
#define SAFE_DELETE( x )	\
    if( NULL != x )			\
    {						\
		delete x;			\
		x = NULL;			\
    }
#endif

#ifndef SAFE_ARRAYDELETE
/// ��ȫ��ɾ����������ָ��
#define SAFE_ARRAYDELETE( x )	\
    if( NULL != x )				\
    {							\
		delete[] x;				\
		x = NULL;				\
    }
#endif




#define LOWPART_LONGLONG		((ULONGLONG) 0x00000000FFFFFFFFULL)
#define HIGHPART_LONGLONG		((ULONGLONG) 0xFFFFFFFF00000000ULL)


/// ��ȡ64λ�����ĵ�32λֵ
#define LO_DWORD(x)				( (DWORD) ( (x) & LOWPART_LONGLONG ) )

/// ��ȡ64λ�����ĸ�32λֵ
#define HI_DWORD(x)				( (DWORD) ( ( (x) & HIGHPART_LONGLONG ) >> 32 ) )


