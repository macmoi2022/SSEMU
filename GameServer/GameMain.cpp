#include "stdafx.h"
#include "GameMain.h"
#include "CommandManager.h"
#include "DSProtocol.h"
#include "HackCheck.h"
#include "JSProtocol.h"
#include "MasterSkillTree.h"
#include "MonsterAI.h"
#include "MonsterManager.h"
#include "ObjectManager.h"
#include "QueueTimer.h"
#include "ServerInfo.h"
#include "SocketManagerUdp.h"
#include "User.h"
#include "Util.h"
#include "ScriptLoader.h"
#include "Protect.h"
#include "RaffleEvent.h"

CConnection gJoinServerConnection;
CConnection gDataServerConnection;

void GameMainInit(HWND hwnd) // OK
{
	if(CreateMutex(0,0,gServerInfo->m_ServerMutex) == 0 || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ErrorMessageBox("Could not open GameServer (Already Exists)");
		return;
	}

	PROTECT_START

	#if( PROTECT_STATE == 1)
	#if(GAMESERVER_UPDATE == 101)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S0_GAME_SERVER);
	#endif
	#if(GAMESERVER_UPDATE == 201)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S2_GAME_SERVER);
	#endif
	#if(GAMESERVER_UPDATE == 301)
	#if(GAMESERVER_SHOP == 1)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S3_GAME_SERVER);
	#endif
	#if(GAMESERVER_SHOP == 2)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S3_GAME_SERVER);
	#endif
	#endif
	#if(GAMESERVER_UPDATE == 401)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S4_GAME_SERVER);
	#endif
	#if(GAMESERVER_UPDATE == 601)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S6_GAME_SERVER);
	#endif
	#if(GAMESERVER_UPDATE == 801)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S8_GAME_SERVER);
	#endif
	#endif


	gObjInit();

	gServerInfo->ReadInit();

	InitHackCheck();

	_beginthread(&MainThread, 0, NULL);

	gObjSetExperienceTable();

	gMonsterManager->SetMonsterData();

	#if(GAMESERVER_UPDATE>=401)

	gMasterSkillTree->SetMasterLevelExperienceTable();

	#endif

	gJoinServerConnection.Init(hwnd,JoinServerProtocolCore);

	gDataServerConnection.Init(hwnd,DataServerProtocolCore);

	PROTECT_FINAL
}

void ConnectServerInfoSend() // OK
{
	PROTECT_START

	SDHP_GAME_SERVER_LIVE_SEND pMsg;

	pMsg.header.set(0xA1,sizeof(pMsg));

	pMsg.ServerCode = gServerInfo->m_ServerCode;

	pMsg.UserCount = gObjTotalUser;

	pMsg.UserTotal = gServerInfo->m_ServerMaxUserNumber;

	gSocketManagerUdp->DataSend((BYTE*)&pMsg,pMsg.header.size);

	PROTECT_FINAL
}

bool JoinServerConnect(DWORD wMsg) // OK
{
	if(gJoinServerConnection.Connect(gServerInfo->m_JoinServerAddress,(WORD)gServerInfo->m_JoinServerPort,wMsg) == 0)
	{
		LogAdd(LOG_RED,"[JoinServer] Connection failure.");
		return 0;
	}

	GJServerInfoSend();
	return 1;
}

bool DataServerConnect(DWORD wMsg) // OK
{
	if(gDataServerConnection.Connect(gServerInfo->m_DataServerAddress,(WORD)gServerInfo->m_DataServerPort,wMsg) == 0)
	{
		LogAdd(LOG_RED,"[DataServer] Connection failure.");
		return 0;
	}

	GDServerInfoSend();
	return 1;
}

bool JoinServerReconnect(HWND hwnd,DWORD wMsg) // OK
{
	if(gJoinServerConnection.CheckState() == 0)
	{
		gJoinServerConnection.Init(hwnd,JoinServerProtocolCore);
		return JoinServerConnect(wMsg);
	}

	return 1;
}

bool DataServerReconnect(HWND hwnd,DWORD wMsg) // OK
{
	if(gDataServerConnection.CheckState() == 0)
	{
		gDataServerConnection.Init(hwnd,DataServerProtocolCore);
		return DataServerConnect(wMsg);
	}

	return 1;
}

void JoinServerMsgProc(WPARAM wParam,LPARAM lParam) // OK
{
	switch(LOWORD(lParam))
	{
		case FD_READ:
			gJoinServerConnection.DataRecv();
			break;
		case FD_WRITE:
			gJoinServerConnection.DataSendEx();
			break;
		case FD_CLOSE:
			gJoinServerConnection.Disconnect();
			gObjAllDisconnect();
			break;
	}
}

void DataServerMsgProc(WPARAM wParam,LPARAM lParam) // OK
{
	switch(LOWORD(lParam))
	{
		case FD_READ:
			gDataServerConnection.DataRecv();
			break;
		case FD_WRITE:
			gDataServerConnection.DataSendEx();
			break;
		case FD_CLOSE:
			gDataServerConnection.Disconnect();
			gObjAllDisconnect();
			break;
	}
}

void CALLBACK QueueTimerCallback(PVOID lpParameter,BOOLEAN TimerOrWaitFired) // OK
{
	PROTECT_START

	static CCriticalSection critical;

	critical.lock();

	switch(((int)lpParameter))
	{
		case QUEUE_TIMER_MONSTER:
			gObjectManager->ObjectMonsterAndMsgProc();
			break;
		case QUEUE_TIMER_MONSTER_MOVE:
			gObjectManager->ObjectMoveProc();
			break;
		#if(GAMESERVER_UPDATE>=201)
		case QUEUE_TIMER_MONSTER_AI:
			gMonsterAI->MonsterAIProc();
			break;
		case QUEUE_TIMER_MONSTER_AI_MOVE:
			gMonsterAI->MonsterMoveProc();
			break;
		#endif
		case QUEUE_TIMER_EVENT:
			gObjEventRunProc();
			break;
		case QUEUE_TIMER_VIEWPORT:
			gObjViewportProc();
			break;
		case QUEUE_TIMER_FIRST:
			gObjFirstProc();
			break;
		case QUEUE_TIMER_CLOSE:
			gObjCloseProc();
			break;
		case QUEUE_TIMER_ACCOUNT_LEVEL:
			gObjAccountLevelProc();
			break;
		case QUEUE_TIMER_LUA:
			gScriptLoader->OnTimerThread();
			break;
		case QUEUE_TIMER_LUA_ASYNC:
			gScriptLoader->OnSQLAsyncResult();
			break;
	}

	critical.unlock();

	PROTECT_FINAL
}
