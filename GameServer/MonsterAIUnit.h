// MonsterAIUnit.h: interface for the CMonsterAIUnit class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterAIUnitInfo.h"

#define MAX_MONSTER_AI_UNIT_INFO 100

class CMonsterAIUnit
{
	CMonsterAIUnit();
	virtual ~CMonsterAIUnit();
	SingletonInstance(CMonsterAIUnit)
public:
	void Load(char* path);
	void Init();
	CMonsterAIUnitInfo* FindAIUnitInfo(int UnitNumber);
	CMonsterAIUnitInfo m_MonsterAIUnitInfoArray[MAX_MONSTER_AI_UNIT_INFO];
};

#define gMonsterAIUnit SingNull(CMonsterAIUnit)