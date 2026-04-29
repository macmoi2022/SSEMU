// JewelMix.cpp: implementation of the CJewelMix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomJewelBank.h"
#include "CommandManager.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomJewelBank::CCustomJewelBank() // OK
{

}

CCustomJewelBank::~CCustomJewelBank() // OK
{

}

int CCustomJewelBank::GetJewelSimpleType(int ItemIndex) // OK
{
	switch (ItemIndex)
	{
	case GET_ITEM(12, 15): // Chaos
		return 0;
	case GET_ITEM(14, 16): // Life
		return 1;
	case GET_ITEM(14, 14): // Soul
		return 2;
	case GET_ITEM(14, 23): // Bless
		return 3;
	case GET_ITEM(14, 22): // Creation
		return 4;
	case GET_ITEM(14, 31): // Guardian
		return 5;
	case GET_ITEM(14, 42): // Harmony
		return 6;
	case GET_ITEM(14, 41): // Gemstone
		return 7;
	case GET_ITEM(14, 43): // Lower refining stone
		return 8;
	case GET_ITEM(14, 44): // Higher refining stone
		return 9;
	}

	return -1;
}

int CCustomJewelBank::GetJewelSimpleIndex(int type) // OK
{
	switch (type)
	{
	case 0:
		return GET_ITEM(12, 15); // Chaos
	case 1:
		return GET_ITEM(14, 16); // Life
	case 2:
		return GET_ITEM(14, 14); // Soul
	case 3:
		return GET_ITEM(14, 13); // Bless
	case 4:
		return GET_ITEM(14, 22); // Creation
	case 5:
		return GET_ITEM(14, 31); // Guardian
	case 6:
		return GET_ITEM(14, 42); // Harmony
	case 7:
		return GET_ITEM(14, 41); // Gemstone
	case 8:
		return GET_ITEM(14, 43); // Lower refining stone
	case 9:
		return GET_ITEM(14, 44); // Higher refining stone
	}

	return -1;
}

int CCustomJewelBank::GetJewelBundleIndex(int type) // OK
{
	switch (type)
	{
	case 0:
		return GET_ITEM(12, 30);
	case 1:
		return GET_ITEM(12, 31);
	case 2:
		return GET_ITEM(12, 136);
	case 3:
		return GET_ITEM(12, 137);
	case 4:
		return GET_ITEM(12, 138);
	case 5:
		return GET_ITEM(12, 139);
	case 6:
		return GET_ITEM(12, 140);
	case 7:
		return GET_ITEM(12, 141);
	case 8:
		return GET_ITEM(12, 142);
	case 9:
		return GET_ITEM(12, 143);
	}

	return -1;
}

void CCustomJewelBank::JewelBankRecv(PSBMSG_JEWELBANK_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int Slot = lpMsg->slot;

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Interface.use != 0)
	{
		return;
	}

	if (lpObj->ChaosLock != 0)
	{
		return;
	}

	if (INVENTORY_FULL_RANGE(Slot) == 0)
	{
		return;
	}

	if (lpObj->Inventory[Slot].IsItem() == 0)
	{
		if (!lpObj->Inventory[Slot].m_Index == 7212)
			return;

	}

	int count = gItemManager->GetInventoryItemCount(lpObj, this->GetJewelSimpleIndex(lpMsg->slot), 0);

	if (count <= 0)
	{
		return;
	}

	int JewelIndex = this->GetJewelSimpleType(lpObj->Inventory[Slot].m_Index);

	int JewelType = JewelIndex;
	int JewelCount = 1;

	switch (Slot)
	{
	case 0:
		lpObj->JewelChaosCount += JewelCount;
		break;
	case 1:
		lpObj->JewelLifeCount += JewelCount;
		break;
	case 2:
		lpObj->JewelSoulCount += JewelCount;
		break;
	case 3:
		lpObj->JewelBlessCount += JewelCount;
		break;
	case 4:
		lpObj->JewelCreationCount += JewelCount;
		break;
	case 5:
		lpObj->JewelGuardianCount += JewelCount;
		break;
	case 6:
		lpObj->JewelHarmonyCount += JewelCount;
		break;
	case 7:
		lpObj->JewelGemStoneCount += JewelCount;
		break;
	case 8:
		lpObj->JewelLowStoneCount += JewelCount;
		break;
	case 9:
		lpObj->JewelHighStoneCount += JewelCount;
		break;
	}

	gItemManager->DeleteInventoryItemCount(lpObj, this->GetJewelSimpleIndex(lpMsg->slot), 0, JewelCount);

	SDHP_CUSTOM_JEWELBANK_SEND pMsg = { 0 };

	pMsg.header.set(0xF7, 0x04, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	pMsg.type = Slot;

	pMsg.count = +(JewelCount);

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));

	this->GCCustomJewelBankInfoSend(lpObj);

}

void CCustomJewelBank::JewelBankWithDrawRecv(PSBMSG_JEWELBANKWITHDRAW_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int Type = lpMsg->type;
	int Count = lpMsg->count;

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Interface.use != 0)
	{
		return;
	}

	if (lpObj->ChaosLock != 0)
	{
		return;
	}

	if (Type < 0 || Count < 0)
	{
		return;
	}

	int FreeSpaces = gItemManager->CheckItemInventorySpaceCount(lpObj, 1, 1);

	if (FreeSpaces <= 0)
	{
		return;
	}

	int JewelCount = Count;

	if (Count == 99)
	{
		JewelCount = FreeSpaces;
	}

	switch (Type)
	{
	case 0:

		if (Count == 99 && lpObj->JewelChaosCount < JewelCount)
		{
			JewelCount = lpObj->JewelChaosCount;
			lpObj->JewelChaosCount = 0;
		}
		else if (lpObj->JewelChaosCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelChaosCount -= JewelCount;
		}
		break;

	case 1:
		if (Count == 99 && lpObj->JewelLifeCount < JewelCount)
		{
			JewelCount = lpObj->JewelLifeCount;
			lpObj->JewelLifeCount = 0;
		}
		else if (lpObj->JewelLifeCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelLifeCount -= JewelCount;
		}
		break;
	case 2:
		if (Count == 99 && lpObj->JewelSoulCount < JewelCount)
		{
			JewelCount = lpObj->JewelSoulCount;
			lpObj->JewelSoulCount = 0;
		}
		else if (lpObj->JewelSoulCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelSoulCount -= JewelCount;
		}
		break;

	case 3:
		if (Count == 99 && lpObj->JewelBlessCount < JewelCount)
		{
			JewelCount = lpObj->JewelBlessCount;
			lpObj->JewelBlessCount = 0;
		}
		else if (lpObj->JewelBlessCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelBlessCount -= JewelCount;
		}
		break;

	case 4:
		if (Count == 99 && lpObj->JewelCreationCount < JewelCount)
		{
			JewelCount = lpObj->JewelCreationCount;
			lpObj->JewelCreationCount = 0;
		}
		else if (lpObj->JewelCreationCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelCreationCount -= JewelCount;
		}
		break;

	case 5:
		if (Count == 99 && lpObj->JewelGuardianCount < JewelCount)
		{
			JewelCount = lpObj->JewelGuardianCount;
			lpObj->JewelGuardianCount = 0;
		}
		else if (lpObj->JewelGuardianCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelGuardianCount -= JewelCount;
		}
		break;

	case 6:
		if (Count == 99 && lpObj->JewelHarmonyCount < JewelCount)
		{
			JewelCount = lpObj->JewelHarmonyCount;
			lpObj->JewelHarmonyCount = 0;
		}
		else if (lpObj->JewelHarmonyCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelHarmonyCount -= JewelCount;
		}
		break;

	case 7:
		if (Count == 99 && lpObj->JewelGemStoneCount < JewelCount)
		{
			JewelCount = lpObj->JewelGemStoneCount;
			lpObj->JewelGemStoneCount = 0;
		}
		else if (lpObj->JewelGemStoneCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelGemStoneCount -= JewelCount;
		}
		break;

	case 8:
		if (Count == 99 && lpObj->JewelLowStoneCount < JewelCount)
		{
			JewelCount = lpObj->JewelLowStoneCount;
			lpObj->JewelLowStoneCount = 0;
		}
		else if (lpObj->JewelLowStoneCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelLowStoneCount -= JewelCount;
		}
		break;
	case 9:
		if (Count == 99 && lpObj->JewelHighStoneCount < JewelCount)
		{
			JewelCount = lpObj->JewelHighStoneCount;
			lpObj->JewelHighStoneCount = 0;
		}
		else if (lpObj->JewelHighStoneCount < JewelCount)
		{
			return;
		}
		else
		{
			lpObj->JewelHighStoneCount -= JewelCount;
		}
		break;
	}

	int ItemIndex = this->GetJewelSimpleIndex(Type);

	int ItemLevel = 0;

	if ((Type == 0 || Type == 1) && Count != 99)
	{
		if (JewelCount == 10)
		{
			ItemIndex = this->GetJewelBundleIndex(Type);
		}
		else if (JewelCount == 20)
		{
			ItemIndex = this->GetJewelBundleIndex(Type);
			ItemLevel = 1;
		}
		else if (JewelCount == 30)
		{
			ItemIndex = this->GetJewelBundleIndex(Type);
			ItemLevel = 2;
		}
	}

	if (ItemIndex < 0)
	{
		return;
	}

	if ((Type != 0 && Type != 1) || Count == 99)
	{
		for (int i = 0; i < JewelCount; i++)
		{
			GDCreateItemSend(aIndex, 0xEB, 0, 0, ItemIndex, ItemLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		}
	}
	else
	{
		GDCreateItemSend(aIndex, 0xEB, 0, 0, ItemIndex, ItemLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	}


	SDHP_CUSTOM_JEWELBANK_SEND pMsg = { 0 };

	pMsg.header.set(0xF7, 0x04, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	pMsg.type = Type;

	pMsg.count = -(JewelCount);

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));

	this->GCCustomJewelBankInfoSend(lpObj);

}

void CCustomJewelBank::CustomJewelBankInfoSend(int index)
{
	LPOBJ lpObj = &gObj[index];

	if (gObjIsConnectedGP(index) == 0)
	{
		return;
	}

	SDHP_CUSTOM_JEWELBANK_INFO_SEND pMsg = { 0 };

	pMsg.header.set(0xF7, 0x05, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

void CCustomJewelBank::CustomJewelBankInfoRecv(SDHP_CUSTOM_JEWELBANK_INFO_RECV* lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->index];

	if (gObjIsConnectedGP(lpMsg->index) == 0)
	{
		return;
	}

	lpObj->JewelChaosCount = lpMsg->Chaos;
	lpObj->JewelLifeCount = lpMsg->Life;
	lpObj->JewelSoulCount = lpMsg->Soul;
	lpObj->JewelBlessCount = lpMsg->Bless;
	lpObj->JewelCreationCount = lpMsg->Creation;
	lpObj->JewelGuardianCount = lpMsg->Guardian;
	lpObj->JewelHarmonyCount = lpMsg->Harmony;
	lpObj->JewelGemStoneCount = lpMsg->GemStone;
	lpObj->JewelLowStoneCount = lpMsg->LowStone;
	lpObj->JewelHighStoneCount = lpMsg->HighStone;

	this->GCCustomJewelBankInfoSend(lpObj);
}

void CCustomJewelBank::GCCustomJewelBankInfoSend(LPOBJ lpObj)
{
	if (gObjIsConnectedGP(lpObj->Index) == 0)
	{
		return;
	}

	PSBMSG_JEWELBANK_SEND pMsg = { 0 };

	pMsg.h.set(0xF3, 0xF5, sizeof(pMsg));

	pMsg.Chaos = lpObj->JewelChaosCount;
	pMsg.Life = lpObj->JewelLifeCount;
	pMsg.Soul = lpObj->JewelSoulCount;
	pMsg.Bless = lpObj->JewelBlessCount;
	pMsg.Creation = lpObj->JewelCreationCount;
	pMsg.Guardian = lpObj->JewelGuardianCount;
	pMsg.Harmony = lpObj->JewelHarmonyCount;
	pMsg.GemStone = lpObj->JewelGemStoneCount;
	pMsg.LowStone = lpObj->JewelLowStoneCount;
	pMsg.HighStone = lpObj->JewelHighStoneCount;

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
}