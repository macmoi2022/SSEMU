// CrywolfUtil.cpp: implementation of the CCrywolfUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrywolfUtil.h"
#include "DSProtocol.h"
#include "Map.h"
#include "MapServerManager.h"
#include "Notice.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrywolfUtil::CCrywolfUtil() // OK
{

}

CCrywolfUtil::~CCrywolfUtil() // OK
{

}

void CCrywolfUtil::SendAllUserAnyData(BYTE* lpMsg,int size) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0 && gObj[n].Type == OBJECT_USER)
		{
			DataSend(n,lpMsg,size);
		}
	}
	
	#endif
}

void CCrywolfUtil::SendCrywolfUserAnyData(BYTE* lpMsg,int size) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0 && gObj[n].Type == OBJECT_USER && gObj[n].Map == MAP_CRYWOLF)
		{
			DataSend(n,lpMsg,size);
		}
	}
	
	#endif
}

void CCrywolfUtil::SendCrywolfUserAnyMsg(int message,...) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	char buff[256] = {0};

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0 && gObj[n].Type == OBJECT_USER && gObj[n].Map == MAP_CRYWOLF)
		{
			va_list arg;
			va_start(arg,message);
			vsprintf_s(buff,gMessage->GetText(message,gObj[n].Lang),arg);
			va_end(arg);

			gNotice->GCNoticeSend(n,0,0,0,0,0,0,"%s",buff);
		}
	}

	#endif
}

bool CCrywolfUtil::CrywolfAllUserScoreSort(LPOBJ const &lpObj,LPOBJ const &lpTarget) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return ((lpObj->CrywolfMVPScore>=lpTarget->CrywolfMVPScore)?1:0);
	
	#else

	return 0;

	#endif
}
