#pragma once

// �����̼߳䴫����Ϣ�Ķ���Task,
// Task�����֣�
// һ����RunningTask, ֻ�ڶ����߳���ִ�С��� RunningTask.h �� RunningTask.cpp
// һ����MsgTask, ֻ����Ϣ�߳���ִ�С��� MsgTask.h �� MsgTask.cpp

class Task
{
public:
	Task() { }
	virtual ~Task() { }
	virtual void Run() = 0;
};