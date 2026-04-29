//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct CUSTOMGATE_INFO
{
	int GateIndex;
	int GateItemIndex;
	int MapStart;
	int RangeStartX;
	int RangeStartY;
	int MapEnd;
	int RangeEndX;
	int RangeEndY;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int AccountLevel;
};

class CCustomGate
{

	CCustomGate();
	virtual ~CCustomGate();
	SingletonInstance(CCustomGate)
public:
	void Load(char* path);
	void MainProc();
	bool GetGate(LPOBJ lpObj);
private:
	std::map<int, CUSTOMGATE_INFO> m_CustomGate;
};


#define gCustomGate SingNull(CCustomGate)