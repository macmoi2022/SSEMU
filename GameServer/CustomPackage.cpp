#include "stdafx.h"
#include "CustomPackage.h"
#include "ItemManager.h"
#include "Util.h"

cPackage::cPackage()
{

}

cPackage::~cPackage()
{

}

void cPackage::RecvPackageType(CG_SENDPACKAGE_RECV* Data, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int Type = Data->Type;

	PMSG_ITEM_USE_RECV pMsg;

	switch (Type)
	{
	case 0:
		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 207), -1) : pMsg.SourceSlot);
		pMsg.TargetSlot = 0xFF;
		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
		}
		break;
	case 1:
		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 208), -1) : pMsg.SourceSlot);
		pMsg.TargetSlot = 0xFF;
		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
		}

		break;
	case 2:
		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 209), -1) : pMsg.SourceSlot);
		pMsg.TargetSlot = 0xFF;
		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
		}

		break;
	case 3:
		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 12), -1);

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
		}

		break;
	case 4:
		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = gItemManager->GetInventoryItemSlot(lpObj, GET_ITEM(14, 212), -1);

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager->CGItemUseRecv(&pMsg, lpObj->Index);
		}

		break;
	}
}
