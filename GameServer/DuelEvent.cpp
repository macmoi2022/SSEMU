#include "StdAfx.h"
#include "DuelEvent.h"
#include <fstream>
#include "Tokenizer.h"
#include "Notice.h"
#include "Party.h"
#include "EffectManager.h"
#include "ServerInfo.h"
#include "CommandManager.h"
#include "ItemBagManager.h"
#include "DSProtocol.h"

cDuelEvent::cDuelEvent() : _State(Empty)
{
}

cDuelEvent::~cDuelEvent()
{
}

bool cDuelEvent::Load(char* path)
{
	this->_List = 0;

	this->_Players[0] = std::vector<DuelPlayer>();
	this->_Players[1] = std::vector<DuelPlayer>();

	Tokenizer          Token;
	TokenizerGroup     Group;
	TokenizerSection   Section;

	Token.ParseFile(std::string(path), Group);

	if (Group.GetSection(0, Section))
	{
		this->_Active = Section.Rows[0].GetInt(0) > 0 ? true : false;
		strcpy_s(this->_Syntax, sizeof(this->_Syntax), (Section.Rows[0].GetStringPtr(1)));
		this->_Patente = Section.Rows[0].GetInt(2) > 0 ? true : false;

		this->_Level		= Section.Rows[1].GetInt(0);
		this->_MapNumber[0] = Section.Rows[1].GetInt(1);
		this->_X[0]			= Section.Rows[1].GetInt(2);
		this->_Y[0]			= Section.Rows[1].GetInt(3);
		this->_MapNumber[1] = Section.Rows[1].GetInt(4);
		this->_X[1]			= Section.Rows[1].GetInt(5);
		this->_Y[1]			= Section.Rows[1].GetInt(6);
	}

	if (Group.GetSection(1, Section))
	{
		for (int i = 0; i < Section.RowCount; i++)
		{
			this->EventStruct[this->_List].Day = Section.Rows[i].GetInt(0);
			this->EventStruct[this->_List].Hours = Section.Rows[i].GetInt(1);
			this->EventStruct[this->_List].Minutes = Section.Rows[i].GetInt(2);
			this->EventStruct[this->_List].Time = Section.Rows[i].GetInt(3);
			this->EventStruct[this->_List].Class = Section.Rows[i].GetInt(4);
			this->_List++;
		}
	}

	return true;
}

bool cDuelEvent::GameMaster(LPOBJ lpObj, char* arg)
{
	int Time = gCommandManager->GetNumber(arg, 0);
	int Class = gCommandManager->GetNumber(arg, 1);

	if (Time == NULL)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Erro de sintaxe!");
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Digite: <tempo> <class>");
		return false;
	}

	if (Class == NULL)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Exemplo -> Evento para BKs: 3 1");
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "ID de classes: ");
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "0: Wizard    | 1: Knight    | 2: Elf");
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "3: Gladiator | 4: Dark Lord | 5: Todos");
		return false;
	}

	if (this->_State == Empty && Time != NULL && Class != NULL)
	{
		gObjTeleport(lpObj->Index, this->_MapNumber[1], this->_X[1], this->_Y[1]);
		this->Start(Time, Class);
		return true;
	}

	return false;
}

bool cDuelEvent::Check(LPOBJ lpObj)
{
	if (this->_State == Register)
	{
		if (this->_Type != 5 && lpObj->Class != this->_Type)
		{
			switch (this->_Type)
			{
			case 0:
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1116));
				break;
			case 1:
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1117));
				break;
			case 2:
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1118));
				break;
			case 3:
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1119));
				break;
			case 4:
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1120));
				break;
			}

			return true;
		}

		if (this->_Players[0].size() > 0)
		{
			for (auto it = this->_Players[0].cbegin(); it != this->_Players[0].cend(); ++it)
			{
				if (it->lpObj == lpObj)
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

		DuelPlayer Player;

		Player.Die = false;

		Player.lpObj = lpObj;

		lpObj->InEvent = true;

		this->_Players[0].push_back(Player);

		gEffectManager->ClearAllEffect(lpObj);

		gObjTeleport(lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);

		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1115));
	}

	return true;
}

void cDuelEvent::Start(int Time, int Class)
{
	if (Time == 0)
	{
		Time++;
	}

	if (this->_Players[0].size() > 0)
	{
		this->_Players[0].clear();
	}

	if (this->_Players[1].size() > 0)
	{
		this->_Players[1].clear();
	}

	this->_State = Register;
	this->_Count = (Time * 60) + 1;
	this->_Type = (BYTE)(Class);
}

void cDuelEvent::Run()
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
					this->Start(this->EventStruct[i].Time, this->EventStruct[i].Class);
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
				if (this->_Players[0].size() < 2)
				{
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							switch (this->_Type)
							{
							case 0:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1136));
								break;
							case 1:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1137));
								break;
							case 2:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1138));
								break;
							case 3:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1139));
								break;
							case 4:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1140));
								break;
							case 5:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								break;
							}
							
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1141));

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
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1142));

							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1143));
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
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							switch (this->_Type)
							{
							case 0:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1136));
								break;
							case 1:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1137));
								break;
							case 2:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1138));
								break;
							case 3:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1139));
								break;
							case 4:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1140));
								break;
							case 5:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								break;
							}
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1144), (this->_Count / 60));
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1145), this->_Syntax);
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
				switch (this->_Players[0].size())
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
					this->_Selected[0] = this->_Players[0].front();

					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							switch (this->_Type)
							{
							case 0:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1136));
								break;
							case 1:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1137));
								break;
							case 2:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1138));
								break;
							case 3:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1139));
								break;
							case 4:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1140));
								break;
							case 5:
								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
								break;
							}
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1146), this->_Selected[0].lpObj->Name);
						}
					}

					this->_Players[1].push_back(this->_Selected[0]);

					this->_State = NextStage;
					this->_Count = 0;
				}
				break;
				default:
				{
					auto Player1 = this->_Players[0].begin();
					auto Player2 = (--this->_Players[0].end());

					while (true)
					{
						if (Player1->lpObj->Socket == INVALID_SOCKET || Player1->lpObj->Connected != 3)
						{
							this->_Players[0].erase(Player1);
							Player1 = this->_Players[0].begin();
						}
						else if (Player2->lpObj->Socket == INVALID_SOCKET || Player2->lpObj->Connected != 3)
						{
							this->_Players[0].erase(Player2);
							Player2 = (--this->_Players[0].end());
						}
						else
						{
							break;
						}
					}

					if (Player1 == Player2)
					{
						goto SelectWO;
					}
					else
					{
						this->_Selected[0] = *Player1;
						this->_Selected[1] = *Player2;

						this->_Selected[0].Die = false;
						this->_Selected[0].Quit = false;
						this->_Selected[0].Select = true;
						this->_Selected[0].Score = 0;
						this->_Selected[1].Die = false;
						this->_Selected[1].Quit = false;
						this->_Selected[1].Select = true;
						this->_Selected[1].Score = 0;

						for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
						{
							if (gObj[i].Connected == 3)
							{
								switch (this->_Type)
								{
								case 0:
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1136));
									break;
								case 1:
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1137));
									break;
								case 2:
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1138));
									break;
								case 3:
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1139));
									break;
								case 4:
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1140));
									break;
								case 5:
									gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1135));
									break;
								}

								gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1147), this->_Selected[0].lpObj->Name, this->_Selected[1].lpObj->Name);
							}
						}

						gNotice->GCNoticeSend(this->_Selected[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1148));
						gNotice->GCNoticeSend(this->_Selected[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1148));

						this->_Sended = false;

						this->_State = Progress;
						this->_Count = 6;

						gObjTeleport(this->_Selected[0].lpObj->Index, this->_MapNumber[1], this->_X[1], this->_Y[1] - 1);
						gObjTeleport(this->_Selected[1].lpObj->Index, this->_MapNumber[1], this->_X[1], this->_Y[1] + 1);

						this->_Selected[0].lpObj->PKLevel = 6;
						this->_Selected[1].lpObj->PKLevel = 6;

						this->_Selected[0].lpObj->PKCount = 3;
						this->_Selected[1].lpObj->PKCount = 3;

						this->_Selected[0].lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;
						this->_Selected[1].lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;

						GCPKLevelSend(this->_Selected[0].lpObj->Index, this->_Selected[0].lpObj->PKLevel);
						GCPKLevelSend(this->_Selected[1].lpObj->Index, this->_Selected[1].lpObj->PKLevel);

						this->_Players[0].erase(Player2);
						this->_Players[0].erase(Player1);
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
			case 3:
			case 2:
			case 1:
			{
				gNotice->GCNoticeSend(this->_Selected[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1149), this->_Count);
				gNotice->GCNoticeSend(this->_Selected[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1149), this->_Count);
			}
			break;
			case 0:
			{
				if (this->_Sended == false)
				{
					gNotice->GCNoticeSend(this->_Selected[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1150));
					gNotice->GCNoticeSend(this->_Selected[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1150));

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
				if (this->_Selected[0].Die && this->_Selected[1].Die)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1151));

					this->_Sended = false;

					this->_State = Progress;
					this->_Count = 6;
				}
				else
				{
					BYTE Winner = (this->_Selected[0].Die) ? 1 : 0;
					BYTE Loser = (Winner == 1) ? 0 : 1;

					this->_Selected[Winner].Score++;

					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1152));

							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1153), this->_Selected[0].lpObj->Name, this->_Selected[1].lpObj->Name);

							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1154), this->_Selected[0].Score, this->_Selected[1].Score);
						}
					}

					if (this->_Selected[Winner].Score >= 3)
					{
						gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1155), this->_Selected[Winner].lpObj->Name);

						this->_Players[1].push_back(this->_Selected[Winner]);

						this->_Selected[0].lpObj->PKLevel = 3;
						this->_Selected[0].Select = false;
						this->_Selected[1].lpObj->PKLevel = 3;
						this->_Selected[1].Select = false;

						this->_Selected[0].lpObj->PKCount = 3;
						this->_Selected[1].lpObj->PKCount = 3;

						this->_Selected[0].lpObj->PKTime = 0;
						this->_Selected[1].lpObj->PKTime = 0;

						GCPKLevelSend(this->_Selected[0].lpObj->Index, 3);
						GCPKLevelSend(this->_Selected[1].lpObj->Index, 3);

						gObjTeleport(this->_Selected[Winner].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);
						gObjTeleport(this->_Selected[Loser].lpObj->Index, 0, 125, 125);

						this->_Selected[Loser].lpObj->InEvent = false;

						this->_State = Select;
						this->_Count = 0;
					}
					else
					{
						gNotice->GCNoticeSend(this->_Selected[0].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1148));
						gNotice->GCNoticeSend(this->_Selected[1].lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1148));

						this->_Sended = false;

						this->_State = Progress;
						this->_Count = 6;
					}
				}

				this->_Selected[0].Die = false;
				this->_Selected[1].Die = false;
			}
		}
		break;
		case NextStage:
		{
			if (this->_Players[0].size() > 0)
			{
				this->_Players[0].clear();
			}

			this->_Players[0] = this->_Players[1];

			if (this->_Players[1].size() > 0)
			{
				this->_Players[1].clear();
			}

			this->_Selected[0].lpObj = NULL;
			this->_Selected[1].lpObj = NULL;

			switch (this->_Players[0].size())
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
				if ((this->_Players[0].size() % 2) == 0)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1156));
				}
				else
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1157));
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
			BYTE Winner = (this->_Selected[0].Quit) ? 1 : 0;
			BYTE Loser = (Winner == 1) ? 0 : 1;

			gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1155), this->_Selected[Winner].lpObj->Name);

			this->_Players[1].push_back(this->_Selected[Winner]);

			this->_Selected[0].lpObj->PKLevel = 3;
			this->_Selected[0].Select = false;
			this->_Selected[1].lpObj->PKLevel = 3;
			this->_Selected[1].Select = false;

			this->_Selected[0].lpObj->PKCount = 3;
			this->_Selected[1].lpObj->PKCount = 3;

			this->_Selected[0].lpObj->PKTime = 0;
			this->_Selected[1].lpObj->PKTime = 0;

			GCPKLevelSend(this->_Selected[0].lpObj->Index, 3);
			GCPKLevelSend(this->_Selected[1].lpObj->Index, 3);

			gObjTeleport(this->_Selected[Winner].lpObj->Index, this->_MapNumber[0], this->_X[0], this->_Y[0]);
			gObjTeleport(this->_Selected[Loser].lpObj->Index, 0, 125, 125);

			this->_Selected[Loser].lpObj->InEvent = false;

			this->_State = Select;
			this->_Count = 0;
		}
		break;
		case Final:
		{
			if (this->_Players[0].size() == 0)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1158));

				this->_Players[0].clear();
				this->_Players[1].clear();

				this->_State = Empty;
				this->_Count = 0;
			}
			else
			{
				LPOBJ lpObj = this->_Players[0].front().lpObj;

				gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_DUEL, 0, 1, lpObj, lpObj->Map, lpObj->X, lpObj->Y);

				switch (this->_Type)
				{
				case 0:
					GDRankingUpdateSend(lpObj->Index, "Duelsm", 1);
					break;
				case 1:
					GDRankingUpdateSend(lpObj->Index, "Duelbk", 1);
					break;
				case 2:
					GDRankingUpdateSend(lpObj->Index, "Duelelf", 1);
					break;
				case 3:
					GDRankingUpdateSend(lpObj->Index, "Duelmg", 1);
					break;
				case 4:
					GDRankingUpdateSend(lpObj->Index, "Dueldl", 1);
					break;
				case 5:
					GDRankingUpdateSend(lpObj->Index, "Duelall", 1);
					break;
				}

				if (this->_Patente)
				{
					switch (this->_Type)
					{
					case 0:
						GDAtualizarPatenteSend(lpObj->Index, 3);
						break;
					case 1:
						GDAtualizarPatenteSend(lpObj->Index, 4);
						break;
					case 2:
						GDAtualizarPatenteSend(lpObj->Index, 5);
						break;
					case 3:
						GDAtualizarPatenteSend(lpObj->Index, 6);
						break;
					case 4:
						GDAtualizarPatenteSend(lpObj->Index, 7);
						break;
					case 5:
						GDAtualizarPatenteSend(lpObj->Index, 8);
						break;
					}
				}


				for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
				{
					if (gObj[i].Connected == 3)
					{
						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1159), lpObj->Name);
					}
				}

				gObjTeleport(lpObj->Index, 0, 125, 125);

				lpObj->InEvent = false;

				this->_Players[0].clear();
				this->_Players[1].clear();

				this->_State = Empty;
				this->_Count = 0;
			}
		}
		break;
		}
	}
}

bool cDuelEvent::Attack(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if (this->_State == Progress)
	{
		if (this->_Selected[0].lpObj != lpObj && this->_Selected[1].lpObj != lpObj &&
			this->_Selected[0].lpObj != lpTargetObj && this->_Selected[1].lpObj != lpTargetObj)
		{
			return true;
		}

		if (this->_Selected[0].lpObj == lpObj && this->_Selected[1].lpObj == lpTargetObj ||
			this->_Selected[1].lpObj == lpObj && this->_Selected[0].lpObj == lpTargetObj)
		{
			if (this->_Count == 0)
			{
				return true;
			}

			return false;
		}

		if (this->_Selected[0].lpObj == lpObj && this->_Selected[1].lpObj->Type == OBJECT_MONSTER ||
			this->_Selected[1].lpObj == lpObj && this->_Selected[0].lpObj->Type == OBJECT_MONSTER)
		{
			return true;
		}

		return false;
	}

	return true;
}

void cDuelEvent::Die(LPOBJ lpObj)
{
	if (this->_State == Progress || this->_State == Died)
	{
		if (this->_Selected[0].lpObj == lpObj)
		{
			this->_Selected[0].Die = true;
		}

		if (this->_Selected[1].lpObj == lpObj)
		{
			this->_Selected[1].Die = true;
		}

		if (this->_State == Progress && (this->_Selected[0].Die || this->_Selected[1].Die))
		{
			this->_State = Died;
			this->_Count = 4;
		}
	}
}

void cDuelEvent::Quit(LPOBJ lpObj)
{
	if (this->_State != Empty)
	{
		if (this->_Selected[0].lpObj == lpObj)
		{
			if (this->_Selected[0].Select == true)
			{
				this->_Selected[0].Quit = true;

				this->_Selected[0].lpObj->Move = true;

				this->_Selected[0].lpObj->PKLevel = 3;

				this->_Selected[0].lpObj->PKCount = 3;

				this->_Selected[0].lpObj->PKTime = 0;

				GCPKLevelSend(this->_Selected[0].lpObj->Index, 3);

				this->_State = WO;
				this->_Count = 0;
			}
		}
		else if (this->_Selected[1].lpObj == lpObj)
		{
			if (this->_Selected[1].Select == true)
			{
				this->_Selected[1].Quit = true;

				this->_Selected[1].lpObj->Move = true;

				this->_Selected[1].lpObj->PKLevel = 3;

				this->_Selected[1].lpObj->PKCount = 3;

				this->_Selected[1].lpObj->PKTime = 0;

				GCPKLevelSend(this->_Selected[1].lpObj->Index, 3);

				this->_State = WO;
				this->_Count = 0;
			}
		}
		else
		{
			if (this->_Players[0].size() > 0)
			{
				for (auto it = this->_Players[0].cbegin(); it != this->_Players[0].cend(); ++it)
				{
					if (it->lpObj == lpObj)
					{
						lpObj->Move = true;

						lpObj->InEvent = false;

						lpObj->PKLevel = 3;
						lpObj->PKCount = 3;
						lpObj->PKTime = 0;

						GCPKLevelSend(lpObj->Index, 3);

						this->_Players[0].erase(it);
						break;
					}
				}
			}

			if (this->_Players[1].size() > 0)
			{
				for (auto it = this->_Players[1].cbegin(); it != this->_Players[1].cend(); ++it)
				{
					if (it->lpObj == lpObj)
					{
						lpObj->Move = true;

						lpObj->InEvent = false;

						lpObj->PKLevel = 3;
						lpObj->PKCount = 3;
						lpObj->PKTime = 0;

						GCPKLevelSend(lpObj->Index, 3);

						this->_Players[1].erase(it);
						break;
					}
				}
			}
		}
	}
}

int cDuelEvent::GetRemainingTime()
{
	if (this->_State == Progress || this->_State == Final)
	{
		return this->_Count * 60;
	}

	return 0;
}

int cDuelEvent::GetNextEventTime()
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

cDuelEvent DuelEvent;