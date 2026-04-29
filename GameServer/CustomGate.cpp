//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomGate.h"
#include "CastleSiegeSync.h"
#include "Map.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "Message.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "Path.h"
#include "ScheduleManager.h"
#include "Util.h"
#include "EffectManager.h"
#include "GensSystem.h"
#include "ServerInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomGate::CCustomGate() // OK
{
	this->m_CustomGate.clear();
}

CCustomGate::~CCustomGate() // OK
{

}

void CCustomGate::Load(char* path) // OK
{

	CReadFile* lpReadFile = new CReadFile;

	if (lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR, path);
		return;
	}

	if (lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_CustomGate.clear();

	try
	{
		while (true)
		{
			if (lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpReadFile->GetString()) == 0)
			{
				break;
			}

			CUSTOMGATE_INFO info;

			info.GateIndex = lpReadFile->GetNumber();

			info.MapStart = lpReadFile->GetAsNumber();

			info.RangeStartX = lpReadFile->GetAsNumber();

			info.RangeStartY = lpReadFile->GetAsNumber();

			info.MapEnd = lpReadFile->GetAsNumber();

			info.RangeEndX = lpReadFile->GetAsNumber();

			info.RangeEndY = lpReadFile->GetAsNumber();

			info.MinLevel = lpReadFile->GetAsNumber();

			info.MaxLevel = lpReadFile->GetAsNumber();

			info.MinReset = lpReadFile->GetAsNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.AccountLevel = lpReadFile->GetAsNumber();

			this->m_CustomGate.insert(std::pair<int, CUSTOMGATE_INFO>(info.GateIndex, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}


void CCustomGate::MainProc() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{

		if (gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		if (this->GetGate(&gObj[n]) == true)
		{
			continue;
		}
	}
}

bool CCustomGate::GetGate(LPOBJ lpObj) // OK
{
	for (std::map<int, CUSTOMGATE_INFO>::iterator it = this->m_CustomGate.begin(); it != this->m_CustomGate.end(); it++)
	{
		if (it->second.MapStart == lpObj->Map && it->second.RangeStartX == lpObj->X && it->second.RangeStartY == lpObj->Y)
		{
			if (it->second.MinLevel != -1 && lpObj->Level < it->second.MinLevel)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(224, gServerInfo->m_ServerLang), it->second.MinLevel);
				return false;
			}

			if (it->second.MaxLevel != -1 && lpObj->Level > it->second.MaxLevel)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(230, gServerInfo->m_ServerLang), it->second.MaxLevel);
				return false;
			}

			if (it->second.MinReset != -1 && lpObj->Reset < it->second.MinReset)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(231, gServerInfo->m_ServerLang), it->second.MinReset);
				return false;
			}

			if (it->second.MaxReset != -1 && lpObj->Reset > it->second.MaxReset)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(232, gServerInfo->m_ServerLang), it->second.MaxReset);
				return false;
			}

			if (lpObj->AccountLevel < it->second.AccountLevel)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(226, gServerInfo->m_ServerLang));
				return false;
			}

			if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->Store.Open != 0)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(226, gServerInfo->m_ServerLang));
				return false;
			}

			if (it->second.MapEnd == MAP_ATLANS && (lpObj->Inventory[8].IsItem() != 0 && (lpObj->Inventory[8].m_Index == GET_ITEM(13, 2) || lpObj->Inventory[8].m_Index == GET_ITEM(13, 3)))) // Uniria,Dinorant
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(274, gServerInfo->m_ServerLang));
				return false;
			}

			if (gEffectManager->CheckStunEffect(lpObj) != 0 || gEffectManager->CheckImmobilizeEffect(lpObj) != 0)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(226, gServerInfo->m_ServerLang));
				return false;
			}

			CItem* Inv = &lpObj->Inventory[INVENTORY_SLOT_HELPER];

#if(GAMESERVER_UPDATE>=201)
			if ((it->second.MapEnd == MAP_ICARUS || it->second.MapEnd == MAP_KANTURU3) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 37) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 4))) // Dinorant,Fenrir
#else(GAMESERVER_UPDATE==101)
			if ((it->second.MapEnd == MAP_ICARUS) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 37) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 4))) // Dinorant,Fenrir
#endif
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(228, gServerInfo->m_ServerLang));
				return false;
			}

#if(GAMESERVER_UPDATE>=501)

			if (lpObj->GensFamily == GENS_FAMILY_NONE && gMapManager->GetMapGensBattle(it->second.MapEnd) != 0)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(229, gServerInfo->m_ServerLang));
				return false;
			}

#endif

			gObjTeleport(lpObj->Index, it->second.MapEnd, it->second.RangeEndX, it->second.RangeEndY);

			LogAdd(LOG_BLACK, "[CustomMove][%s][%s] Map:%d X:%d Y:%d)", lpObj->Account, lpObj->Name, lpObj->Map, lpObj->X, lpObj->Y);

			return false;
		}

	}
	return true;
}