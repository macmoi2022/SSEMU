#include "stdafx.h"
#include "CustomAttribute.h"
#include "ReadFile.h"
#include "Util.h"
#include "ItemManager.h"

CItemAttribute::CItemAttribute()
{
	this->m_ItemAttribute.clear();
}

CItemAttribute::~CItemAttribute()
{
}

void CItemAttribute::Load(char* path)
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

	this->m_ItemAttribute.clear();

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

			ITEM_VALUES_INFO info;

			memset(&info, 0, sizeof(info));

			info.Index = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(), lpReadFile->GetAsNumber()));

			info.SucessAttack = lpReadFile->GetAsNumber();

			info.Defense = lpReadFile->GetAsNumber();

			info.SucessDefense = lpReadFile->GetAsNumber();

			info.AddLife = lpReadFile->GetAsNumber();

			info.Reflet = lpReadFile->GetAsNumber();

			info.DamageMin = lpReadFile->GetAsNumber();

			info.DamageMax = lpReadFile->GetAsNumber();

			info.ExcDamage = lpReadFile->GetAsNumber();

			info.CriticalDamage = lpReadFile->GetAsNumber();

			this->m_ItemAttribute.insert(std::pair<int, ITEM_VALUES_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CItemAttribute::SetValues(LPOBJ lpObj, int index)
{
	try
	{
		std::map<int, ITEM_VALUES_INFO>::iterator it = this->m_ItemAttribute.find(index);

		if (it == this->m_ItemAttribute.end())
		{
			return;
		}

		if (it->second.SucessAttack > 0)
		{
			lpObj->AttackSuccessRate += it->second.SucessAttack;
		}
		if (it->second.Defense > 0)
		{
			lpObj->Defense += it->second.Defense;
		}
		if (it->second.SucessDefense > 0)
		{
			lpObj->DefenseSuccessRate += it->second.SucessDefense;
		}
		if (it->second.AddLife > 0)
		{
			lpObj->AddLife += it->second.AddLife;
		}
		if (it->second.Reflet > 0)
		{
			lpObj->DamageReflect += it->second.Reflet;
		}
		if (it->second.DamageMin > 0)
		{
			lpObj->PhysiDamageMin += it->second.DamageMin;
			lpObj->PhysiDamageMinRight += it->second.DamageMin;
			lpObj->PhysiDamageMinLeft += it->second.DamageMin;
		}
		if (it->second.DamageMax > 0)
		{
			lpObj->PhysiDamageMax += it->second.DamageMax;
			lpObj->PhysiDamageMaxRight += it->second.DamageMax;
			lpObj->PhysiDamageMaxLeft += it->second.DamageMax;
		}
		if (it->second.CriticalDamage > 0)
		{
			lpObj->CriticalDamageRate += it->second.CriticalDamage;
		}
		if (it->second.ExcDamage > 0)
		{
			lpObj->ExcellentDamageRate += it->second.ExcDamage;
		}
	}
	catch (...)
	{
		LogAdd(LOG_RED, "[ItemAttribute::SetValues] Error");
	}
}
