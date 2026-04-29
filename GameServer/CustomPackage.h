#pragma once

#include "User.h"
#include "Protocol.h"

struct CG_SENDPACKAGE_RECV
{
	PSBMSG_HEAD Head;
	int Type;
};

class cPackage
{
public:
	cPackage();
	virtual ~cPackage();
	SingletonInstance(cPackage)
public:
	void RecvPackageType(CG_SENDPACKAGE_RECV* Data, int aIndex);



};

#define gPackage SingNull(cPackage)