// QuestWorldObjective.cpp: implementation of the CQuestWorldObjective class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestWorldObjective.h"
#include "DSProtocol.h"
#include "GensSystem.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Notice.h"
#include "Party.h"
#include "QuestWorld.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestWorldObjective::CQuestWorldObjective() // OK
{
	#if(GAMESERVER_UPDATE>=501)

	this->m_QuestWorldObjectiveInfo.clear();

	#endif
}

CQuestWorldObjective::~CQuestWorldObjective() // OK
{

}

void CQuestWorldObjective::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=501)

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

	this->m_QuestWorldObjectiveInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(strcmp("end",lpReadFile->GetAsString()) == 0)
				{
					break;
				}

				QUEST_WORLD_OBJECTIVE_INFO info;

				info.RequireGroup = section+1;

				info.Sort = lpReadFile->GetNumber();

				info.Type = lpReadFile->GetAsNumber();

				info.Index = lpReadFile->GetAsNumber();

				info.Quantity = lpReadFile->GetAsNumber();

				info.Level = lpReadFile->GetAsNumber();

				info.Option1 = lpReadFile->GetAsNumber();

				info.Option2 = lpReadFile->GetAsNumber();

				info.Option3 = lpReadFile->GetAsNumber();

				info.NewOption = lpReadFile->GetAsNumber();

				info.MapNumber = lpReadFile->GetAsNumber();

				info.DropMinLevel = lpReadFile->GetAsNumber();

				info.DropMaxLevel = lpReadFile->GetAsNumber();

				info.ItemDropRate = lpReadFile->GetAsNumber();

				info.RequireIndex = lpReadFile->GetAsNumber();

				info.RequireState = lpReadFile->GetAsNumber();

				for(int n=0;n < MAX_CLASS;n++){info.RequireClass[n] = lpReadFile->GetAsNumber();}

				this->m_QuestWorldObjectiveInfo.push_back(info);
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

#endif
}

int CQuestWorldObjective::GetQuestWorldObjectiveCount(LPOBJ lpObj,QUEST_WORLD_OBJECTIVE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	switch(lpInfo->Type)
	{
		case QUEST_WORLD_OBJECTIVE_MONSTER:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_SKILL:
			return ((gSkillManager->GetSkill(lpObj,lpInfo->Index)==0)?0:1);
		case QUEST_WORLD_OBJECTIVE_ITEM:
			return gItemManager->GetInventoryItemCount(lpObj,lpInfo->Index,lpInfo->Level);
		case QUEST_WORLD_OBJECTIVE_LEVEL:
			return lpObj->Level;
		case QUEST_WORLD_OBJECTIVE_SPECIAL:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_BUFF:
			return ((gEffectManager->CheckEffect(lpObj,lpInfo->Index)==0)?0:1);
		case QUEST_WORLD_OBJECTIVE_CHAOS_CASTLE_PLAYER_KILL:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_CHAOS_CASTLE_MONSTER_KILL:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_BLOOD_CASTLE_GATE_DESTROY:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_BLOOD_CASTLE_CLEAR:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_CHAOS_CASTLE_CLEAR:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_DEVIL_SQUARE_CLEAR:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_ILLUSION_TEMPLE_CLEAR:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_DEVIL_SQUARE_POINTS:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
		case QUEST_WORLD_OBJECTIVE_MONEY:
			return lpObj->Money;
		case QUEST_WORLD_OBJECTIVE_CONTRIBUTION:
			return lpObj->GensContribution;
		case QUEST_WORLD_OBJECTIVE_NPC:
			return this->GetQuestWorldObjectiveKillCount(lpObj,lpInfo);
	}

	#endif

	return 0;
}

int CQuestWorldObjective::GetQuestWorldObjectiveKillCount(LPOBJ lpObj,QUEST_WORLD_OBJECTIVE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	QUEST_WORLD_LIST* lpQuestWorldList = gQuestWorld->GetQuestWorldList(lpObj,lpInfo->RequireIndex,lpInfo->RequireGroup);

	if(lpQuestWorldList != 0)
	{
		return lpQuestWorldList->ObjectiveCount[lpInfo->Sort];
	}

	#endif

	return 0;
}

bool CQuestWorldObjective::GetQuestWorldObjective(LPOBJ lpObj,int QuestIndex,int QuestGroup,QUEST_WORLD_OBJECTIVE* lpObjective,int Sort) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex)
		{
			continue;
		}

		if(it->RequireGroup != QuestGroup)
		{
			continue;
		}

		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Sort != Sort)
		{
			continue;
		}

		lpObjective->type = it->Type;

		lpObjective->index = it->Index;

		lpObjective->value = it->Quantity;

		lpObjective->count = this->GetQuestWorldObjectiveCount(lpObj,&(*it));

		if(it->Type == QUEST_WORLD_OBJECTIVE_ITEM)
		{
			CItem item;

			item.m_Level = it->Level;

			item.Convert(it->Index,it->Option1,it->Option2,it->Option3,it->NewOption,0,0,0,0,0xFF);

			gItemManager->ItemByteConvert(lpObjective->ItemInfo,item);
		}

		return 1;
	}

	#endif

	return 0;
}

bool CQuestWorldObjective::CheckQuestWorldObjectiveRequisite(LPOBJ lpObj,QUEST_WORLD_OBJECTIVE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	if(lpInfo->RequireIndex != -1 && gQuestWorld->CheckQuestWorldListState(lpObj,lpInfo->RequireIndex,lpInfo->RequireGroup,lpInfo->RequireState) == 0)
	{
		return 0;
	}

	if(lpInfo->RequireClass[lpObj->Class] == 0 || lpInfo->RequireClass[lpObj->Class] > (lpObj->ChangeUp+1))
	{
		return 0;
	}

	return 1;

	#else

	return 0;

	#endif
}

bool CQuestWorldObjective::CheckQuestWorldObjective(LPOBJ lpObj,int QuestIndex,int QuestGroup) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex || it->RequireGroup != QuestGroup)
		{
			continue;
		}

		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(this->GetQuestWorldObjectiveCount(lpObj,&(*it)) < it->Quantity)
		{
			return 0;
		}
	}

	return 1;

	#else

	return 0;

	#endif
}

bool CQuestWorldObjective::CheckQuestWorldObjectiveItemCount(LPOBJ lpObj,int ItemIndex,int ItemLevel) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_WORLD_OBJECTIVE_ITEM)
		{
			continue;
		}

		if(it->Index == ItemIndex && it->Level == ItemLevel && it->Quantity <= this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
		{
			return 0;
		}
	}

	#endif

	return 1;
}

void CQuestWorldObjective::AddQuestWorldObjectiveKillCount(LPOBJ lpObj,QUEST_WORLD_OBJECTIVE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	QUEST_WORLD_LIST* lpQuestWorldList = gQuestWorld->GetQuestWorldList(lpObj,lpInfo->RequireIndex,lpInfo->RequireGroup);

	if(lpQuestWorldList != 0)
	{
		lpQuestWorldList->ObjectiveCount[lpInfo->Sort]++;
	}

	#endif
}

void CQuestWorldObjective::RemoveQuestWorldObjective(LPOBJ lpObj,int QuestIndex,int QuestGroup) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex || it->RequireGroup != QuestGroup)
		{
			continue;
		}

		if(it->Type == QUEST_WORLD_OBJECTIVE_ITEM)
		{
			gItemManager->DeleteInventoryItemCount(lpObj,it->Index,it->Level,it->Quantity);
			continue;
		}

		if(it->Type == QUEST_WORLD_OBJECTIVE_MONEY)
		{
			lpObj->Money -= it->Quantity;
			GCMoneySend(lpObj->Index,lpObj->Money);
			continue;
		}
	}

	#endif
}

void CQuestWorldObjective::PressButton(LPOBJ lpObj,int QuestIndex,int QuestGroup) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex || it->RequireGroup != QuestGroup)
		{
			continue;
		}

		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_WORLD_OBJECTIVE_SPECIAL)
		{
			continue;
		}

		if(it->Quantity > this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
		{
			this->AddQuestWorldObjectiveKillCount(lpObj,&(*it));
			return;
		}
	}

	#endif
}

bool CQuestWorldObjective::MonsterItemDrop(LPOBJ lpMonster) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	int aIndex = gMonster->GetTopHitDamageUser(lpMonster);

	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(lpObj->PartyNumber) != 0 && gServerInfo->m_QuestWorldMonsterItemDropParty != 0)
	{
		return this->MonsterItemDropParty(lpMonster,lpObj->PartyNumber);
	}

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_WORLD_OBJECTIVE_ITEM)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
		{
			continue;
		}

		if(it->DropMinLevel != -1 && it->DropMinLevel > lpMonster->Level)
		{
			continue;
		}

		if(it->DropMinLevel != -1 && it->DropMaxLevel != -1 && it->DropMaxLevel < lpMonster->Level)
		{
			continue;
		}

		if(it->DropMinLevel == -1 && it->DropMaxLevel != -1 && it->DropMaxLevel != lpMonster->Class)
		{
			continue;
		}

		if(it->ItemDropRate > (GetLargeRand()%10000) && it->Quantity > this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
		{
			GDCreateItemSend(aIndex,lpMonster->Map,(BYTE)lpMonster->X,(BYTE)lpMonster->Y,it->Index,it->Level,0,it->Option1,it->Option2,it->Option3,aIndex,it->NewOption,0,0,0,0,0xFF,0);
			return 1;
		}
	}

	#endif

	return 0;
}

bool CQuestWorldObjective::MonsterItemDropParty(LPOBJ lpMonster,int PartyNumber) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		int aIndex = gParty->m_PartyInfo[PartyNumber].Index[n];

		if(OBJECT_RANGE(aIndex) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[aIndex];

		for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
		{
			if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
			{
				continue;
			}

			if(it->Type != QUEST_WORLD_OBJECTIVE_ITEM)
			{
				continue;
			}

			if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
			{
				continue;
			}

			if(it->DropMinLevel != -1 && it->DropMinLevel > lpMonster->Level)
			{
				continue;
			}

			if(it->DropMinLevel != -1 && it->DropMaxLevel != -1 && it->DropMaxLevel < lpMonster->Level)
			{
				continue;
			}

			if(it->DropMinLevel == -1 && it->DropMaxLevel != -1 && it->DropMaxLevel != lpMonster->Class)
			{
				continue;
			}

			if(it->ItemDropRate > (GetLargeRand()%10000) && it->Quantity > this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
			{
				GDCreateItemSend(aIndex,lpMonster->Map,(BYTE)lpMonster->X,(BYTE)lpMonster->Y,it->Index,it->Level,0,it->Option1,it->Option2,it->Option3,aIndex,it->NewOption,0,0,0,0,0xFF,0);
				return 1;
			}
		}
	}

	#endif

	return 0;
}

void CQuestWorldObjective::MonsterKill(LPOBJ lpMonster) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	int aIndex = gMonster->GetTopHitDamageUser(lpMonster);

	if(OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(lpObj->PartyNumber) != 0 && gServerInfo->m_QuestWorldMonsterKillParty != 0)
	{
		return this->MonsterKillParty(lpMonster,lpObj->PartyNumber);
	}

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_WORLD_OBJECTIVE_MONSTER)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
		{
			continue;
		}

		if(it->Index == lpMonster->Class && it->Quantity > this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
		{
			this->AddQuestWorldObjectiveKillCount(lpObj,&(*it));
			return;
		}
	}

	#endif
}

void CQuestWorldObjective::MonsterKillParty(LPOBJ lpMonster,int PartyNumber) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for(int n = 0; n < MAX_PARTY_USER; n++)
	{
		int aIndex = gParty->m_PartyInfo[PartyNumber].Index[n];

		if(OBJECT_RANGE(aIndex) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[aIndex];

		for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
		{
			if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
			{
				continue;
			}

			if(it->Type != QUEST_WORLD_OBJECTIVE_MONSTER)
			{
				continue;
			}

			if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
			{
				continue;
			}

			if(it->Index == lpMonster->Class && it->Quantity > this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
			{
				this->AddQuestWorldObjectiveKillCount(lpObj,&(*it));
				return;
			}
		}
	}

	#endif
}

void CQuestWorldObjective::QuestWorldObjectiveSpecial(LPOBJ lpObj,int type,int index) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	for (std::vector<QUEST_WORLD_OBJECTIVE_INFO>::iterator it = this->m_QuestWorldObjectiveInfo.begin(); it != this->m_QuestWorldObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestWorldObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != type)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		if(it->Type == QUEST_WORLD_OBJECTIVE_NPC && it->Index == 0)
		{
			if(lpObj->GensFamily == GENS_FAMILY_VARNERT && index != 543)
			{
				continue;
			}
			
			if(lpObj->GensFamily == GENS_FAMILY_DUPRIAN && index != 544)
			{
				continue;
			}
		}
		else
		{
			if(index != -1 && it->Index != index)
			{
				continue;
			}
		}

		if(it->Quantity > this->GetQuestWorldObjectiveCount(lpObj,&(*it)))
		{
			this->AddQuestWorldObjectiveKillCount(lpObj,&(*it));
			return;
		}
	}

	#endif
}