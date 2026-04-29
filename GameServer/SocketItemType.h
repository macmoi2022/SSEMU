// SocketItemType.h: interface for the CSocketItemType class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Item.h"

struct SOCKET_ITEM_TYPE_INFO
{
	int ItemIndex;
	int MaxSocket;
};

class CSocketItemType
{
	CSocketItemType();
	virtual ~CSocketItemType();
	SingletonInstance(CSocketItemType)
public:
	void Load(char* path);
	bool CheckSocketItemType(int index);
	long GetSocketItemMaxSocket(int index);
private:
	std::map<int,SOCKET_ITEM_TYPE_INFO> m_SocketItemTypeInfo;
};

#define gSocketItemType SingNull(CSocketItemType)