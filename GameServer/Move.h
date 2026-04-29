// Move.h: interface for the CMove class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"
#include "User.h"

#define MAX_MOVE 100

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_TELEPORT_RECV
{
	PBMSG_HEAD header; // C1:1C
	#if(GAMESERVER_UPDATE>=401)
	WORD gate;
	#else
	BYTE gate;
	#endif
	BYTE x;
	BYTE y;
};

struct PMSG_TELEPORT_MOVE_RECV
{
	PBMSG_HEAD header; // C1:8E
	BYTE type;
	DWORD reserved;
	WORD number;
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_TELEPORT_SEND
{
	PBMSG_HEAD header; // C3:1C
	#if(GAMESERVER_UPDATE>=401)
	WORD gate;
	#else
	BYTE gate;
	#endif
	BYTE map;
	BYTE x;
	BYTE y;
	BYTE dir;
};


struct PBMSG_HEADLaMO	// Packet - Byte Type
{
public:
	void set(LPBYTE lpBuf, BYTE head, BYTE size)	// line : 18
	{
		lpBuf[0] = 0xC1;
		lpBuf[1] = size;
		lpBuf[2] = head;
	};	// line : 22

	void setE(LPBYTE lpBuf, BYTE head, BYTE size)	// line : 25
	{
		lpBuf[0] = 0xC3;
		lpBuf[1] = size;
		lpBuf[2] = head;
	};	// line : 29

	BYTE c;
	BYTE size;
	BYTE headcode;
};


struct PMSG_TELEPORT_RESULT
{
	PBMSG_HEADLaMO h;	// C3:1C
	BYTE MoveNumber;	// 3
	BYTE MapNumber;	// 4
	BYTE MapX;	// 5
	BYTE MapY;	// 6
	BYTE Dir;	// 7
};

//**********************************************//
//**********************************************//
//**********************************************//

struct MOVE_INFO
{
	int Index;
	char Name[32];
	int Money;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int Enable[MAX_ACCOUNT_LEVEL];
	int Gate;
};

class CMove
{
	CMove();
	virtual ~CMove();
	SingletonInstance(CMove)
public:
	void Load(char* path);
	bool GetInfo(int index,MOVE_INFO* lpInfo);
	bool GetInfoByName(char* name,MOVE_INFO* lpInfo);
	void Move(LPOBJ lpObj,int index);
	void CGTeleportRecv(PMSG_TELEPORT_RECV* lpMsg,int aIndex);
	void CGTeleportMoveRecv(PMSG_TELEPORT_MOVE_RECV* lpMsg,int aIndex);
	void GCTeleportSend(int aIndex,int gate,BYTE map,BYTE x,BYTE y,BYTE dir);
	void CGTeleportError(int aIndex);
private:
	std::map<int,MOVE_INFO> m_MoveInfo;
};

#define gMove SingNull(CMove)