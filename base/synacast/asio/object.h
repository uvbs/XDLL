
#pragma once


typedef void * HOBJ;
typedef unsigned int UINT;

#pragma pack(push,1)
typedef struct _MESSAGE{
	HOBJ	Target_obj;		//消息接收者的句柄|指针
	HOBJ	Sender_obj;		//消息发送者的句柄|指针
	UINT	Msg;			//消息编号
	UINT	PARAM_1;		//四个参数
	UINT	PARAM_2;
	UINT	PARAM_3;
	UINT	PARAM_4;
	UINT	Result;			//消息处理完成标志
} MESSAGE, *LPMESSAGE;

#pragma pack(pop)


/// 声明类的消息处理函数
#define PPL_DECLARE_MESSAGE_MAP() virtual int HandleMSG(MESSAGE& Message);

/// 开始类的消息处理函数的定义
#define PPL_BEGIN_MESSAGE_MAP(thisClass)			\
	int thisClass::HandleMSG(MESSAGE& Message)		\
{												\
	int ret = 0;								\
	switch (Message.Msg)						\
{

/// 增加消息msg的处理函数func
#define PPL_MESSAGE_HANDLER(msg, func) case msg: ret = func(Message); break;

/// 结束类的消息处理函数的定义
#define PPL_END_MESSAGE_MAP(baseClass) default: ret = baseClass::HandleMSG(Message); } return ret; }



class CBasicObjectInterface
{
public:
	void SendObjectMessage(UINT msg, HOBJ sender = 0, UINT param1 = 0, UINT param2 = 0, UINT param3 = 0, UINT param4 = 0)
	{
	}

protected:
	void OnMessage()
	{

	}
};



class BasicObject : public CBasicObjectInterface
{
public:
};

typedef BasicObject CBasicObject;


