// HackMoveSpeedCheck.cpp: implementation of the CHackMoveSpeedCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HackMoveSpeedCheck.h"
#include "ItemManager.h"
#include "Log.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHackMoveSpeedCheck::CHackMoveSpeedCheck() // OK
{

}

CHackMoveSpeedCheck::~CHackMoveSpeedCheck() // OK
{

}

void CHackMoveSpeedCheck::MainProc(LPOBJ lpObj) // OK
{
	if(gServerInfo->m_CheckMoveHack == 0)
	{
		return;
	}

	if(lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->Live == 0)
	{
		return;
	}

	if((GetTickCount()-lpObj->MoveHack.time) > (DWORD)gServerInfo->m_CheckMoveHackMaxDelay)
	{
		lpObj->MoveHack.time = GetTickCount();

		int MaxDistance = gServerInfo->m_CheckMoveHackMaxCount;

		if(lpObj->Inventory[INVENTORY_SLOT_HELPER].IsItem() != 0 && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,37))
		{
			MaxDistance += 2;
		}
		
		int Distance = gObjCalcDistance(lpObj,lpObj->MoveHack.X,lpObj->MoveHack.Y);

		if(Distance > MaxDistance)
		{
			gLog->Output(LOG_HACK,"[HackMoveCheck][%s][%s] Move count error (Map: %d, Count: [%d][%d]",lpObj->Account,lpObj->Name,lpObj->Map,Distance,MaxDistance);

			if(gServerInfo->m_CheckMoveHackAction == 1)
			{
				GCNewMessageSend(lpObj->Index,0,0,719);
			}
			else if(gServerInfo->m_CheckMoveHackAction == 2)
			{
				gObjUserKill(lpObj->Index);
			}
		}

		lpObj->MoveHack.X = lpObj->X;

		lpObj->MoveHack.Y = lpObj->Y;
	}
}

void CHackMoveSpeedCheck::Reset(LPOBJ lpObj) // OK
{
	lpObj->MoveHack.X = lpObj->X;

	lpObj->MoveHack.Y = lpObj->Y;

	lpObj->MoveHack.time = GetTickCount();
}