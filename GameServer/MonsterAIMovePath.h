// MonsterAIMovePath.h: interface for the CMonsterAIMovePath class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterAIMovePathInfo.h"

#define MAX_MONSTER_AI_MOVE_PATH_INFO 300

class CMonsterAIMovePath
{
public:
	CMonsterAIMovePath();
	virtual ~CMonsterAIMovePath();
	void Load(char* path);
	void Init();
	void GetMonsterNearestPath(int sx,int sy,int tx,int ty,int* ox,int* oy,int NearestDistance);
	CMonsterAIMovePathInfo m_MonsterAIMovePathInfoArray[MAX_MONSTER_AI_MOVE_PATH_INFO];
	int m_MonsterAIMovePathInfoCount;
	bool m_loaded;
};