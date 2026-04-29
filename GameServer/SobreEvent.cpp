#include "stdafx.h"
#include "SobreEvent.h"
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

cSobreEvent::cSobreEvent() : _State(Empty)
{
}

cSobreEvent::~cSobreEvent()
{
}

bool cSobreEvent::Load(char* path)
{
	this->_List = 0;

	this->_Players = std::vector<LPOBJ>();

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
		this->_MapNumber = Section.Rows[1].GetInt(1);
		this->_X = Section.Rows[1].GetInt(2);
		this->_Y = Section.Rows[1].GetInt(3);
		this->_Player = Section.Rows[1].GetInt(4);
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

bool cSobreEvent::GameMaster(LPOBJ lpObj, char* arg)
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
		this->Start(Time, 5);
		return true;
	}

	return false;
}

bool cSobreEvent::Check(LPOBJ lpObj)
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

		auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpObj);

		if (it != this->_Players.cend())
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1113));
			return true;
		}
		else if (lpObj->Level < this->_Level)
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1111), this->_Level);
			return true;
		}
		else if (lpObj->PartyNumber != -1)
		{
			gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1112));
			return true;
		}
		else if (lpObj->InDuel != 0)
		{
			gNotice->GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1114));
			return true;
		}

		this->_Players.push_back(lpObj);

		lpObj->InEvent = true;

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

		gObjTeleport(lpObj->Index, this->_MapNumber, this->_X, this->_Y);

		gEffectManager->ClearAllEffect(lpObj);

		lpObj->PKLevel = 6;

		lpObj->PKCount = 3;

		lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;

		GCPKLevelSend(lpObj->Index, lpObj->PKLevel);

		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1115));

	}

	return false;
}

void cSobreEvent::Start(int Time, int Class)
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
	this->_Type = (BYTE)(Class);
}

void cSobreEvent::Run()
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
				if (this->_Players.size() < this->_Player)
				{
					for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
					{
						if (gObj[i].Connected == 3)
						{
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1197));
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1198));

							if (gObj[i].InEvent == true)
							{
								gObj[i].InEvent = false;

								gObj[i].Move = true;

								gObj[i].PKLevel = 3;
								GCPKLevelSend(i, gObj[i].PKLevel);
							}
						}
					}

					this->_State = Empty;
					this->_Count = 0;
				}
				else
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1199));

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
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1200), (this->_Count / 60));
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1201), this->_Syntax);
							gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1202), this->_Player);
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
				for (auto it = this->_Players.cbegin(); it != this->_Players.cend(); ++it)
				{
					if ((*it)->Connected == 3)
					{
						gNotice->GCNoticeSend((*it)->Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1203), this->_Count);
					}
				}
			}
			break;
			case 0:
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1204));

				this->_State = Fight;

				this->_Count = 60 * 5;
			}
			break;
			}
		}
		break;
		case Fight:
		{
			if (this->_Players.size() == 1 || this->_Players.size() == 0)
			{
				this->_State = Final;
				this->_Count = 0;
			}
			else if (this->_Count == 0)
			{
				this->_State = Final;
			}
		}
		break;
		case Final:
		{
			if (this->_Players.size() > 1)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1205));

				this->_Players.clear();

				this->_State = Empty;
				this->_Count = 0;
			}
			if (this->_Players.size() == 0)
			{
				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1205));

				this->_Players.clear();

				this->_State = Empty;
				this->_Count = 0;
			}
			else
			{
				LPOBJ lpObj = this->_Players.front();

				lpObj->InEvent = false;

				lpObj->Move = true;

				lpObj->PKLevel = 3;

				GCPKLevelSend(lpObj->Index, lpObj->PKLevel);

				gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1206), lpObj->Name);

				gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_SOBRE, 0, 1, lpObj, lpObj->Map, lpObj->X, lpObj->Y);

				GDRankingUpdateSend(lpObj->Index, "topsurvivor", 1);

				if (this->_Patente)
				{
					GDAtualizarPatenteSend(lpObj->Index, 11);
				}

				gObjTeleport(lpObj->Index, 0, 125, 125);

				this->_Players.clear();

				this->_State = Empty;
				this->_Count = 0;
			}
		}
		break;
		}
	}
}

bool cSobreEvent::Attack(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if (this->_State != Register && this->_State != Progress) return true;

	auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpTargetObj);

	if (it != this->_Players.cend())
	{
		if (this->_State == Register || this->_State == Progress)
		{
			if ((*it)->Type == OBJECT_USER)
			{
				return false;
			}
		}
	}

	return true;
}

void cSobreEvent::Die(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if (this->_State == Fight)
	{
		if (this->_Players.size() > 1 && lpObj->InEvent == true)
		{
			auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpObj);

			if (it != this->_Players.cend())
			{
				if (lpObj->Type == OBJECT_USER && lpTargetObj->Type == OBJECT_USER)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1207), lpObj->Name);

					lpObj->Dead = true;

					lpObj->Move = true;

					lpObj->InEvent = false;

					gObjTeleport(lpObj->Index, 0, 125, 125);

					this->_Players.erase(it);
				}
			}
		}
	}
}

void cSobreEvent::Quit(LPOBJ lpObj)
{
	if (this->_State != Empty)
	{
		if (this->_Players.size() > 0)
		{
			auto it = std::find(this->_Players.cbegin(), this->_Players.cend(), lpObj);

			if (it != this->_Players.cend())
			{
				lpObj->InEvent = false;

				lpObj->PKLevel = 3;
				GCPKLevelSend(lpObj->Index, 3);

				this->_Players.erase(it);
			}
		}
	}
}


int cSobreEvent::GetRemainingTime()
{
	if (this->_State == Progress || this->_State == Fight || this->_State == Final)
	{
		return this->_Count * 60;
	}

	return 0;
}

int cSobreEvent::GetNextEventTime()
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

cSobreEvent SobreEvent;