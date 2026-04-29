// C7astleSiege.cpp: implementation of the CCastleSiege class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CastleSiege.h"
#include "QueryManager.h"
#include "SocketManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleSiege::CCastleSiege() // OK
{

}

CCastleSiege::~CCastleSiege() // OK
{

}

void CCastleSiege::GDCastleSiegeStateRecv(SDHP_CASTLE_SIEGE_STATE_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_STATE_SEND pMsg;

	pMsg.header.set(0x80,0x01,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	memset(pMsg.CastleOwner,0,sizeof(pMsg.CastleOwner));

	memset(pMsg.MasterName,0,sizeof(pMsg.MasterName));

	if(gQueryManager->ExecQuery("EXEC WZ_CS_GetOwnerGuildMaster %d",lpMsg->MapServerGroup) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		gQueryManager->GetAsString("OwnerGuild",pMsg.CastleOwner,sizeof(pMsg.CastleOwner));
		
		gQueryManager->GetAsString("OwnerGuildMaster",pMsg.MasterName,sizeof(pMsg.MasterName));

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeNpcBuyRecv(SDHP_CASTLE_SIEGE_NPC_BUY_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_NPC_BUY_SEND pMsg;

	pMsg.header.set(0x80,0x03,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	pMsg.NpcClass = lpMsg->NpcClass;

	pMsg.NpcIndex = lpMsg->NpcIndex;

	pMsg.BuyCost = lpMsg->BuyCost;

	if (gQueryManager->ExecQuery("EXEC WZ_CS_ReqNpcBuy %d,%d,%d,%d,%d,%d,%d,%d,%d,%d",lpMsg->MapServerGroup,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->DefenseLevel,lpMsg->RegenLevel,lpMsg->LifeLevel,lpMsg->Life,lpMsg->X,lpMsg->Y,lpMsg->Dir) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeNpcRepairRecv(SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_NPC_REPAIR_SEND pMsg;

	pMsg.header.set(0x80,0x04,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	pMsg.NpcClass = lpMsg->NpcClass;

	pMsg.NpcIndex = lpMsg->NpcIndex;
	
	pMsg.RepairCost = lpMsg->RepairCost;

	if (gQueryManager->ExecQuery("EXEC WZ_CS_ReqNpcRepair %d,%d,%d",lpMsg->MapServerGroup,lpMsg->NpcClass,lpMsg->NpcIndex) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");
		
		pMsg.Life = gQueryManager->GetAsInteger("NPC_HP");
		
		pMsg.MaxLife = gQueryManager->GetAsInteger("NPC_MAXHP");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeNpcUpgradeRecv(SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_NPC_UPGRADE_SEND pMsg;

	pMsg.header.set(0x80,0x05,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	pMsg.NpcClass = lpMsg->NpcClass;

	pMsg.NpcIndex = lpMsg->NpcIndex;

	pMsg.UpgradeType = lpMsg->UpgradeType;
	
	pMsg.UpgradeValue = lpMsg->UpgradeValue;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ReqNpcUpgrade %d,%d,%d,%d,%d",lpMsg->MapServerGroup,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeTaxMoneyInfoRecv(SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_SEND pMsg;

	pMsg.header.set(0x80,0x06,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	if (gQueryManager->ExecQuery("EXEC WZ_CS_GetCastleTaxInfo %d",lpMsg->MapServerGroup) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = 1;

		pMsg.TributeMoney = gQueryManager->GetAsInteger64("MONEY");

		pMsg.TaxRateChaos = gQueryManager->GetAsInteger("TAX_RATE_CHAOS");

		pMsg.TaxRateStore = gQueryManager->GetAsInteger("TAX_RATE_STORE");

		pMsg.TaxRateHunt = gQueryManager->GetAsInteger("TAX_HUNT_ZONE");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeTaxRateChangeRecv(SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_SEND pMsg;

	pMsg.header.set(0x80,0x07,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ModifyTaxRate %d,%d,%d",lpMsg->MapServerGroup,lpMsg->TaxType,lpMsg->TaxRate) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		pMsg.TaxType = gQueryManager->GetAsInteger("TaxKind");

		pMsg.TaxRate = gQueryManager->GetAsInteger("TaxRate");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeMoneyOutRecv(SDHP_CASTLE_SIEGE_MONEY_OUT_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_MONEY_OUT_SEND pMsg;

	pMsg.header.set(0x80,0x08,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	pMsg.Money = lpMsg->Money;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ModifyMoney %d,%d",lpMsg->MapServerGroup,lpMsg->Money) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");
		
		pMsg.LastMoney = gQueryManager->GetAsInteger64("MONEY");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeDateChangeRecv(SDHP_CASTLE_SIEGE_DATE_CHANGE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_CS_ModifyCastleSchedule %d, '%d-%d-%d 00:00:00', '%d-%d-%d 00:00:00'",lpMsg->MapServerGroup,lpMsg->StartYear,lpMsg->StartMonth,lpMsg->StartDay,lpMsg->EndYear,lpMsg->EndMonth,lpMsg->EndDay);

	gQueryManager->Close();
}

void CCastleSiege::GDCastleSiegeRegisterInfoRecv(SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_REGISTER_INFO_SEND pMsg;

	pMsg.header.set(0x80,0x0A,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	memcpy(pMsg.GuildName,lpMsg->GuildName,sizeof(pMsg.GuildName));

	if(gQueryManager->ExecQuery("EXEC WZ_CS_GetGuildMarkRegInfo %d,'%s'",lpMsg->MapServerGroup,lpMsg->GuildName) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 2;

		pMsg.MarkCount = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = 1;

		pMsg.MarkCount = gQueryManager->GetAsInteger("REG_MARKS");
		
		pMsg.GiveUp = gQueryManager->GetAsInteger("IS_GIVEUP");

		pMsg.Position = gQueryManager->GetAsInteger("SEQ_NUM");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeEndRecv(SDHP_CASTLE_SIEGE_END_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_CS_ModifySiegeEnd %d,%d",lpMsg->MapServerGroup,1);
	
	gQueryManager->Close();
}

void CCastleSiege::GDCastleSiegeOwnerChangeRecv(SDHP_CASTLE_SIEGE_OWNER_CHANGE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_CS_ModifyCastleOwnerInfo %d,%d,'%s'",lpMsg->MapServerGroup,lpMsg->CastleOccupied,lpMsg->CastleOwner);

	gQueryManager->Close();
}

void CCastleSiege::GDCastleSiegeRegisterRecv(SDHP_CASTLE_SIEGE_REGISTER_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_REGISTER_SEND pMsg;

	pMsg.header.set(0x80,0x0D,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	memcpy(pMsg.GuildName,lpMsg->GuildName,sizeof(pMsg.GuildName));
	
	if(gQueryManager->ExecQuery("EXEC WZ_CS_ReqRegAttackGuild %d,'%s'",lpMsg->MapServerGroup,lpMsg->GuildName) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeResetStateRecv(SDHP_CASTLE_SIEGE_RESET_STATE_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_RESET_STATE_SEND pMsg;

	pMsg.header.set(0x80,0x0E,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ResetCastleSiege %d",lpMsg->MapServerGroup) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeRegisterMarkRecv(SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_REGISTER_MARK_SEND pMsg;

	pMsg.header.set(0x80,0x10,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	memcpy(pMsg.GuildName,lpMsg->GuildName,sizeof(pMsg.GuildName));

	if (gQueryManager->ExecQuery("EXEC WZ_CS_ReqRegGuildMark %d,'%s'",lpMsg->MapServerGroup,lpMsg->GuildName) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.MarkCount = 0;

		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		pMsg.MarkCount = gQueryManager->GetAsInteger("REG_MARKS");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeGiveUpGuildRecv(SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_GIVEUP_GUILD_SEND pMsg;

	pMsg.header.set(0x80,0x12,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	memcpy(pMsg.GuildName,lpMsg->GuildName,sizeof(pMsg.GuildName));

	pMsg.GiveUp = lpMsg->GiveUp;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ModifyGuildGiveUp %d,'%s',%d",lpMsg->MapServerGroup,lpMsg->GuildName,lpMsg->GiveUp) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.MarkCount = 0;

		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		pMsg.MarkCount = gQueryManager->GetAsInteger("DEL_MARKS");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeNpcRemoveRecv(SDHP_CASTLE_SIEGE_NPC_REMOVE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("DELETE MuCastle_NPC WHERE MAP_SVR_GROUP=%d AND NPC_NUMBER=%d AND NPC_INDEX=%d",lpMsg->MapServerGroup,lpMsg->NpcClass,lpMsg->NpcIndex);

	gQueryManager->Close();
}

void CCastleSiege::GDCastleSiegeSyncStateRecv(SDHP_CASTLE_SIEGE_SYNC_STATE_RECV* lpMsg) // OK
{
	SDHP_CASTLE_SIEGE_SYNC_STATE_SEND pMsg;

	pMsg.header.set(0x80,0x17,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.State = lpMsg->State;
	
	pMsg.TaxRateChaos = lpMsg->TaxRateChaos;
	
	pMsg.TaxRateStore = lpMsg->TaxRateStore;

	pMsg.TaxRateHunt = lpMsg->TaxRateHunt;
	
	memcpy(pMsg.CastleOwner,lpMsg->CastleOwner,sizeof(pMsg.CastleOwner));

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void CCastleSiege::GDCastleSiegeAddTributeMoneyRecv(SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_SEND pMsg;

	pMsg.header.set(0x80,0x18,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ModifyMoney %d,%d",lpMsg->MapServerGroup,lpMsg->money) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");
		
		pMsg.money = gQueryManager->GetAsInteger64("MONEY");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeResetTaxInfoRecv(SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_RESET_TAX_INFO_SEND pMsg;

	pMsg.header.set(0x80,0x19,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_ResetCastleTaxInfo %d",lpMsg->MapServerGroup) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.result = gQueryManager->GetAsInteger("QueryResult");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeParticipantClearRecv(SDHP_CASTLE_SIEGE_PARTICIPANT_CLEAR_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_CS_ResetSiegeGuildInfo %d",lpMsg->MapServerGroup);
	
	gQueryManager->Close();
}

void CCastleSiege::GDCastleSiegeRegisteredClearRecv(SDHP_CASTLE_SIEGE_REGISTERED_CLEAR_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_CS_ResetRegSiegeInfo %d",lpMsg->MapServerGroup);
	
	gQueryManager->Close();
}

void CCastleSiege::GDCastleSiegeInitDataRecv(SDHP_CASTLE_SIEGE_INIT_DATA_RECV* lpMsg,int index) // OK
{
	BYTE send[4096];

	SDHP_CASTLE_SIEGE_INIT_DATA_SEND pMsg;

	pMsg.header.set(0x81,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.count = 0;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_GetCastleTotalInfo %d,%d",lpMsg->MapServerGroup,lpMsg->CastleCycle) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.result = 0;

		gQueryManager->Close();
	}
	else
	{
		pMsg.StartYear = gQueryManager->GetAsInteger("SYEAR");
		pMsg.StartMonth = gQueryManager->GetAsInteger("SMONTH");
		pMsg.StartDay = gQueryManager->GetAsInteger("SDAY");
		pMsg.EndYear = gQueryManager->GetAsInteger("EYEAR");
		pMsg.EndMonth = gQueryManager->GetAsInteger("EMONTH");
		pMsg.EndDay = gQueryManager->GetAsInteger("EDAY");
		pMsg.CastleGuildListLoad = gQueryManager->GetAsInteger("SIEGE_GUILDLIST_SETTED");
		pMsg.CastleSiegeEnded = gQueryManager->GetAsInteger("SIEGE_ENDED");
		pMsg.CastleOccupied = gQueryManager->GetAsInteger("CASTLE_OCCUPY");
		gQueryManager->GetAsString("OWNER_GUILD",pMsg.CastleOwner,sizeof(pMsg.CastleOwner));
		pMsg.TributeMoney = gQueryManager->GetAsInteger64("MONEY");
		pMsg.TaxRateChaos = gQueryManager->GetAsInteger("TAX_RATE_CHAOS");
		pMsg.TaxRateStore = gQueryManager->GetAsInteger("TAX_RATE_STORE");
		pMsg.TaxRateHunt = gQueryManager->GetAsInteger("TAX_HUNT_ZONE");
		pMsg.FirstRun = gQueryManager->GetAsInteger("FIRST_CREATE");

		gQueryManager->Close();

		SDHP_CASTLE_SIEGE_INIT_DATA info;

		if(gQueryManager->ExecQuery("EXEC WZ_CS_GetCastleNpcInfo %d",lpMsg->MapServerGroup) != 0)
		{
			while (gQueryManager->Fetch() != SQL_NO_DATA)
			{
				info.NpcClass = gQueryManager->GetAsInteger("NPC_NUMBER");
				info.NpcIndex = gQueryManager->GetAsInteger("NPC_INDEX");
				info.DefenseLevel = gQueryManager->GetAsInteger("NPC_DF_LEVEL");
				info.RegenLevel = gQueryManager->GetAsInteger("NPC_RG_LEVEL");
				info.LifeLevel = gQueryManager->GetAsInteger("NPC_MAXHP");
				info.Life = gQueryManager->GetAsInteger("NPC_HP");
				info.X = gQueryManager->GetAsInteger("NPC_X");
				info.Y = gQueryManager->GetAsInteger("NPC_Y");
				info.Dir = gQueryManager->GetAsInteger("NPC_DIR");

				memcpy(&send[size],&info,sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}

			pMsg.result = 1;
		}

		gQueryManager->Close();
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager->DataSend(index, send, size);
}

void CCastleSiege::GDCastleSiegeRegisterListRecv(SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV* lpMsg,int index) // OK
{
	BYTE send[4096];

	SDHP_CASTLE_SIEGE_REGISTER_LIST_SEND pMsg;

	pMsg.header.set(0x83,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.aIndex = lpMsg->aIndex;

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_REGISTER_LIST info;

	if(gQueryManager->ExecQuery("SELECT TOP 100 * FROM MuCastle_REG_SIEGE WHERE MAP_SVR_GROUP=%d ORDER BY SEQ_NUM ASC",lpMsg->MapServerGroup) != 0)
	{
		while(gQueryManager->Fetch() != SQL_NO_DATA)
		{
			gQueryManager->GetAsString("REG_SIEGE_GUILD",info.GuildName,sizeof(info.GuildName));

			info.MarkCount = gQueryManager->GetAsInteger("REG_MARKS");
			
			info.GiveUp = gQueryManager->GetAsInteger("IS_GIVEUP");
			
			info.Position = gQueryManager->GetAsInteger("SEQ_NUM");

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}

		pMsg.result = 1;
	}

	gQueryManager->Close();

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index,send,size);
}

void CCastleSiege::GDCastleSiegeNpcCreateRecv(SDHP_CASTLE_SIEGE_NPC_CREATE_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_NPC_CREATE_SEND pMsg;

	pMsg.header.set(0x84,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	gQueryManager->ExecQuery("DELETE MuCastle_NPC WHERE MAP_SVR_GROUP=%d",lpMsg->MapServerGroup);

	gQueryManager->Close();

	for(int n = 0; n < lpMsg->count; n++)
	{
		SDHP_CASTLE_SIEGE_NPC_CREATE* lpInfo = (SDHP_CASTLE_SIEGE_NPC_CREATE*)(((BYTE*)lpMsg) + sizeof(SDHP_CASTLE_SIEGE_NPC_CREATE_RECV) + (sizeof(SDHP_CASTLE_SIEGE_NPC_CREATE)*n));
		
		gQueryManager->ExecQuery("INSERT INTO MuCastle_NPC (MAP_SVR_GROUP,NPC_NUMBER,NPC_INDEX,NPC_DF_LEVEL,NPC_RG_LEVEL,NPC_MAXHP,NPC_HP,NPC_X,NPC_Y,NPC_DIR,NPC_CREATEDATE) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,GetDate())",lpMsg->MapServerGroup,lpInfo->NpcClass,lpInfo->NpcIndex,lpInfo->NpcDefense,lpInfo->NpcRegen,lpInfo->NpcMaxLife,lpInfo->NpcLife,lpInfo->X,lpInfo->Y,lpInfo->Dir);

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeBuildGuildListRecv(SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV* lpMsg,int index) // OK
{
	BYTE send[4096];

	SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0x85,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST info;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_GetCalcRegGuildList %d",lpMsg->MapServerGroup) != 0)
	{
		while(gQueryManager->Fetch() != SQL_NO_DATA)
		{
			gQueryManager->GetAsString("REG_SIEGE_GUILD",info.GuildName,sizeof(info.GuildName));

			info.MarkCount = gQueryManager->GetAsInteger("REG_MARKS");
			
			info.MemberCount = gQueryManager->GetAsInteger("GUILD_MEMBER");

			info.MasterLevel = gQueryManager->GetAsInteger("GM_LEVEL");
			
			info.Position = gQueryManager->GetAsInteger("SEQ_NUM");

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}

		pMsg.result = 1;
	}

	gQueryManager->Close();

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index,send,size);
}

void CCastleSiege::GDCastleSiegeLoadUnionListRecv(SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_RECV* lpMsg,int index) // OK
{
	BYTE send[2048];

	SDHP_CASTLE_SIEGE_UNION_LIST_SEND pMsg;

	pMsg.header.set(0x86,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_UNION_LIST info;

	for(int n = 0; n < lpMsg->count; n++)
	{
		SDHP_CASTLE_SIEGE_LOAD_UNION_LIST* lpInfo = (SDHP_CASTLE_SIEGE_LOAD_UNION_LIST*)(((BYTE*)lpMsg) + sizeof(SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_RECV) + (sizeof(SDHP_CASTLE_SIEGE_LOAD_UNION_LIST)*n));

		if(gQueryManager->ExecQuery("EXEC WZ_CS_GetCsGuildUnionInfo %s",lpInfo->GuildName) != 0)
		{
			while (gQueryManager->Fetch() != SQL_NO_DATA)
			{
				memset(&info,0,sizeof(info));

				gQueryManager->GetAsString("GUILD_NAME",info.GuildName,sizeof(info.GuildName));

				info.Side = lpInfo->Side;

				memcpy(&send[size],&info,sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}

			pMsg.result = 1;
		}

		gQueryManager->Close();
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index,send,size);
}

void CCastleSiege::GDCastleSiegeSaveGuildListRecv(SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV* lpMsg,int index) // OK
{
	SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0x87,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	gQueryManager->ExecQuery("DELETE MuCastle_SIEGE_GUILDLIST WHERE MAP_SVR_GROUP=%d",lpMsg->MapServerGroup);

	gQueryManager->Close();

	for(int n = 0; n < lpMsg->count; n++)
	{
		SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST* lpInfo = (SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST*)(((BYTE*)lpMsg) + sizeof(SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV) + (sizeof(SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST)*n));

		if(gQueryManager->ExecQuery("EXEC WZ_CS_SetSiegeGuildInfo %d,'%s',%d,%d,%d",lpMsg->MapServerGroup,lpInfo->GuildName,lpInfo->Side,lpInfo->Involved,lpInfo->Score) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
		{
			pMsg.result = 0;

			gQueryManager->Close();
		}
		else
		{
			pMsg.result = 1;

			gQueryManager->Close();
		}
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCastleSiege::GDCastleSiegeLoadGuildListRecv(SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV* lpMsg,int index)
{
	BYTE send[4096];

	SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0x88,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST info;

	if(gQueryManager->ExecQuery("EXEC WZ_CS_GetSiegeGuildInfo %d",lpMsg->MapServerGroup) != 0)
	{
		while(gQueryManager->Fetch() != SQL_NO_DATA)
		{
			gQueryManager->GetAsString("GUILD_NAME",info.GuildName,sizeof(info.GuildName));

			info.Side = gQueryManager->GetAsInteger("GUILD_ID");
			
			info.Involved = gQueryManager->GetAsInteger("GUILD_INVOLVED");

			info.Score = gQueryManager->GetAsInteger("GUILD_SCORE");

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}

		pMsg.result = 1;
	}

	gQueryManager->Close();

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index,send,size);
}

void CCastleSiege::GDCastleSiegeNpcSaveRecv(SDHP_CASTLE_SIEGE_NPC_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("DELETE MuCastle_NPC WHERE MAP_SVR_GROUP=%d",lpMsg->MapServerGroup);

	gQueryManager->Close();

	for(int n = 0; n < lpMsg->count; n++)
	{
		SDHP_CASTLE_SIEGE_NPC_SAVE* lpInfo = (SDHP_CASTLE_SIEGE_NPC_SAVE*)(((BYTE*)lpMsg) + sizeof(SDHP_CASTLE_SIEGE_NPC_SAVE_RECV) + (sizeof(SDHP_CASTLE_SIEGE_NPC_SAVE)*n));
		
		gQueryManager->ExecQuery("INSERT INTO MuCastle_NPC (MAP_SVR_GROUP,NPC_NUMBER,NPC_INDEX,NPC_DF_LEVEL,NPC_RG_LEVEL,NPC_MAXHP,NPC_HP,NPC_X,NPC_Y,NPC_DIR,NPC_CREATEDATE) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,GetDate())",lpMsg->MapServerGroup,lpInfo->NpcClass,lpInfo->NpcIndex,lpInfo->NpcDefense,lpInfo->NpcRegen,lpInfo->NpcMaxLife,lpInfo->NpcLife,lpInfo->X,lpInfo->Y,lpInfo->Dir);

		gQueryManager->Close();
	}
}