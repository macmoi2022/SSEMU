//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct NPC_TYPE_INFO
{
	int Index;
	int MonsterClass;
	int Map;
	int X;
	int Y;
	char Command[128];
};

class CCustomNpcCommand
{

	CCustomNpcCommand();
	virtual ~CCustomNpcCommand();
	SingletonInstance(CCustomNpcCommand)
public:
	void Load(char* path);
	bool GetNpcCommand(LPOBJ lpObj, LPOBJ lpNpc);
private:
	std::map<int, NPC_TYPE_INFO> m_CustomNpcCommand;
};

#define gCustomNpcCommand SingNull(CCustomNpcCommand)
