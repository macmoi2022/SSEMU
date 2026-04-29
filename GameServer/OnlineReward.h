// OnlineReward.h: interface for the COnlineReward class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct ONLINE_REWARD_INFO
{
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int Delay;
	int Coin1[MAX_ACCOUNT_LEVEL];
	int Coin2[MAX_ACCOUNT_LEVEL];
	int Coin3[MAX_ACCOUNT_LEVEL];
};

class COnlineReward
{
	COnlineReward();
	virtual ~COnlineReward();
	SingletonInstance(COnlineReward)
public:
	void Load(char* path);
	void MainProc();
	void GetOnlineReward(int aIndex);
private:
	std::vector<ONLINE_REWARD_INFO> m_OnlineRewardInfo;
};

#define gOnlineReward SingNull(COnlineReward)