#include "StdAfx.h"
#include "DeathEvent.h"
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

cDeathEvent::cDeathEvent() : _Active(false), _Progress(false), _Portal(false), _Players(0)
{

}


bool cDeathEvent::Load(char* path)
{
	this->_List = 0;

	memset(this->DeathStruct, 0, sizeof(this->DeathStruct));

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
			this->DeathStruct[this->_List]._Day = Section.Rows[i].GetInt(0);
			this->DeathStruct[this->_List]._Hours = Section.Rows[i].GetInt(1);
			this->DeathStruct[this->_List]._Minutes = Section.Rows[i].GetInt(2);
			this->DeathStruct[this->_List]._TimeOpen = Section.Rows[i].GetInt(3);
			this->DeathStruct[this->_List]._Duration = Section.Rows[i].GetInt(4) * 60 * 1000;
			this->_List++;
		}
	}

	return true;
}

bool cDeathEvent::Check(LPOBJ lpObj)
{
	if (this->_Portal == false)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1110));
		return false;
	}
	else if (lpObj->Level < DeathEvent._Level)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1111), DeathEvent._Level);
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
	else if (DeathEvent.PlayerStruct[lpObj->Index]._InEvent == true)
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

	DeathEvent._Players++;

	gEffectManager->ClearAllEffect(lpObj);

	this->PlayerStruct[lpObj->Index]._InEvent = true;

	this->PlayerStruct[lpObj->Index]._AttackBlock = true;

	lpObj->PKLevel = 6;

	lpObj->PKCount = 3;

	lpObj->PKTime = gServerInfo->m_PKDownRequirePoint3;

	GCPKLevelSend(lpObj->Index, lpObj->PKLevel);

	gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1115));

	gObjTeleport(lpObj->Index, this->_MapNumber, this->_X, this->_Y);

	return true;
}

void cDeathEvent::Run()
{
	if (this->_Active)
	{
		SYSTEMTIME Now;
		GetLocalTime(&Now);

		for (int i = 0; i < this->_List; i++)
		{
			if (Now.wDayOfWeek == this->DeathStruct[i]._Day && Now.wHour == this->DeathStruct[i]._Hours && Now.wMinute == this->DeathStruct[i]._Minutes && Now.wSecond == 0)
			{
				this->_Value = 0; this->_Players = 0; this->_Portal = true;

				for (int minutos = this->DeathStruct[i]._TimeOpen; minutos > 0; minutos--)
				{
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1124));
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1125), minutos);
					gNotice->GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage->GetMessage(1126), this->_Syntax);

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
								this->PlayerStruct[Index]._Kills = 0;
								this->PlayerStruct[Index]._AttackBlock = false;
							}

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1124));

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1127));

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1128));
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
								gObj[Index].InEvent = false;
								gObj[Index].Kills = 0;
								gObj[Index].AttackBlock = false;

								gObj[Index].Move = true;

								gObj[Index].PKLevel = 3;

								GCPKLevelSend(Index, gObj[Index].PKLevel);
							}

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1124));

							gNotice->GCNoticeSend(Index, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1129));
						}
					}

					this->_Players = 0;
					this->_Portal = false;
				}

				if (this->_Progress == true)
				{
					Sleep(this->DeathStruct[i]._Duration);

					this->Winner();
				}
			}
		}
	}
}

bool cDeathEvent::Attack(LPOBJ lpObj, LPOBJ Target)
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

void cDeathEvent::Killer(LPOBJ lpObj, LPOBJ Target)
{
	if (this->_Progress == true)
	{
		if (lpObj->Type == OBJECT_USER && Target->Type == OBJECT_USER)
		{
			if (this->PlayerStruct[lpObj->Index]._InEvent == true && this->PlayerStruct[Target->Index]._InEvent == true)
			{
				this->PlayerStruct[lpObj->Index]._Kills++;

				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage->GetMessage(1130), this->PlayerStruct[lpObj->Index]._Kills);

				for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
				{
					if (gObj[i].Connected == 3 && this->PlayerStruct[i]._InEvent == true)
					{
						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1131));

						gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1132), lpObj->Name, this->PlayerStruct[lpObj->Index]._Kills);
					}
				}
			}
		}
	}
}

void cDeathEvent::Winner()
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

		gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_EVENT_DEATH, -1, -1, lpObj, gObj[this->_Value].Map, gObj[this->_Value].X, gObj[this->_Value].Y);

		GDRankingUpdateSend(lpObj->Index, "TopDeath", 1);

		for (int i = OBJECT_START_USER; i < MAX_OBJECT; ++i)
		{
			if (gObj[i].Connected == 3 && gObj[i].Type == OBJECT_USER)
			{			

				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1124));

				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1133), gObj[this->_Value].Name, this->PlayerStruct[this->_Value]._Kills);

			}
		}

		if (this->_Patente)
		{
			GDAtualizarPatenteSend(lpObj->Index, 9);
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
				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1124));

				gNotice->GCNoticeSend(i, 0, 0, 0, 0, 0, 0, gMessage->GetMessage(1134));
			}
		}

		this->_Value = 0; this->_Players = 0; this->_Progress = false;

		Finish();
	}
}

void cDeathEvent::Finish()
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

			GCPKLevelSend(i, gObj[i].PKLevel);

			gObjTeleport(gObj[i].Index, 0, 125, 125);
		}
	}
}

void cDeathEvent::Quit(LPOBJ lpObj)
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

//DWORD cDeathEvent::GetRemainingTime()
//{
//	if (this->_Progress == false)
//	{
//		return 0;
//	}
//
//	return this->_List;
//
//}
//
//DWORD cDeathEvent::GetNextEventTime()
//{
//	SYSTEMTIME Now;
//	GetLocalTime(&Now);
//
//	DWORD NextTime = 0xFFFFFFFF;
//
//	for (int i = 0; i < this->_List; i++)
//	{
//		if (Now.wDayOfWeek == this->DeathStruct[i]._Day)
//		{
//			DWORD CurrentTime = (Now.wHour * 3600) + (Now.wMinute * 60) + Now.wSecond;
//			DWORD EventTime = (this->DeathStruct[i]._Hours * 3600) + (this->DeathStruct[i]._Minutes * 60);
//
//			if (EventTime > CurrentTime)
//			{
//				DWORD TimeLeft = EventTime - CurrentTime;
//				if (TimeLeft < NextTime)
//				{
//					NextTime = TimeLeft;
//				}
//			}
//		}
//	}
//
//	return (NextTime == 0xFFFFFFFF) ? 0 : NextTime;
//}

int cDeathEvent::GetRemainingTime()
{
	if (this->_Progress == true)
	{
		SYSTEMTIME Now;
		GetLocalTime(&Now);

		for (int i = 0; i < this->_List; i++)
		{
			if (Now.wDayOfWeek == this->DeathStruct[i]._Day &&
				Now.wHour == this->DeathStruct[i]._Hours &&
				Now.wMinute == this->DeathStruct[i]._Minutes)
			{
				return this->DeathStruct[i]._Duration / 1000;
			}
		}
	}
	return 0;
}

int cDeathEvent::GetNextEventTime()
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
			if (this->DeathStruct[i]._Day == diaEncontrado)
			{
				int segundosTotalEvento = (this->DeathStruct[i]._Hours * 3600) + (this->DeathStruct[i]._Minutes * 60);
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

cDeathEvent DeathEvent;