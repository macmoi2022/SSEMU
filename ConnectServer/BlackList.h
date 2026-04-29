// BlackList.h: interface for the CBlackList class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct BLACKLIST_IP_INFO
{
	char IpAddress[17];
}; 

class CBlackList
{
	CBlackList();
	SingletonInstance(CBlackList)
public:
	void Load(char* path);
	bool CheckIpAddress(char* IpAddress);

public:
	std::map<std::string,BLACKLIST_IP_INFO> m_BlackListIpInfo;
};

#define gBlackList SingNull(CBlackList)