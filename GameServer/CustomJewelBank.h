// JewelMix.h: interface for the CJewelMix class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"
#include "User.h"

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PSBMSG_JEWELBANK_RECV
{
	PSBMSG_HEAD header; // C1:F3:F4
	int slot;
};

struct PSBMSG_JEWELBANKWITHDRAW_RECV
{
	PSBMSG_HEAD header; // C1:F3:F4
	int type;
	int count;
};

struct PSBMSG_JEWELBANK_UPDATE_RECV
{
	PSBMSG_HEAD header; // C1:F3:F4
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PSBMSG_JEWELBANK_SEND
{
	PSBMSG_HEAD h;
	int Chaos;
	int Life;
	int Soul;
	int Bless;
	int Creation;
	int Guardian;
	int Harmony;
	int GemStone;
	int LowStone;
	int HighStone;
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//
struct SDHP_CUSTOM_JEWELBANK_SEND
{
	PSBMSG_HEAD header;
	WORD index;
	char account[11];
	WORD type;
	DWORD count;
};

struct SDHP_CUSTOM_JEWELBANK_INFO_SEND
{
	PSBMSG_HEAD header; // C1:F5
	WORD index;
	char account[11];
};


//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//
struct SDHP_CUSTOM_JEWELBANK_INFO_RECV
{
	PSBMSG_HEAD header; // C1:F5
	WORD index;
	int Chaos;
	int Life;
	int Soul;
	int Bless;
	int Creation;
	int Guardian;
	int Harmony;
	int GemStone;
	int LowStone;
	int HighStone;
};
//**********************************************//
//**********************************************//
//**********************************************//

class CCustomJewelBank
{

	CCustomJewelBank();
	virtual ~CCustomJewelBank();
	SingletonInstance(CCustomJewelBank)
public:
	int GetJewelSimpleType(int ItemIndex);
	int GetJewelSimpleIndex(int type);
	int GetJewelBundleIndex(int type);
	void JewelBankRecv(PSBMSG_JEWELBANK_RECV* lpMsg, int aIndex);
	void JewelBankWithDrawRecv(PSBMSG_JEWELBANKWITHDRAW_RECV* lpMsg, int aIndex);
	void CustomJewelBankInfoSend(int index);
	void CustomJewelBankInfoRecv(SDHP_CUSTOM_JEWELBANK_INFO_RECV* lpMsg);
	void GCCustomJewelBankInfoSend(LPOBJ lpObj);
};

#define gCustomJewelBank SingNull(CCustomJewelBank)
