// MonsterAIRule.cpp: implementation of the CMonsterAIRule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIRule.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIRule::CMonsterAIRule() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->Init();

	#endif
}

CMonsterAIRule::~CMonsterAIRule() // OK
{

}

void CMonsterAIRule::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=201)

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

	this->Init();

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
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CMonsterAIRuleInfo info;

					info.m_RuleNumber = lpReadFile->GetNumber();

					info.m_MonsterClass = lpReadFile->GetAsNumber();

					strcpy_s(info.m_RuleDesc,lpReadFile->GetAsString());

					info.m_MonsterAIUnit = lpReadFile->GetAsNumber();

					info.m_RuleCondition = lpReadFile->GetAsNumber();

					info.m_WaitTime = lpReadFile->GetAsNumber();

					info.m_ContinuanceTime = lpReadFile->GetAsNumber();

					info.m_Month = lpReadFile->GetAsNumber();

					info.m_Day = lpReadFile->GetAsNumber();

					info.m_WeekDay = lpReadFile->GetAsNumber();

					info.m_Hour = lpReadFile->GetAsNumber();

					info.m_Minute = lpReadFile->GetAsNumber();

					if(this->m_MonsterAIRuleInfoCount >= 0 && this->m_MonsterAIRuleInfoCount < MAX_MONSTER_AI_RULE_INFO)
					{
						this->m_MonsterAIRuleInfoArray[this->m_MonsterAIRuleInfoCount++] = info;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

void CMonsterAIRule::Init() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_MONSTER_AI_RULE_TABLE;n++)
	{
		this->m_MonsterCurrentAIUnitTable[n] = 0;
	}

	for(int n=0;n < MAX_MONSTER_AI_RULE_INFO;n++)
	{
		this->m_MonsterAIRuleInfoArray[n].Reset();
	}

	this->m_MonsterAIRuleInfoCount = 0;

	#endif
}

int CMonsterAIRule::GetCurrentAIUnit(int MonsterClass) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(MonsterClass < 0 || MonsterClass >= MAX_MONSTER_AI_RULE_TABLE)
	{
		return 0;
	}

	return this->m_MonsterCurrentAIUnitTable[MonsterClass];

	#else

	return 0;

	#endif
}

void CMonsterAIRule::MonsterAIRuleProc() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < this->m_MonsterAIRuleInfoCount;n++)
	{
		CMonsterAIRuleInfo* lpMonsterAIRuleInfo = &this->m_MonsterAIRuleInfoArray[n];

		if(lpMonsterAIRuleInfo->m_MonsterClass < 0 || lpMonsterAIRuleInfo->m_MonsterClass >= MAX_MONSTER_AI_RULE_TABLE)
		{
			continue;
		}

		if(lpMonsterAIRuleInfo->IsValid() == 0)
		{
			this->m_MonsterCurrentAIUnitTable[lpMonsterAIRuleInfo->m_MonsterClass] = 0;
		}
		else
		{
			this->m_MonsterCurrentAIUnitTable[lpMonsterAIRuleInfo->m_MonsterClass] = lpMonsterAIRuleInfo->m_MonsterAIUnit;
		}
	}

	#endif
}
