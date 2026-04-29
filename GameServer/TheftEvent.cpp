#include "stdafx.h"
#include "TheftEvent.h"
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
#include "Util.h"

cTheftEvent::cTheftEvent() : _Active(false), _Progress(false), _Portal(false), _Players(0)
{
}

bool cTheftEvent::Load(char* path)
{
	this->_Count = 0;

	memset(this->TheftStruct, 0, sizeof(this->TheftStruct));

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
	}

	if (Group.GetSection(1, Section))
	{
		for (int i = 0; i < Section.RowCount; i++)
		{
			this->TheftStruct[this->_Count]._Day = Section.Rows[i].GetInt(0);
			this->TheftStruct[this->_Count]._Hours = Section.Rows[i].GetInt(1);
			this->TheftStruct[this->_Count]._Minutes = Section.Rows[i].GetInt(2);
			this->TheftStruct[this->_Count]._TimeOpen = Section.Rows[i].GetInt(3);
			this->TheftStruct[this->_Count]._Duration = Section.Rows[i].GetInt(4) * 60 * 1000;
			this->_Count++;
		}
	}

	return true;
}

bool cTheftEvent::Check(LPOBJ lpObj)
{
	if (this->_Portal == false)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1110));
		return false;
	}
	else if (lpObj->Level < TheftEvent._Level)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1111), TheftEvent._Level);
		return false;
	}
	else if (lpObj->PartyNumber != -1)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1112));
		return false;
	}
	else if (lpObj->InEvent == true)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1113));
		return false;
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

	TheftEvent._Players++;

	gEffectManager->ClearAllEffect(lpObj);

	this->PlayerStruct[lpObj->Index]._InEvent = true;

	this->PlayerStruct[lpObj->Index]._AttackBlock = true;

	lpObj->PKLevel = 6;

	lpObj->PKCount = 3;

	lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;

	GCPKLevelSend(lpObj->Index, lpObj->PKLevel);	

	gObjTeleport(lpObj->Index, this->_MapNumber, this->_X, this->_Y);
	
	gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1115));

	return true;
}

void cTheftEvent::Run()
{
	if (this->_Active)
	{
		SYSTEMTIME Now;
		GetLocalTime(&Now);

		for (int i = 0; i < this->_Count; i++)
		{
			if (Now.wDayOfWeek == this->TheftStruct[i]._Day && Now.wHour == this->TheftStruct[i]._Hours && Now.wMinute == this->TheftStruct[i]._Minutes && Now.wSecond == 0)
			{
				this->_Value = 0; this->_Players = 0; this->_Portal = true;

				for (int minutos = this->TheftStruct[i]._TimeOpen; minutos > 0; minutos--)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1208));
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1209), minutos);
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1210), this->_Syntax);

					Sleep(60000);
				}

				if (this->_Players > 1)
				{
					for (int Index = OBJECT_START_USER; Index < MAX_OBJECT; ++Index)
					{
						if (gObj[Index].Connected == 3)
						{
							if (this->PlayerStruct[Index]._InEvent == true)
							{
								this->PlayerStruct[Index]._Kills = 1;
								this->PlayerStruct[Index]._AttackBlock = false;
							}

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1208));

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1211));

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1212));
						}
					}

					this->_Portal = false;
					this->_Progress = true;
				}
				else
				{
					for (int Index = OBJECT_START_USER; Index < MAX_OBJECT; ++Index)
					{
						if (gObj[Index].Connected == 3)
						{
							if (this->PlayerStruct[Index]._InEvent == true)
							{
								this->PlayerStruct[Index]._InEvent = false;
								this->PlayerStruct[Index]._Kills = 0;
								this->PlayerStruct[Index]._AttackBlock = false;

								gObj[Index].Move = true;

								gObj[Index].PKLevel = 6;

								gObj[Index].PKCount = 3;

								gObj[Index].PKTime = gServerInfo->m_PKDownRequirePoint3;

								GCPKLevelSend(Index, gObj[Index].PKLevel);
							}

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1208));

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1213));
						}
					}

					this->_Players = 0;
					this->_Portal = false;
				}

				if (this->_Progress == true)
				{
					Sleep(this->TheftStruct[i]._Duration);

					this->Winner();
				}
			}
		}
	}
}

bool cTheftEvent::Attack(LPOBJ lpObj, LPOBJ Target)
{
	if (this->_Portal == true)
	{
		if (lpObj->Type == OBJECT_USER && Target->Type == OBJECT_USER)
		{
			if (this->PlayerStruct[lpObj->Index]._AttackBlock == true)
			{
				return false;
			}
		}
	}

	return true;
}

void cTheftEvent::Killer(LPOBJ lpObj, LPOBJ Target)
{
	if (this->_Progress == true)
	{
		if (lpObj->Type == OBJECT_USER && Target->Type == OBJECT_USER)
		{
			if (this->PlayerStruct[lpObj->Index]._InEvent == true && this->PlayerStruct[Target->Index]._InEvent == true)
			{
				this->PlayerStruct[lpObj->Index]._Kills += this->PlayerStruct[Target->Index]._Kills;

				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1214), this->PlayerStruct[Target->Index]._Kills, this->PlayerStruct[lpObj->Index]._Kills);

				gNotice->GCNoticeSend(Target->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1215), this->PlayerStruct[Target->Index]._Kills);

				this->PlayerStruct[Target->Index]._Kills = 1;

				for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
				{
					if (gObj[i].Connected == 3 && this->PlayerStruct[i]._InEvent == true)
					{
						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1216));

						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1217), lpObj->Name, this->PlayerStruct[lpObj->Index]._Kills);
					}
				}
			}
		}
	}
}

void cTheftEvent::Winner()
{
	for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
	{
		if (gObj[i].Connected == 3)
		{
			if (this->_Progress == true && this->PlayerStruct[i]._InEvent == true)
			{
				if (this->PlayerStruct[i]._Kills > this->PlayerStruct[this->_Value]._Kills)
				{
					this->_Value = i;
				}
			}
		}
	}

	if (this->_Value && this->PlayerStruct[this->_Value]._Kills > 0)
	{
		LPOBJ lpObj = &gObj[this->_Value];

		gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_THEFT, 0, 1, lpObj, gObj[this->_Value].Map, gObj[this->_Value].X, gObj[this->_Value].Y);

		GDRankingUpdateSend(lpObj->Index, "TopTheft", 1);

		for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
		{
			if (gObj[i].Connected == 3 && gObj[i].Type == OBJECT_USER)
			{
				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1208));

				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1218), gObj[this->_Value].Name, this->PlayerStruct[this->_Value]._Kills);

			}
		}

		if (this->_Patente)
		{
			GDAtualizarPatenteSend(lpObj->Index, 10);
		}

		this->_Value = 0; this->_Players = 0; this->_Progress = false;

		Finish();
	}
	else
	{
		for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
		{
			if (gObj[i].Connected == 3)
			{
				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1208));

				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1219));
			}
		}

		this->_Value = 0; this->_Players = 0; this->_Progress = false;

		Finish();
	}
}

void cTheftEvent::Finish()
{
	for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
	{
		if (gObj[i].Connected == 3 && this->PlayerStruct[i]._InEvent == true)
		{
			this->PlayerStruct[i]._InEvent = false;

			this->PlayerStruct[i]._AttackBlock = false;

			this->PlayerStruct[i]._Kills = 0;

			gObj[i].Move = true;

			gObj[i].PKLevel = 3;

			gObj[i].PKTime = 0;

			GCPKLevelSend(i, gObj[i].PKLevel);

			gObjTeleport(gObj[i].Index, 0, 125, 125);

		}
	}
}

void cTheftEvent::Quit(LPOBJ lpObj)
{
	if (lpObj->Type == OBJECT_USER && this->PlayerStruct[lpObj->Index]._InEvent == true)
	{
		this->PlayerStruct[lpObj->Index]._AttackBlock = false;

		this->PlayerStruct[lpObj->Index]._Kills = 0;

		lpObj->Move = true;

		this->_Players--;

		lpObj->PKLevel = 3;

		GCPKLevelSend(lpObj->Index, lpObj->PKLevel);

		this->PlayerStruct[lpObj->Index]._InEvent = false;
	}
}

DWORD cTheftEvent::GetRemainingTime()
{
	if (this->_Progress == false)
	{
		return 0;
	}

	return this->_Count;

}

DWORD cTheftEvent::GetNextEventTime()
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

		for (int i = 0; i < this->_Count; i++)
		{
			if (this->TheftStruct[i]._Day == diaEncontrado)
			{
				int segundosTotalEvento = (this->TheftStruct[i]._Hours * 3600) + (this->TheftStruct[i]._Minutes * 60);
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

cTheftEvent TheftEvent;