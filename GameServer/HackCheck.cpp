#include "stdafx.h"
#include "HackCheck.h"
#include "ServerInfo.h"
#include "ThemidaSDK.h"

BYTE EncDecKey1;
BYTE EncDecKey2;

void DecryptData(BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < size;n++)
	{
		lpMsg[n] = (lpMsg[n]^EncDecKey1)-EncDecKey2;
	}
}

void EncryptData(BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < size;n++)
	{
		lpMsg[n] = (lpMsg[n]+EncDecKey2)^EncDecKey1;
	}
}

void InitHackCheck() // OK
{
	VM_START

	char CustomerName[32] = {0};

	wsprintf(CustomerName, gServerInfo->m_CustomerName);

	WORD EncDecKey = 0;

	for(int n=0;n < sizeof(CustomerName);n++)
	{
		EncDecKey += (BYTE)(CustomerName[n]^gServerInfo->m_ServerSerial[(n%sizeof(gServerInfo->m_ServerSerial))]);
	}

	EncDecKey1 = (BYTE)0xE2;
	EncDecKey2 = (BYTE)0x76;

	EncDecKey1 += LOBYTE(EncDecKey);
	EncDecKey2 += HIBYTE(EncDecKey);

	VM_END
}
