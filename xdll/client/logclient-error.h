#pragma once

#define LogClientError_NoError  0
#define LogClientError_NoErrorAsyncPending  1
#define LogClientError_Other  2
#define LogClientError_HasUnfinishedRequest  3
#define LogClientError_LocalEnvironment  4
#define LogClientError_ConnectOrSend    5
#define LogClientError_LogRequest  6   // something wrong in log request content
#define LogClientError_LogServer  7   // log server has internal problem

