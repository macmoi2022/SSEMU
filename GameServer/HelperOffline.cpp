// HelperOffline.cpp: implementation of the CHelperOffline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HelperOffline.h"
#include "Attack.h"
#include "CustomAttack.h"
#include "EffectManager.h"
#include "Helper.h"
#include "ItemLevel.h"
#include "ItemManager.h"
#include "Kalima.h"
#include "Map.h"
#include "MasterSkillTree.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Party.h"
#include "Reconnect.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "SocketManager.h"
#include "Util.h"
#include "Viewport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHelperOffline::CHelperOffline() // OK
{
	
}

CHelperOffline::~CHelperOffline() // OK
{

}

void CHelperOffline::MainProc(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.Started == 0 || lpObj->Helper.Offline == 0)
	{
		return;
	}

	if(lpObj->Live == 0)
	{
		return;
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,MAP_ATTR_SAFE) != 0)
	{
		if(gServerInfo->m_HelperOfflineKeepEnable[lpObj->AccountLevel] == 0)
		{
			this->HelperOfflineClose(lpObj);
		}

		return;
	}

	if(gServerInfo->m_HelperOfflineMaxTimeLimit[lpObj->AccountLevel] > 0)
	{
		if(((DWORD)time(0)) >= ((DWORD)lpObj->Helper.OnlineTime+gServerInfo->m_HelperOfflineMaxTimeLimit[lpObj->AccountLevel]))
		{
			this->HelperOfflineClose(lpObj);
			return;
		}
	}

	this->HelperOfflineUsePotion(lpObj);

	if((++lpObj->Helper.SecondProc) > 5)
	{
		lpObj->Helper.SecondProc = 0;

		this->HelperOfflineReloadArrow(lpObj);

		this->HelperOfflineRepairItems(lpObj);

		this->HelperOfflinePetZenPick(lpObj);
	}

	if(lpObj->Helper.CurrentAction == HELPER_ACTION_BUFF)
	{
		this->HelperOfflineAutoBuff(lpObj);
		
		lpObj->Helper.ActionCount = 0;
		lpObj->Helper.CurrentAction = HELPER_ACTION_MOVE;
	}
	else if (lpObj->Helper.CurrentAction == HELPER_ACTION_MOVE)
	{
		if(this->HelperOfflineAutoMoveOrigin(lpObj) == 0)
		{
			lpObj->Helper.ActionCount = 0;
			lpObj->Helper.CurrentAction = HELPER_ACTION_PICK;
		}
		else
		{
			if((++lpObj->Helper.ActionCount) > 15)
			{
				lpObj->Helper.ActionCount = 0;
				lpObj->Helper.CurrentAction = HELPER_ACTION_PICK;
			}
		}
	}
	else if (lpObj->Helper.CurrentAction == HELPER_ACTION_PICK)
	{
		this->HelperOfflineAutoPick(lpObj);

		if((++lpObj->Helper.ActionCount) > 15)
		{
			lpObj->Helper.ActionCount = 0;
			lpObj->Helper.CurrentAction = HELPER_ACTION_ATTACK;
		}
	}
	else if (lpObj->Helper.CurrentAction == HELPER_ACTION_ATTACK)
	{
		if(this->HelperOfflineCountTarget(lpObj) > 0)
		{
			if(GetTickCount() < lpObj->Helper.AttackTime)
			{
				return;
			}

			if(lpObj->ComboSkill.m_index != -1)
			{
				if(lpObj->ComboSkill.m_time < GetTickCount())
				{
					lpObj->ComboSkill.Init();
				}
			}

			this->HelperOfflineAutoAttack(lpObj);

			if((GetLargeRand()%5) == 0)
			{
				lpObj->Helper.ActionCount = 0;
				lpObj->Helper.CurrentAction = HELPER_ACTION_BUFF;
			}
		}
		else
		{
			lpObj->Helper.ActionCount = 0;
			lpObj->Helper.CurrentAction = HELPER_ACTION_BUFF;
		}
	}

	this->HelperOfflineSupport(lpObj);

	#endif
}

void CHelperOffline::SecondProc(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.Offline != 0)
	{
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->OnlineRewardTime = ((gServerInfo->m_HelperOfflineCoinGain==0)?GetTickCount():lpObj->OnlineRewardTime);
	}

	#endif
}

void CHelperOffline::CommandHelperOffline(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gServerInfo->m_HelperOfflineSwitch == 0)
	{
		return;
	}
	
	if(lpObj->Helper.Started == 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,570);
		return;
	}

	lpObj->Socket = INVALID_SOCKET;

	lpObj->Helper.StartX = lpObj->X;

	lpObj->Helper.StartY = lpObj->Y;

	lpObj->Helper.Offline = 1;

	lpObj->Helper.TimerCoin = GetTickCount();

	lpObj->Helper.OnlineTime = ((DWORD)time(0));

	lpObj->Helper.ActionCount = 0;

	lpObj->Helper.CurrentAction = HELPER_ACTION_BUFF;

	closesocket(lpObj->PerSocketContext->Socket);

	gReconnect->SetReconnectInfo(lpObj);

	#endif
}

void CHelperOffline::HelperOfflineClose(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.Offline != 0)
	{
		gReconnect->GDReconnectInfoRemoveSend(lpObj->Name);
		gObjDel(lpObj->Index);
		lpObj->Helper.Offline = 0;
	}

	#endif
}

void CHelperOffline::HelperOfflineUsePotion(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.AutoPotion == 0)
	{
		return;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26,sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;

	pMsg.TargetSlot = 0xFF;

	pMsg.type = 0;

	for(int n=0;n < 10;n++)
	{
		if(((lpObj->Life*100)/(lpObj->MaxLife+lpObj->AddLife)) < lpObj->Helper.PotPercent)
		{
			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,3),-1):pMsg.SourceSlot);

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,2),-1):pMsg.SourceSlot);

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,1),-1):pMsg.SourceSlot);

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,0),-1):pMsg.SourceSlot);

			if(INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
			{
				gItemManager->CGItemUseRecv(&pMsg,lpObj->Index);
			}
		}
	}

	#endif
}

void CHelperOffline::HelperOfflineReloadArrow(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Class != CLASS_FE)
	{
		return;
	}

	if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index >= GET_ITEM(4,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index < GET_ITEM(5,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index != GET_ITEM(4,15) && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Slot == 0)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index != GET_ITEM(4,7) || lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Durability < 1)
		{
			if(gItemManager->GetInventoryItemCount(lpObj,GET_ITEM(4,7),-1) > 0)
			{
				PMSG_ITEM_MOVE_RECV pMsg;

				pMsg.header.set(0x24,sizeof(pMsg));

				pMsg.SourceFlag = 0;

				pMsg.SourceSlot = gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(4,7),-1);

				pMsg.TargetFlag = 0;

				pMsg.TargetSlot = 1;

				if(INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0) 
				{
					gItemManager->CGItemMoveRecv(&pMsg,lpObj->Index);
					gItemManager->InventoryDelItem(lpObj->Index,pMsg.SourceSlot);
					gItemManager->GCItemDeleteSend(lpObj->Index,pMsg.SourceSlot,1);
				}
			}
		}
	}

	if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index >= GET_ITEM(4,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index < GET_ITEM(5,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index != GET_ITEM(4,7) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Slot == 1)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index != GET_ITEM(4,15) || lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Durability < 1)
		{
			if(gItemManager->GetInventoryItemCount(lpObj,GET_ITEM(4,15),-1) > 0)
			{
				PMSG_ITEM_MOVE_RECV pMsg;

				pMsg.header.set(0x24,sizeof(pMsg));

				pMsg.SourceFlag = 0;

				pMsg.SourceSlot = gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(4,15),-1);

				pMsg.TargetFlag = 0;

				pMsg.TargetSlot = 0;

				if(INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
				{ 
					gItemManager->CGItemMoveRecv(&pMsg,lpObj->Index);
					gItemManager->InventoryDelItem(lpObj->Index,pMsg.SourceSlot);
					gItemManager->GCItemDeleteSend(lpObj->Index,pMsg.SourceSlot,1);
				}
			}
		}
	}

	#endif
}

void CHelperOffline::HelperOfflineRepairItems(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.RepairItem == 0)
	{
		return;
	}

	for(int n=0;n < INVENTORY_WEAR_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->Inventory[n].m_Durability >= lpObj->Inventory[n].m_BaseDurability)
		{
			continue;
		}

		DWORD Money = 0;

		if(gItemManager->RepairItem(lpObj,&lpObj->Inventory[n],n,0,&Money) != 0)
		{
			gObjectManager->CharacterCalcAttribute(lpObj->Index);
		}
	}

	#endif
}

void CHelperOffline::HelperOfflinePetZenPick(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gServerInfo->m_HelperOfflinePickEnable[lpObj->AccountLevel] == 0)
	{
		return;
	}

	if(lpObj->Inventory[INVENTORY_SLOT_HELPER].IsItem() == 0 || lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Durability < 1)
	{
		return;
	}

	if(lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,67) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,80) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,106) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,123)) // Rudolf, Panda, Unicorn, Skeleton
	{
		return;
	}

	for(int n=0;n < MAX_MAP_ITEM;n++)
	{
		CMapItem* lpItem = &gMap[lpObj->Map].m_Item[n];

		if(lpItem->IsItem() == 0)
		{
			continue;
		}

		if(lpItem->m_Give == 1 || lpItem->m_Live == 0)
		{
			continue;
		}

		if(lpItem->m_Index != GET_ITEM(14,15)) // Money
		{
			continue;
		}

		if(gViewport->CheckViewportObjectPosition(lpObj->Index,lpObj->Map,lpItem->m_X,lpItem->m_Y,5) != 0)
		{
			PMSG_ITEM_GET_RECV pMsg;

			pMsg.header.set(0x22, sizeof(pMsg));

			pMsg.index[0] = SET_NUMBERHB(n);
			pMsg.index[1] = SET_NUMBERLB(n);

			gItemManager->CGItemGetRecv(&pMsg,lpObj->Index);
		}
	}

	#endif
}

void CHelperOffline::HelperOfflineAutoBuff(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gServerInfo->m_HelperOfflineBuffEnable[lpObj->AccountLevel] == 0)
	{
		return;
	}

	if(lpObj->Helper.AutoBuff != 0)
	{
		for(int n = 0; n < 3; n++)
		{
			if(lpObj->Helper.Buff[n] == 0)
			{
				continue;
			}

			CSkill* lpSkill = gSkillManager->GetSkill(lpObj,lpObj->Helper.Buff[n]);

			if(lpSkill != 0 && gEffectManager->CheckEffect(lpObj,gSkillManager->GetSkillEffect(lpSkill->m_index)) == 0)
			{
				this->HelperOfflineUseSkill(lpObj,lpObj,lpSkill,1);
			}
		}
	}

	if(lpObj->Class == CLASS_FE && lpObj->Helper.AutoHeal != 0)
	{
		CSkill* lpSkill = gSkillManager->GetSkill(lpObj,SKILL_HEAL);

		if(lpSkill != 0 && ((lpObj->Life*100)/(lpObj->MaxLife+lpObj->AddLife)) < lpObj->Helper.HealPercent)
		{
			this->HelperOfflineUseSkill(lpObj,lpObj,lpSkill,1);
		}
	}

	if(lpObj->Class == CLASS_SU && lpObj->Helper.AutoDrainLife != 0)
	{
		CSkill* lpSkill = gSkillManager->GetSkill(lpObj,SKILL_DRAIN_LIFE);

		if(lpSkill != 0 && ((lpObj->Life*100)/(lpObj->MaxLife+lpObj->AddLife)) < lpObj->Helper.DrainPercent)
		{
			this->HelperOfflineUseSkill(lpObj,lpObj,lpSkill,1);
		}
	}

	#endif
}

void CHelperOffline::HelperOfflineSupport(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gServerInfo->m_HelperOfflineBuffEnable[lpObj->AccountLevel] == 0)
	{
		return;
	}

	if(lpObj->Class != CLASS_FE || lpObj->Helper.Party == 0 || OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return;
	}

	if(lpObj->Helper.PartyAutoHeal != 0)
	{
		CSkill* lpSkill = gSkillManager->GetSkill(lpObj,SKILL_HEAL);

		if(lpSkill != 0)
		{
			for(int n = 0; n < MAX_PARTY_USER; n++)
			{
				int bIndex = gParty->m_PartyInfo[lpObj->PartyNumber].Index[n];

				if(OBJECT_RANGE(bIndex) == 0)
				{
					continue;
				}

				if(lpObj->Index == bIndex)
				{
					continue;
				}

				LPOBJ lpTarget = &gObj[bIndex];

				if(lpTarget->Live == 0 || lpTarget->State != OBJECT_PLAYING || lpTarget->Map != lpObj->Map || gObjCalcDistance(lpObj,lpTarget) > 6)
				{
					continue;
				}

				if(((lpTarget->Life*100)/(lpTarget->MaxLife+lpTarget->AddLife)) < lpObj->Helper.PartyHealPercent)
				{
					this->HelperOfflineUseSkill(lpObj,lpTarget,lpSkill,1);
				}
			}
		}
	}

	if(lpObj->Helper.PartyAutoBuff != 0)
	{
		if((GetTickCount()-lpObj->Helper.BuffPartyTime) >= (DWORD)(lpObj->Helper.PartyBuffTime*1000))
				{
			lpObj->Helper.BuffPartyTime = GetTickCount();

			CSkill* lpSkill = gSkillManager->GetSkill(lpObj,lpObj->Helper.Buff[lpObj->Helper.CurrentPartySkill]);
		
			if(lpSkill != 0)
			{
				if(lpSkill->m_skill != SKILL_GREATER_LIFE && lpSkill->m_skill != SKILL_GREATER_CRITICAL_DAMAGE && lpSkill->m_skill != SKILL_INFINITY_ARROW && lpSkill->m_skill != SKILL_SWORD_POWER && lpSkill->m_skill != SKILL_MAGIC_CIRCLE && lpSkill->m_skill != SKILL_GREATER_IGNORE_DEFENSE_RATE && lpSkill->m_skill != SKILL_FITNESS && lpSkill->m_skill != SKILL_GREATER_DEFENSE_SUCCESS_RATE && lpSkill->m_skill != SKILL_BLOOD_HOWLING)
				{
					for(int n=0;n < MAX_PARTY_USER;n++)
					{
						int bIndex = gParty->m_PartyInfo[lpObj->PartyNumber].Index[n];

						if(OBJECT_RANGE(bIndex) == 0)
						{
							continue;
						}

						if(lpObj->Index == bIndex)
						{
							continue;
						}

						LPOBJ lpTarget = &gObj[bIndex];

						if(lpTarget->Live == 0 || lpTarget->State != OBJECT_PLAYING || lpTarget->Map != lpObj->Map || gObjCalcDistance(lpObj,lpTarget) > 6)
						{
							continue;
						}

						this->HelperOfflineUseSkill(lpObj,lpTarget,lpSkill,1);
					}
				}
			}

			if((++lpObj->Helper.CurrentPartySkill) > 2)
			{
				lpObj->Helper.CurrentPartySkill = 0;
			}
		}
	}

	#endif
}

void CHelperOffline::HelperOfflineAutoPick(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gServerInfo->m_HelperOfflinePickEnable[lpObj->AccountLevel] == 0)
	{
		return;
	}

	if(lpObj->Helper.PickAllItem == 0 && lpObj->Helper.PickSelected == 0)
	{
		return;
	}

	for(int n=0;n<MAX_MAP_ITEM;n++)
	{
		CMapItem* lpItem = &gMap[lpObj->Map].m_Item[n];

		if(lpItem->IsItem() == 0)
		{
			continue;
		}

		if(lpItem->m_Give == 1 || lpItem->m_Live == 0)
		{
			continue;
		}

		if(gViewport->CheckViewportObjectPosition(lpObj->Index,lpObj->Map,lpItem->m_X,lpItem->m_Y,lpObj->Helper.Range[1]) != 0)
		{
			if(gItemManager->CheckItemInventorySpace(lpObj,lpItem->m_Index) == 0)
			{
				continue;
			}

			if(lpObj->Helper.PickAllItem != 0 && lpItem->m_Index != GET_ITEM(14,15))
			{
				this->HelperOfflinePickItem(lpObj,n,lpItem->m_X,lpItem->m_Y);
				continue;
			}

			if(lpObj->Helper.PickSelected != 0)
			{
				if(lpObj->Helper.PickJewel != 0 && gItemManager->IsJewelItem(lpItem->m_Index) != 0)
				{
					this->HelperOfflinePickItem(lpObj,n,lpItem->m_X,lpItem->m_Y);
					continue;
				}
				
				if(lpObj->Helper.PickSet != 0 && lpItem->IsSetItem() != 0)
				{
					this->HelperOfflinePickItem(lpObj,n,lpItem->m_X,lpItem->m_Y);
					continue;
				}

				if(lpObj->Helper.PickExc != 0 && lpItem->IsExcItem() != 0)
				{
					this->HelperOfflinePickItem(lpObj,n,lpItem->m_X,lpItem->m_Y);
					continue;
				}

				if(lpObj->Helper.PickMoney != 0 && lpItem->m_Index == GET_ITEM(14,15))
				{
					this->HelperOfflinePickItem(lpObj,n,lpItem->m_X,lpItem->m_Y);
					continue;
				}

				if(lpObj->Helper.PickExtra != 0)
				{
					char buff[256] = { 0 };

					memcpy(buff,CharToLower(gItemManager->GetItemName(lpItem->m_Index,lpItem->m_Level)),sizeof(buff));

					if(lpItem->m_Level > 0 && gItemLevel->CheckItemlevel(lpItem->m_Index,lpItem->m_Level) == 0)
					{
						wsprintf(buff,"%s +%d",buff,lpItem->m_Level);
					}

					if(lpItem->m_Option1 != 0)
					{
						wsprintf(buff,"%s +skill",buff);
					}

					if(lpItem->m_Option2 != 0)
					{
						wsprintf(buff,"%s +luck",buff);
					}

					if(lpItem->m_Option3 != 0)
					{
						wsprintf(buff,"%s +option",buff);
					}

					if(lpItem->IsSocketItem() != 0)
					{
						wsprintf(buff,"%s +socket%x",buff,GetSocketOptionCount(lpItem->m_SocketOption));
					}

					for(int i=0;i < MAX_HELPER_ITEM;i++) 
					{
						if(strlen(lpObj->Helper.ItemList[i]) < 2)
						{
							continue;
						}

						if(strstr(buff,lpObj->Helper.ItemList[i]) != 0)
						{
							this->HelperOfflinePickItem(lpObj,n,lpItem->m_X,lpItem->m_Y);
							continue;
						}
					}
				}
			}
		}
	}

	#endif
}

bool CHelperOffline::HelperOfflineAutoMoveOrigin(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.OriginalPosition == 0)
	{
		return 0;
	}

	if(lpObj->Helper.CurrentAction != HELPER_ACTION_MOVE)
	{
		return 0;
	}

	if(lpObj->Helper.MoveTime == 0)
	{
		lpObj->Helper.MoveTime = 1;
	}

	if((GetTickCount()-lpObj->Helper.AutoMoveTime) < (DWORD)(lpObj->Helper.MoveTime*1000))
	{
		return 0;
	}

	lpObj->Helper.AutoMoveTime = GetTickCount();

	if(lpObj->Helper.StartX == lpObj->X && lpObj->Helper.StartY == lpObj->Y)
	{
		return 0;
	}

	for(int n = 0; n < 100; n++)
	{
		if(this->HelperOfflineMove(lpObj,lpObj->Helper.StartX,lpObj->Helper.StartY) == 0)
		{
			return 0;
		}
	}

	#endif

	return 1;
}

void CHelperOffline::HelperOfflineAutoAttack(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	for(int n=0;n < 2;++n)
	{
		CSkill* lpSkill = this->HelperOfflineGetSkill(lpObj,lpObj->Helper.Skill[n+1]);

		if(lpSkill == 0 || lpSkill->m_skill == SKILL_HEAL)
		{
			continue;
		}

		if(lpObj->Helper.Combo != 0)
		{
			if(lpObj->ComboSkill.m_index == 0 || lpObj->ComboSkill.m_index == 1)
			{
				if(lpObj->ComboSkill.m_skill[lpObj->ComboSkill.m_index] != lpSkill->m_index)
				{
					if(this->HelperOfflineFindTarget(lpObj,0,0) != 0)
					{
						this->HelperOfflineUseSkill(lpObj,&gObj[lpObj->TargetNumber],lpSkill,0);
					}
				}
			}

			continue;
		}

		if(lpObj->Helper.SkillDelay[n] == 0 && lpObj->Helper.SkillCondition[n] == 0)
		{
			continue;
		}

		if(lpObj->Helper.SkillCondition[n] != 0)
		{
			if(lpObj->Helper.SkillSubCon[n] == 3 && this->HelperOfflineCountTarget(lpObj) < 6)
			{
				continue;
			}
			else if(lpObj->Helper.SkillSubCon[n] == 2 && this->HelperOfflineCountTarget(lpObj) < 5)
			{
				continue;
			}
			else if(lpObj->Helper.SkillSubCon[n] == 1 && this->HelperOfflineCountTarget(lpObj) < 4)
			{
				continue;
			}
			else if(lpObj->Helper.SkillSubCon[n] == 0 && this->HelperOfflineCountTarget(lpObj) < 3)
			{
				continue;
			}
		}

		if(lpObj->Helper.SkillDelay[n] != 0)
		{
			if((GetTickCount()-lpObj->Helper.SkillTimerDelay[n]) < (DWORD)(lpObj->Helper.Delay[n]*1000))
			{
				continue;
			}
		}

		if(this->HelperOfflineFindTarget(lpObj,lpObj->Helper.SkillPreCon[n],0) != 0)
		{
			if(this->HelperOfflineUseSkill(lpObj,&gObj[lpObj->TargetNumber],lpSkill,0) != 0)
			{
				lpObj->Helper.SkillTimerDelay[n] = GetTickCount();
			}
		}
	}

	if(lpObj->Helper.Combo == 0 || lpObj->ComboSkill.m_index == -1)
	{
		CSkill* lpSkill = this->HelperOfflineGetSkill(lpObj,lpObj->Helper.Skill[0]);

		if(lpSkill != 0 && lpSkill->m_skill != SKILL_HEAL)
		{
			if(this->HelperOfflineFindTarget(lpObj,0,0) != 0)
			{
				this->HelperOfflineUseSkill(lpObj,&gObj[lpObj->TargetNumber],lpSkill,0);
			}
		}
	}

	if(lpObj->Helper.RegularAttack != 0)
	{
		if(this->HelperOfflineFindTarget(lpObj,0,0) != 0)
		{
			if(gObjCalcDistance(lpObj,&gObj[lpObj->TargetNumber]) > 2)
			{
				if(this->HelperOfflineMove(lpObj,gObj[lpObj->TargetNumber].X,gObj[lpObj->TargetNumber].Y) != 0)
				{
					lpObj->Helper.AttackTime = GetTickCount()+1000;

					PMSG_ATTACK_RECV pMsg;

					pMsg.header.set(PROTOCOL_CODE2,sizeof(pMsg));

					pMsg.index[0] = SET_NUMBERHB(lpObj->TargetNumber);

					pMsg.index[1] = SET_NUMBERLB(lpObj->TargetNumber);

					pMsg.action = ACTION_ATTACK1;

					pMsg.dir = GetPathPacketDirPos(gObj[lpObj->TargetNumber].X-lpObj->X,gObj[lpObj->TargetNumber].Y-lpObj->Y);

					gAttack->CGAttackRecv(&pMsg,lpObj->Index);

					return;
				}
			}

			
		}
	}

	#endif
}

bool CHelperOffline::HelperOfflinePotionMP(LPOBJ lpObj,int SkillNumber) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gSkillManager->CheckSkillMana(lpObj,SkillNumber) != 0)
	{
		return 1;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26,sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;

	pMsg.TargetSlot = 0xFF;

	pMsg.type = 0;

	for(int n=0;n < 20;n++)
	{
		if(gSkillManager->CheckSkillMana(lpObj,SkillNumber) == 0)
		{
			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,4),-1):pMsg.SourceSlot);

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,5),-1):pMsg.SourceSlot);

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,6),-1):pMsg.SourceSlot);

			if(lpObj->Helper.PotionElite != 0){ pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,71),-1):pMsg.SourceSlot); }

			if(pMsg.SourceSlot == 0xFF) 
			{ 
				break;
			}

			gItemManager->CGItemUseRecv(&pMsg,lpObj->Index);
		}
		else
		{
			return 1;
		}
	}

	return 0;

	#endif
}

bool CHelperOffline::HelperOfflineMove(LPOBJ lpObj,int x,int y) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(gEffectManager->CheckStunEffect(lpObj) != 0 || gEffectManager->CheckImmobilizeEffect(lpObj) != 0)
	{
		return 0;
	}

	if(lpObj->X == x && lpObj->Y == y)
	{
		return 0;
	}

	PATH_INFO path;

	if(gMap[lpObj->Map].PathFinding2(lpObj->X,lpObj->Y,x,y,&path) == 0)
	{
		return 0;
	}

	lpObj->TX = lpObj->X;
	lpObj->TY = lpObj->Y;
	lpObj->MTX= lpObj->X;
	lpObj->MTY = lpObj->Y;
	lpObj->OldX = lpObj->X;
	lpObj->OldY = lpObj->Y;

	gMap[lpObj->Map].DelStandAttr(lpObj->X,lpObj->Y);

	lpObj->PathCur = 1;
	lpObj->PathCount = path.PathNum;
	lpObj->PathStartEnd = 1;
	lpObj->PathX[0] = lpObj->X;
	lpObj->PathY[0] = lpObj->Y;
	lpObj->PathDir[0] = lpObj->Dir;

	for(int n=1;n < lpObj->PathCount;n++)
	{
		lpObj->PathX[n] = path.PathX[n];
		lpObj->PathY[n] = path.PathY[n];
		lpObj->PathDir[n] = GetPathPacketDirPos((lpObj->PathX[n]-lpObj->PathX[(n-1)]),(lpObj->PathY[n]-lpObj->PathY[(n-1)]));
		lpObj->TX += RoadPathTable[(lpObj->PathDir[n]*2)+0];
		lpObj->TY += RoadPathTable[(lpObj->PathDir[n]*2)+1];
		lpObj->MTX += RoadPathTable[(lpObj->PathDir[n]*2)+0];
		lpObj->MTY += RoadPathTable[(lpObj->PathDir[n]*2)+1];
	}

	gMap[lpObj->Map].SetStandAttr(lpObj->TX,lpObj->TY);

	PMSG_MOVE_SEND pMsg;

	pMsg.header.set(PROTOCOL_CODE1,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);

	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	pMsg.x = (BYTE)lpObj->MTX;

	pMsg.y = (BYTE)lpObj->MTY;

	pMsg.dir = lpObj->Dir << 4;

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);

	return 1;

	#else

	return 0;

	#endif
}

void CHelperOffline::HelperOfflinePickItem(LPOBJ lpObj,int index,int x,int y) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	for(int n = 0; n < 50; n++)
	{
		if(gObjCalcDistance(lpObj,x,y) > 2)
		{
			if(this->HelperOfflineMove(lpObj,x,y) == 0)
			{
				break;
			}
		}
	}

	PMSG_ITEM_GET_RECV pMsg;

	pMsg.header.set(0x22,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(index);
	pMsg.index[1] = SET_NUMBERLB(index);

	gItemManager->CGItemGetRecv(&pMsg,lpObj->Index);

	#endif
}

int CHelperOffline::HelperOfflineCountTarget(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE || OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0 || lpObj->VpPlayer2[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		if(gSkillManager->CheckSkillTarget(lpObj,lpObj->VpPlayer2[n].index,-1,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(gObjCalcDistance(lpObj,&gObj[lpObj->VpPlayer2[n].index]) >= lpObj->Helper.Range[0])
		{
			continue;
		}

		count++;
	}

	return count;

	#else

	return 0;

	#endif
}

int CHelperOffline::HelperOfflineFindTarget(LPOBJ lpObj,int self,int another) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	lpObj->TargetNumber = -1;

	int NearestDistance = lpObj->Helper.Range[0];

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE || OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0 || lpObj->VpPlayer2[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		if(another != 0 && lpObj->VpPlayer2[n].index == lpObj->TargetNumber)
		{
			continue;
		}

		if(gMap[lpObj->Map].CheckWall(lpObj->X,lpObj->Y,gObj[lpObj->VpPlayer2[n].index].X,gObj[lpObj->VpPlayer2[n].index].Y) == 0)
		{
			continue;
		}

		if(gSkillManager->CheckSkillTarget(lpObj,lpObj->VpPlayer2[n].index,-1,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(gObjCalcDistance(lpObj,&gObj[lpObj->VpPlayer2[n].index]) >= NearestDistance)
		{
			continue;
		}

		if(self != 0 && gObj[lpObj->VpPlayer2[n].index].TargetNumber != lpObj->Index)
		{
			continue;
		}

		NearestDistance = gObjCalcDistance(lpObj,&gObj[lpObj->VpPlayer2[n].index]);

		lpObj->TargetNumber = lpObj->VpPlayer2[n].index;
	}

	return ((lpObj->TargetNumber==-1)?0:1);

	#else

	return 0;

	#endif
}

CSkill* CHelperOffline::HelperOfflineGetSkill(LPOBJ lpObj,int SkillNumber) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	CSkill* lpSkill = gSkillManager->GetMasterSkill(lpObj,SkillNumber);

	if(lpSkill != 0)
	{
		return lpSkill;
	}

	lpSkill = gSkillManager->GetSkill(lpObj,SkillNumber);

	if(lpSkill != 0)
	{
		return lpSkill;
	}

	return 0;

	#else

	return 0;

	#endif
}

bool CHelperOffline::HelperOfflineUseSkill(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,bool buff)// OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpSkill == 0)
	{
		return 0;
	}

	CUSTOM_ATTACK_SKILL_INFO* lpInfo = gCustomAttack->GetInfo(lpObj->Class,lpSkill->m_skill,1);

	if(lpInfo == 0)
	{
		return 0;
	}

	if(GetTickCount() < lpObj->Helper.AttackTime)
	{
		return 0;
	}

	if(gSkillManager->CheckSkillBP(lpObj,lpSkill->m_index) == 0)
	{
		return 0;
	}

	if(this->HelperOfflinePotionMP(lpObj,lpSkill->m_index) == 0)
	{
		return 0;
	}

	int range = (((range=gSkillManager->GetSkillRange(lpSkill->m_skill))==0)?1:range);

	if(lpObj->Helper.ShortDistance != 0)
	{
		range = 2;
	}

	if(gObjCalcDistance(lpObj,lpTarget) > range)
	{
		if(this->HelperOfflineMove(lpObj,lpTarget->X,lpTarget->Y) != 0)
		{
			return 0;
		}
	}

	if(gMap[lpObj->Map].CheckWall(lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		this->HelperOfflineFindTarget(lpObj,0,1);
		return 0;
	}

	switch(lpInfo->Group)
	{
		case 2:
			gCustomAttack->CustomAttackMultilAttack(lpObj,lpTarget->Index,lpInfo->Index);
			break;
		case 3:
			gCustomAttack->CustomAttackDurationlAttack(lpObj,lpTarget->Index,lpInfo->Index);
			break;
		case 4:
			gCustomAttack->CustomAttackRageAttack(lpObj,lpTarget->Index,lpInfo->Index);
			break;
		default:
			gCustomAttack->CustomAttackSkillAttack(lpObj,lpTarget->Index,lpInfo->Index);
			break;
	}

	if(buff == 0)
	{
		DWORD SkillDelay = gSkillManager->GetSkillDelay(lpSkill->m_skill);

		if(SkillDelay == 0)
		{
			SkillDelay = gCustomAttack->GetAttackDelay(lpObj,lpInfo->Index);
		}

		lpObj->Helper.AttackTime = (GetTickCount()+SkillDelay);
	}

	return 1;

	#else

	return 0;

	#endif
}

int CHelperOffline::GetExperienceRate(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	if(lpObj->Helper.Started != 0)
	{
		if(lpObj->Helper.Offline == 0)
		{
			return gServerInfo->m_HelperExperienceRate[lpObj->AccountLevel];
		}
		else
		{
			return gServerInfo->m_HelperOfflineExperienceRate[lpObj->AccountLevel];
		}
	}
	else
	{
		return 100;
	}

	#else

	return 100;

	#endif
}