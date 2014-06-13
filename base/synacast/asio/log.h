
#pragma once


#include <ppl/util/log.h>

#define PPL_LOG_TYPE_TRACKER 4200

#define TRACKER_INFO(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_TRACKER, _PPL_INFO, message)
#define TRACKER_EVENT(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_TRACKER, _PPL_EVENT, message)
#define TRACKER_WARN(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_TRACKER, _PPL_WARN, message)
#define TRACKER_ERROR(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_TRACKER, _PPL_ERROR, message)
#define TRACKER_DEBUG(message)		PPL_LOG_IMPL(PPL_LOG_TYPE_TRACKER, _PPL_DEBUG, message)



#define NETWORK			2000

#define NETWORK_INFO(message)		PPL_LOG_IMPL(NETWORK, _PPL_INFO, message)
#define NETWORK_EVENT(message)		PPL_LOG_IMPL(NETWORK, _PPL_EVENT, message)
#define NETWORK_WARN(message)		PPL_LOG_IMPL(NETWORK, _PPL_WARN, message)
#define NETWORK_ERROR(message)		PPL_LOG_IMPL(NETWORK, _PPL_ERROR, message)
#define NETWORK_DEBUG(message)		PPL_LOG_IMPL(NETWORK, _PPL_DEBUG, message)

