// MonsterAIGroup.h: interface for the CMonsterAIGroup class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterAIGroupMember.h"

#define MAX_MONSTER_AI_GROUP 50
#define MAX_MONSTER_AI_GROUP_MEMBER 100

class CMonsterAIGroup
{
	CMonsterAIGroup();
	virtual ~CMonsterAIGroup();
	SingletonInstance(CMonsterAIGroup)
public:
	void Load(char* path);
	void ChangeAIOrder(int GroupNumber,int AIOrder);
	BOOL DelGroupInstance(int GroupNumber);
	void DelAllGroupInfo();
	int FindGroupLeader(int GroupNumber);
	int FindGroupMemberObjectIndex(int GroupNumber,int guid);
	CMonsterAIGroupMember* FindGroupMember(int GroupNumber,int guid);
	CMonsterAIGroupMember* FindGroupMemberToHeal(int ObjIndex,int GroupNumber,int guid,int distance,int rate);
	CMonsterAIGroupMember* FindGroupMemberToSommon(int ObjIndex,int GroupNumber,int guid);
	void InitAll();
	void Init(int GroupNumber);
	CMonsterAIGroupMember m_MonsterAIGroupMemberArray[MAX_MONSTER_AI_GROUP][MAX_MONSTER_AI_GROUP_MEMBER];
	int m_MonsterAIGroupMemberCount[MAX_MONSTER_AI_GROUP];
};

#define gMonsterAIGroup SingNull(CMonsterAIGroup)