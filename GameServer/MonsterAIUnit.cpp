// MonsterAIUnit.cpp: implementation of the CMonsterAIUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIUnit.h"
#include "ReadFile.h"
#include "MonsterAIAutomata.h"
#include "MonsterAIElement.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIUnit::CMonsterAIUnit() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->Init();

	#endif
}

CMonsterAIUnit::~CMonsterAIUnit() // OK
{

}

void CMonsterAIUnit::Load(char* path) // OK
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

					CMonsterAIUnitInfo info;

					info.m_UnitNumber = lpReadFile->GetNumber();

					strcpy_s(info.m_UnitName,lpReadFile->GetAsString());

					info.m_DelayTime = lpReadFile->GetAsNumber();

					info.m_AutomataInfo = gMonsterAIAutomata->FindAutomataInfo(lpReadFile->GetAsNumber());

					info.m_AIClassNormal = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassMove = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassAttack = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassHeal = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassAvoid = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassHelp = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassSpecial = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassEvent = gMonsterAIElement->FindAIElementInfo(lpReadFile->GetAsNumber());

					info.m_AIClassMap[0] = info.m_AIClassNormal;

					info.m_AIClassMap[1] = info.m_AIClassMove;

					info.m_AIClassMap[2] = info.m_AIClassAttack;

					info.m_AIClassMap[3] = info.m_AIClassHeal;

					info.m_AIClassMap[4] = info.m_AIClassAvoid;

					info.m_AIClassMap[5] = info.m_AIClassHelp;

					info.m_AIClassMap[6] = info.m_AIClassSpecial;

					info.m_AIClassMap[7] = info.m_AIClassEvent;

					if(info.m_UnitNumber >= 0 && info.m_UnitNumber < MAX_MONSTER_AI_UNIT_INFO)
					{
						this->m_MonsterAIUnitInfoArray[info.m_UnitNumber] = info;
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

void CMonsterAIUnit::Init() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_MONSTER_AI_UNIT_INFO;n++)
	{
		this->m_MonsterAIUnitInfoArray[n].Reset();
	}

	#endif
}

CMonsterAIUnitInfo* CMonsterAIUnit::FindAIUnitInfo(int UnitNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(UnitNumber < 0 || UnitNumber >= MAX_MONSTER_AI_UNIT_INFO)
	{
		return 0;
	}

	if(this->m_MonsterAIUnitInfoArray[UnitNumber].m_UnitNumber == UnitNumber)
	{
		return &this->m_MonsterAIUnitInfoArray[UnitNumber];
	}

	#endif

	return 0;
}
