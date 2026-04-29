// CustomPet.h: interface for the CCustomPet class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Item.h"
#include "User.h"

#define MAX_CUSTOM_PET 256

struct CUSTOM_PET_INFO
{
	int Index;
	int ItemIndex;
	int IncDamageRate;
	int IncLife;
	int IncMana;
	int IncAttackSpeed;
	int IncDoubleDamageRate;
	int IncTripleDamageRate;
	int IncExperience;
	int IncResistDoubleDamage;
	int IncResistIgnoreDefense;
	int IncResistIgnoreSD;
	int IncResistCriticalDamage;
	int IncResisteExcellentDamage;
	int IncBlockStuck;
	int IncReflectRate;
	int Recovery;
};

class CCustomPet
{
	CCustomPet();
	virtual ~CCustomPet();
	SingletonInstance(CCustomPet)
public:
	void Load(char* path);
	bool CheckCustomPetByItem(int ItemIndex);
	bool PetIsInmortal(int ItemIndex);
	bool PetIsBlockStuck(int ItemIndex);
	int  GetCustomPetDamageRate(int ItemIndex);
	void CalcCustomPetOption(LPOBJ lpObj, bool flag);
public:
	std::map<int, CUSTOM_PET_INFO> m_CustomPetInfo;
};

#define gCustomPet SingNull(CCustomPet)