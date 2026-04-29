#include "stdafx.h"
#include "DailyReward.h"
#include "Util.h"
#include "GameMain.h"
#include "ReadFile.h"
#include "Util.h"
#include "DSProtocol.h"
#include "Notice.h"
#include "ItemManager.h"

cDailyReward::cDailyReward()
{
}

cDailyReward::~cDailyReward()
{
}

void cDailyReward::OnSecondProc(int aIndex)
{
	tm today;
	time_t ltime;
	time(&ltime);
	localtime_s(&today, &ltime);

	int rIndex = aIndex - OBJECT_START_USER;

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	if (this->Users[rIndex].mDay != today.tm_mday)
	{
		this->OnRequestInfo(aIndex);
		this->Users[rIndex].mDay = today.tm_mday;
	}
}

void cDailyReward::Read(char* path)
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

	this->m_Configs.clear();

	int type = -1;

	int sNum = 1;
	int lNum = 1;

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

					cDR_Config info;

					info.Day = lpReadFile->GetNumber();

					info.Item = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(), lpReadFile->GetAsNumber()));

					info.Level = lpReadFile->GetAsNumber();

					info.Op1 = lpReadFile->GetAsNumber();

					info.Op2 = lpReadFile->GetAsNumber();

					info.Op3 = lpReadFile->GetAsNumber();

					info.Dur = lpReadFile->GetAsNumber();

					info.Ex = lpReadFile->GetAsNumber();

					info.Anc = lpReadFile->GetAsNumber();

					info.Expire = lpReadFile->GetAsNumber();

					strcpy_s(info.Text, lpReadFile->GetAsString());

					strcpy_s(info.Info, lpReadFile->GetAsString());

					this->m_Configs.insert(std::pair<int, cDR_Config>(info.Day, info));
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

void cDailyReward::CharZero(int aIndex)
{
	int rIndex = aIndex - OBJECT_START_USER;

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}
	this->Users[rIndex].aIndex = -1;
	this->Users[rIndex].mDay = 0;

	for (int i = 0; i < 31; i++)
	{
		this->Users[rIndex].State[i] = 0;
	}

}

void cDailyReward::DGDailyInfo(LPBYTE buf)
{
	SDHP_DAILY_REWARD_INFO_RECV* Data = (SDHP_DAILY_REWARD_INFO_RECV*)buf;

	int rIndex = Data->index - OBJECT_START_USER;

	if (!OBJECT_USER_RANGE(Data->index))
	{
		return;
	}

	tm today;
	time_t ltime;
	time(&ltime);
	localtime_s(&today, &ltime);

	this->Users[rIndex].mDay = today.tm_mday;

	this->Users[rIndex].aIndex = Data->index;

	for (int i = 0; i < 31; i++)
	{
		this->Users[rIndex].State[i] = Data->Status[i];
	}


	this->OnRequestInfo(Data->index);
}

void cDailyReward::GDDailySave(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	SDHP_DAILY_REWARD_INFO_RECV pMsg;

	pMsg.header.set(0x68, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.acc, lpObj->Account, sizeof(pMsg.acc));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	pMsg.result = 1;

	int rIndex = aIndex - OBJECT_START_USER;

	for (int i = 0; i < 31; i++)
	{
		pMsg.Status[i] = this->Users[rIndex].State[i];
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

void cDailyReward::CGRequest(BYTE* a, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	PMSG_DAILY_REWARD_RECV* Data = (PMSG_DAILY_REWARD_RECV*)a;

	switch (Data->Type)
	{
	case 0x0:
		this->OnRequestInfo(aIndex);
		break;
	case 0x1:
		this->GCDailyInfo(Data->Value, aIndex);
		break;
	case 0x2:
		this->OnRequestRecvGift(Data->Value, aIndex);
		break;
	default:
		break;
	}
}

unsigned char last_day_of_month(int year, unsigned char month) {
	return month != 2 ? ((month ^ (month >> 3))) | 30 :
		year % 4 ? 29 : 28;
}

void cDailyReward::GCDailyUserInfo(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	tm today;
	time_t ltime;
	time(&ltime);
	localtime_s(&today, &ltime);

	tm fmday = { 0, 0, 0, 1, today.tm_mon, today.tm_year };
	time_t time_temp = mktime(&fmday);
	tm* tmonth = localtime(&time_temp);

	PMSG_DAILY_REWARD_USER_INFO_SEND rSend;

	rSend.h.set((LPBYTE)&rSend, 0xFB, 0x17, sizeof(rSend));

	rSend.CurDay = today.tm_mday;
	rSend.MaxDay = last_day_of_month(today.tm_year + 190, today.tm_mon + 1);
	rSend.FWday = tmonth->tm_wday;

	int rIndex = aIndex - OBJECT_START_USER;

	for (int i = 0; i < 31; i++)
	{
		rSend.State[i] = this->Users[rIndex].State[i];
	}

	rSend.Timer = (DWORD)ltime;
	DataSend(aIndex, (LPBYTE)&rSend, sizeof(rSend));
}

cDR_Config* cDailyReward::GetInfo(int day) // OK
{
	std::map<int, cDR_Config>::iterator it = this->m_Configs.find(day);

	if (it != this->m_Configs.end())
	{
		return &it->second;
	}
	return 0;
}

void cDailyReward::GCDailyInfo(int day, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}
	PMSG_DAILY_REWARD_DAY_INFO_SEND rSend;
	rSend.h.set((LPBYTE)&rSend, 0xFB, 0x18, sizeof(rSend));

	cDR_Config* Day = this->GetInfo(day);

	if (Day == 0)
	{
		return;
	}

	rSend.Type = Day->Item;

	rSend.Dur = Day->Dur;

	this->ItemByteConvert(rSend.Item, day);

	memcpy_s(rSend.Text, sizeof(rSend.Text), Day->Text, sizeof(rSend.Text));

	memcpy_s(rSend.Info, sizeof(rSend.Info), Day->Info, sizeof(rSend.Info));

	DataSend(aIndex, (LPBYTE)&rSend, sizeof(rSend));
}

void cDailyReward::OnRequestInfo(int aIndex)
{

	tm today;
	time_t ltime;
	time(&ltime);
	localtime_s(&today, &ltime);

	this->GCDailyUserInfo(aIndex);
	this->GCDailyInfo(today.tm_mday, aIndex);
}

void cDailyReward::ItemByteConvert(BYTE* lpMsg, int day) // OK
{
	cDR_Config* Day = this->GetInfo(day);
	if (Day == 0)
	{
		return;
	}
	lpMsg[0] = Day->Item & 0xFF;

	lpMsg[1] = 0;
	lpMsg[1] |= Day->Level * 8;
	lpMsg[1] |= Day->Op1 * 128;
	lpMsg[1] |= Day->Op2 * 4;
	lpMsg[1] |= Day->Op3 & 3;

	lpMsg[2] = Day->Dur;

	lpMsg[3] = 0;
	lpMsg[3] |= (Day->Item & 0x100) >> 1;
	lpMsg[3] |= ((Day->Op3 > 3) ? 0x40 : 0);
	lpMsg[3] |= Day->Ex;

	lpMsg[4] = Day->Anc;
}

void cDailyReward::OnRequestRecvGift(int day, int aIndex)
{
	if (!OBJECT_USER_RANGE(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	cDR_Config* Day = this->GetInfo(day);
	if (Day == 0)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "No Reward!");
		return;
	}

	int rIndex = aIndex - OBJECT_START_USER;


	if (day > 0 && day <= 31)
	{
		if (Users[rIndex].State[day - 1] != 0)
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "You Complete!");
			return;
		}

		if (gItemManager->CheckItemInventorySpace(lpObj, 4, 4) == 0)
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Inventory full!");
			return;
		}

		if (day != Users[rIndex].mDay)
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "You Can't Receive Gifts!");
			return;
		}

		if (Day->Item != -1)
		{
			GDCreateItemSend(lpObj->Index, 0xEB, (BYTE)lpObj->X, (BYTE)lpObj->Y, Day->Item, Day->Level, Day->Dur, Day->Op1, Day->Op2, Day->Op3, -1, Day->Ex, Day->Anc, 0, 0, 0, 0xFF, 0);
		}

		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "You Received Success Gifts!");
		Users[rIndex].State[day - 1] = 1;
		OnRequestInfo(aIndex);
		GDCharacterInfoSaveSend(lpObj->Index);
	}
}