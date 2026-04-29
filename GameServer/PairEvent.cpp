#include "StdAfx.h"
#include "PairEvent.h"
#include <fstream>
#include "Tokenizer.h"
#include "Notice.h"
#include "Party.h"
#include "EffectManager.h"
#include "ServerInfo.h"
#include "DSProtocol.h"
#include "CommandManager.h"
#include "ItemBagManager.h"

cPairEvent::cPairEvent() : _State(Empty)
{
}

cPairEvent::~cPairEvent()
{
}

bool cPairEvent::Load(char* path)
{
	this->_List = 0;

	memset(this->EventStruct, 0, sizeof(this->EventStruct));

	this->_Teams[0] = std::vector<PairTeam>();
	this->_Teams[1] = std::vector<PairTeam>();
	this->_Registered = std::vector<LPOBJ>();

	Tokenizer          Token;
	TokenizerGroup     Group;
	TokenizerSection   Section;

	Token.ParseFile(std::string(path), Group);

	if (Group.GetSection(0, Section))
	{
		this->_Active = Section.Rows[0].GetInt(0) > 0 ? true : false;
		strcpy_s(this->_Syntax, sizeof(this->_Syntax), (Section.Rows[0].GetStringPtr(1)));
		this->_Patente = Section.Rows[0].GetInt(2) > 0 ? true : false;

		this->_Level = Section.Rows[1].GetInt(0);
		this->_MapNumber[0] = Section.Rows[1].GetInt(1);
		this->_X[0] = Section.Rows[1].GetInt(2);
		this->_Y[0] = Section.Rows[1].GetInt(3);
		this->_MapNumber[1] = Section.Rows[1].GetInt(4);
		this->_X[1] = Section.Rows[1].GetInt(5);
		this->_Y[1] = Section.Rows[1].GetInt(6);
	}

	if (Group.GetSection(1, Section))
	{
		for (int i = 0; i < Section.RowCount; i++)
		{
			this->EventStruct[this->_List].Day = Section.Rows[i].GetInt(0);
			this->EventStruct[this->_List].Hours = Section.Rows[i].GetInt(1);
			this->EventStruct[this->_List].Minutes = Section.Rows[i].GetInt(2);
			this->EventStruct[this->_List].Time = Section.Rows[i].GetInt(3);
			this->_List++;
		}
	}

	return true;
}

bool cPairEvent::GameMaster(LPOBJ lpObj, char* arg)
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
		gObjTeleport(lpObj->Index, this->_MapNumber[1], this->_X[1], this->_Y[1]);
		this->Start(Time, 5);
		return true;
	}

	return false;
}

bool cPairEvent::Check(LPOBJ lpObj)
{
	if (this->_State == Register && lpObj->Live == TRUE)
	{
		if (this->_Registered.size() > 0)
		{
			for (auto it = this->_Registered.cbegin(); it != this->_Registered.cend(); ++it)
			{
				if ((*it) == lpObj)
				{
					gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1113));
					return true;
				}
			}
		}

		if (lpObj->Level < this->_Level)
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1111), this->_Level);
			return true;
		}
		else if (lpObj->InDuel != 0)
		{
			gNotice->GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1114));
			return true;
		}

		if (lpObj->PartyNumber >= 0)
		{
			if (gParty->GetMemberCount(lpObj->PartyNumber) <= 2)
			{
				gParty->Destroy(lpObj->PartyNumber);
			}
			else
			{
				gParty->DelMember(lpObj->PartyNumber, lpObj->Index);
			}
		}

		lpObj->InEvent = true;

		this->_Registered.push_back(lpObj);

		gEffectManager->ClearAllEffect(lpObj);

		gObjTeleport(lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1115));
	}

	return true;
}

void cPairEvent::Start(int Time, int Class)
{
	if (Time == 0)
	{
		Time++;
	}

	if (this->_Teams[0].size() > 0)
	{
		this->_Teams[0].clear();
	}

	if (this->_Teams[1].size() > 0)
	{
		this->_Teams[1].clear();
	}

	if (this->_Registered.size() > 0)
	{
		this->_Registered.clear();
	}

	this->_State = Register;
	this->_Count = (Time * 60) + 1;
	this->_Type = (BYTE)(Class);
}

void cPairEvent::Run()
{
	if (this->_Active)
	{
		SYSTEMTIME Now;
		GetLocalTime(&Now);

		for (int i = 0; i < this->_List; i++)
		{
			if (Now.wDayOfWeek == this->EventStruct[i].Day && Now.wHour == this->EventStruct[i].Hours && Now.wMinute == this->EventStruct[i].Minutes && Now.wSecond == 0)
			{
				if (this->_State == Empty)
				{
					this->Start(this->EventStruct[i].Time, 5);
				}
			}
		}

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
				if (this->_Registered.size() < 4)
				{
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1160));
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1161));

							if (gObj[i].InEvent == true)
							{
								gObj[i].InEvent = false;

								gObj[i].Move = true;
							}
						}
					}

					this->_State = Empty;
					this->_Count = 0;
				}
				else
				{
					PairTeam Team;
					memset(&Team, 0, sizeof(Team));

					std::sort(this->_Registered.begin(), this->_Registered.end());

					BYTE id = 0;

					for (std::vector<LPOBJ>::const_iterator it = this->_Registered.cbegin(); it != this->_Registered.cend(); ++it)
					{
						Team.Player[id].lpObj = (*it);

						++id;

						if (id == 2)
						{
							this->_Teams[0].push_back(Team);

							id = 0;
						}
					}

					if (id == 1)
					{
						gNotice->GCNoticeSend(Team.Player[0].lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1162));

						Team.Player[0].lpObj->Move = true;
						Team.Player[0].lpObj->InEvent = false;

						auto it = std::find(this->_Registered.cbegin(), this->_Registered.cend(), Team.Player[0].lpObj);

						if (it != this->_Registered.cend())
						{
							this->_Registered.erase(it);
						}

						Team.Player[0].lpObj = NULL;
						Team.Player[1].lpObj = NULL;
					}

					sprintf_s(this->_Buffer[0], gMessage->GetMessage(1163));
					sprintf_s(this->_Buffer[1], gMessage->GetMessage(1164));

					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[0]);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[1]);
						}
					}

					this->_State = Select;
					this->_Count = 3;
				}
			}
			else
			{
				if ((this->_Count % 60) == 0)
				{
					sprintf_s(this->_Buffer[0], gMessage->GetMessage(1160));
					sprintf_s(this->_Buffer[1], gMessage->GetMessage(1165), (this->_Count / 60));
					sprintf_s(this->_Buffer[2], gMessage->GetMessage(1166), this->_Syntax);

					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[0]);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[1]);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[2]);
						}
					}
				}
			}
		}
		break;
		case Select:
		{
			if (this->_Count == 0)
			{
				switch (this->_Teams[0].size())
				{
				case 0:
				{
					this->_State = NextStage;
					this->_Count = 0;
				}
				break;
				case 1:
				{
				SelectWO:
					this->_Selected[0] = this->_Teams[0].front();

					sprintf_s(this->_Buffer[0], gMessage->GetMessage(1160));

					if (this->_Selected[0].Player[0].lpObj && this->_Selected[0].Player[1].lpObj)
					{
						sprintf_s(this->_Buffer[1], gMessage->GetMessage(1167), this->_Selected[0].Player[0].lpObj->Name, this->_Selected[0].Player[1].lpObj->Name);
					}
					else if (this->_Selected[0].Player[0].lpObj && !this->_Selected[0].Player[1].lpObj)
					{
						sprintf_s(this->_Buffer[1], gMessage->GetMessage(1168), this->_Selected[0].Player[0].lpObj->Name);
					}
					else if (!this->_Selected[0].Player[0].lpObj && this->_Selected[0].Player[1].lpObj)
					{
						sprintf_s(this->_Buffer[1], gMessage->GetMessage(1168), this->_Selected[0].Player[1].lpObj->Name);
					}

					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[0]);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[1]);
						}
					}

					this->_Teams[1].push_back(this->_Selected[0]);

					this->_State = NextStage;
					this->_Count = 0;
				}
				break;
				default:
				{
					std::vector<PairTeam>::iterator Team1, Team2;

					while (true)
					{
						Team1 = this->_Teams[0].begin();
						Team2 = (--this->_Teams[0].end());

						if (!Team1->Player[0].lpObj)
						{
							Team1->Player[0].Quit = true;
						}

						if (!Team1->Player[1].lpObj)
						{
							Team1->Player[1].Quit = true;
						}

						if (!Team2->Player[0].lpObj)
						{
							Team2->Player[0].Quit = true;
						}

						if (!Team2->Player[1].lpObj)
						{
							Team2->Player[1].Quit = true;
						}

						if (Team1->Player[0].Quit && Team1->Player[1].Quit || !Team1->Player[0].lpObj || !Team1->Player[1].lpObj)
						{
							this->_Teams[0].erase(Team1);
							continue;
						}

						if (Team2->Player[0].Quit && Team2->Player[1].Quit || !Team2->Player[0].lpObj || !Team2->Player[1].lpObj)
						{
							this->_Teams[0].erase(Team2);
							continue;
						}

						break;
					}

					if (Team1 == Team2)
					{
						goto SelectWO;
					}
					else
					{
						this->_Selected[0] = *Team1;
						this->_Selected[0].Score = 0;
						this->_Selected[0].Player[0].Die = false;
						this->_Selected[0].Player[0].Quit = false;
						this->_Selected[0].Player[0].Select = false;
						this->_Selected[0].Player[1].Die = false;
						this->_Selected[0].Player[1].Quit = false;
						this->_Selected[0].Player[1].Select = false;

						this->_Selected[1] = *Team2;
						this->_Selected[1].Score = 0;
						this->_Selected[1].Player[0].Die = false;
						this->_Selected[1].Player[0].Quit = false;
						this->_Selected[1].Player[0].Select = false;
						this->_Selected[1].Player[1].Die = false;
						this->_Selected[1].Player[1].Quit = false;
						this->_Selected[1].Player[1].Select = false;

						sprintf_s(this->_Buffer[0], gMessage->GetMessage(1160));

						if (this->_Selected[0].Player[0].lpObj && this->_Selected[0].Player[1].lpObj && this->_Selected[1].Player[0].lpObj && this->_Selected[1].Player[1].lpObj)
						{
							sprintf_s(this->_Buffer[1], gMessage->GetMessage(1169), this->_Selected[0].Player[0].lpObj->Name, this->_Selected[0].Player[1].lpObj->Name, this->_Selected[1].Player[0].lpObj->Name, this->_Selected[1].Player[1].lpObj->Name);
						}

						for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
						{
							if (gObj[i].Connected == 3)
							{
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[0]);
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[1]);
							}
						}

						this->_Sended = false;
						this->_State = Progress;
						this->_Count = 8;
						this->_Teams[0].erase(Team2);
						this->_Teams[0].erase(Team1);
					}
				}
				break;
				}
			}
		}
		break;
		case Progress:
		{
			switch (this->_Count)
			{
			case 5:
			{
				if (this->_Selected[0].Player[0].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[0].Player[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1170));

					this->_Selected[0].Player[0].lpObj->PKLevel = 6;

					this->_Selected[0].Player[0].lpObj->PKCount = 3;

					this->_Selected[0].Player[0].lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;;

					this->_Selected[0].Player[0].Select = true;

					gObjTeleport(this->_Selected[0].Player[0].lpObj->Index, this->_MapNumber[1], this->_X[1] - 1, this->_Y[1] - 1);

					GCPKLevelSend(this->_Selected[0].Player[0].lpObj->Index, 6);
				}

				if (this->_Selected[0].Player[1].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[0].Player[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1170));

					this->_Selected[0].Player[1].lpObj->PKLevel = 6;

					this->_Selected[0].Player[1].lpObj->PKCount = 3;

					this->_Selected[0].Player[1].lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;;

					this->_Selected[0].Player[1].Select = true;

					gObjTeleport(this->_Selected[0].Player[1].lpObj->Index, this->_MapNumber[1], this->_X[1] + 1, this->_Y[1] - 1);

					GCPKLevelSend(this->_Selected[0].Player[1].lpObj->Index, 6);
				}

				if (this->_Selected[1].Player[0].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[1].Player[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1170));

					this->_Selected[1].Player[0].lpObj->PKLevel = 6;

					this->_Selected[1].Player[0].lpObj->PKCount = 3;

					this->_Selected[1].Player[0].lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;;

					this->_Selected[1].Player[0].Select = true;

					gObjTeleport(this->_Selected[1].Player[0].lpObj->Index, this->_MapNumber[1], this->_X[1] - 1, this->_Y[1] + 1);

					GCPKLevelSend(this->_Selected[1].Player[0].lpObj->Index, 6);
				}

				if (this->_Selected[1].Player[1].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[1].Player[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1170));

					this->_Selected[1].Player[1].lpObj->PKLevel = 6;

					this->_Selected[1].Player[1].lpObj->PKCount = 3;

					this->_Selected[1].Player[1].lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;;

					this->_Selected[1].Player[1].Select = true;

					gObjTeleport(this->_Selected[1].Player[1].lpObj->Index, this->_MapNumber[1], this->_X[1] + 1, this->_Y[1] + 1);

					GCPKLevelSend(this->_Selected[1].Player[1].lpObj->Index, 6);
				}
			}
			break;
			case 3:
			case 2:
			case 1:
			{
				if (this->_Selected[0].Player[0].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[0].Player[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1171), this->_Count);
				}

				if (this->_Selected[0].Player[1].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[0].Player[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1171), this->_Count);
				}

				if (this->_Selected[1].Player[0].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[1].Player[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1171), this->_Count);
				}

				if (this->_Selected[1].Player[1].lpObj)
				{
					gNotice->GCNoticeSend(this->_Selected[1].Player[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1171), this->_Count);
				}
			}
			break;
			case 0:
			{
				if (this->_Sended == false)
				{
					if (this->_Selected[0].Player[0].lpObj)
					{
						gNotice->GCNoticeSend(this->_Selected[0].Player[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1172));
					}

					if (this->_Selected[0].Player[1].lpObj)
					{
						gNotice->GCNoticeSend(this->_Selected[0].Player[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1172));
					}

					if (this->_Selected[1].Player[0].lpObj)
					{
						gNotice->GCNoticeSend(this->_Selected[1].Player[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1172));
					}

					if (this->_Selected[1].Player[1].lpObj)
					{
						gNotice->GCNoticeSend(this->_Selected[1].Player[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1172));
					}

					this->_Sended = true;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case Died:
		{
			if (this->_Count == 0)
			{
				if ((this->_Selected[0].Player[0].Die || !this->_Selected[0].Player[0].lpObj) &&
					(this->_Selected[0].Player[1].Die || !this->_Selected[0].Player[1].lpObj) &&
					(this->_Selected[1].Player[0].Die || !this->_Selected[1].Player[0].lpObj) &&
					(this->_Selected[1].Player[1].Die || !this->_Selected[1].Player[1].lpObj))
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1173));

					this->_Sended = false;

					this->_State = Progress;
					this->_Count = 8;
				}
				else
				{
					BYTE Winner = ((this->_Selected[0].Player[0].Die && this->_Selected[0].Player[1].Die) || (this->_Selected[0].Player[0].Die && this->_Selected[0].Player[1].Quit) || (this->_Selected[0].Player[0].Quit && this->_Selected[0].Player[1].Die)) ? 1 : 0;
					BYTE Loser = (Winner == 1) ? 0 : 1;

					this->_Selected[Winner].Score++;

					sprintf_s(this->_Buffer[0], gMessage->GetMessage(1174));

					if (this->_Selected[0].Player[0].lpObj && this->_Selected[0].Player[1].lpObj && this->_Selected[1].Player[0].lpObj && this->_Selected[1].Player[1].lpObj)
					{
						sprintf_s(this->_Buffer[1], gMessage->GetMessage(1175), this->_Selected[0].Player[0].lpObj->Name, this->_Selected[0].Player[1].lpObj->Name, this->_Selected[1].Player[0].lpObj->Name, this->_Selected[1].Player[1].lpObj->Name);
					}

					sprintf_s(this->_Buffer[2], gMessage->GetMessage(1176), this->_Selected[0].Score, this->_Selected[1].Score);

					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[0]);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[1]);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[2]);
						}
					}

					if (this->_Selected[Winner].Score >= 3)
					{
						if (this->_Selected[Winner].Player[0].lpObj && this->_Selected[Winner].Player[1].lpObj)
						{
							gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1177), this->_Selected[Winner].Player[0].lpObj->Name, this->_Selected[Winner].Player[1].lpObj->Name);
						}
						else if (this->_Selected[Winner].Player[0].lpObj && !this->_Selected[Winner].Player[1].lpObj)
						{
							gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1178), this->_Selected[Winner].Player[0].lpObj->Name);
						}
						else if (!this->_Selected[Winner].Player[0].lpObj && this->_Selected[Winner].Player[1].lpObj)
						{
							gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1178), this->_Selected[Winner].Player[1].lpObj->Name);
						}

						this->_Teams[1].push_back(this->_Selected[Winner]);

						if (this->_Selected[Winner].Player[0].lpObj)
						{
							this->_Selected[Winner].Player[0].lpObj->PKLevel = 3;

							this->_Selected[Winner].Player[0].lpObj->PKCount = 3;

							this->_Selected[Winner].Player[0].lpObj->PKTime = 0;

							this->_Selected[Winner].Player[0].Select = false;

							GCPKLevelSend(this->_Selected[Winner].Player[0].lpObj->Index, 3);

							gObjTeleport(this->_Selected[Winner].Player[0].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);
						}

						if (this->_Selected[Winner].Player[1].lpObj)
						{
							this->_Selected[Winner].Player[1].lpObj->PKLevel = 3;

							this->_Selected[Winner].Player[1].lpObj->PKCount = 3;

							this->_Selected[Winner].Player[1].lpObj->PKTime = 0;

							this->_Selected[Winner].Player[1].Select = false;

							GCPKLevelSend(this->_Selected[Winner].Player[1].lpObj->Index, 3);

							gObjTeleport(this->_Selected[Winner].Player[1].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);
						}

						if (this->_Selected[Loser].Player[0].lpObj)
						{
							this->_Selected[Loser].Player[0].lpObj->PKLevel = 3;

							this->_Selected[Loser].Player[0].lpObj->PKCount = 3;

							this->_Selected[Loser].Player[0].lpObj->PKTime = 0;

							this->_Selected[Loser].Player[0].Select = false;

							GCPKLevelSend(this->_Selected[Loser].Player[0].lpObj->Index, 3);

							gObjTeleport(this->_Selected[Loser].Player[0].lpObj->Index, 0, 125, 125);

							this->_Selected[Loser].Player[0].lpObj->InEvent = false;
						}

						if (this->_Selected[Loser].Player[1].lpObj)
						{
							this->_Selected[Loser].Player[1].lpObj->PKLevel = 3;

							this->_Selected[Loser].Player[1].lpObj->PKCount = 3;

							this->_Selected[Loser].Player[1].lpObj->PKTime = 0;

							this->_Selected[Loser].Player[1].Select = false;

							GCPKLevelSend(this->_Selected[Loser].Player[1].lpObj->Index, 3);

							gObjTeleport(this->_Selected[Loser].Player[1].lpObj->Index, 0, 125, 125);

							this->_Selected[Loser].Player[1].lpObj->InEvent = false;
						}

						this->_State = Select;
						this->_Count = 0;
					}
					else
					{
						this->_Sended = false;
						this->_State = Progress;
						this->_Count = 8;
					}
				}

				this->_Selected[0].Player[0].Die = false;
				this->_Selected[0].Player[1].Die = false;
				this->_Selected[1].Player[0].Die = false;
				this->_Selected[1].Player[1].Die = false;
			}
		}
		break;
		case NextStage:
		{
			if (this->_Teams[0].size() > 0)
			{
				this->_Teams[0].clear();
			}

			this->_Teams[0] = this->_Teams[1];

			if (this->_Teams[1].size() > 0)
			{
				this->_Teams[1].clear();
			}

			memset(&this->_Selected, 0, sizeof(this->_Selected));

			switch (this->_Teams[0].size())
			{
			case 0:
			{
				this->_State = Empty;
				this->_Count = 0;
			}
			break;
			case 1:
			{
				this->_State = Final;
				this->_Count = 0;
			}
			break;
			default:
			{
				if ((this->_Teams[0].size() % 2) == 0)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1179));
				}
				else
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1180));
				}

				this->_State = Select;
				this->_Count = 0;
			}
			break;
			}
		}
		break;
		case WO:
		{
			BYTE Winner = (this->_Selected[0].Player[0].Quit && this->_Selected[0].Player[1].Quit) || (!this->_Selected[0].Player[0].lpObj) || (!this->_Selected[0].Player[1].lpObj) ? 1 : 0;
			BYTE Loser = (Winner == 1) ? 0 : 1;

			if (this->_Selected[Winner].Player[0].lpObj && this->_Selected[Winner].Player[1].lpObj)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1167), this->_Selected[Winner].Player[0].lpObj->Name, this->_Selected[Winner].Player[1].lpObj->Name);
			}
			else if (this->_Selected[Winner].Player[0].lpObj && !this->_Selected[Winner].Player[1].lpObj)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1168), this->_Selected[Winner].Player[0].lpObj->Name);
			}
			else if (!this->_Selected[Winner].Player[0].lpObj && this->_Selected[Winner].Player[1].lpObj)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1168), this->_Selected[Winner].Player[1].lpObj->Name);
			}

			this->_Teams[1].push_back(this->_Selected[Winner]);

			this->_Selected[0].Player[0].Select = false;
			this->_Selected[0].Player[1].Select = false;
			this->_Selected[1].Player[0].Select = false;
			this->_Selected[1].Player[1].Select = false;

			if (this->_Selected[Winner].Player[0].lpObj)
			{
				this->_Selected[Winner].Player[0].lpObj->PKLevel = 3;

				this->_Selected[Winner].Player[0].lpObj->PKCount = 3;

				this->_Selected[Winner].Player[0].lpObj->PKTime = 0;

				GCPKLevelSend(this->_Selected[Winner].Player[0].lpObj->Index, 3);

				gObjTeleport(this->_Selected[Winner].Player[0].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);
			}

			if (this->_Selected[Winner].Player[1].lpObj)
			{
				this->_Selected[Winner].Player[1].lpObj->PKLevel = 3;

				this->_Selected[Winner].Player[1].lpObj->PKCount = 3;

				this->_Selected[Winner].Player[1].lpObj->PKTime = 0;

				GCPKLevelSend(this->_Selected[Winner].Player[1].lpObj->Index, 3);

				gObjTeleport(this->_Selected[Winner].Player[1].lpObj->Index, this->_MapNumber[1], this->_X[1], this->_Y[1]);
			}

			if (this->_Selected[Loser].Player[0].lpObj)
			{
				this->_Selected[Loser].Player[0].lpObj->PKLevel = 3;

				this->_Selected[Loser].Player[0].lpObj->PKCount = 3;

				this->_Selected[Loser].Player[0].lpObj->PKTime = 0;

				GCPKLevelSend(this->_Selected[Loser].Player[0].lpObj->Index, 3);

				gObjTeleport(this->_Selected[Loser].Player[0].lpObj->Index, 0, 125, 125);

				this->_Selected[Loser].Player[0].lpObj->InEvent = false;
			}

			if (this->_Selected[Loser].Player[1].lpObj)
			{
				this->_Selected[Loser].Player[1].lpObj->PKLevel = 3;

				this->_Selected[Loser].Player[1].lpObj->PKCount = 3;

				this->_Selected[Loser].Player[1].lpObj->PKTime = 0;

				GCPKLevelSend(this->_Selected[Loser].Player[1].lpObj->Index, 3);

				gObjTeleport(this->_Selected[Loser].Player[1].lpObj->Index, 0, 125, 125);

				this->_Selected[Loser].Player[1].lpObj->InEvent = false;
			}

			this->_State = Select;
			this->_Count = 0;
		}
		break;
		case Final:
		{
			PairTeam Team = this->_Teams[0].front();

			if (Team.Player[0].lpObj)
			{
				gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_PAIR, 0, 1, Team.Player[0].lpObj, Team.Player[0].lpObj->Map, Team.Player[0].lpObj->X, Team.Player[0].lpObj->Y);
				GDRankingUpdateSend(Team.Player[0].lpObj->Index, "TopDupla", 1);

				if (this->_Patente)
				{
					GDAtualizarPatenteSend(Team.Player[0].lpObj->Index, 12);
				}
			}

			if (Team.Player[1].lpObj)
			{
				gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_PAIR, 0, 1, Team.Player[1].lpObj, Team.Player[0].lpObj->Map, Team.Player[0].lpObj->X, Team.Player[0].lpObj->Y);
				GDRankingUpdateSend(Team.Player[1].lpObj->Index, "TopDupla", 13);

				if (this->_Patente)
				{
					GDAtualizarPatenteSend(Team.Player[1].lpObj->Index, 10);
				}
			}

			sprintf_s(this->_Buffer[0], gMessage->GetMessage(1160));

			if (Team.Player[0].lpObj && Team.Player[1].lpObj)
			{
				sprintf_s(this->_Buffer[1], gMessage->GetMessage(1181), Team.Player[0].lpObj->Name, Team.Player[1].lpObj->Name);
			}
			else if (Team.Player[0].lpObj && !Team.Player[1].lpObj)
			{
				sprintf_s(this->_Buffer[1], gMessage->GetMessage(1182), Team.Player[0].lpObj->Name);
			}
			else if (!Team.Player[0].lpObj && Team.Player[1].lpObj)
			{
				sprintf_s(this->_Buffer[1], gMessage->GetMessage(1182), Team.Player[1].lpObj->Name);
			}

			for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
			{
				if (gObj[i].Connected == 3)
				{
					gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[0]);
					gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, this->_Buffer[1]);
				}
			}

			if (Team.Player[0].lpObj)
			{
				gObjTeleport(Team.Player[0].lpObj->Index, 0, 125, 125);

				Team.Player[0].lpObj->InEvent = false;
			}

			if (Team.Player[1].lpObj)
			{
				gObjTeleport(Team.Player[1].lpObj->Index, 0, 125, 125);

				Team.Player[1].lpObj->InEvent = false;
			}

			this->_Teams[0].clear();
			this->_Teams[1].clear();

			this->_Registered.clear();

			this->_State = Empty;
			this->_Count = 0;
		}
		break;
		}
	}
}

bool cPairEvent::Attack(LPOBJ lpObj, LPOBJ Target)
{
	if (this->_State == Select || this->_State == Died || this->_State == NextStage || this->_State == WO)
	{
		if ((this->_Selected[0].Player[0].lpObj == lpObj && this->_Selected[0].Player[1].lpObj == Target) ||
			(this->_Selected[0].Player[1].lpObj == lpObj && this->_Selected[0].Player[0].lpObj == Target) ||
			(this->_Selected[1].Player[0].lpObj == lpObj && this->_Selected[1].Player[1].lpObj == Target) ||
			(this->_Selected[1].Player[1].lpObj == lpObj && this->_Selected[1].Player[0].lpObj == Target))
		{
			return false;
		}
	}
	else if (this->_State == Progress)
	{
		if (this->_Selected[0].Player[0].lpObj != lpObj && this->_Selected[0].Player[0].lpObj != Target &&
			this->_Selected[0].Player[1].lpObj != lpObj && this->_Selected[0].Player[1].lpObj != Target &&
			this->_Selected[1].Player[0].lpObj != lpObj && this->_Selected[1].Player[0].lpObj != Target &&
			this->_Selected[1].Player[1].lpObj != lpObj && this->_Selected[1].Player[1].lpObj != Target)
		{
			return true;
		}

		if ((this->_Selected[0].Player[0].lpObj == lpObj && this->_Selected[0].Player[1].lpObj == Target) ||
			(this->_Selected[0].Player[1].lpObj == lpObj && this->_Selected[0].Player[0].lpObj == Target) ||
			(this->_Selected[1].Player[0].lpObj == lpObj && this->_Selected[1].Player[1].lpObj == Target) ||
			(this->_Selected[1].Player[1].lpObj == lpObj && this->_Selected[1].Player[0].lpObj == Target))
		{
			return false;
		}

		if (((this->_Selected[0].Player[0].lpObj == lpObj || this->_Selected[0].Player[1].lpObj == lpObj) && (this->_Selected[1].Player[0].lpObj == Target || this->_Selected[1].Player[1].lpObj == Target)) ||
			((this->_Selected[1].Player[0].lpObj == lpObj || this->_Selected[1].Player[1].lpObj == lpObj) && (this->_Selected[0].Player[0].lpObj == Target || this->_Selected[0].Player[1].lpObj == Target)))
		{
			if (this->_Count == 0)
			{
				return true;
			}

			return false;
		}

		if (Target->Type == OBJECT_MONSTER)
		{
			return true;
		}
	}

	return true;
}

void cPairEvent::Die(LPOBJ lpObj)
{
	if (this->_State == Progress || this->_State == Died)
	{
		if (this->_Selected[0].Player[0].lpObj == lpObj)
		{
			this->_Selected[0].Player[0].Die = true;
			this->_Selected[0].Player[0].Move = 7;
		}

		if (this->_Selected[0].Player[1].lpObj == lpObj)
		{
			this->_Selected[0].Player[1].Die = true;
			this->_Selected[0].Player[1].Move = 7;
		}

		if (this->_Selected[1].Player[0].lpObj == lpObj)
		{
			this->_Selected[1].Player[0].Die = true;
			this->_Selected[1].Player[0].Move = 7;
		}

		if (this->_Selected[1].Player[1].lpObj == lpObj)
		{
			this->_Selected[1].Player[1].Die = true;
			this->_Selected[1].Player[1].Move = 7;
		}

		if (this->_State == Progress && ((this->_Selected[0].Player[0].Die && this->_Selected[0].Player[1].Die) || (this->_Selected[1].Player[0].Die && this->_Selected[1].Player[1].Die)))
		{
			this->_State = Died;
			this->_Count = 4;
		}
	}
}

void cPairEvent::Move()
{
	if (this->_State == Progress || this->_State == Died)
	{
		if (this->_Selected[0].Player[0].Move > 0)
		{
			this->_Selected[0].Player[0].Move--;

			if (this->_Selected[0].Player[0].Move == 1 && this->_Selected[0].Player[0].lpObj)
			{
				if (this->_Selected[0].Player[0].lpObj->RegenOk == 0)
				{
					gObjTeleport(this->_Selected[0].Player[0].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);

					this->_Selected[0].Player[0].Move = 0;
				}
			}
		}

		if (this->_Selected[0].Player[1].Move > 0)
		{
			this->_Selected[0].Player[1].Move--;

			if (this->_Selected[0].Player[1].Move == 1 && this->_Selected[0].Player[1].lpObj)
			{
				if (this->_Selected[0].Player[1].lpObj->RegenOk == 0)
				{
					gObjTeleport(this->_Selected[0].Player[1].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);

					this->_Selected[0].Player[1].Move = 0;
				}
			}
		}

		if (this->_Selected[1].Player[0].Move > 0)
		{
			this->_Selected[1].Player[0].Move--;

			if (this->_Selected[1].Player[0].Move == 1 && this->_Selected[1].Player[0].lpObj)
			{
				if (this->_Selected[1].Player[0].lpObj->RegenOk == 0)
				{
					gObjTeleport(this->_Selected[1].Player[0].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);

					this->_Selected[1].Player[0].Move = 0;
				}
			}
		}

		if (this->_Selected[1].Player[1].Move > 0)
		{
			this->_Selected[1].Player[1].Move--;

			if (this->_Selected[1].Player[1].Move == 1 && this->_Selected[1].Player[1].lpObj)
			{
				if (this->_Selected[1].Player[1].lpObj->RegenOk == 0)
				{
					gObjTeleport(this->_Selected[1].Player[1].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);

					this->_Selected[1].Player[1].Move = 0;
				}
			}
		}
	}
}

void cPairEvent::Quit(LPOBJ lpObj)
{
	if (this->_State != Empty)
	{
		auto it = std::find(this->_Registered.cbegin(), this->_Registered.cend(), lpObj);

		if (it != this->_Registered.cend())
		{
			lpObj->PKLevel = 3;

			gObjTeleport(lpObj->Index, 0, 125, 125);

			lpObj->InEvent = false;

			this->_Registered.erase(it);
		}

		if (this->_Selected[0].Player[0].lpObj == lpObj || this->_Selected[0].Player[1].lpObj == lpObj)
		{
			BYTE id = this->_Selected[0].Player[0].lpObj == lpObj ? 0 : 1;

			if (this->_Selected[0].Player[id].Select == true)
			{
				this->_Selected[0].Player[id].Quit = true;

				if (this->_Selected[0].Player[id].lpObj)
				{
					this->_Selected[0].Player[id].lpObj->Map = 0;
					this->_Selected[0].Player[id].lpObj->X = 125;
					this->_Selected[0].Player[id].lpObj->Y = 125;
					this->_Selected[0].Player[id].lpObj->PKLevel = 3;

					this->_Selected[0].Player[id].lpObj->PKCount = 3;

					this->_Selected[0].Player[id].lpObj->PKTime = 0;

					GCPKLevelSend(lpObj->Index, 3);

					lpObj->InEvent = false;

					this->_Selected[0].Player[id].lpObj = NULL;
				}
			}

			if (!this->_Selected[0].Player[0].lpObj || !this->_Selected[0].Player[1].lpObj)
			{
				this->_State = WO;
				this->_Count = 0;
			}
		}
		else if (this->_Selected[1].Player[0].lpObj == lpObj || this->_Selected[1].Player[1].lpObj == lpObj)
		{
			BYTE id = this->_Selected[1].Player[0].lpObj == lpObj ? 0 : 1;

			if (this->_Selected[1].Player[id].Select == true)
			{
				this->_Selected[1].Player[id].Quit = true;

				if (this->_Selected[1].Player[id].lpObj)
				{
					gObjTeleport(this->_Selected[1].Player[id].lpObj->Index, 0, 125, 125);

					this->_Selected[1].Player[id].lpObj->PKLevel = 3;

					this->_Selected[1].Player[id].lpObj->PKCount = 3;

					this->_Selected[1].Player[id].lpObj->PKTime = 0;

					GCPKLevelSend(lpObj->Index, 3);

					lpObj->InEvent = false;

					this->_Selected[1].Player[id].lpObj = NULL;
				}
			}

			if (!this->_Selected[1].Player[0].lpObj || !this->_Selected[1].Player[1].lpObj)
			{
				this->_State = WO;
				this->_Count = 0;
			}
		}
		else
		{
			if (this->_Teams[0].size() > 0)
			{
				for (auto it = this->_Teams[0].begin(); it != this->_Teams[0].end(); ++it)
				{
					LPOBJ lpTemp = NULL;

					if (it->Player[0].lpObj == lpObj)
					{
						lpTemp = it->Player[0].lpObj;
					}

					if (it->Player[1].lpObj == lpObj)
					{
						lpTemp = it->Player[1].lpObj;
					}

					if (lpTemp)
					{
						it->Player[lpTemp == it->Player[0].lpObj ? 0 : 1].lpObj = NULL;

						gObjTeleport(lpObj->Index, 0, 125, 125);

						lpObj->InEvent = false;

						if (!it->Player[0].lpObj && !it->Player[1].lpObj || !it->Player[0].lpObj || !it->Player[1].lpObj)
						{
							if (it->Player[0].lpObj)
							{
								it->Player[0].lpObj->InEvent = false;

								gObjTeleport(it->Player[0].lpObj->Index, 0, 125, 125);
							}

							if (it->Player[1].lpObj)
							{
								it->Player[1].lpObj->InEvent = false;

								gObjTeleport(it->Player[1].lpObj->Index, 0, 125, 125);
							}

							this->_Teams[0].erase(it);
						}

						break;
					}
				}
			}

			if (this->_Teams[1].size() > 0)
			{
				for (auto it = this->_Teams[1].begin(); it != this->_Teams[1].end(); ++it)
				{
					LPOBJ lpTemp = NULL;

					if (it->Player[0].lpObj == lpObj)
					{
						lpTemp = it->Player[0].lpObj;
					}

					if (it->Player[1].lpObj == lpObj)
					{
						lpTemp = it->Player[1].lpObj;
					}

					if (lpTemp)
					{
						gObjTeleport(lpObj->Index, 0, 125, 125);

						lpObj->InEvent = false;

						it->Player[lpTemp == it->Player[0].lpObj ? 0 : 1].lpObj = NULL;

						if (!it->Player[0].lpObj && !it->Player[1].lpObj || !it->Player[0].lpObj || !it->Player[1].lpObj)
						{
							if (it->Player[0].lpObj)
							{
								it->Player[0].lpObj->InEvent = false;

								gObjTeleport(it->Player[0].lpObj->Index, 0, 125, 125);
							}

							if (it->Player[1].lpObj)
							{
								it->Player[1].lpObj->InEvent = false;

								gObjTeleport(it->Player[1].lpObj->Index, 0, 125, 125);
							}

							this->_Teams[1].erase(it);
						}

						break;
					}
				}
			}
		}
	}
}

int cPairEvent::GetRemainingTime()
{
	if (this->_State == Progress || this->_State == Final)
	{
		return this->_Count * 60;
	}

	return 0;
}

int cPairEvent::GetNextEventTime()
{
	SYSTEMTIME tempoAtual;
	GetLocalTime(&tempoAtual);

	int diaAtual = tempoAtual.wDayOfWeek;
	int horaAtual = tempoAtual.wHour;
	int minutoAtual = tempoAtual.wMinute;
	int segundoAtual = tempoAtual.wSecond;

	int menorDiferencaTempo = -1;

	// Procura nos próximos 7 dias
	for (int Dia = 0; Dia < 7; Dia++)
	{
		int diaEncontrado = (diaAtual + Dia) % 7;

		for (int i = 0; i < this->_List; i++)
		{
			if (this->EventStruct[i].Day == diaEncontrado)
			{
				int segundosTotalEvento = (this->EventStruct[i].Hours * 3600) + (this->EventStruct[i].Minutes * 60);
				int segundosTotalAtual = (horaAtual * 3600) + (minutoAtual * 60) + segundoAtual;

				int diferencaSegundos;

				if (Dia == 0) // Hoje
				{
					diferencaSegundos = segundosTotalEvento - segundosTotalAtual;
					if (diferencaSegundos <= 0) // Evento já passou hoje, ignora
					{
						continue;
					}
				}
				else // Dias futuros
				{
					// Calcula o tempo total até o evento no dia futuro
					diferencaSegundos = ((24 * 3600) - segundosTotalAtual) + segundosTotalEvento + ((Dia - 1) * 24 * 3600);
				}

				if (menorDiferencaTempo == -1 || diferencaSegundos < menorDiferencaTempo)
				{
					menorDiferencaTempo = diferencaSegundos;
				}
			}
		}
	}

	return (menorDiferencaTempo == -1) ? 0 : menorDiferencaTempo;
}

cPairEvent PairEvent;