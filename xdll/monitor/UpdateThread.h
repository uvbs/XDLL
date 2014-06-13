#pragma once


class UpdateThread : public Win32Thread  
{
public:
	UpdateThread();
	virtual ~UpdateThread();
	int Loop();
	void Run(LPVOID lpParam) { Loop(); }
};
