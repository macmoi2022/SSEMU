// CrywolfUtil.h: interface for the CCrywolfUtil class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CCrywolfUtil
{
	CCrywolfUtil();
	virtual ~CCrywolfUtil();
	SingletonInstance(CCrywolfUtil)
public:
	void SendAllUserAnyData(BYTE* lpMsg,int size);
	void SendCrywolfUserAnyData(BYTE* lpMsg,int size);
	void SendCrywolfUserAnyMsg(int message,...);
	static bool CrywolfAllUserScoreSort(LPOBJ const &lpObj,LPOBJ const &lpTarget);
};

#define gCrywolfUtil SingNull(CCrywolfUtil)