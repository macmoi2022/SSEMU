// Monster.h: interface for the CMonster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CMonster
{
	CMonster();
	virtual ~CMonster();
	SingletonInstance(CMonster)
public:
	void MainProc(LPOBJ lpObj);
	void ActionProc(LPOBJ lpObj);
	void ObjectStateProc(LPOBJ lpObj,int code,int aIndex,int SubCode);
	int AddMonster(int map);
	int AddSummon();
	bool RegenMonster(LPOBJ lpObj);
	bool SetMonster(int aIndex,int MonsterClass);
	bool SetPosition(int aIndex,int index);
	int SummonSetEnemy(LPOBJ lpObj,int aIndex);
	void SummonKill(int aIndex);
	void FindTarget(LPOBJ lpObj);
	void TrapFindTarget(LPOBJ lpObj);
	void GetXYToPatrol(LPOBJ lpObj);
	bool GetXYToChase(LPOBJ lpObj);
	void GetXYToOwner(LPOBJ lpObj);
	void FindPathToMove(LPOBJ lpObj);
	bool MoveCheck(LPOBJ lpObj,int tx,int ty);
	bool CheckImmobilize(LPOBJ lpObj);
	bool CheckViewport(int aIndex,int bIndex);
	void InitHitDamage(LPOBJ lpObj);
	void SetHitDamage(LPOBJ lpObj,int aIndex,int damage);
	int DelHitDamageUser(LPOBJ lpObj);
	int GetTopHitDamageUser(LPOBJ lpObj);
	int GetTopHitDamageParty(LPOBJ lpObj,int PartyNumber,int* TopHitDamageUser);
	void DropItem(LPOBJ lpObj,LPOBJ lpTarget);
	void MonsterDie(LPOBJ lpObj,LPOBJ lpTarget);
	void MonsterMultilAttack(LPOBJ lpObj,int skill);
	void MonsterSkillAttack(LPOBJ lpObj,LPOBJ lpTarget);
	void ProcessHit(LPOBJ lpObj,int aIndex);
	int MonsterCount(int index,int map,int x,int y,int tx,int ty);
};

#define gMonster SingNull(CMonster)