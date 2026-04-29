// CustomQuiz.cpp: implementation of the CCustomQuiz class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CommandManager.h"
#include "CustomQuiz.h"
#include "DSProtocol.h"
#include "ItemBagManager.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "ScheduleManager.h"
#include "User.h"
#include "Util.h"
#include "Viewport.h"
#include "ReadFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomQuiz::CCustomQuiz() // OK
{
	this->m_CustomQuiz.clear();

	this->Active = 0;
	this->IndexSelected = -1;
	this->IndexInfo = -1;

	for (int n = 0;n < MAX_CUSTOM_QUIZ;n++)
	{
		CUSTOM_QUIZ_INFO* lpInfo = &this->m_CustomQuizInfo[n];

		lpInfo->Index = n;
		lpInfo->State = CUSTOM_QUIZ_STATE_BLANK;
		lpInfo->RemainTime = 0;
		lpInfo->TargetTime = 0;
		lpInfo->TickCount = GetTickCount();
		lpInfo->AlarmMinSave = -1;
		lpInfo->AlarmMinLeft = -1;
	}
}

CCustomQuiz::~CCustomQuiz() // OK
{

}

void CCustomQuiz::Init() // OK
{
	for (int n = 0;n < MAX_CUSTOM_QUIZ;n++)
	{
		if (this->m_CustomQuizSwitch == 0)
		{
			this->SetState(&this->m_CustomQuizInfo[n], CUSTOM_QUIZ_STATE_BLANK);
		}
		else
		{
			this->SetState(&this->m_CustomQuizInfo[n], CUSTOM_QUIZ_STATE_EMPTY);
		}
	}

}

void CCustomQuiz::ReadCustomQuizInfo(char* section, char* path) // OK
{
	this->m_CustomQuizSwitch = GetPrivateProfileInt(section, "CustomQuizSwitch", 0, path);
}

void CCustomQuiz::Load(char* path) // OK
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

	for (int n = 0;n < MAX_CUSTOM_QUIZ;n++)
	{
		this->m_CustomQuizInfo[n].StartTime.clear();
	}

	this->m_CustomQuiz.clear();

	try
	{
		while (true)
		{
			if (lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_QUIZ_START_TIME info;

					int index = lpReadFile->GetNumber();

					info.Year = lpReadFile->GetAsNumber();

					info.Month = lpReadFile->GetAsNumber();

					info.Day = lpReadFile->GetAsNumber();

					info.DayOfWeek = lpReadFile->GetAsNumber();

					info.Hour = lpReadFile->GetAsNumber();

					info.Minute = lpReadFile->GetAsNumber();

					info.Second = lpReadFile->GetAsNumber();

					this->m_CustomQuizInfo[index].StartTime.push_back(info);
				}
				else if (section == 1)
				{
					if (strcmp("end", lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int index = lpReadFile->GetNumber();

					strcpy_s(this->m_CustomQuizInfo[index].RuleInfo.Name, lpReadFile->GetAsString());

					this->m_CustomQuizInfo[index].RuleInfo.AlarmTime = lpReadFile->GetAsNumber();

					this->m_CustomQuizInfo[index].RuleInfo.EventTime = lpReadFile->GetAsNumber();

				}
				else if (section == 2)
				{
					if (strcmp("end", lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_QUIZ_QUESTION info_question;

					info_question.Index = lpReadFile->GetNumber();

					info_question.Money = lpReadFile->GetAsNumber();

					strcpy_s(info_question.Question, lpReadFile->GetAsString());

					strcpy_s(info_question.Answer, lpReadFile->GetAsString());

					this->m_CustomQuiz.insert(std::pair<int, CUSTOM_QUIZ_QUESTION>(info_question.Index, info_question));
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CCustomQuiz::MainProc() // OK
{
	for (int n = 0;n < MAX_CUSTOM_QUIZ;n++)
	{
		CUSTOM_QUIZ_INFO* lpInfo = &this->m_CustomQuizInfo[n];

		if ((GetTickCount() - lpInfo->TickCount) >= 1000)
		{
			lpInfo->TickCount = GetTickCount();

			lpInfo->RemainTime = (int)difftime(lpInfo->TargetTime, time(0));

			switch (lpInfo->State)
			{
			case CUSTOM_QUIZ_STATE_BLANK:
				this->ProcState_BLANK(lpInfo);
				break;
			case CUSTOM_QUIZ_STATE_EMPTY:
				this->ProcState_EMPTY(lpInfo);
				break;
			case CUSTOM_QUIZ_STATE_START:
				this->ProcState_START(lpInfo);
				break;
			}
		}
	}
}

void CCustomQuiz::ProcState_BLANK(CUSTOM_QUIZ_INFO* lpInfo) // OK
{

}

void CCustomQuiz::ProcState_EMPTY(CUSTOM_QUIZ_INFO* lpInfo) // OK
{
	if (lpInfo->RemainTime > 0 && lpInfo->RemainTime <= (lpInfo->RuleInfo.AlarmTime * 60))
	{
		if ((lpInfo->AlarmMinSave = (((lpInfo->RemainTime % 60) == 0) ? ((lpInfo->RemainTime / 60) - 1) : (lpInfo->RemainTime / 60))) != lpInfo->AlarmMinLeft)
		{
			lpInfo->AlarmMinLeft = lpInfo->AlarmMinSave;

			gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1102), lpInfo->RuleInfo.Name, (lpInfo->AlarmMinLeft + 1));
		}
	}

	if (lpInfo->RemainTime <= 0)
	{
		int index = (GetLargeRand() % this->m_CustomQuiz.size());

		CUSTOM_QUIZ_QUESTION CustomQuiz;

		if (this->GetInfo(index, &CustomQuiz) == 1)
		{
			gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1103), lpInfo->RuleInfo.Name);

			gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1104), lpInfo->RuleInfo.Name, CustomQuiz.Question);

			this->Active = 1;
			this->IndexSelected = index;
			this->IndexInfo = lpInfo->Index;
		}
		else
		{
			this->Active = 0;
			this->IndexSelected = -1;
			this->IndexInfo = -1;
		}

		this->SetState(lpInfo, CUSTOM_QUIZ_STATE_START);
	}
}

void CCustomQuiz::ProcState_START(CUSTOM_QUIZ_INFO* lpInfo) // OK
{
	if (this->Active == 0)
	{
		this->IndexSelected = -1;

		this->IndexInfo = -1;

		this->SetState(lpInfo, CUSTOM_QUIZ_STATE_EMPTY);
	}

	if (lpInfo->RemainTime > 0 && lpInfo->RemainTime <= (lpInfo->RuleInfo.EventTime * 60) - 1)
	{
		int minutes = lpInfo->RemainTime / 60;

		if ((lpInfo->RemainTime % 60) == 0)
		{
			CUSTOM_QUIZ_QUESTION CustomQuiz;

			if (this->GetInfo(this->IndexSelected, &CustomQuiz) == 1)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1103), lpInfo->RuleInfo.Name);

				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1104), lpInfo->RuleInfo.Name, CustomQuiz.Question);
			}
		}
	}

	if (lpInfo->RemainTime <= 0)
	{
		gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1105), lpInfo->RuleInfo.Name);

		this->Active = 0;

		this->IndexSelected = -1;

		this->IndexInfo = -1;

		this->SetState(lpInfo, CUSTOM_QUIZ_STATE_EMPTY);
	}
}

void CCustomQuiz::SetState(CUSTOM_QUIZ_INFO* lpInfo, int state) // OK
{
	switch ((lpInfo->State = state))
	{
	case CUSTOM_QUIZ_STATE_BLANK:
		this->SetState_BLANK(lpInfo);
		break;
	case CUSTOM_QUIZ_STATE_EMPTY:
		this->SetState_EMPTY(lpInfo);
		break;
	case CUSTOM_QUIZ_STATE_START:
		this->SetState_START(lpInfo);
		break;
	}
}

void CCustomQuiz::SetState_BLANK(CUSTOM_QUIZ_INFO* lpInfo) // OK
{

}

void CCustomQuiz::SetState_EMPTY(CUSTOM_QUIZ_INFO* lpInfo) // OK
{
	lpInfo->AlarmMinSave = -1;
	lpInfo->AlarmMinLeft = -1;

	this->CheckSync(lpInfo);
}

void CCustomQuiz::SetState_START(CUSTOM_QUIZ_INFO* lpInfo) // OK
{
	lpInfo->AlarmMinSave = -1;
	lpInfo->AlarmMinLeft = -1;

	lpInfo->RemainTime = lpInfo->RuleInfo.EventTime * 60;

	lpInfo->TargetTime = (int)(time(0) + lpInfo->RemainTime);
}

void CCustomQuiz::CheckSync(CUSTOM_QUIZ_INFO* lpInfo) // OK
{
	if (lpInfo->StartTime.empty() != 0)
	{
		this->SetState(lpInfo, CUSTOM_QUIZ_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for (std::vector<CUSTOM_QUIZ_START_TIME>::iterator it = lpInfo->StartTime.begin();it != lpInfo->StartTime.end();it++)
	{
		ScheduleManager.AddSchedule(it->Year, it->Month, it->Day, it->Hour, it->Minute, it->Second, it->DayOfWeek);
	}

	if (ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(lpInfo, CUSTOM_QUIZ_STATE_BLANK);
		return;
	}

	lpInfo->RemainTime = (int)difftime(ScheduleTime.GetTime(), time(0));

	lpInfo->TargetTime = (int)ScheduleTime.GetTime();
}

bool CCustomQuiz::GetInfo(int index, CUSTOM_QUIZ_QUESTION* lpInfo) // OK
{
	std::map<int, CUSTOM_QUIZ_QUESTION>::iterator it = this->m_CustomQuiz.find(index);

	if (it == this->m_CustomQuiz.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

void CCustomQuiz::CommandQuiz(LPOBJ lpObj, char* arg)
{
	if (this->Active == 0)
	{
		return;
	}

	char answer[128] = { 0 };

	CUSTOM_QUIZ_QUESTION CustomQuiz;

	if (this->GetInfo(this->IndexSelected, &CustomQuiz) == 1)
	{
		if (strcmp(_strlwr(arg), _strlwr(CustomQuiz.Answer)) == 0)
		{
			gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1106), this->m_CustomQuizInfo[this->IndexInfo].RuleInfo.Name, lpObj->Name);
			gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1107), this->m_CustomQuizInfo[this->IndexInfo].RuleInfo.Name, CustomQuiz.Answer);

			if (lpObj->Money <= 0)
			{
				lpObj->Money = 0;
			}
			else if (gObjCheckMaxMoney(lpObj->Index, CustomQuiz.Money) == 0)
			{
				lpObj->Money = MAX_MONEY;
			}
			else
			{
				lpObj->Money += CustomQuiz.Money;
			}

			GCMoneySend(lpObj->Index, lpObj->Money);

			gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_QUIZ, -1, -1, lpObj, lpObj->Map, lpObj->X, lpObj->Y);

			this->Active = 0;

			this->m_CustomQuizInfo[this->IndexInfo].RemainTime = 0;

			GCFireworksSend(lpObj, lpObj->X, lpObj->Y);


			return;
		}
		else
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1108), this->m_CustomQuizInfo[this->IndexInfo].RuleInfo.Name);
		}
	}
}

void CCustomQuiz::StartQuiz()
{
	time_t theTime = time(NULL);
	struct tm* aTime = localtime(&theTime);
	int hour = aTime->tm_hour;
	int minute = aTime->tm_min + 2;

	if (minute >= 60)
	{
		hour++;
		minute = minute - 60;
	}

	CUSTOM_QUIZ_START_TIME info;

	info.Year = -1;

	info.Month = -1;

	info.Day = -1;

	info.DayOfWeek = -1;

	info.Hour = hour;

	info.Minute = minute;

	info.Second = 0;

	this->m_CustomQuizInfo[0].StartTime.push_back(info);

	LogAdd(LOG_BLUE, "[Set Quiz Start] At %2d:%2d:00", hour, minute);

	this->Init();
}

