
#pragma once


/**
 * @file
 * @brief 事件侦听器相关的定义
 */




/// 可被侦听的对象
template <typename ListenerT, typename TrivialListenerT = ListenerT>
class Listenable
{
public:
	typedef ListenerT listener_type;
	typedef TrivialListenerT trivial_listener_type;

	Listenable()
	{
		SetListener(0);
	}
	void SetListener(listener_type* listener)
	{
		m_listener = ((listener != 0) ? listener : NullListener());
	}
protected:
	listener_type* GetListener() const	{ return m_listener; }
	static listener_type* NullListener()
	{
		static trivial_listener_type listener;
		return &listener;
	}
private:
	listener_type* m_listener;
};




#define PPL_DEFINE_LISTENABLE(thisClass, listenerType, fakeListenerType) typedef Listenable<listenerType, fakeListenerType> thisClass




