// IpManager.h: interface for the CIpManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct IP_ADDRESS_INFO
{
	char IpAddress[16];
	DWORD TickCount;
	int count;
};

class CIpManager
{
	CIpManager();
	SingletonInstance(CIpManager)
public:
	bool CheckIpAddress(char* IpAddress);
	void InsertIpAddress(char* IpAddress);
	void RemoveIpAddress(char* IpAddress);
private:
	std::map<std::string,IP_ADDRESS_INFO> m_IpAddressInfo;
};

#define gIpManager SingNull(CIpManager)