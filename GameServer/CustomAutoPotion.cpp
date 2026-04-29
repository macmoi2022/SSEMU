#include "stdafx.h"
#include "CommandManager.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Viewport.h"
#include "CustomAutoPotion.h"

CAUTOHP gAutoHP;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAUTOHP::CAUTOHP() // OK
{
}

CAUTOHP::~CAUTOHP() // OK
{

}

void CAUTOHP::AutoHp(LPOBJ lpObj) // OK
{

	if (lpObj->AUTOHP == 0)
	{
		return;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26, sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 3), -1) : pMsg.SourceSlot);
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 2), -1) : pMsg.SourceSlot);
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 1), -1) : pMsg.SourceSlot);
	pMsg.TargetSlot = 0xFF;
	pMsg.type = 0;

	if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
	{
		gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
	}
}

void CAUTOHP::AutoMana(LPOBJ lpObj) // OK
{

	if (lpObj->AUTOHP == 0)
	{
		return;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26, sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 4), -1) : pMsg.SourceSlot);
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 5), -1) : pMsg.SourceSlot);
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 6), -1) : pMsg.SourceSlot);
	pMsg.TargetSlot = 0xFF;
	pMsg.type = 0;


	if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
	{
		gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
	}
}

void CAUTOHP::AutoAntidote(LPOBJ lpObj) // OK
{

	if (lpObj->AUTOHP == 0)
	{
		return;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26, sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 8), -1) : pMsg.SourceSlot);
	pMsg.TargetSlot = 0xFF;
	pMsg.type = 0;

	if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
	{
		gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
	}
}

void CAUTOHP::AutoComplex(LPOBJ lpObj) // OK
{

	if (lpObj->AUTOHP == 0)
	{
		return;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26, sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 38), -1) : pMsg.SourceSlot);
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 39), -1) : pMsg.SourceSlot);
	pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 40), -1) : pMsg.SourceSlot);
	pMsg.TargetSlot = 0xFF;
	pMsg.type = 0;

	if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
	{
		gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
	}
}

void CAUTOHP::MainProc() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			gAutoHP.AutoHp(&gObj[n]);
			gAutoHP.AutoMana(&gObj[n]);
			gAutoHP.AutoAntidote(&gObj[n]);

			if (GAMESERVER_UPDATE >= 201)
			{
				gAutoHP.AutoComplex(&gObj[n]);
			}
		}
	}
}

