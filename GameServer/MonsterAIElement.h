// MonsterAIElement.h: interface for the CMonsterAIElement class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Map.h"
#include "MonsterAIElementInfo.h"
#include "MonsterAIMovePath.h"

#define MAX_MONSTER_AI_ELEMENT_INFO 100

enum MONSTER_AI_ELEMENT_STATE
{
	MAE_STATE_NORMAL = 0,
	MAE_STATE_MOVE = 1,
	MAE_STATE_ATTACK = 2,
	MAE_STATE_HEAL = 3,
	MAE_STATE_AVOID = 4,
	MAE_STATE_HELP = 5,
	MAE_STATE_SPECIAL = 6,
	MAE_STATE_EVENT = 7,
};

class CMonsterAIElement
{
	CMonsterAIElement();
	virtual ~CMonsterAIElement();
	SingletonInstance(CMonsterAIElement)
public:
	void Load(char* path);
	void Init();
	CMonsterAIElementInfo* FindAIElementInfo(int ElementNumber);
	CMonsterAIElementInfo m_MonsterAIElementInfoArray[MAX_MONSTER_AI_ELEMENT_INFO];
	CMonsterAIMovePath m_MonsterAIMovePath[MAX_MAP];
};

#define gMonsterAIElement SingNull(CMonsterAIElement)