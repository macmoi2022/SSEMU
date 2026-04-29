// Helper.cpp: implementation of the CHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Helper.h"
#include "CastleSiege.h"
#include "GameMain.h"
#include "Map.h"
#include "MapManager.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHelper::CHelper() // OK
{

}

CHelper::~CHelper() // OK
{

}

void CHelper::MainProc() // OK
{
	#if(GAMESERVER_UPDATE>=603)

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		if(lpObj->Helper.Started == 0 || lpObj->Helper.Offline != 0)
		{
			continue;
		}

		if(lpObj->Live == 0)
		{
			continue;
		}

		if(gMapManager->GetMapHelperEnable(lpObj->Map) == 0)
		{
			this->DisableHelper(lpObj,1);
			continue;
		}

		if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,MAP_ATTR_SAFE) != 0)
		{
			this->DisableHelper(lpObj,1);
			continue;
		}

		DWORD time = GetTickCount();

		if((time-lpObj->Helper.TimerMoney) >= ((DWORD)(gServerInfo->m_HelperActiveDelay*60000)))
		{
			DWORD money = (lpObj->Level+lpObj->MasterLevel)*gServerInfo->m_HelperActiveMoney[lpObj->Helper.Stage];

			if(lpObj->Money < money)
			{
				this->DisableHelper(lpObj,2);
				continue;
			}

			lpObj->Helper.TimerMoney = time;

			lpObj->Money -= money;

			GCMoneySend(lpObj->Index,lpObj->Money);

			this->GCHelperStartSend(lpObj->Index,((time-lpObj->Helper.TimerStage)/60000),money,0);
		}

		if((time-lpObj->Helper.TimerStage) > 12000000)
		{
			lpObj->Helper.TimerMoney = time;

			lpObj->Helper.TimerStage = time;

			if((++lpObj->Helper.Stage) >= MAX_HELPER_STAGE)
			{
				this->DisableHelper(lpObj,1);
				continue;
			}
		}
	}

	#endif
}

void CHelper::EnableHelper(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.Started != 0)
	{
		return;
	}

	if(gMapManager->GetMapHelperEnable(lpObj->Map) == 0)
	{
		return;
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,MAP_ATTR_SAFE) != 0)
	{
		return;
	}

	if(lpObj->Level < gServerInfo->m_HelperActiveLevel)
	{
		return;
	}

	DWORD money = (lpObj->Level+lpObj->MasterLevel)*gServerInfo->m_HelperActiveMoney[0];

	if(lpObj->Money > money)
	{
		lpObj->Money -= money;

		GCMoneySend(lpObj->Index,lpObj->Money);

		lpObj->Helper.Stage = 0;

		lpObj->Helper.TimerCoin = GetTickCount();

		lpObj->Helper.TimerMoney = GetTickCount();

		lpObj->Helper.TimerStage = GetTickCount();

		lpObj->Helper.StartX = lpObj->X;

		lpObj->Helper.StartY = lpObj->Y;

		lpObj->Helper.Started = 1;

		this->GCHelperStartSend(lpObj->Index,0,money,0);
	}
	else
	{
		this->GCHelperStartSend(lpObj->Index,0,0,2);
	}

	#endif
}

void CHelper::DisableHelper(LPOBJ lpObj,int result) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.Started == 0)
	{
		return;
	}

	lpObj->Helper.Started = 0;

	this->GCHelperStartSend(lpObj->Index,0,0,result);

	#endif
}

void CHelper::HelperInfoData(int aIndex,BYTE* lpMsg) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	//LogAdd(LOG_BLUE, "================================================================================================================");
	lpObj->Helper.Range[0]				=  *(BYTE*)(lpMsg+0x01) & 0x0F;
	lpObj->Helper.LongDistance			= (*(DWORD*)(lpMsg+0x18) >> 3) & 1;
	lpObj->Helper.OriginalPosition		= (*(DWORD*)(lpMsg+0x18) >> 4) & 1;
	lpObj->Helper.MoveTime				=  *(WORD*)(lpMsg+0x02);
	lpObj->Helper.Combo					= (*(DWORD*)(lpMsg+0x18) >> 5) & 1;
	//LogAdd(LOG_BLACK, "[Hunting Range: %d] [LongDistance: %d] [OriginalPosition: %d] [MoveTime: %d] [Combo: %d]", lpObj->Helper.Range[0],lpObj->Helper.LongDistance, lpObj->Helper.OriginalPosition, lpObj->Helper.MoveTime, lpObj->Helper.Combo);
	lpObj->Helper.Skill[0]				= *(WORD*)(lpMsg+0x04);
	//LogAdd(LOG_BLACK, "[Basic Skill: %d]",lpObj->Helper.Skill[0]);
	lpObj->Helper.Skill[1]				= *(WORD*)(lpMsg+0x06);
	lpObj->Helper.SkillDelay[0]			= (*(DWORD*)(lpMsg+0x18) >> 11) & 1;
	lpObj->Helper.SkillCondition[0]		= (*(DWORD*)(lpMsg+0x18) >> 12) & 1;
	lpObj->Helper.SkillPreCon[0]		= ((*(DWORD*)(lpMsg+0x18) >> 13) & 1) % 2;
	lpObj->Helper.SkillSubCon[0]		= ((*(DWORD*)(lpMsg+0x18) >> 8) >> 6) % 4;
	lpObj->Helper.Delay[0]				= *(WORD*)(lpMsg+0x08);
	//LogAdd(LOG_BLACK, "[Activation 1 Skill: %d, Delay: %d, DelayMin: %d, Condition: %d, Pre: %d, Sub: %d]", lpObj->Helper.Skill[1], lpObj->Helper.SkillDelay[0], lpObj->Helper.Delay[0], lpObj->Helper.SkillCondition[0], lpObj->Helper.SkillPreCon[0], lpObj->Helper.SkillSubCon[0]);
	lpObj->Helper.Skill[2]				= *(WORD*)(lpMsg+0x0A);
	lpObj->Helper.SkillDelay[1]			= (*(DWORD*)(lpMsg+0x18) >> 16) & 1;
	lpObj->Helper.SkillCondition[1]		= (*(DWORD*)(lpMsg+0x18) >> 17) & 1;
	lpObj->Helper.SkillPreCon[1]		= ((*(DWORD*)(lpMsg+0x18) >> 18) & 1) % 2;
	lpObj->Helper.SkillSubCon[1]		= ((*(DWORD*)(lpMsg+0x18) >> 13) >> 6) % 4;
	lpObj->Helper.Delay[1]				= *(WORD*)(lpMsg+0x0C);
	//LogAdd(LOG_BLACK, "[Activation 2 Skill: %d, Delay: %d, DelayMin: %d, Condition: %d, Pre: %d, Sub: %d]", lpObj->Helper.Skill[2], lpObj->Helper.SkillDelay[1], lpObj->Helper.Delay[1], lpObj->Helper.SkillCondition[1], lpObj->Helper.SkillPreCon[1], lpObj->Helper.SkillSubCon[1]);
	lpObj->Helper.AutoBuff				= (*(DWORD*)(lpMsg+0x18) >> 10) & 1;
	lpObj->Helper.Buff[0]				= *(WORD*)(lpMsg+0x10);
	lpObj->Helper.Buff[1]				= *(WORD*)(lpMsg+0x12);
	lpObj->Helper.Buff[2]				= *(WORD*)(lpMsg+0x14);
	//LogAdd(LOG_BLACK, "[AutoBuff: %d] [Buff 1: %d] [Buff 2: %d] [Buff 3: %d]",lpObj->Helper.AutoBuff,lpObj->Helper.Buff[0],lpObj->Helper.Buff[1],lpObj->Helper.Buff[2]);
	lpObj->Helper.AutoPotion			= *(DWORD*)(lpMsg+0x18) & 1;
	lpObj->Helper.PotPercent			= (*(WORD*)(lpMsg+0x16) & 0x0F) * 10;
	//LogAdd(LOG_BLACK, "[AutoPotion: %d] [PotPercent: %d]",lpObj->Helper.AutoPotion,lpObj->Helper.PotPercent);
	lpObj->Helper.AutoHeal				= (*(DWORD*)(lpMsg+0x18) >> 1) & 1;
	lpObj->Helper.HealPercent			= ((*(WORD*)(lpMsg+0x16) >> 4) & 0x0F) * 10;
	//LogAdd(LOG_BLACK, "[AutoHeal: %d] [PotPercent: %d]",lpObj->Helper.AutoHeal,lpObj->Helper.HealPercent);
	lpObj->Helper.AutoDrainLife			= (*(DWORD*)(lpMsg+0x18) >> 2) & 1;
	lpObj->Helper.DrainPercent			= ((*(WORD*)(lpMsg+0x16) >> 8) & 0x0F) * 10;
	//LogAdd(LOG_BLACK, "[AutoDrainLife: %d] [DrainPercent: %d]",lpObj->Helper.AutoDrainLife,lpObj->Helper.DrainPercent);
	lpObj->Helper.Party					= (*(DWORD*)(lpMsg+0x18) >> 6) & 1;
	lpObj->Helper.PartyAutoHeal			= (*(DWORD*)(lpMsg+0x18) >> 7) & 1;
	lpObj->Helper.PartyHealPercent		= ((*(WORD*)(lpMsg+0x16) >> 12) & 0x0F) * 10;
	lpObj->Helper.PartyAutoBuff			= (*(DWORD*)(lpMsg+0x18) >> 8) & 1;
	lpObj->Helper.PartyBuffTime			= *(WORD*)(lpMsg+0x0E);
	//LogAdd(LOG_BLACK, "[Party: %d] [PartyAutoHeal: %d] [PartyHealPercent: %d] [PartyAutoBuff: %d] [PartyBuffTime: %d]",lpObj->Helper.Party,lpObj->Helper.PartyAutoHeal,lpObj->Helper.PartyHealPercent,lpObj->Helper.PartyAutoBuff,lpObj->Helper.PartyBuffTime);
	lpObj->Helper.Range[1]				= (*(WORD*)(lpMsg+0x01) >> 4) & 0x0F;
	lpObj->Helper.RepairItem			= (*(DWORD*)(lpMsg+0x18) >> 21) & 1;
	lpObj->Helper.PickAllItem			= (*(DWORD*)(lpMsg+0x18) >> 22) & 1;
	lpObj->Helper.PickSelected			= (*(DWORD*)(lpMsg+0x18) >> 23) & 1;
	lpObj->Helper.PickJewel				= (*(WORD*)(lpMsg+0x00) >> 3) & 1;
	lpObj->Helper.PickExc				= (*(WORD*)(lpMsg+0x00) >> 4) & 1;
	lpObj->Helper.PickSet				= (*(WORD*)(lpMsg+0x00) >> 5) & 1;
	lpObj->Helper.PickMoney				= (*(WORD*)(lpMsg+0x00) >> 6) & 1;
	lpObj->Helper.PickExtra				= (*(WORD*)(lpMsg+0x00) >> 7) & 1;
	//LogAdd(LOG_BLACK, "[Range: %d] [Repair: %d] [PickAll: %d] [PickSel: %d] [PickJewel: %d] [PickExc: %d] [PickSet: %d] [PickZen: %d] [PickExtra: %d]", lpObj->Helper.Range[1],lpObj->Helper.RepairItem, lpObj->Helper.PickAllItem, lpObj->Helper.PickSelected, lpObj->Helper.PickJewel, lpObj->Helper.PickExc, lpObj->Helper.PickSet, lpObj->Helper.PickMoney, lpObj->Helper.PickExtra);
	lpObj->Helper.DarkSpirit			= (*(DWORD*)(lpMsg+0x18) >> 9) & 1;
	lpObj->Helper.DarkSpiritAuto		= (*(DWORD*)(lpMsg+0x18) >> 24) & 1;
	lpObj->Helper.DarkSpiritAttack		= (*(DWORD*)(lpMsg+0x18) >> 25) & 1;
	//LogAdd(LOG_BLACK, "[DarkSpirit: %d] [DarkSpiritAuto: %d] [DarkSpiritAttack: %d]",lpObj->Helper.DarkSpirit,lpObj->Helper.DarkSpiritAuto,lpObj->Helper.DarkSpiritAttack);
	lpObj->Helper.AutoAcceptFriend		= (*(DWORD*)(lpMsg+0x18) >> 26) & 1;
	lpObj->Helper.AutoAcceptGuild		= (*(DWORD*)(lpMsg+0x18) >> 27) & 1;
	lpObj->Helper.PotionElite			= (*(DWORD*)(lpMsg+0x18) >> 28) & 1;
	lpObj->Helper.ShortDistance			= (*(DWORD*)(lpMsg+0x18) >> 29) & 1;
	lpObj->Helper.RegularAttack			= (*(DWORD*)(lpMsg+0x18) >> 30) & 1;

	memset(lpObj->Helper.ItemList,0,sizeof(lpObj->Helper.ItemList));
	for(int n=0;n < MAX_HELPER_ITEM;n++) 
	{
		memcpy(lpObj->Helper.ItemList[n],CharToLower((char*)(lpMsg+0x40+n*0x10)),sizeof(lpObj->Helper.ItemList[n]));
		//LogAdd(LOG_BLUE,"Obtaining List[%d] : %s",n, lpObj->Helper.ItemList[n]);
	}
	//LogAdd(LOG_BLUE, "================================================================================================================");

	#endif
}

void CHelper::CGHelperDataRecv(PMSG_HELPER_DATA_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	this->GDHelperDataSaveSend(aIndex,lpMsg->data);

	this->HelperInfoData(aIndex,lpMsg->data);

	#endif
}

void CHelper::CGHelperStartRecv(PMSG_HELPER_START_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gServerInfo->m_HelperSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Attack.Started != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,571);
		return;
	}

	if(lpMsg->type == 0)
	{
		this->EnableHelper(lpObj);
	}
	else
	{
		lpObj->Helper.Started = 0;

		this->GCHelperStartSend(aIndex,0,0,1);
	}

	#endif
}

void CHelper::GCHelperStartSend(int aIndex,int time,int money,int result) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	PMSG_HELPER_START_SEND pMsg;

	pMsg.header.set(0xBF,0x51,sizeof(pMsg));

	pMsg.time = time;

	pMsg.money = money;

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CHelper::DGHelperDataRecv(SDHP_HELPER_DATA_RECV* lpMsg) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGHelperDataRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	PMSG_HELPER_DATA_SEND pMsg;

	pMsg.header.set(0xAE,sizeof(pMsg));

	pMsg.result = lpMsg->result;

	memcpy(pMsg.data,lpMsg->data,sizeof(pMsg.data));

	DataSend(lpMsg->index,(BYTE*)&pMsg,sizeof(pMsg));

	this->HelperInfoData(lpMsg->index,lpMsg->data);

	#endif
}

void CHelper::GDHelperDataSend(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_HELPER_DATA_SEND pMsg;

	pMsg.header.set(0x17,0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CHelper::GDHelperDataSaveSend(int aIndex,BYTE* data) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	SDHP_HELPER_DATA_SAVE_SEND pMsg;

	pMsg.header.set(0x17,0x30,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	memcpy(pMsg.data,data,sizeof(pMsg.data));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));

	#endif
}