// CustomPet.cpp: implementation of the CCustomPet class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CustomPet.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomPet::CCustomPet() // OK
{
	this->m_CustomPetInfo.clear();
}

CCustomPet::~CCustomPet() // OK
{

}

void CCustomPet::Load(char* path) // OK
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

	this->m_CustomPetInfo.clear();

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

			CUSTOM_PET_INFO info;

			memset(&info, 0, sizeof(info));

			info.Index = lpReadFile->GetNumber();

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(), lpReadFile->GetAsNumber()));

			info.IncLife = lpReadFile->GetAsNumber();

			info.IncMana = lpReadFile->GetAsNumber();

			info.IncDamageRate = lpReadFile->GetAsNumber();

			info.IncAttackSpeed = lpReadFile->GetAsNumber();

			info.IncDoubleDamageRate = lpReadFile->GetAsNumber();

			info.IncTripleDamageRate = lpReadFile->GetAsNumber();

			info.IncExperience = lpReadFile->GetAsNumber();

			info.IncResistDoubleDamage = lpReadFile->GetAsNumber();

			info.IncResistIgnoreDefense = lpReadFile->GetAsNumber();

			info.IncResistIgnoreSD = lpReadFile->GetAsNumber();

			info.IncResistCriticalDamage = lpReadFile->GetAsNumber();

			info.IncResisteExcellentDamage = lpReadFile->GetAsNumber();

			info.IncBlockStuck = lpReadFile->GetAsNumber();

			info.IncReflectRate = lpReadFile->GetAsNumber();

			info.Recovery = lpReadFile->GetAsNumber();

			this->m_CustomPetInfo.insert(std::pair<int, CUSTOM_PET_INFO>(info.ItemIndex, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}



bool CCustomPet::CheckCustomPetByItem(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		return true;
	}

	return false;
}


bool CCustomPet::PetIsInmortal(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		if (it->second.Recovery != 0)
		{
			return true;
		}
	}

	return false;
}

bool CCustomPet::PetIsBlockStuck(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		if (it->second.IncBlockStuck != 0)
		{
			return true;
		}
	}

	return false;
}

int CCustomPet::GetCustomPetDamageRate(int ItemIndex) // OK
{
	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(ItemIndex);

	if (it != this->m_CustomPetInfo.end())
	{
		return it->second.IncDamageRate;
	}

	return 0;
}

void CCustomPet::CalcCustomPetOption(LPOBJ lpObj, bool flag)
{
	if (flag != 0)
	{
		return;
	}

	CItem* pEquipet = &lpObj->Inventory[8];

	std::map<int, CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.find(pEquipet->m_Index);

	if (it != this->m_CustomPetInfo.end())
	{
		if (pEquipet->m_Durability > 0)
		{
			lpObj->AddLife += it->second.IncLife;
			//--
			lpObj->AddMana += it->second.IncMana;
			//--
			lpObj->PhysiSpeed += it->second.IncAttackSpeed;

			lpObj->MagicSpeed += it->second.IncAttackSpeed;
			//--
			lpObj->DoubleDamageRate += it->second.IncDoubleDamageRate;
			//--
			lpObj->TripleDamageRate += it->second.IncTripleDamageRate;
			//--
			lpObj->ExperienceRate += it->second.IncExperience;

			lpObj->MasterExperienceRate += it->second.IncExperience;
			//--
			lpObj->ResistDoubleDamageRate += it->second.IncDoubleDamageRate;
			//--
			lpObj->ResistIgnoreDefenseRate += it->second.IncResistIgnoreDefense;
			//--
			lpObj->ResistIgnoreShieldGaugeRate += it->second.IncResistIgnoreSD;
			//--
			lpObj->ResistCriticalDamageRate += it->second.IncResistCriticalDamage;
			//--
			lpObj->ResistExcellentDamageRate += it->second.IncResisteExcellentDamage;
			//--
			lpObj->ResistStunRate += it->second.IncBlockStuck;
			//--
			lpObj->DamageReflect += it->second.IncReflectRate;

		}
	}
}