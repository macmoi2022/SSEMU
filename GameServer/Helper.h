// Helper.h: interface for the CHelper class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"

#define MAX_HELPER_STAGE 5
#define MAX_HELPER_ITEM 12

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_HELPER_DATA_RECV
{
	PWMSG_HEAD header; // C1:AE
	BYTE result;
	BYTE data[256];
};

struct PMSG_HELPER_START_RECV
{
	PSBMSG_HEAD header; // C1:BF:51
	BYTE type;
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_HELPER_DATA_SEND
{
	PWMSG_HEAD header; // C1:AE
	BYTE result;
	BYTE data[256];
};

struct PMSG_HELPER_START_SEND
{
	PSBMSG_HEAD header; // C1:BF:51
	DWORD time;
	DWORD money;
	BYTE result;
};

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_HELPER_DATA_RECV
{
	PSWMSG_HEAD header; // C1:17:00
	WORD index;
	char account[11];
	char name[11];
	BYTE result;
	BYTE data[256];
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_HELPER_DATA_SEND
{
	PSBMSG_HEAD header; // C1:17:00
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_HELPER_DATA_SAVE_SEND
{
	PSWMSG_HEAD header; // C1:17:30
	WORD index;
	char account[11];
	char name[11];
	BYTE data[256];
};

//**********************************************//
//**********************************************//
//**********************************************//

class CHelper
{
	CHelper();
	virtual ~CHelper();
	SingletonInstance(CHelper)
public:
	void MainProc();
	void EnableHelper(LPOBJ lpObj);
	void DisableHelper(LPOBJ lpObj,int result);
	void HelperInfoData(int aIndex,BYTE* lpMsg);
	void CGHelperDataRecv(PMSG_HELPER_DATA_RECV* lpMsg,int aIndex);
	void CGHelperStartRecv(PMSG_HELPER_START_RECV* lpMsg,int aIndex);
	void GCHelperStartSend(int aIndex,int time,int money,int result);
	void DGHelperDataRecv(SDHP_HELPER_DATA_RECV* lpMsg);
	void GDHelperDataSend(int aIndex);
	void GDHelperDataSaveSend(int aIndex,BYTE* data);
};

#define gHelper SingNull(CHelper)