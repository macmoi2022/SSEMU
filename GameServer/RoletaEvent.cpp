#include "StdAfx.h"
#include "RoletaEvent.h"
#include "Protocol.h"
#include "Notice.h"
#include "ItemBagManager.h"
#include "EffectManager.h"
#include "Party.h"
#include <fstream>
#include "Tokenizer.h"
#include "CommandManager.h"
#include "ServerInfo.h"
#include "DSProtocol.h"
#include "GameMaster.h"
#include "Util.h"

cRoletaEvent::cRoletaEvent() : _State(Empty)
{
}

cRoletaEvent::~cRoletaEvent()
{
}

bool cRoletaEvent::Load(char* path)
{
	this->_Players = std::vector<LPOBJ>();

	Tokenizer          Token;
	TokenizerGroup     Group;
	TokenizerSection   Section;

	Token.ParseFile(std::string(path), Group);

	if (Group.GetSection(0, Section))
	{
		this->_Active = Section.Rows[0].GetInt(0) > 0 ? true : false;
		strcpy_s(this->_Syntax, sizeof(this->_Syntax), (Section.Rows[0].GetStringPtr(1)));

		this->_Level = Section.Rows[1].GetInt(0);
		this->_MapNumber = Section.Rows[1].GetInt(1);
		this->_X = Section.Rows[1].GetInt(2);
		this->_Y = Section.Rows[1].GetInt(3);
	}

	return true;
}

bool cRoletaEvent::GameMaster(LPOBJ lpObj, char* arg)
{
	int Time = gCommandManager->GetNumber(arg, 0);

	if (Time == NULL)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Erro de sintaxe!");
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Digite: <tempo>");
		return false;
	}

	if (this->_State == Empty && Time != NULL)
	{

		gObjTeleport(lpObj->Index, this->_MapNumber, this->_X, this->_Y);
		this->Start(Time);
		return true;
	}

	return false;
}

bool cRoletaEvent::Check(LPOBJ lpObj)
{
	if (this->_State == Register)
	{
		auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpObj);

		if (it != this->_Players.cend())
		{
			gNotice->GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1113));
			return true;
		}
		else if (lpObj->Level < this->_Level)
		{
			gNotice->GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1111), this->_Level);
			return true;
		}
		else if (lpObj->InDuel != 0)
		{
			gNotice->GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1114));
			return true;
		}

		this->_Players.push_back(lpObj);

		this->PlayerStruct[lpObj->Index]._InEvent = true;

		gObjTeleport(lpObj->Index, this->_MapNumber, this->_X, this->_Y);

		gNotice->GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1115));
	}

	return false;
}

void cRoletaEvent::Start(int Time)
{
	if (Time == 0)
	{
		Time++;
	}

	if (this->_Players.size() > 0)
	{
		this->_Players.clear();
	}

	this->_State = Register;
	this->_Count = (Time * 60) + 1;
}

void cRoletaEvent::Run() 
{
	if (this->_Active)
	{

		if (this->_State == Empty)
		{
			return;
		}

		if (this->_Count > 0)
		{
			this->_Count--;
		}

		switch (this->_State)
		{
		case Register:
		{
			if (this->_Count == 0)
			{
				if (this->_Players.size() < 2)
				{
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1183));

							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1184));

							if (this->PlayerStruct[i]._InEvent == true)
							{
								this->PlayerStruct[i]._InEvent = false;

								gObj[i].Move = true;
							}
						}
					}

					this->_State = Empty;
					this->_Count = 0;
				}
				else
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1185));

					this->_State = Progress;
					this->_Count = 6;
				}
			}
			else
			{
				if ((this->_Count % 60) == 0)
				{
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1183));

							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1186), (this->_Count / 60));

							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1187), this->_Syntax);
						}
					}
				}
			}
		}
		break;
		case Progress:
		{
			switch (this->_Count)
			{
			case 5:
			case 4:
			case 3:
			case 2:
			case 1:
			{
				for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
				{
					if (gObj[i].Connected == 3)
					{
						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1183));

						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1188), this->_Count);
					}
				}
			}
			break;
			case 0:
			{

				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1189));

				this->_State = Waiting;
			}
			break;
			}
		}
		break;
		case Waiting:
		{
			if (this->_Players.size() == 1 || this->_Players.size() == 0)
			{
				this->_State = Final;
				this->_Count = 0;
			}
		}
		break;
		case Final:
		{
			LPOBJ lpObj = this->_Players.front();

			gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_ROLETA, -1, -1, lpObj, lpObj->Map, lpObj->X, lpObj->Y);

			GDRankingUpdateSend(lpObj->Index, "TopRoleta", 1);

			this->PlayerStruct[lpObj->Index]._InEvent = false;

			for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
			{
				if (gObj[i].Connected == 3)
				{
					gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1183));

					gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1190), lpObj->Name);

				}
			}

			GCFireworksSend(lpObj, lpObj->X, lpObj->Y);

			this->_Players.clear();

			this->_State = Empty;
			this->_Count = 0;
		}
		break;
		}
	}
}

bool cRoletaEvent::Attack(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpTargetObj);

	if (it != this->_Players.cend())
	{
		if (this->_State != Empty)
		{
			if ((*it)->Type == OBJECT_USER && this->PlayerStruct[(*it)->Index]._InEvent == true)
			{
				return false;
			}
		}
	}

	return true;
}

bool cRoletaEvent::Trade(int Target, int aIndex)
{
	if (this->_State == Waiting && this->_Time == 0 && gObj[aIndex].Authority > AUTHORITY_USER)
	{
		if (this->PlayerStruct[Target]._InEvent == true)
		{
			this->_Time = 5;

			this->PlayerStruct[Target]._Index = Target;

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cRoletaEvent::Fire, (void*)Target, 0, 0);
			return false;
		}
	}

	return true;
}

void cRoletaEvent::Fire(int aIndex)
{
	while (true)
	{
		if (RoletaEvent._Time > 0)
		{
			RoletaEvent._Time--;

			switch (RoletaEvent._Time)
			{
			case 4:
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1191));
			}
			break;
			case 3:
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1192));
			}
			break;
			case 2:
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1193));
			}
			break;
			case 1:
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1194));
			}
			break;
			case 0:
			{
				int Target = RoletaEvent.PlayerStruct[aIndex]._Index;

				srand(time(NULL));

				int Sortear = rand() % 6;

				if (Sortear == 3)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1195), gObj[Target].Name);

					RoletaEvent.Die(&gObj[Target]);
				}
				else
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1196), gObj[Target].Name);
				}
			}
			break;
			}
		}
		else
		{
			_endthread();
		}

		Sleep(1000);
	}
}

void cRoletaEvent::Die(LPOBJ Target)
{
	auto it = std::find(RoletaEvent._Players.cbegin(), RoletaEvent._Players.cend(), Target);

	if (it != this->_Players.cend())
	{
		Target->Live = FALSE;
		Target->RegenTime = GetTickCount();
		Target->DieRegen = TRUE;

		GCUserDieSend(Target, Target->Index, 0, 0);

		Target->Move = true;

		gObjTeleport(Target->Index, 0, 125, 125);

		this->PlayerStruct[Target->Index]._InEvent = false;

		this->_Players.erase(it);
	}
}

void cRoletaEvent::Quit(LPOBJ lpObj)
{
	if (this->_State != Empty)
	{
		if (this->_Players.size() > 0)
		{
			auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpObj);

			if (it != this->_Players.cend())
			{
				lpObj->Move = true;

				this->PlayerStruct[lpObj->Index]._InEvent = false;

				this->_Players.erase(it);
			}
		}
	}
}

cRoletaEvent RoletaEvent;