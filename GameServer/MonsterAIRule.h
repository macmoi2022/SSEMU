// MonsterAIRule.h: interface for the CMonsterAIRule class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterAIRuleInfo.h"
#include "MonsterManager.h"

#define MAX_MONSTER_AI_RULE_TABLE MAX_MONSTER_INFO
#define MAX_MONSTER_AI_RULE_INFO 200

class CMonsterAIRule
{
	CMonsterAIRule();
	virtual ~CMonsterAIRule();
	SingletonInstance(CMonsterAIRule)
public:
	void Load(char* path);
	void Init();
	int GetCurrentAIUnit(int MonsterClass);
	void MonsterAIRuleProc();
	int m_MonsterCurrentAIUnitTable[MAX_MONSTER_AI_RULE_TABLE];
	CMonsterAIRuleInfo m_MonsterAIRuleInfoArray[MAX_MONSTER_AI_RULE_INFO];
	int m_MonsterAIRuleInfoCount;
};

#define gMonsterAIRule SingNull(CMonsterAIRule)