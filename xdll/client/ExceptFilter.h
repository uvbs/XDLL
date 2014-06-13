
#pragma once



inline LONG APIExceptFilter(const char* tag, PEXCEPTION_POINTERS excepInfo)
{
#ifdef NEED_LOG
	Tracer::Trace(
		"ExceptFilter %s %d code=0x%p addr=0x%p param=%d,%d,0x%p\n", 
		tag, 
		::GetCurrentThreadId(), 
		excepInfo->ExceptionRecord->ExceptionCode, 
		excepInfo->ExceptionRecord->ExceptionAddress, 
		excepInfo->ExceptionRecord->NumberParameters, 
		excepInfo->ExceptionRecord->ExceptionInformation[0], 
		excepInfo->ExceptionRecord->ExceptionInformation[1]);

	printf(
		"ExceptFilter %s %d code=0x%p addr=0x%p param=%d,%d,0x%p\n", 
		tag, 
		::GetCurrentThreadId(), 
		excepInfo->ExceptionRecord->ExceptionCode, 
		excepInfo->ExceptionRecord->ExceptionAddress, 
		excepInfo->ExceptionRecord->NumberParameters, 
		excepInfo->ExceptionRecord->ExceptionInformation[0], 
		excepInfo->ExceptionRecord->ExceptionInformation[1]);
#endif
	return EXCEPTION_EXECUTE_HANDLER;
}


