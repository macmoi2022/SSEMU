#include "stdafx.h"
#include "CustomLuckyWheel.h"
#include "PcPoint.h"
#include "DSProtocol.h"
#include "Util.h"
#include "ItemManager.h"
#include "User.h"
#include "Notice.h"
#include "Message.h"
#include "ServerInfo.h"
#include "ItemLevel.h"
#include "ReadFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLuckyWheel::CLuckyWheel()
{
	this->Init();
}

CLuckyWheel::~CLuckyWheel()
{
}

void CLuckyWheel::Init()
{
	for (int n = 0; n < MAX_LUCKYWHEEL_ITEM; n++)
	{
		this->m_LuckyWheelInfo[n].Index = -1;
	}

	this->m_LuckyWheelInfo.clear();
}

void CLuckyWheel::Load(char* path) // OK
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

	this->Init();

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

			LUCKYWHEEL_INFO info;

			memset(&info, 0, sizeof(info));

			info.Index = lpReadFile->GetNumber();

			info.ItemType = lpReadFile->GetAsNumber();

			info.ItemIndex = lpReadFile->GetAsNumber();

			info.Level = lpReadFile->GetAsNumber();

			info.Skill = lpReadFile->GetAsNumber();

			info.Luck = lpReadFile->GetAsNumber();

			info.Option = lpReadFile->GetAsNumber();

			info.Exc = lpReadFile->GetAsNumber();

			this->m_LuckyWheelInfo.insert(std::pair<int, LUCKYWHEEL_INFO>(info.Index, info));

		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CLuckyWheel::SetInfo(LUCKYWHEEL_INFO info)
{
	if (info.Index < 0 || info.Index >= MAX_LUCKYWHEEL_ITEM)
	{
		return;
	}
	this->m_LuckyWheelInfo[info.Index] = info;
}

void CLuckyWheel::Start(LPOBJ lpObj)
{

	if (lpObj->Coin1 < ((DWORD)gServerInfo->LuckyWheelMoney))
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(909), gServerInfo->LuckyWheelMoney);
		return;
	}	

	gObjCoinSub(lpObj->Index, gServerInfo->LuckyWheelMoney, 0, 0);

	int number = rand() % 12;

	Sleep(3000);
	srand((unsigned int)time((NULL)));

	GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, GET_ITEM(m_LuckyWheelInfo[number].ItemType, m_LuckyWheelInfo[number].ItemIndex), m_LuckyWheelInfo[number].Level, 0, m_LuckyWheelInfo[number].Skill, m_LuckyWheelInfo[number].Luck, m_LuckyWheelInfo[number].Option, -1, m_LuckyWheelInfo[number].Exc, 0, 0, 0, 0, 0xFF, 0);

	ITEM_WIN_SEND pMsg;
	pMsg.header.set(0xFB, 0x16, sizeof(pMsg));
	pMsg.number = number;
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}


void CLuckyWheel::ReloadLuckyWheelInterface() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			this->GCLuckyWheelSend(n);
			this->SendInfo(n);

		}
	}
}

bool CLuckyWheel::GCLuckyWheelSend(int aIndex) // OK
{
	BYTE send[2048];

	PMSG_LUCKYWHEEL_LIST_SEND pMsg;

	pMsg.header.set(0xFB, 0xFB, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_LUCKYWHEEL_LIST info;

	for (std::map<int, LUCKYWHEEL_INFO>::iterator it = this->m_LuckyWheelInfo.begin(); it != this->m_LuckyWheelInfo.end(); it++)
	{
		info.Index = it->second.Index;

		info.ItemType = it->second.ItemType;

		info.ItemIndex = it->second.ItemIndex;

		info.Level = it->second.Level;

		info.Luck = it->second.Luck;

		info.Skill = it->second.Skill;

		info.Option = it->second.Option;

		info.Exc = it->second.Exc;


		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);

	return 1;
}


void CLuckyWheel::SendInfo(int aIndex)
{
	CG_LUCKYREWARD_SEND pMsg;

	pMsg.header.set(0xFB, 0xFC, sizeof(pMsg));

	pMsg.CoinValue = gServerInfo->LuckyWheelMoney;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}