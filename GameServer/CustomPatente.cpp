#include "StdAfx.h"
#include "CastleSiege.h"
#include "CustomPatente.h"
#include <fstream>
#include "Tokenizer.h"
#include "Util.h"

bool cPatent::Load(char* path)
{
	this->_Count = 0;

	memset(this->PatentStruct, 0, sizeof(this->PatentStruct));

	Tokenizer          Token;
	TokenizerGroup     Group;
	TokenizerSection   Section;

	Token.ParseFile(std::string(path), Group);


	if (Group.GetSection(0, Section))
	{
		for (int i = 0; i < Section.RowCount; i++)
		{
			this->PatentStruct[this->_Count].Min = Section.Rows[i].GetInt(0);
			this->_Count++;
		}
	}

	return true;
}

void cPatent::GCReqRankLevelUser(int aIndex, int Target)
{
	PMSG_CUSTOM_RANKUSER Result = { 0 };

	int Rank = this->GetRankIndex(Target);

	Result.h.set(0xF3, 0xE5, sizeof(Result));

	Result.Index = Target;

	if (gObj[Target].Authority > 1)
	{
		Result.Rank = -1;
	}
	else
	{
		Result.Rank = Rank;

		if (gCastleSiege->GetState() == CS_STATE_START)
		{
			Result.Rank = -1;
		}
		else
		{
			Result.Rank = Rank;
		}
	}

	DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
}

int cPatent::GetRankIndex(int aIndex)
{
	int Value = gObj[aIndex].Patente;

	for (int i = 0; i < this->_Count; i++)
	{
		if (Value == this->PatentStruct[i].Min)
		{
			return i + 1;
		}
	}
	return -1;
}

cPatent Patent;