#pragma once

// 凡是线程间传递消息的都叫Task,
// Task有两种，
// 一种是RunningTask, 只在队列线程中执行。见 RunningTask.h 和 RunningTask.cpp
// 一种是MsgTask, 只在消息线程中执行。间 MsgTask.h 和 MsgTask.cpp

class Task
{
public:
	Task() { }
	virtual ~Task() { }
	virtual void Run() = 0;
};