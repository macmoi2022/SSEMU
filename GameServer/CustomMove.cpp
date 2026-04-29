// CustomMove.cpp: implementation of the CCustomMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandManager.h"
#include "CustomMove.h"
#include "Log.h"
#include "Map.h"
#include "ReadFile.h"
#include "ServerInfo.h"
#include "MapManager.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "DeathEvent.h"
#include "DuelEvent.h"
#include "PairEvent.h"
#include "RoletaEvent.h"
#include "SobreEvent.h"
#include "TheftEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomMove::CCustomMove() // OK
{
	this->m_CustomMoveInfo.clear();
}

CCustomMove::~CCustomMove() // OK
{

}

void CCustomMove::Load(char* path) // OK
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

	this->m_CustomMoveInfo.clear();

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

			CUSTOMMOVE_INFO info;

			info.Index = lpReadFile->GetNumber();

			strcpy_s(info.Name, lpReadFile->GetAsString());

			info.Map = lpReadFile->GetAsNumber();

			info.X = lpReadFile->GetAsNumber();

			info.Y = lpReadFile->GetAsNumber();

			info.MinLevel = lpReadFile->GetAsNumber();

			info.MaxLevel = lpReadFile->GetAsNumber();

			info.MinReset = lpReadFile->GetAsNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.MinMReset = lpReadFile->GetAsNumber();

			info.MaxMReset = lpReadFile->GetAsNumber();

			info.AccountLevel = lpReadFile->GetAsNumber();

			info.PkMove = lpReadFile->GetAsNumber();

			this->m_CustomMoveInfo.insert(std::pair<int, CUSTOMMOVE_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CCustomMove::GetInfo(int index, CUSTOMMOVE_INFO* lpInfo) // OK
{
	std::map<int, CUSTOMMOVE_INFO>::iterator it = this->m_CustomMoveInfo.find(index);

	if (it == this->m_CustomMoveInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CCustomMove::GetInfoByName(char* message, CUSTOMMOVE_INFO* lpInfo) // OK
{
	char command[32] = { 0 };

	memset(command, 0, sizeof(command));

	gCommandManager->GetString(message, command, sizeof(command), 0);


	for (std::map<int, CUSTOMMOVE_INFO>::iterator it = this->m_CustomMoveInfo.begin(); it != this->m_CustomMoveInfo.end(); it++)
	{
		if (_stricmp(it->second.Name, command) == 0)
		{
			(*lpInfo) = it->second;
			return 1;
		}
	}

	return 0;
}

void CCustomMove::GetMove(LPOBJ lpObj, int index) // OK
{
	CUSTOMMOVE_INFO CustomMoveInfo;

	if (this->GetInfo(index, &CustomMoveInfo) == 0)
	{
		return;
	}

	if (CustomMoveInfo.MinLevel != -1 && lpObj->Level < CustomMoveInfo.MinLevel)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(224, gServerInfo->m_ServerLang), CustomMoveInfo.MinLevel);
		return;
	}

	if (CustomMoveInfo.MaxLevel != -1 && lpObj->Level > CustomMoveInfo.MaxLevel)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(230, gServerInfo->m_ServerLang), CustomMoveInfo.MaxLevel);
		return;
	}

	if (CustomMoveInfo.MinReset != -1 && lpObj->Reset < CustomMoveInfo.MinReset)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(231, gServerInfo->m_ServerLang), CustomMoveInfo.MinReset);
		return;
	}

	if (CustomMoveInfo.MaxReset != -1 && lpObj->Reset > CustomMoveInfo.MaxReset)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(232, gServerInfo->m_ServerLang), CustomMoveInfo.MaxReset);
		return;
	}

	if (CustomMoveInfo.MinMReset != -1 && lpObj->MasterReset < CustomMoveInfo.MinMReset)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(233, gServerInfo->m_ServerLang), CustomMoveInfo.MinMReset);
		return;
	}

	if (CustomMoveInfo.MaxMReset != -1 && lpObj->MasterReset > CustomMoveInfo.MaxMReset)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(234, gServerInfo->m_ServerLang), CustomMoveInfo.MaxMReset);
		return;
	}

	if (lpObj->AccountLevel < CustomMoveInfo.AccountLevel)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(226, gServerInfo->m_ServerLang));
		return;
	}

	if (CustomMoveInfo.PkMove == 0 && lpObj->PKLevel >= 5)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(227, gServerInfo->m_ServerLang));
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->Store.Open != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(226, gServerInfo->m_ServerLang));
		return;
	}

	if (CustomMoveInfo.Map == MAP_ATLANS && (lpObj->Inventory[8].IsItem() != 0 && (lpObj->Inventory[8].m_Index == GET_ITEM(13, 2) || lpObj->Inventory[8].m_Index == GET_ITEM(13, 3)))) // Uniria,Dinorant
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(274, gServerInfo->m_ServerLang));
		return;
	}

	if ((CustomMoveInfo.Map == MAP_ICARUS) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 37))) // Dinorant,Fenrir
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetText(228, gServerInfo->m_ServerLang));
		return;
	}

	// - Events
	DeathEvent.Quit(lpObj);
	DuelEvent.Quit(lpObj);
	PairEvent.Quit(lpObj);
	RoletaEvent.Quit(lpObj);
	SobreEvent.Quit(lpObj);
	TheftEvent.Quit(lpObj);

	gObjTeleport(lpObj->Index, CustomMoveInfo.Map, CustomMoveInfo.X, CustomMoveInfo.Y);
	gLog->Output(LOG_COMMAND, "[CustomMove][%s][%s] - (MoveIndex: %d)", lpObj->Account, lpObj->Name, index);
}