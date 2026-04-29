// OnlineReward.cpp: implementation of the COnlineReward class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OnlineReward.h"
#include "BonusManager.h"

#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COnlineReward::COnlineReward() // OK
{
	this->m_OnlineRewardInfo.clear();
}

COnlineReward::~COnlineReward() // OK
{

}

void COnlineReward::Load(char* path) // OK
{
	#if(GAMESERVER_SHOP>=1)

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

	this->m_OnlineRewardInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			ONLINE_REWARD_INFO info;

			info.MinLevel = lpReadFile->GetNumber();

			info.MaxLevel = lpReadFile->GetAsNumber();

			info.MinReset = lpReadFile->GetAsNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.Delay = lpReadFile->GetAsNumber();

			info.Coin1[0] = lpReadFile->GetAsNumber();

			info.Coin1[1] = lpReadFile->GetAsNumber();

			info.Coin1[2] = lpReadFile->GetAsNumber();

			info.Coin1[3] = lpReadFile->GetAsNumber();

			#if(GAMESERVER_SHOP==3)

			info.Coin2[0] = lpReadFile->GetAsNumber();

			info.Coin2[1] = lpReadFile->GetAsNumber();

			info.Coin2[2] = lpReadFile->GetAsNumber();

			info.Coin2[3] = lpReadFile->GetAsNumber();

			info.Coin3[0] = lpReadFile->GetAsNumber();

			info.Coin3[1] = lpReadFile->GetAsNumber();

			info.Coin3[2] = lpReadFile->GetAsNumber();

			info.Coin3[3] = lpReadFile->GetAsNumber();

			#endif

			this->m_OnlineRewardInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

void COnlineReward::MainProc() // OK
{
	#if(GAMESERVER_SHOP>=1)

	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if(gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		this->GetOnlineReward(n);
	}

	#endif
}

void COnlineReward::GetOnlineReward(int aIndex) // OK
{
	#if(GAMESERVER_SHOP>=1)

	LPOBJ lpObj = &gObj[aIndex];

	for(std::vector<ONLINE_REWARD_INFO>::iterator it = this->m_OnlineRewardInfo.begin(); it != this->m_OnlineRewardInfo.end(); it++)
	{
		if(it->MinLevel != -1 && it->MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->MaxLevel != -1 && it->MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->MinReset != -1 && it->MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->MaxReset != -1 && it->MaxReset < lpObj->Reset)
		{
			continue;
		}

		if((GetTickCount()-lpObj->OnlineRewardTime) < (DWORD)it->Delay*1000)
		{
			continue;
		}

		lpObj->OnlineRewardTime = GetTickCount();

		if(it->Coin1[lpObj->AccountLevel] > 0 || it->Coin2[lpObj->AccountLevel] > 0 || it->Coin3[lpObj->AccountLevel] > 0)
		{
			gObjCoinAdd(lpObj->Index,gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_COIN1,it->Coin1[lpObj->AccountLevel]),gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_COIN2,it->Coin2[lpObj->AccountLevel]),gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_COIN3,it->Coin3[lpObj->AccountLevel]));
		}
	}

	#endif
}