// CommandRequirement.cpp: implementation of the CCommandRequirement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandRequirement.h"
#include "EffectManager.h"
#include "ItemManager.h"
#include "Notice.h"
#include "Quest.h"
#include "ReadFile.h"
#include "Util.h"

CCommandRequirement::CCommandRequirement() // OK
{
	this->m_CommandRequerimentInfo.clear();
}

CCommandRequirement::~CCommandRequirement() // OK
{

}

void CCommandRequirement::Load(char* path) // OK
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

	this->m_CommandRequerimentInfo.clear();

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

			COMMAND_REQUIREMENT_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.AccountLevel = lpReadFile->GetAsNumber();
			
			info.MinLevel = lpReadFile->GetAsNumber();
			
			info.MaxLevel = lpReadFile->GetAsNumber();
			
			info.MinReset = lpReadFile->GetAsNumber();
			
			info.MaxReset = lpReadFile->GetAsNumber();
			
			info.CheckItem = lpReadFile->GetAsNumber();

			info.ItemIndex = lpReadFile->GetAsNumber();

			info.ItemIndex = (info.ItemIndex!=-1)?SafeGetItem(GET_ITEM(info.ItemIndex,lpReadFile->GetAsNumber())):info.ItemIndex;

			info.ItemCount = lpReadFile->GetAsNumber();
			
			info.ItemLevel = lpReadFile->GetAsNumber();
			
			info.EffectState = lpReadFile->GetAsNumber();
			
			info.QuestIndex = lpReadFile->GetAsNumber();
			
			info.QuestState = lpReadFile->GetAsNumber();
			
			info.PKLevel = lpReadFile->GetAsNumber();
			
			info.PKCount = lpReadFile->GetAsNumber();
			
			info.GuildRank = lpReadFile->GetAsNumber();
			
			info.MapIndex = lpReadFile->GetAsNumber();

			info.X = lpReadFile->GetAsNumber();

			info.Y = lpReadFile->GetAsNumber();

			info.TX = lpReadFile->GetAsNumber();

			info.TY = lpReadFile->GetAsNumber();
			
			info.DayOfWeek = lpReadFile->GetAsNumber();
			
			info.MinHour = lpReadFile->GetAsNumber();
			
			info.MaxHour = lpReadFile->GetAsNumber();
			
			info.FailMessage = lpReadFile->GetAsNumber();

			for(int n=0;n < MAX_CLASS; n++){info.RequireClass[n] = lpReadFile->GetAsNumber();}

			this->m_CommandRequerimentInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CCommandRequirement::CommandCheck(LPOBJ lpObj,int index) // OK
{
	SYSTEMTIME SystemTime;

	GetSystemTime(&SystemTime);

	for(std::vector<COMMAND_REQUIREMENT_INFO>::iterator it = this->m_CommandRequerimentInfo.begin(); it != this->m_CommandRequerimentInfo.end(); it++)
	{
		if(it->Index != index)
		{
			continue;
		}

		if(it->AccountLevel != -1 && it->AccountLevel > lpObj->AccountLevel)
		{
			continue;
		}

		if(it->MinLevel != -1 && it->MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->MaxLevel != -1 && it->MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->MinReset != -1 && it->MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->MaxReset != -1 && it->MaxReset < lpObj->Reset)
		{
			continue;
		}

		if(it->RequireClass[lpObj->Class] == 0 || it->RequireClass[lpObj->Class] > (lpObj->ChangeUp+1))
		{
			continue;
		}

		if(it->CheckItem != -1)
		{
			for(int n = 0; n < INVENTORY_WEAR_SIZE; n++)
			{
				if(lpObj->Inventory[n].IsItem() != 0)
				{
					if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
					return 1;
				}
			}
		}

		if(it->ItemIndex != -1 && gItemManager->GetInventoryItemCount(lpObj,it->ItemIndex,it->ItemLevel) < it->ItemCount)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->EffectState != -1 && gEffectManager->CheckEffect(lpObj,it->EffectState) == 0)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->QuestIndex != -1 && gQuest->CheckQuestListState(lpObj,it->QuestIndex,it->QuestState) == 0)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->PKLevel != -1 && it->PKLevel != lpObj->PKLevel)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->PKCount != -1 && it->PKCount > lpObj->PKCount)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->GuildRank != -1 && it->GuildRank != lpObj->GuildRank)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->MapIndex != -1 && it->MapIndex != lpObj->Map)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->X != -1 && it->Y != -1 && ((it->X > lpObj->X || it->TX < lpObj->X) || (it->Y > lpObj->Y || it->TY < lpObj->Y)))
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->DayOfWeek != -1 && it->DayOfWeek != (SystemTime.wDayOfWeek+1))
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->MinHour != -1 && it->MinHour > SystemTime.wHour)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}

		if(it->MaxHour != -1 && it->MaxHour < SystemTime.wHour)
		{
			if(it->FailMessage != -1){gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->FailMessage);}
			return 1;
		}
	}

	return 0;
}

void CCommandRequirement::CommandDone(LPOBJ lpObj,int index) // OK
{
	for(std::vector<COMMAND_REQUIREMENT_INFO>::iterator it = this->m_CommandRequerimentInfo.begin(); it != this->m_CommandRequerimentInfo.end(); it++)
	{
		if(it->Index != index)
		{
			continue;
		}

		if(it->AccountLevel != -1 && it->AccountLevel > lpObj->AccountLevel)
		{
			continue;
		}

		if(it->MinLevel != -1 && it->MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->MaxLevel != -1 && it->MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->MinReset != -1 && it->MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->MaxReset != -1 && it->MaxReset < lpObj->Reset)
		{
			continue;
		}

		if(it->RequireClass[lpObj->Class] == 0 || it->RequireClass[lpObj->Class] > (lpObj->ChangeUp+1))
		{
			continue;
		}

		if(it->ItemIndex != -1)
		{
			gItemManager->DeleteInventoryItemCount(lpObj,it->ItemIndex,it->ItemLevel,it->ItemCount);
		}
	}
}