// Move.cpp: implementation of the CMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Move.h"
#include "CastleSiege.h"
#include "CustomArena.h"
#include "DefaultClassInfo.h"
#include "DoubleGoer.h"
#include "EffectManager.h"
#include "Gate.h"
#include "GensSystem.h"
#include "IllusionTemple.h"
#include "ImperialGuardian.h"
#include "InvasionManager.h"
#include "ItemManager.h"
#include "Map.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMove::CMove() // OK
{
	this->m_MoveInfo.clear();
}

CMove::~CMove() // OK
{

}

void CMove::Load(char* path) // OK
{
	CReadFile* lpReadFile = new CReadFile;

	if(lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_MoveInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			MOVE_INFO info;

			info.Index = lpReadFile->GetNumber();

			strcpy_s(info.Name,lpReadFile->GetAsString());

			info.Money = lpReadFile->GetAsNumber();

			info.MinLevel = lpReadFile->GetAsNumber();

			info.MaxLevel = lpReadFile->GetAsNumber();

			info.MinReset = lpReadFile->GetAsNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.Enable[0] = lpReadFile->GetAsNumber();

			info.Enable[1] = lpReadFile->GetAsNumber();

			info.Enable[2] = lpReadFile->GetAsNumber();

			info.Enable[3] = lpReadFile->GetAsNumber();

			info.Gate = lpReadFile->GetAsNumber();

			this->m_MoveInfo.insert(std::pair<int,MOVE_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CMove::GetInfo(int index,MOVE_INFO* lpInfo) // OK
{
	std::map<int,MOVE_INFO>::iterator it = this->m_MoveInfo.find(index);

	if(it == this->m_MoveInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CMove::GetInfoByName(char* name,MOVE_INFO* lpInfo) // OK
{
	for(std::map<int,MOVE_INFO>::iterator it=this->m_MoveInfo.begin();it != this->m_MoveInfo.end();it++)
	{
		if(_stricmp(it->second.Name,name) == 0)
		{
			(*lpInfo) = it->second;
			return 1;
		}
	}

	return 0;
}

void CMove::Move(LPOBJ lpObj,int index) // OK
{
	MOVE_INFO MoveInfo;

	if(this->GetInfo(index,&MoveInfo) == 0)
	{
		return;
	}

	if(MoveInfo.MinLevel != -1 && lpObj->Level < gGate->GetMoveLevel(lpObj,gGate->GetGateMap(MoveInfo.Gate),MoveInfo.MinLevel))
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,224,gGate->GetMoveLevel(lpObj,gGate->GetGateMap(MoveInfo.Gate),MoveInfo.MinLevel));
		return;
	}

	if(MoveInfo.MaxLevel != -1 && lpObj->Level > MoveInfo.MaxLevel)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,230,MoveInfo.MaxLevel);
		return;
	}

	if(MoveInfo.MinReset != -1 && lpObj->Reset < MoveInfo.MinReset)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,231,MoveInfo.MinReset);
		return;
	}

	if(MoveInfo.MaxReset != -1 && lpObj->Reset > MoveInfo.MaxReset)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,232,MoveInfo.MaxReset);
		return;
	}

	if(gEffectManager->GetEffect(lpObj,EFFECT_SEAL_OF_MOVEMENT) == 0)
	{
		if(lpObj->Money < ((DWORD)MoveInfo.Money))
		{
			gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,225,MoveInfo.Money);
			return;
		}
	}

	if(MoveInfo.Enable[lpObj->AccountLevel] == 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,226);
		return;
	}

	if(gServerInfo->m_PKLimitFree == 0 && lpObj->PKLevel > 4 && gServerInfo->m_PKLimitMove != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,227);
		return;
	}

	if(lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->Store.Open != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,226);
		return;
	}

	if(gGate->GetGateMap(MoveInfo.Gate) == MAP_ATLANS && (lpObj->Inventory[INVENTORY_SLOT_HELPER].IsItem() != 0 && (lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,2) || lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,3)))) // Uniria,Dinorant
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,228);
		return;
	}

	#if(GAMESERVER_UPDATE>=201)

	if((gGate->GetGateMap(MoveInfo.Gate) == MAP_ICARUS || gGate->GetGateMap(MoveInfo.Gate) == MAP_KANTURU3) && (lpObj->Inventory[INVENTORY_SLOT_WING].IsItem() == 0 && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,3) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,37))) // Dinorant,Fenrir
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,228);
		return;
	}

	#else

	if(gGate->GetGateMap(MoveInfo.Gate) == MAP_ICARUS && lpObj->Inventory[INVENTORY_SLOT_WING].IsItem() == 0 && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,3) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,37)) // Dinorant,Fenrir
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,228);
		return;
	}

	#endif

	#if(GAMESERVER_UPDATE>=501)

	if(lpObj->GensFamily == GENS_FAMILY_NONE && gMapManager->GetMapGensBattle(gGate->GetGateMap(MoveInfo.Gate)) != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,229);
		return;
	}

	#endif

	for(int n=0;n < MAX_SELF_DEFENSE;n++)
	{
		if(lpObj->SelfDefense[n] >= 0)
		{
			if(GetTickCount() < lpObj->SelfDefenseTime[n])
			{
				gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,265);
				return;
			}
		}
	}

	if(gCustomArena->CheckMap(gGate->GetGateMap(MoveInfo.Gate)) == 0 || gCustomArena->CheckEnterEnabled(lpObj,MoveInfo.Gate) != 0)
	{
		if(gObjMoveGate(lpObj->Index,MoveInfo.Gate) != 0)
		{
			lpObj->Store.RedrawAbs = 1;

			if(gEffectManager->GetEffect(lpObj,EFFECT_SEAL_OF_MOVEMENT) == 0)
			{
				lpObj->Money -= MoveInfo.Money;
				GCMoneySend(lpObj->Index,lpObj->Money);
			}
		}
	}
}

void CMove::CGTeleportRecv(PMSG_TELEPORT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(gEffectManager->CheckStunEffect(lpObj) != 0)
	{
		return;
	}

	if(lpObj->Permission[12] != 0 || lpObj->Permission[13] != 0)
	{
		return;
	}

	if(lpMsg->gate == 0)
	{
		//CORREÇÃO DELAY SKILL_TELEPORT
		if (gSkillManager->CheckSkillDelay(lpObj, SKILL_TELEPORT) == 0)
		{
			this->CGTeleportError(aIndex);
			return;
		}

		if(gObjCheckTeleportArea(aIndex,lpMsg->x,lpMsg->y) == 0)
		{
			this->GCTeleportSend(lpObj->Index,0,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,lpObj->Dir);
			return;
		}

		if(lpObj->Map == MAP_CASTLE_SIEGE)
		{
			gCastleSiege->CheckTeleportMagicAxisY(lpObj->X,lpObj->Y,&lpMsg->y);
		}

		CSkill* lpSkill = gSkillManager->GetSkill(lpObj,SKILL_TELEPORT);

		if(lpSkill != 0)
		{
			#if(GAMESERVER_UPDATE>=301)

			if(IT_MAP_RANGE(lpObj->Map) != 0 && gIllusionTemple->GetState(GET_IT_LEVEL(lpObj->Map)) != IT_STATE_START)
			{
				lpMsg->x = (BYTE)lpObj->X;
				lpMsg->y = (BYTE)lpObj->Y;
			}

			#endif

			#if(GAMESERVER_UPDATE>=501)

			if(DG_MAP_RANGE(lpObj->Map) != 0 && gDoubleGoer->GetState() != DG_STATE_START)
			{
				lpMsg->x = (BYTE)lpObj->X;
				lpMsg->y = (BYTE)lpObj->Y;
			}

			#endif

			if(gSkillManager->CheckSkillMana(lpObj,lpSkill->m_index) == 0 || gSkillManager->CheckSkillBP(lpObj,lpSkill->m_index) == 0)
			{
				return;
			}

			gSkillManager->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);

			gObjTeleportMagicUse(aIndex,lpMsg->x,lpMsg->y);

			lpObj->Mana -= (gSkillManager->GetSkillMana(lpSkill->m_index)*lpObj->MPConsumptionRate)/100;

			lpObj->BP -= (gSkillManager->GetSkillBP(lpSkill->m_index)*lpObj->BPConsumptionRate)/100;

			GCManaSend(lpObj->Index,0xFF,(int)lpObj->Mana,lpObj->BP);
		}
	}
	else
	{
		if(gGate->IsInGate(lpObj,lpMsg->gate) == 0)
		{
			gObjClearViewport(lpObj);

			this->GCTeleportSend(lpObj->Index,lpMsg->gate,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,lpObj->Dir);

			gObjectManager->CharacterUpdateMapEffect(lpObj);
		}
		else
		{
			#if(GAMESERVER_UPDATE>=501)

			if(IG_MAP_RANGE(lpObj->Map) == 0)
			{
				gObjMoveGate(aIndex,lpMsg->gate);
			}
			else
			{
				gImperialGuardian->MoveGate(lpObj,lpMsg->gate);
			}

			#else

			gObjMoveGate(aIndex,lpMsg->gate);

			#endif
		}
	}
}

void CMove::CGTeleportMoveRecv(PMSG_TELEPORT_MOVE_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	this->Move(&gObj[aIndex],lpMsg->number);

	#endif
}

void CMove::GCTeleportSend(int aIndex,int gate,BYTE map,BYTE x,BYTE y,BYTE dir) // OK
{
	PMSG_TELEPORT_SEND pMsg;

	pMsg.header.setE(0x1C,sizeof(pMsg));

	pMsg.gate = ((gate>0)?1:gate);
	pMsg.map = map;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.dir = dir;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}
void CMove::CGTeleportError(int aIndex) // OK
{
	PMSG_TELEPORT_RESULT pTeleportResult;

	pTeleportResult.h.c = 0xC3;
	pTeleportResult.h.size = sizeof(pTeleportResult);
	pTeleportResult.h.headcode = 0x1C;
	pTeleportResult.MoveNumber = 0;
	pTeleportResult.MapNumber = gObj[aIndex].Map;
	pTeleportResult.MapX = gObj[aIndex].X;
	pTeleportResult.MapY = gObj[aIndex].Y;
	pTeleportResult.Dir = gObj[aIndex].Dir;

	DataSend(aIndex, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
}