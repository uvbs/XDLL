
#pragma once


#include <ppl/stl/stream.h>


#pragma warning(disable: 4127)

#include <string>
using std::string;


extern void log_impl(unsigned long type, unsigned long level, const string& cText);

extern void file_impl(unsigned long type, unsigned long level, const string& cText);

const int _PPL_NOSES	= 500;
const int _PPL_DEBUG	= 1000;
const int _PPL_INFO		= 2000;
const int _PPL_WARN		= 3000;
const int _PPL_EVENT	= 4000;
const int _PPL_ERROR	= 5000;


#ifdef NEED_LOG

#include <iostream>
#include <sstream>

#define PPL_LOG_IMPL(type, level, message) do{\
	std::ostringstream temposs; \
	temposs << "XD:" << message; \
	std::cout << temposs.str() << std::endl; } while (false)

#define PPL_FILE_IMPL(type, level, message) do{\
	std::ostringstream temposs; \
	temposs << message; \
	file_impl(type, level, temposs.str()); } while (false)

#else

#define PPL_LOG_IMPL(type, level, message)

#define PPL_FILE_IMPL(type, level, message)



#endif


#if 0

#define PPL_TRACE(message) do{\
	std::ostringstream temposs; \
	temposs << "pplive_peer " << message; \
	::OutputDebugStringA(temposs.str().c_str()); } while (false)

#else

#define PPL_TRACE(message) 

#endif


#define PPL_LOG(message)		PPL_LOG_IMPL(0, 0, message)



#define PPL_LOG_TYPE_LOGCORE 0


#define LOGCORE_INFO(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_LOGCORE, _PPL_INFO, message)
#define LOGCORE_EVENT(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_LOGCORE, _PPL_EVENT, message)
#define LOGCORE_WARN(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_LOGCORE, _PPL_WARN, message)
#define LOGCORE_ERROR(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_LOGCORE, _PPL_ERROR, message)
#define LOGCORE_DEBUG(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_LOGCORE, _PPL_DEBUG, message)



#define PPL_LOG_TYPE_UTIL 100

#define UTIL_INFO(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_UTIL, _PPL_INFO, message)
#define UTIL_EVENT(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_UTIL, _PPL_EVENT, message)
#define UTIL_WARN(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_UTIL, _PPL_WARN, message)
#define UTIL_ERROR(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_UTIL, _PPL_ERROR, message)
#define UTIL_DEBUG(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_UTIL, _PPL_DEBUG, message)


#define PPL_LOG_TYPE_APP 500


#define APP_INFO(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_APP, _PPL_INFO, message)
#define APP_EVENT(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_APP, _PPL_EVENT, message)
#define APP_WARN(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_APP, _PPL_WARN, message)
#define APP_ERROR(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_APP, _PPL_ERROR, message)
#define APP_DEBUG(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_APP, _PPL_DEBUG, message)

#define APPF_INFO(message)		PPL_FILE_IMPL(PPL_LOG_TYPE_APP, _PPL_INFO, message)
#define APPF_EVENT(message)		PPL_FILE_IMPL(PPL_LOG_TYPE_APP, _PPL_EVENT, message)
#define APPF_WARN(message)		PPL_FILE_IMPL(PPL_LOG_TYPE_APP, _PPL_WARN, message)
#define APPF_ERROR(message)		PPL_FILE_IMPL(PPL_LOG_TYPE_APP, _PPL_ERROR, message)
#define APPF_DEBUG(message)		PPL_FILE_IMPL(PPL_LOG_TYPE_APP, _PPL_DEBUG, message)

