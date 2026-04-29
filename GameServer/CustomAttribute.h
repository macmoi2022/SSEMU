#pragma once

#include "User.h"

struct ITEM_VALUES_INFO
{
	int Index;
	int SucessAttack;
	int Defense;
	int SucessDefense;
	int AddLife;
	int Reflet;
	int CriticalDamage;
	int ExcDamage;
	int DamageMin;
	int DamageMax;
};

class CItemAttribute
{
	CItemAttribute();
	virtual ~CItemAttribute();
	SingletonInstance(CItemAttribute)
public:
	void Load(char* path);
	void SetValues(LPOBJ lpObj, int index);

private:
	std::map<int, ITEM_VALUES_INFO> m_ItemAttribute;

};

#define gItemAttribute SingNull(CItemAttribute)