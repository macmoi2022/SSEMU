#pragma once

#include "User.h"

struct RESPAWN_MANAGER_INFO
{
	int m_GateMap;
	int m_GateAccLevel[MAX_ACCOUNT_LEVEL];
};

class CCustomRespawn
{

	CCustomRespawn();
	virtual ~CCustomRespawn();
	SingletonInstance(CCustomRespawn)
public:
	void Load(char* Path);
	int GetGate(LPOBJ lpObj, int MapIndex);
	int GetRespawnLocation(LPOBJ lpObj);
private:
	std::vector<RESPAWN_MANAGER_INFO> m_Info;
};

#define gCustomRespawn SingNull(CCustomRespawn)