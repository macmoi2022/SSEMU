// SkillDamage.h: interface for the CSkillDamage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Skill.h"

struct SKILL_DAMAGE_INFO
{
	int Index;
	int RatePvM;
	int RatePvP;
};

struct SKILL_EFFECT_INFO
{
	int Index;
	int Rate;
};

class CSkillDamage
{
	CSkillDamage();
	virtual ~CSkillDamage();
	SingletonInstance(CSkillDamage)
public:
	void Load(char* path);
	int GetDamageRate(CSkill* lpSkill,WORD type,int damage);
	int GetEffectRate(CSkill* lpSkill);
private:
	std::map<int,SKILL_DAMAGE_INFO> m_SkillDamageRateInfo;
	std::map<int,SKILL_EFFECT_INFO> m_SkillEffectRateInfo;
};

#define gSkillDamage SingNull(CSkillDamage)