//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct CUSTOMEVO_INFO
{
	int Index;
	int ItemIndex;
	int ResultIndex;
};

class CCustomEvo
{

	CCustomEvo();
	virtual ~CCustomEvo();
	SingletonInstance(CCustomEvo)
public:
	void Load(char* path);
	std::map<int, CUSTOMEVO_INFO> m_CustomEvo;
};


#define gCustomEvo SingNull(CCustomEvo)