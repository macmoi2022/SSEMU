#include "StdAfx.h"
#include "RaffleEvent.h"
#include "Notice.h"
#include "Util.h"
#include <fstream>
#include "Tokenizer.h"
#include "ItemBagManager.h"

cRaffleEvent::cRaffleEvent() : _Active(false)
{
}

bool cRaffleEvent::Load(char* path)
{
	this->_Count[0] = 0; this->_Count[1] = 0;

	memset(this->RaffleStruct, 0, sizeof(this->RaffleStruct));

	Tokenizer          Token;
	TokenizerGroup     Group;
	TokenizerSection   Section;

	Token.ParseFile(std::string(path), Group);

	if (Group.GetSection(0, Section))
	{
		this->_Active = Section.Rows[0].GetInt(0) > 0 ? true : false;
	}

	if (Group.GetSection(1, Section))
	{
		for (int i = 0; i < Section.RowCount; i++)
		{
			this->RaffleStruct[this->_Count[0]]._Day = Section.Rows[i].GetInt(0);
			this->RaffleStruct[this->_Count[0]]._Hours = Section.Rows[i].GetInt(1);
			this->RaffleStruct[this->_Count[0]]._Minutes = Section.Rows[i].GetInt(2);
			this->_Count[0]++;
		}
	}

	return true;
}

void cRaffleEvent::Run()
{
	if (this->_Active)
	{
		SYSTEMTIME Now;
		GetLocalTime(&Now);

		for (int i = 0; i < this->_Count[0]; i++)
		{
			if (Now.wDayOfWeek == this->RaffleStruct[i]._Day && Now.wHour == this->RaffleStruct[i]._Hours && Now.wMinute == this->RaffleStruct[i]._Minutes && Now.wSecond == 0)
			{
				this->_Count[1] = 0;

				for (int Index = OBJECT_START_USER; Index < MAX_OBJECT; ++Index)
				{
					if (gObj[Index].Connected == 3 && gObj[Index].Authority == AUTHORITY_USER)
					{
						this->_Received[this->_Count[1]++] = Index;
					}
				}

				if (this->_Count[1] > 0)
				{
					int Sortear = rand() % this->_Count[1];

					int Target = this->_Received[Sortear];

					for (int Index = OBJECT_START_USER; Index < MAX_OBJECT; ++Index)
					{
						if (gObj[Index].Connected == 3)
						{
							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1121));
							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1122), gObj[Target].Name);
						}
					}

					LPOBJ lpObj = &gObj[Target];

					gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_RAFFLE, -1, -1, lpObj, lpObj->Map, lpObj->X, lpObj->Y);

					gNotice->GCNoticeSend(Target, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1123));

				}
			}
		}
	}
}

cRaffleEvent RaffleEvent;