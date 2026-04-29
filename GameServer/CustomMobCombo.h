#pragma once

#define MAX_MOB_COMBO 100

struct CUSTOM_MOB_COMBO
{
	int  m_MonsterClass;
};

class CCustomMobCombo
{
	CCustomMobCombo();
	~CCustomMobCombo();
	SingletonInstance(CCustomMobCombo)
public:
	int m_count;
	void Load(char* path);
	CUSTOM_MOB_COMBO* GetMob(int MonsterClass);
	CUSTOM_MOB_COMBO m_CustomMobCombo[MAX_MOB_COMBO];
private:

};

#define gCustomMobCombo SingNull(CCustomMobCombo)