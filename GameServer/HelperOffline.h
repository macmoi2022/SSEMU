// HelperOffline.h: interface for the CHelperOffline class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

enum eHelperStage
{
	HELPER_ACTION_BUFF = 0,
	HELPER_ACTION_MOVE = 1,
	HELPER_ACTION_PICK = 2,
	HELPER_ACTION_ATTACK = 3,
};

class CHelperOffline
{
	CHelperOffline();
	virtual ~CHelperOffline();
	SingletonInstance(CHelperOffline)
public:
	void MainProc(LPOBJ lpObj);
	void SecondProc(LPOBJ lpObj);
	void CommandHelperOffline(LPOBJ lpObj);
	void HelperOfflineClose(LPOBJ lpObj);
	void HelperOfflineUsePotion(LPOBJ lpObj);
	void HelperOfflineReloadArrow(LPOBJ lpObj);
	void HelperOfflineRepairItems(LPOBJ lpObj);
	void HelperOfflinePetZenPick(LPOBJ lpObj);
	void HelperOfflineAutoBuff(LPOBJ lpObj);
	void HelperOfflineSupport(LPOBJ lpObj);
	void HelperOfflineAutoPick(LPOBJ lpObj);
	bool HelperOfflineAutoMoveOrigin(LPOBJ lpObj);
	void HelperOfflineAutoAttack(LPOBJ lpObj);
	bool HelperOfflinePotionMP(LPOBJ lpObj,int SkillNumber);
	bool HelperOfflineMove(LPOBJ lpObj,int x,int y);
	void HelperOfflinePickItem(LPOBJ lpObj,int index,int x,int y);
	int HelperOfflineCountTarget(LPOBJ lpObj);
	int HelperOfflineFindTarget(LPOBJ lpObj,int self,int another);
	CSkill* HelperOfflineGetSkill(LPOBJ lpObj,int SkillNumber);
	bool HelperOfflineUseSkill(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,bool buff);
	int GetExperienceRate(LPOBJ lpObj);
};

#define gHelperOffline SingNull(CHelperOffline)