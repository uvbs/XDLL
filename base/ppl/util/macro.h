
#pragma once

#pragma warning(disable: 4127)

//template<typename T, size_t N> inline size_t count_of( T (&)[N] ) { return N; }


#ifndef SIZEOF_ARRAY
	/// 获取C数组的尺寸
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



/// 对结构体清零
#define FILL_ZERO(obj) memset(&(obj), 0, sizeof(obj))


/** 
 * @brief 从内存块读取根据指定类型的数据
 * @param buf 内存块地址
 * @param type 数据类型
 */
#define READ_MEMORY(buf, type) (*reinterpret_cast<const type*>(buf))

/** 
 * @brief 将指定类型的数据写到内存块
 * @param buf 内存块地址
 * @param val 数据
 * @param type 数据类型
 */
#define WRITE_MEMORY(buf, val, type) do { *reinterpret_cast<type*>(buf) = (val); } while (false)

/// 限制指定变量的最小值
#define LIMIT_MIN(val, minVal) do { if ((val) < (minVal)) (val) = (minVal); assert((val) >= (minVal)); } while (false)

/// 限制指定变量的最大值
#define LIMIT_MAX(val, maxVal) do { if ((val) > (maxVal)) (val) = (maxVal); assert((val) <= (maxVal)); } while (false)

/// 限制指定变量的最小值和最大值
#define LIMIT_MIN_MAX(val, minVal, maxVal) \
	do { \
		if ((val) > (maxVal)) (val) = (maxVal); \
		if ((val) < (minVal)) (val) = (minVal); \
		assert((val) >= (minVal)); \
		assert((val) <= (maxVal)); \
	} while (false)



/// 由两个32位整数构造一个64位整数
#define MAKE_ULONGLONG(low, high)		(ULONGLONG) ( ( ((ULONGLONG)(high)) << 32 ) | ((DWORD)(low)) )


#define MAKE_DWORD(low, high)      ((DWORD)(((WORD)(low)) | ((DWORD)((WORD)(high))) << 16))


/// 遍历stl的容器(non-const方式)
#define STL_FOR_EACH(containerType, container, iter)	\
	for ( containerType::iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )

/// 遍历stl的容器(const方式)
#define STL_FOR_EACH_CONST(containerType, container, iter)	\
	for ( containerType::const_iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )




/// 连接2个符号
#define  CPPUNIT_JOIN(symbol1, symbol2)   _CPPUNIT_DO_JOIN( symbol1, symbol2 ) 
#define  _CPPUNIT_DO_JOIN(symbol1, symbol2)   _CPPUNIT_DO_JOIN2( symbol1, symbol2 ) 
#define  _CPPUNIT_DO_JOIN2(symbol1, symbol2)   symbol1##symbol2 

/// 生成唯一的名称
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
/// 安全的释放COM指针
#define SAFE_RELEASE( x )	\
    if( NULL != x )			\
    {						\
		x->Release( );		\
		x = NULL;			\
    }

#endif

#ifndef SAFE_DELETE
/// 安全的删除对象指针
#define SAFE_DELETE( x )	\
    if( NULL != x )			\
    {						\
		delete x;			\
		x = NULL;			\
    }
#endif

#ifndef SAFE_ARRAYDELETE
/// 安全的删除对象数组指针
#define SAFE_ARRAYDELETE( x )	\
    if( NULL != x )				\
    {							\
		delete[] x;				\
		x = NULL;				\
    }
#endif




#define LOWPART_LONGLONG		((ULONGLONG) 0x00000000FFFFFFFFULL)
#define HIGHPART_LONGLONG		((ULONGLONG) 0xFFFFFFFF00000000ULL)


/// 提取64位整数的低32位值
#define LO_DWORD(x)				( (DWORD) ( (x) & LOWPART_LONGLONG ) )

/// 提取64位整数的高32位值
#define HI_DWORD(x)				( (DWORD) ( ( (x) & HIGHPART_LONGLONG ) >> 32 ) )


