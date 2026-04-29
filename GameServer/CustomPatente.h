#pragma once

struct CUSTOM_RANKUSER_DATA
{
	int  Min;
	int  Max;
};

struct PMSG_CUSTOM_RANKUSER
{
	PSBMSG_HEAD h;
	int Index;
	int Rank;
};

class cPatent
{
public:
	bool Load(char* path);
	void GCReqRankLevelUser(int aIndex, int Target);
	int GetRankIndex(int aIndex);

public:
	int  _Count;

private:
	CUSTOM_RANKUSER_DATA  PatentStruct[50];
};

extern cPatent Patent;