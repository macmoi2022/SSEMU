// SkillDamage.cpp: interface for the CSkillDamage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillDamage.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillDamage::CSkillDamage() // OK
{
	this->m_SkillDamageRateInfo.clear();

	this->m_SkillEffectRateInfo.clear();
}

CSkillDamage::~CSkillDamage() // OK
{

}

void CSkillDamage::Load(char* path) // OK
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

	this->m_SkillDamageRateInfo.clear();

	this->m_SkillEffectRateInfo.clear();

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

					SKILL_DAMAGE_INFO info;

					info.Index = lpReadFile->GetNumber();

					info.RatePvM = lpReadFile->GetAsNumber();

					info.RatePvP = lpReadFile->GetAsNumber();

					std::map<int,SKILL_DAMAGE_INFO>::iterator it = this->m_SkillDamageRateInfo.find(info.Index);

					if(it == this->m_SkillDamageRateInfo.end())
					{
						this->m_SkillDamageRateInfo.insert(std::pair<int,SKILL_DAMAGE_INFO>(info.Index,info));
					}
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					SKILL_EFFECT_INFO info;

					info.Index = lpReadFile->GetNumber();

					info.Rate = lpReadFile->GetAsNumber();

					std::map<int,SKILL_EFFECT_INFO>::iterator it = this->m_SkillEffectRateInfo.find(info.Index);

					if(it == this->m_SkillEffectRateInfo.end())
					{
						this->m_SkillEffectRateInfo.insert(std::pair<int,SKILL_EFFECT_INFO>(info.Index,info));
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
}

int CSkillDamage::GetDamageRate(CSkill* lpSkill,WORD type,int damage) // OK
{
	if(lpSkill != 0)
	{
		std::map<int,SKILL_DAMAGE_INFO>::iterator it = this->m_SkillDamageRateInfo.find(lpSkill->m_index);

		if(it != this->m_SkillDamageRateInfo.end())
		{
			damage = (damage*((type==OBJECT_MONSTER)? it->second.RatePvM:it->second.RatePvP))/100;
		}
	}
	
	return damage;
}

int CSkillDamage::GetEffectRate(CSkill* lpSkill) // OK
{
	if(lpSkill != 0)
	{
		std::map<int,SKILL_EFFECT_INFO>::iterator it = this->m_SkillEffectRateInfo.find(lpSkill->m_index);

		if(it != this->m_SkillEffectRateInfo.end())
		{
			if((GetLargeRand()%100) > it->second.Rate)
			{
				return 1;
			}
		}
	}
	
	return 0;
}