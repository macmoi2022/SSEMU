// MossMerchant.cpp: implementation of the CMossMerchant class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MossMerchant.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "ItemOptionRate.h"
#include "Map.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Notice.h"
#include "RandomManager.h"
#include "ScheduleManager.h"
#include "ServerInfo.h"
#include "ShopManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMossMerchant::CMossMerchant() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	this->m_State = MOSS_STATE_BLANK;
	this->m_RemainTime = 0;
	this->m_TargetTime = 0;
	this->m_TickCount = GetTickCount();
	this->m_MossMerchantIndex = -1;

	this->m_MossMerchantStartTime.clear();

	this->m_MossMerchantItemInfo.clear();

	#endif
}

CMossMerchant::~CMossMerchant() // OK
{

}

void CMossMerchant::Init() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(gServerInfo->m_MossMerchantEvent == 0)
	{
		this->SetState(MOSS_STATE_BLANK);
	}
	else
	{
		this->SetState(MOSS_STATE_EMPTY);
	}

	#endif
}

void CMossMerchant::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=402)

	CReadFile* lpReadFile = new CReadFile;

	if(lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_MossMerchantStartTime.clear();

	this->m_MossMerchantItemInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}
		
			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					this->m_MossMerchantMapNumber = lpReadFile->GetNumber();

					this->m_MossMerchantPositionX = lpReadFile->GetAsNumber();

					this->m_MossMerchantPositionY = lpReadFile->GetAsNumber();

					this->m_MossMerchantDirection = lpReadFile->GetAsNumber();
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					MOSS_MERCHANT_START_TIME info;

					info.Year = lpReadFile->GetNumber();

					info.Month = lpReadFile->GetAsNumber();

					info.Day = lpReadFile->GetAsNumber();

					info.DayOfWeek = lpReadFile->GetAsNumber();

					info.Hour = lpReadFile->GetAsNumber();

					info.Minute = lpReadFile->GetAsNumber();

					info.Second = lpReadFile->GetAsNumber();

					this->m_MossMerchantStartTime.push_back(info);
				}
				else if(section == 2)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					MOSS_MERCHANT_ITEM_INFO info;

					info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

					info.Group = lpReadFile->GetAsNumber();

					info.Option0 = lpReadFile->GetAsNumber();

					info.Option1 = lpReadFile->GetAsNumber();

					info.Option2 = lpReadFile->GetAsNumber();

					info.Option3 = lpReadFile->GetAsNumber();

					info.Option4 = lpReadFile->GetAsNumber();

					info.Option5 = lpReadFile->GetAsNumber();

					info.Option6 = lpReadFile->GetAsNumber();

					info.GambleRate = lpReadFile->GetAsNumber();

					this->m_MossMerchantItemInfo.push_back(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

void CMossMerchant::MainProc() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(gServerInfo->m_MossMerchantEvent == 0)
	{
		return;
	}

	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_TickCount = GetTickCount();

		this->m_RemainTime = (int)difftime(this->m_TargetTime,time(0));

		switch(this->m_State)
		{
			case MOSS_STATE_BLANK:
				this->ProcState_BLANK();
				break;
			case MOSS_STATE_EMPTY:
				this->ProcState_EMPTY();
				break;
			case MOSS_STATE_START:
				this->ProcState_START();
				break;
		}
	}

	#endif
}

void CMossMerchant::ProcState_BLANK() // OK
{

}

void CMossMerchant::ProcState_EMPTY() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(this->m_RemainTime <= 0)
	{
		gNotice->GCNoticeSendToAll(0,0,0,0,0,0,135);

		this->SetState(MOSS_STATE_START);
	}

	#endif
}

void CMossMerchant::ProcState_START() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(this->m_RemainTime <= 0)
	{
		gNotice->GCNoticeSendToAll(0,0,0,0,0,0,136);

		this->SetState(MOSS_STATE_EMPTY);
	}

	#endif
}

void CMossMerchant::SetState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(gServerInfo->m_MossMerchantEvent == 0)
	{
		return;
	}

	this->m_State = state;

	switch(this->m_State)
	{
		case MOSS_STATE_BLANK:
			this->SetState_BLANK();
			break;
		case MOSS_STATE_EMPTY:
			this->SetState_EMPTY();
			break;
		case MOSS_STATE_START:
			this->SetState_START();
			break;
	}

	#endif
}

void CMossMerchant::SetState_BLANK() // OK
{
	
}

void CMossMerchant::SetState_EMPTY() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(OBJECT_RANGE(this->m_MossMerchantIndex) != 0)
	{
		gObjDel(this->m_MossMerchantIndex);
		this->m_MossMerchantIndex = -1;
	}

	this->CheckSync();

	LogAdd(LOG_BLACK,"[Moss Merchant] SetState EMPTY");

	#endif
}

void CMossMerchant::SetState_START() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	this->SetMossMerchant();

	this->m_RemainTime = gServerInfo->m_MossMerchantEventTime;

	this->m_TargetTime = (int)(time(0)+this->m_RemainTime);

	LogAdd(LOG_BLACK,"[Moss Merchant] SetState START");

	#endif
}

void CMossMerchant::CheckSync() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(this->m_MossMerchantStartTime.empty() != 0)
	{
		this->SetState(MOSS_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for(std::vector<MOSS_MERCHANT_START_TIME>::iterator it=this->m_MossMerchantStartTime.begin();it != this->m_MossMerchantStartTime.end();it++)
	{
		ScheduleManager.AddSchedule(it->Year,it->Month,it->Day,it->Hour,it->Minute,it->Second,it->DayOfWeek);
	}

	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(MOSS_STATE_BLANK);
		return;
	}

	this->m_RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));

	this->m_TargetTime = (int)ScheduleTime.GetTime();

	LogAdd(LOG_BLACK,"[Moss Merchant] Sync Start Time. [%d] min remain",(this->m_RemainTime/60));

	#endif
}

void CMossMerchant::SetMossMerchant() // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(OBJECT_RANGE(this->m_MossMerchantIndex) != 0)
	{
		return;
	}

	int index = gMonster->AddMonster(this->m_MossMerchantMapNumber);
	
	if(OBJECT_RANGE(index) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[index];

	int px = this->m_MossMerchantPositionX;
	int py = this->m_MossMerchantPositionY;

	lpObj->PosNum = -1;
	lpObj->X = px;
	lpObj->Y = py;
	lpObj->TX = px;
	lpObj->TY = py;
	lpObj->OldX = px;
	lpObj->OldY = py;
	lpObj->StartX = px;
	lpObj->StartY = py;
	lpObj->Dir = this->m_MossMerchantDirection;
	lpObj->Map = this->m_MossMerchantMapNumber;

	if(gMonster->SetMonster(index,492) == 0)
	{
		gObjDel(index);
		return;
	}

	this->m_MossMerchantIndex = index;

	LogAdd(LOG_BLACK,"[Moss Merchant] Spawn Monster [492] Map:%d X:%d Y:%d",lpObj->Map,lpObj->X,lpObj->Y);

	#endif
}

bool CMossMerchant::GambleItem(LPOBJ lpObj,int group) // OK
{
	#if(GAMESERVER_UPDATE>=402)

	if(this->m_State != MOSS_STATE_START)
	{
		gNotice->GCNoticeSend(lpObj->Index,0,0,0,0,0,0,137);
		return 0;
	}

	if(gItemManager->CheckItemInventorySpace(lpObj,2,4) == 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,0,0,0,0,0,0,138);
		return 0;
	}

	CRandomManager RandomManager;

	for(std::vector<MOSS_MERCHANT_ITEM_INFO>::iterator it=this->m_MossMerchantItemInfo.begin();it != this->m_MossMerchantItemInfo.end();it++)
	{
		if(it->Group == group)
		{
			RandomManager.AddElement((int)(&(*it)),it->GambleRate);
		}
	}

	MOSS_MERCHANT_ITEM_INFO* lpMossMerchantItemInfo = 0;

	if(RandomManager.GetRandomElement((int*)&lpMossMerchantItemInfo) == 0)
	{
		return 0;
	}

	WORD ItemIndex = lpMossMerchantItemInfo->ItemIndex;
	BYTE ItemLevel = 0;
	BYTE ItemOption1 = 0;
	BYTE ItemOption2 = 0;
	BYTE ItemOption3 = 0;
	BYTE ItemNewOption = 0;
	BYTE ItemSetOption = 0;
	BYTE ItemSocketOption[MAX_SOCKET_OPTION] = {0xFF,0xFF,0xFF,0xFF,0xFF};

	gItemOptionRate->GetItemOption0(lpMossMerchantItemInfo->Option0,&ItemLevel);

	gItemOptionRate->GetItemOption1(lpMossMerchantItemInfo->Option1,&ItemOption1);

	gItemOptionRate->GetItemOption2(lpMossMerchantItemInfo->Option2,&ItemOption2);

	gItemOptionRate->GetItemOption3(lpMossMerchantItemInfo->Option3,&ItemOption3);

	gItemOptionRate->GetItemOption4(lpMossMerchantItemInfo->Option4,&ItemNewOption);

	gItemOptionRate->GetItemOption5(lpMossMerchantItemInfo->Option5,&ItemSetOption);

	gItemOptionRate->GetItemOption6(lpMossMerchantItemInfo->Option6,&ItemSocketOption[0]);

	gItemOptionRate->MakeNewOption(ItemIndex,1,ItemNewOption,&ItemNewOption);

	gItemOptionRate->MakeSetOption(ItemIndex,ItemSetOption,&ItemSetOption);

	gItemOptionRate->MakeSocketOption(ItemIndex,ItemSocketOption[0],&ItemSocketOption[0]);

	GDCreateItemSend(lpObj->Index,0xEB,0,0,ItemIndex,ItemLevel,0,ItemOption1,ItemOption2,ItemOption3,-1,ItemNewOption,ItemSetOption,0,0,ItemSocketOption,0xFF,0);

	return 1;

	#else

	return 0;

	#endif
}
void CMossMerchant::ForceStart()
{
	#if(GAMESERVER_UPDATE>=402)

	SYSTEMTIME SystemTime;

	GetLocalTime(&SystemTime);

	MOSS_MERCHANT_START_TIME info;

	info.Year = SystemTime.wYear;

	info.Month = SystemTime.wMonth;

	info.Day = SystemTime.wDay;

	info.DayOfWeek = -1;

	info.Hour = SystemTime.wHour;

	info.Minute = (SystemTime.wMinute+1);

	info.Second = SystemTime.wSecond;

	this->m_MossMerchantStartTime.push_back(info);

	this->Init();

	#endif
}