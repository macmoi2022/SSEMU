// ConnectionManager.cpp: implementation of the CConnectionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnectionManager.h"
#include "BlackList.h"
#include "Log.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectionManager::CConnectionManager() // OK
{
	this->m_ConnectionTimeInfo.clear();
}

bool CConnectionManager::CheckMaxConnection(int aIndex) // OK
{
	if(gServerInfo->m_MaxConnectionPerIP != 0 || gServerInfo->m_MaxConnectionPerHID != 0)
	{
		int ipCount = 0;

		int hwCount = 0;

		for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
		{
			if(gObj[n].Connected != OBJECT_OFFLINE)
			{
				if(strcmp(gObj[n].IpAddr,gObj[aIndex].IpAddr) == 0)
				{
					ipCount++;
				}

				if(strcmp(gObj[n].HardwareId,gObj[aIndex].HardwareId) == 0)
				{
					hwCount++;
				}
			}
		}

		if(gServerInfo->m_MaxConnectionPerIP > 0 && ipCount > gServerInfo->m_MaxConnectionPerIP)
		{
			gLog->Output(LOG_CONNECT,"[ObjectManager][%d] CloseClient [%s] Reason: IP Limit Exceed [%d][%d]",aIndex,gObj[aIndex].IpAddr,ipCount,gServerInfo->m_MaxConnectionPerIP);
			return false;
		}

		if(gServerInfo->m_MaxConnectionPerHID > 0 && hwCount > gServerInfo->m_MaxConnectionPerHID)
		{
			gLog->Output(LOG_CONNECT,"[ObjectManager][%d] CloseClient [%s] Reason: HID Limit Exceed [%d][%d]",aIndex,gObj[aIndex].HardwareId,hwCount,gServerInfo->m_MaxConnectionPerHID);
			return false;
		}
	}

	return true;
}

void CConnectionManager::CGHardwareIdRecv(PMSG_HARDWARE_ID_INFO_RECV* lpMsg,int aIndex) // OK
{
	char HardwareId[45] = {0};

	PacketArgumentDecrypt(HardwareId,lpMsg->HardwareId,sizeof(HardwareId));

	if(HardwareId[8] != 0x2D && HardwareId[17] != 0x2D && HardwareId[26] != 0x2D && HardwareId[35] != 0x2D)
	{
		gLog->Output(LOG_CONNECT,"[ObjectManager][%d] CloseClient [%s] Reason: Invalid Hardware ID",aIndex,gObj[aIndex].IpAddr);
		CloseClient(aIndex);
		return;
	}

	if(strlen(HardwareId) != 44)
	{
		gLog->Output(LOG_CONNECT,"[ObjectManager][%d] CloseClient [%s] Reason: Invalid Hardware ID",aIndex,gObj[aIndex].IpAddr);
		CloseClient(aIndex);
		return;
	}
	
	if(gBlackList->CheckHardwareId(HardwareId) == 0)
	{
		CloseClient(aIndex);
		return;
	}

	gObj[aIndex].ClientVerify = true;

	gObj[aIndex].Lang = ((lpMsg->Language>2)?0:lpMsg->Language);

	memcpy(gObj[aIndex].HardwareId,HardwareId,sizeof(gObj[aIndex].HardwareId));

	gLog->Output(LOG_CONNECT,"[ObjectManager][%d] Verified client [%s][%s]",aIndex,gObj[aIndex].IpAddr,gObj[aIndex].HardwareId);
}