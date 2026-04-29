// CommandRequirement.h: interface for the CCommandRequirement class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "DefaultClassInfo.h"
#include "User.h"

struct COMMAND_REQUIREMENT_INFO
{
	int Index;
	int AccountLevel;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int CheckItem;
	int ItemIndex;
	int ItemCount;
	int ItemLevel;
	int EffectState;
	int QuestIndex;
	int QuestState;
	int PKLevel;
	int PKCount;
	int GuildRank;
	int MapIndex;
	int X;
	int Y;
	int TX;
	int TY;
	int DayOfWeek;
	int MinHour;
	int MaxHour;
	int FailMessage;
	int RequireClass[MAX_CLASS];
};

class CCommandRequirement
{
	CCommandRequirement();
	virtual ~CCommandRequirement();
	SingletonInstance(CCommandRequirement)
public:
	void Load(char* path);
	bool CommandCheck(LPOBJ lpObj,int index);
	void CommandDone(LPOBJ lpObj,int index);
private:
	std::vector<COMMAND_REQUIREMENT_INFO> m_CommandRequerimentInfo;
};

#define gCommandRequirement SingNull(CCommandRequirement)