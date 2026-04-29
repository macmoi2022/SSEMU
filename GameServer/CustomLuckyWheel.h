#pragma once

#include "Item.h"
#include "User.h"
#include "Protocol.h"

#define MAX_LUCKYWHEEL_ITEM 12

struct LUCKYWHEEL_INFO
{
	int Index;
	int ItemType;
	int ItemIndex;
	int Level;
	int Luck;
	int Skill;
	int Option;
	int Exc;

};

struct ITEM_WIN_SEND
{
	PSBMSG_HEAD header;
	int	number;
};

struct PMSG_LUCKYWHEEL_LIST_SEND
{
	PSWMSG_HEAD header; // C1:F3:E6
	BYTE count;
};

struct PMSG_LUCKYWHEEL_LIST
{
	int Index;
	int ItemType;
	int ItemIndex;
	int Level;
	int Luck;
	int Skill;
	int Option;
	int Exc;
};


struct CG_LUCKYREWARD_SEND
{
	PSBMSG_HEAD header; // C1:F3:E0
	int CoinValue;
};

class CLuckyWheel
{
public:
	CLuckyWheel();
	virtual ~CLuckyWheel();
	SingletonInstance(CLuckyWheel)
public:
	void Init();
	void Load(char* path);
	void SetInfo(LUCKYWHEEL_INFO info);
	void Start(LPOBJ lpUser);
	bool GCLuckyWheelSend(int aIndex);
	void ReloadLuckyWheelInterface();
	void SendInfo(int aIndex);
	int CoinValue;
	int ZenValue;
public:
	std::map<int, LUCKYWHEEL_INFO> m_LuckyWheelInfo;
};

#define gLuckyWheel SingNull(CLuckyWheel)
