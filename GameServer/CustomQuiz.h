// CustomQuiz.h: interface for the CCustomQuiz class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

#define MAX_CUSTOM_QUIZ 50

enum eCustomQuizState
{
	CUSTOM_QUIZ_STATE_BLANK = 0,
	CUSTOM_QUIZ_STATE_EMPTY = 1,
	CUSTOM_QUIZ_STATE_START = 2,
};

struct CUSTOM_QUIZ_START_TIME
{
	int Year;
	int Month;
	int Day;
	int DayOfWeek;
	int Hour;
	int Minute;
	int Second;
};

struct CUSTOM_QUIZ_RULE_INFO
{
	char Name[32];
	int  AlarmTime;
	int  EventTime;
};

struct CUSTOM_QUIZ_QUESTION
{
	int Index;
	int	Money;

	char Question[128];
	char Answer[128];
};

struct CUSTOM_QUIZ_INFO
{
	int Index;
	int State;
	int RemainTime;
	int TargetTime;
	int TickCount;
	int AlarmMinSave;
	int AlarmMinLeft;
	CUSTOM_QUIZ_RULE_INFO RuleInfo;
	std::vector<CUSTOM_QUIZ_START_TIME> StartTime;
};

class CCustomQuiz
{
	CCustomQuiz();
	virtual ~CCustomQuiz();
	SingletonInstance(CCustomQuiz)
public:
	void Init();
	void Load(char* path);
	void ReadCustomQuizInfo(char* section, char* path);
	void MainProc();
	void ProcState_BLANK(CUSTOM_QUIZ_INFO* lpInfo);
	void ProcState_EMPTY(CUSTOM_QUIZ_INFO* lpInfo);
	void ProcState_START(CUSTOM_QUIZ_INFO* lpInfo);
	void SetState(CUSTOM_QUIZ_INFO* lpInfo, int state);
	void SetState_BLANK(CUSTOM_QUIZ_INFO* lpInfo);
	void SetState_EMPTY(CUSTOM_QUIZ_INFO* lpInfo);
	void SetState_START(CUSTOM_QUIZ_INFO* lpInfo);
	void CheckSync(CUSTOM_QUIZ_INFO* lpInfo);
	void StartQuiz();
	bool GetInfo(int index, CUSTOM_QUIZ_QUESTION* lpInfo);
	void CommandQuiz(LPOBJ lpObj, char* arg);

	int m_CustomQuizSwitch;
private:
	std::map<int, CUSTOM_QUIZ_QUESTION> m_CustomQuiz;
	CUSTOM_QUIZ_INFO m_CustomQuizInfo[MAX_CUSTOM_QUIZ];
	int Active;
	int IndexSelected;
	int IndexInfo;


};

#define gCustomQuiz SingNull(CCustomQuiz)