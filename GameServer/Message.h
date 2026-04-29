// Message.h: interface for the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_LANGUAGE 3

struct MESSAGE_INFO
{
	int Index;
	char Message[128];
};

class CMessage
{
	CMessage();
	virtual ~CMessage();
	SingletonInstance(CMessage)
public:
	void Load(char* path,int lang);
	char* GetMessage(int index);
	char* GetText(int index,int lang);
private:
	char m_DefaultMessage[128];
	std::map<int,MESSAGE_INFO> m_MessageInfo[MAX_LANGUAGE];
};

#define gMessage SingNull(CMessage)