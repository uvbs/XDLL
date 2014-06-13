
#pragma once


typedef void * HOBJ;
typedef unsigned int UINT;

#pragma pack(push,1)
typedef struct _MESSAGE{
	HOBJ	Target_obj;		//��Ϣ�����ߵľ��|ָ��
	HOBJ	Sender_obj;		//��Ϣ�����ߵľ��|ָ��
	UINT	Msg;			//��Ϣ���
	UINT	PARAM_1;		//�ĸ�����
	UINT	PARAM_2;
	UINT	PARAM_3;
	UINT	PARAM_4;
	UINT	Result;			//��Ϣ������ɱ�־
} MESSAGE, *LPMESSAGE;

#pragma pack(pop)


/// ���������Ϣ������
#define PPL_DECLARE_MESSAGE_MAP() virtual int HandleMSG(MESSAGE& Message);

/// ��ʼ�����Ϣ�������Ķ���
#define PPL_BEGIN_MESSAGE_MAP(thisClass)			\
	int thisClass::HandleMSG(MESSAGE& Message)		\
{												\
	int ret = 0;								\
	switch (Message.Msg)						\
{

/// ������Ϣmsg�Ĵ�����func
#define PPL_MESSAGE_HANDLER(msg, func) case msg: ret = func(Message); break;

/// ���������Ϣ�������Ķ���
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


