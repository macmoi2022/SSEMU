// CastleSiege.h: interface for the CCastleSiege class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "DataServerProtocol.h"

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_CASTLE_SIEGE_STATE_RECV
{
	PSBMSG_HEAD header; // C1:80:01
	WORD MapServerGroup;
	int aIndex;
};

struct SDHP_CASTLE_SIEGE_NPC_BUY_RECV
{
	PSBMSG_HEAD header; // C1:80:03
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int DefenseLevel;
	int RegenLevel;
	int LifeLevel;
	int Life;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	int BuyCost;
};

struct SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV
{
	PSBMSG_HEAD header; // C1:80:04
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int RepairCost;
};

struct SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV
{
	PSBMSG_HEAD header; // C1:80:05
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int UpgradeType;
	int UpgradeValue;
};

struct SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV
{
	PSBMSG_HEAD header; // C1:80:06
	WORD MapServerGroup;
	int aIndex;
};

struct SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV
{
	PSBMSG_HEAD header; // C1:80:07
	WORD MapServerGroup;
	int aIndex;
	int TaxType;
	int TaxRate;
};

struct SDHP_CASTLE_SIEGE_MONEY_OUT_RECV
{
	PSBMSG_HEAD header; // C1:80:08
	WORD MapServerGroup;
	int aIndex;
	int Money;
};

struct SDHP_CASTLE_SIEGE_DATE_CHANGE_RECV
{
	PSBMSG_HEAD header; // C1:80:09
	WORD MapServerGroup;
	WORD StartYear;
	WORD StartMonth;
	WORD StartDay;
	WORD EndYear;
	WORD EndMonth;
	WORD EndDay;
};

struct SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV
{
	PSBMSG_HEAD header; // C1:80:0A
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_END_RECV
{
	PSBMSG_HEAD header; // C1:80:0B
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_OWNER_CHANGE_RECV
{
	PSBMSG_HEAD header; // C1:80:0C
	WORD MapServerGroup;
	BYTE CastleOccupied;
	char CastleOwner[9];
};

struct SDHP_CASTLE_SIEGE_REGISTER_RECV
{
	PSBMSG_HEAD header; // C1:80:0D
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_RESET_STATE_RECV
{
	PSBMSG_HEAD header; // C1:80:0E
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV
{
	PSBMSG_HEAD header; // C1:80:10
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV
{
	PSBMSG_HEAD header; // C1:80:12
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int GiveUp;
};

struct SDHP_CASTLE_SIEGE_NPC_REMOVE_RECV
{
	PSBMSG_HEAD header; // C1:80:16
	WORD MapServerGroup;
	int NpcClass;
	int NpcIndex;
};

struct SDHP_CASTLE_SIEGE_SYNC_STATE_RECV
{
	PSBMSG_HEAD header; // C1:80:17
	WORD MapServerGroup;
	int State;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
	char CastleOwner[9];
};

struct SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV
{
	PSBMSG_HEAD header; // C1:80:18
	WORD MapServerGroup;
	int money;
};

struct SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV
{
	PSBMSG_HEAD header; // C1:80:19
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_PARTICIPANT_CLEAR_RECV
{
	PSBMSG_HEAD header; // C1:80:1A
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_REGISTERED_CLEAR_RECV
{
	PSBMSG_HEAD header; // C1:80:1B
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_INIT_DATA_RECV
{
	PBMSG_HEAD header; // C1:81
	WORD MapServerGroup;
	int CastleCycle;
};

struct SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV
{
	PBMSG_HEAD header; // C1:83
	WORD MapServerGroup;
	int aIndex;
};

struct SDHP_CASTLE_SIEGE_NPC_CREATE_RECV
{
	PWMSG_HEAD header; // C1:84
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_NPC_CREATE
{
	int NpcClass;
	int NpcIndex;
	int NpcDefense;
	int NpcRegen;
	int NpcMaxLife;
	int NpcLife;
	BYTE X;
	BYTE Y;
	BYTE Dir;
};

struct SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV
{
	PBMSG_HEAD header; // C1:85
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_RECV
{
	PWMSG_HEAD header; // C1:86
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_LOAD_UNION_LIST
{
	char GuildName[9];
	int Side;
};

struct SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV
{
	PWMSG_HEAD  header; // C1:87
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST
{
	char GuildName[9];
	int Side;
	int Involved;
	int Score;
};

struct SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV
{
	PBMSG_HEAD header; // C1:88
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_NPC_SAVE_RECV
{
	PWMSG_HEAD header; // C1:89
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_NPC_SAVE
{
	int NpcClass;
	int NpcIndex;
	int NpcDefense;
	int NpcRegen;
	int NpcMaxLife;
	int NpcLife;
	BYTE X;
	BYTE Y;
	BYTE Dir;
};

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_CASTLE_SIEGE_STATE_SEND
{
	PSBMSG_HEAD header; // C1:80:01
	int result;
	WORD MapServerGroup;
	int aIndex;
	char CastleOwner[9];
	char MasterName[10];
};

struct SDHP_CASTLE_SIEGE_NPC_BUY_SEND
{
	PSBMSG_HEAD header; // C1:80:03
	int result;
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int BuyCost;
};

struct SDHP_CASTLE_SIEGE_NPC_REPAIR_SEND
{
	PSBMSG_HEAD header; // C1:80:04
	int result;
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int Life;
	int MaxLife;
	int RepairCost;
};

struct SDHP_CASTLE_SIEGE_NPC_UPGRADE_SEND
{
	PSBMSG_HEAD header; // C1:80:05
	int result;
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int UpgradeType;
	int UpgradeValue;
};

struct SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_SEND
{
	PSBMSG_HEAD header; // C1:80:06
	int result;
	WORD MapServerGroup;
	int aIndex;
	QWORD TributeMoney;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
};

struct SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_SEND
{
	PSBMSG_HEAD header; // C1:80:07
	int result;
	WORD MapServerGroup;
	int aIndex;
	int TaxType;
	int TaxRate;
};

struct SDHP_CASTLE_SIEGE_MONEY_OUT_SEND
{
	PSBMSG_HEAD header; // C1:80:08
	int result;
	WORD MapServerGroup;
	int aIndex;
	int Money;
	QWORD LastMoney;
};

struct SDHP_CASTLE_SIEGE_REGISTER_INFO_SEND
{
	PSBMSG_HEAD header; // C1:80:0A
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int MarkCount;
	int Position;
	int GiveUp;
};

struct SDHP_CASTLE_SIEGE_REGISTER_SEND
{
	PSBMSG_HEAD header; // C1:80:0D
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_RESET_STATE_SEND
{
	PSBMSG_HEAD header; // C1:80:0E
	int result;
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_REGISTER_MARK_SEND
{
	PSBMSG_HEAD header; // C1:80:10
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int MarkCount;
};

struct SDHP_CASTLE_SIEGE_GIVEUP_GUILD_SEND
{
	PSBMSG_HEAD header; // C1:80:12
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int GiveUp;
	int MarkCount;
};

struct SDHP_CASTLE_SIEGE_SYNC_STATE_SEND
{
	PSBMSG_HEAD header; // C1:80:17
	WORD MapServerGroup;
	int State;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
	char CastleOwner[9];
};

struct SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_SEND
{
	PSBMSG_HEAD header; // C1:80:18
	int result;
	WORD MapServerGroup;
	QWORD money;
};

struct SDHP_CASTLE_SIEGE_RESET_TAX_INFO_SEND
{
	PSBMSG_HEAD header; // C1:80:19
	int result;
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_INIT_DATA_SEND
{
	PWMSG_HEAD header; // C1:81
	int result;
	WORD MapServerGroup;
	WORD StartYear;
	WORD StartMonth;
	WORD StartDay;
	WORD EndYear;
	WORD EndMonth;
	WORD EndDay;
	BYTE CastleGuildListLoad;
	BYTE CastleSiegeEnded;
	BYTE CastleOccupied;
	char CastleOwner[9];
	QWORD TributeMoney;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
	int FirstRun;
	int count;
};

struct SDHP_CASTLE_SIEGE_INIT_DATA
{
	int NpcClass;
	int NpcIndex;
	int DefenseLevel;
	int RegenLevel;
	int LifeLevel;
	int Life;
	BYTE X;
	BYTE Y;
	BYTE Dir;
};

struct SDHP_CASTLE_SIEGE_REGISTER_LIST_SEND
{
	PWMSG_HEAD header; // C1:83
	int result;
	WORD MapServerGroup;
	int aIndex;
	int count;
};

struct SDHP_CASTLE_SIEGE_REGISTER_LIST
{
	char GuildName[9];
	int MarkCount;
	int GiveUp;
	int Position;
};

struct SDHP_CASTLE_SIEGE_NPC_CREATE_SEND
{
	PBMSG_HEAD header; // C1:84
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_SEND
{
	PWMSG_HEAD header; // C1:85
	int result;
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST
{
	char GuildName[9];
	int MarkCount;
	int MemberCount;
	int MasterLevel;
	int Position;
};

struct SDHP_CASTLE_SIEGE_UNION_LIST_SEND
{
	PWMSG_HEAD header; // C1:86
	int result;
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_UNION_LIST
{
	char GuildName[9];
	int Side;
};

struct SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_SEND
{
	PBMSG_HEAD header; // C1:87
	int result;
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_SEND
{
	PWMSG_HEAD header; // C1:88
	int result;
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST
{
	char GuildName[9];
	int Side;
	int Involved;
	int Score;
};

//**********************************************//
//**********************************************//
//**********************************************//

class CCastleSiege
{
	CCastleSiege();
	virtual ~CCastleSiege();
	SingletonInstance(CCastleSiege)
public:
	void GDCastleSiegeStateRecv(SDHP_CASTLE_SIEGE_STATE_RECV* lpMsg,int index);
	void GDCastleSiegeNpcBuyRecv(SDHP_CASTLE_SIEGE_NPC_BUY_RECV* lpMsg,int index);
	void GDCastleSiegeNpcRepairRecv(SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV* lpMsg,int index);
	void GDCastleSiegeNpcUpgradeRecv(SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV* lpMsg,int index);
	void GDCastleSiegeTaxMoneyInfoRecv(SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV* lpMsg,int index);
	void GDCastleSiegeTaxRateChangeRecv(SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV* lpMsg,int index);
	void GDCastleSiegeMoneyOutRecv(SDHP_CASTLE_SIEGE_MONEY_OUT_RECV* lpMsg,int index);
	void GDCastleSiegeDateChangeRecv(SDHP_CASTLE_SIEGE_DATE_CHANGE_RECV* lpMsg);
	void GDCastleSiegeRegisterInfoRecv(SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV* lpMsg,int index);
	void GDCastleSiegeEndRecv(SDHP_CASTLE_SIEGE_END_RECV* lpMsg);
	void GDCastleSiegeOwnerChangeRecv(SDHP_CASTLE_SIEGE_OWNER_CHANGE_RECV* lpMsg);
	void GDCastleSiegeRegisterRecv(SDHP_CASTLE_SIEGE_REGISTER_RECV* lpMsg,int index);
	void GDCastleSiegeResetStateRecv(SDHP_CASTLE_SIEGE_RESET_STATE_RECV* lpMsg,int index);
	void GDCastleSiegeRegisterMarkRecv(SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV* lpMsg,int index);
	void GDCastleSiegeGiveUpGuildRecv(SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV* lpMsg,int index);
	void GDCastleSiegeNpcRemoveRecv(SDHP_CASTLE_SIEGE_NPC_REMOVE_RECV* lpMsg);
	void GDCastleSiegeSyncStateRecv(SDHP_CASTLE_SIEGE_SYNC_STATE_RECV* lpMsg);
	void GDCastleSiegeAddTributeMoneyRecv(SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV* lpMsg,int index);
	void GDCastleSiegeResetTaxInfoRecv(SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV* lpMsg,int index);
	void GDCastleSiegeParticipantClearRecv(SDHP_CASTLE_SIEGE_PARTICIPANT_CLEAR_RECV* lpMsg);
	void GDCastleSiegeRegisteredClearRecv(SDHP_CASTLE_SIEGE_REGISTERED_CLEAR_RECV* lpMsg);
	void GDCastleSiegeInitDataRecv(SDHP_CASTLE_SIEGE_INIT_DATA_RECV* lpMsg,int index);
	void GDCastleSiegeRegisterListRecv(SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV* lpMsg,int index);
	void GDCastleSiegeNpcCreateRecv(SDHP_CASTLE_SIEGE_NPC_CREATE_RECV* lpMsg,int index);
	void GDCastleSiegeBuildGuildListRecv(SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV* lpMsg,int index);
	void GDCastleSiegeLoadUnionListRecv(SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_RECV* lpMsg,int index);
	void GDCastleSiegeSaveGuildListRecv(SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV* lpMsg,int index);
	void GDCastleSiegeLoadGuildListRecv(SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV* lpMsg,int index);
	void GDCastleSiegeNpcSaveRecv(SDHP_CASTLE_SIEGE_NPC_SAVE_RECV* lpMsg);
};

#define gCastleSiege SingNull(CCastleSiege)