// MonsterSkillUnit.h: interface for the CMonsterSkillUnit class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterSkillUnitInfo.h"

#define MAX_MONSTER_SKILL_UNIT_INFO_ARRAY 200

class CMonsterSkillUnit
{
	CMonsterSkillUnit();
	virtual ~CMonsterSkillUnit();
	SingletonInstance(CMonsterSkillUnit)
public:
	void Load(char* path);
	void Init();
	CMonsterSkillUnitInfo* FindSkillUnitInfo(int UnitNumber);
	CMonsterSkillUnitInfo m_MonsterSkillUnitInfoArray[MAX_MONSTER_SKILL_UNIT_INFO_ARRAY];
};

#define gMonsterSkillUnit SingNull(CMonsterSkillUnit)