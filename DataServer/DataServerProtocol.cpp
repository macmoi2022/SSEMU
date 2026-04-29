#include "stdafx.h"
#include "DataServerProtocol.h"
#include "BadSyntax.h"
#include "CastleSiege.h"
#include "CharacterManager.h"
#include "CommandManager.h"
#include "CustomPick.h"
#include "Friend.h"
#include "GensSystem.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GoldenArcher.h"
#include "Helper.h"
#include "LuckyCoin.h"
#include "LuckyItem.h"
#include "MasterSkillTree.h"
#include "NpcTalk.h"
#include "PeriodicItem.h"
#include "QueryManager.h"
#include "Quest.h"
#include "QuestWorld.h"
#include "Reconnect.h"
#include "ServerManager.h"
#include "SocketManager.h"
#include "Util.h"
#include "Warehouse.h"
#include "MuCashShop.h"
#include "PcPoint.h"

void DataServerProtocolCore(int index,BYTE head,BYTE* lpMsg,int size) // OK
{
	PROTECT_START

	gServerManager[index].m_PacketTime = GetTickCount();

	switch(head)
	{
		case 0x00:
			GDServerInfoRecv((SDHP_SERVER_INFO_RECV*)lpMsg,index);
			break;
		case 0x01:
			GDCharacterListRecv((SDHP_CHARACTER_LIST_RECV*)lpMsg,index);
			break;
		case 0x02:
			GDCharacterCreateRecv((SDHP_CHARACTER_CREATE_RECV*)lpMsg,index);
			break;
		case 0x03:
			GDCharacterDeleteRecv((SDHP_CHARACTER_DELETE_RECV*)lpMsg,index);
			break;
		case 0x04:
			GDCharacterInfoRecv((SDHP_CHARACTER_INFO_RECV*)lpMsg,index);
			break;
		case 0x05:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gWarehouse->GDWarehouseItemRecv((SDHP_WAREHOUSE_ITEM_RECV*)lpMsg,index);
					break;
				case 0x30:
					gWarehouse->GDWarehouseItemSaveRecv((SDHP_WAREHOUSE_ITEM_SAVE_RECV*)lpMsg);
					break;
			}
			break;
		case 0x06:
			GDGetItemSerialRecv((SDHP_GET_ITEM_SERIAL_RECV*)lpMsg,index);
			break;
		case 0x07:
			GDCreateItemRecv((SDHP_CREATE_ITEM_RECV*)lpMsg,index);
			break;
		case 0x08:
			GDOptionDataRecv((SDHP_OPTION_DATA_RECV*)lpMsg,index);
			break;
		case 0x09:
			GDPetItemInfoRecv((SDHP_PET_ITEM_INFO_RECV*)lpMsg,index);
			break;
		case 0x0C:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gQuest->GDQuestKillCountRecv((SDHP_QUEST_KILL_COUNT_RECV*)lpMsg,index);
					break;
				case 0x30:
					gQuest->GDQuestKillCountSaveRecv((SDHP_QUEST_KILL_COUNT_SAVE_RECV*)lpMsg);
					break;
			}
			break;
		case 0x0E:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					#if(DATASERVER_UPDATE>=301)
					gNpcTalk->GDNpcLeoTheHelperRecv((SDHP_NPC_LEO_THE_HELPER_RECV*)lpMsg,index);
					#endif
					break;
				case 0x30:
					#if(DATASERVER_UPDATE>=301)
					gNpcTalk->GDNpcLeoTheHelperSaveRecv((SDHP_NPC_LEO_THE_HELPER_SAVE_RECV*)lpMsg);
					#endif
					break;
			}
			break;
		case 0x0F:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gCommandManager->GDCommandResetRecv((SDHP_COMMAND_RESET_RECV*)lpMsg,index);
					break;
				case 0x01:
					gCommandManager->GDCommandMasterResetRecv((SDHP_COMMAND_MASTER_RESET_RECV*)lpMsg,index);
					break;
				case 0x02:
					gCommandManager->GDCommandDivorceRecv((SDHP_COMMAND_DIVORCE_RECV*)lpMsg,index);
					break;
				case 0x03:
					gCommandManager->GDCommandGiftRecv((SDHP_COMMAND_GIFT_RECV*)lpMsg,index);
					break;
				case 0x04:
					gCommandManager->GDCommandRenameRecv((SDHP_COMMAND_RENAME_RECV*)lpMsg,index);
					break;
				case 0x05:
					gCommandManager->GDCommandBanAccountRecv((SDHP_COMMAND_BAN_ACCOUNT_RECV*)lpMsg,index);
					break;
				case 0x06:
					gCommandManager->GDCommandBanCharacterRecv((SDHP_COMMAND_BAN_CHARACTER_RECV*)lpMsg,index);
					break;
			}
			break;
		case 0x13:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gCustomPick->GDCustomPickRecv((SDHP_CUSTOM_PICK_RECV*)lpMsg,index);
					break;
				case 0x30:
					gCustomPick->GDCustomPickSaveRecv((SDHP_CUSTOM_PICK_SAVE_RECV*)lpMsg);
					break;
			}
			break;
		case 0x19:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					#if(DATASERVER_SHOP==1 || DATASERVER_SHOP==4)
					gPcPoint->GDPcPointPointRecv((SDHP_PC_POINT_POINT_RECV*)lpMsg,index);
					#endif
					break;
				case 0x01:
					#if(DATASERVER_SHOP==1 || DATASERVER_SHOP==4)
					gPcPoint->GDPcPointItemBuyRecv((SDHP_PC_POINT_ITEM_BUY_RECV*)lpMsg,index);
					#endif
					break;
				case 0x02:
					#if(DATASERVER_SHOP==1 || DATASERVER_SHOP==4)
					gPcPoint->GDPcPointRecievePointRecv((SDHP_PC_POINT_RECIEVE_POINT_RECV*)lpMsg,index);
					#endif
					break;
				case 0x30:
					#if(DATASERVER_SHOP==1 || DATASERVER_SHOP==4)
					gPcPoint->GDPcPointAddPointSaveRecv((SDHP_PC_POINT_ADD_POINT_SAVE_RECV*)lpMsg);
					#endif
					break;
				case 0x31:
					#if(DATASERVER_SHOP==1 || DATASERVER_SHOP==4)
					gPcPoint->GDPcPointSubPointSaveRecv((SDHP_PC_POINT_SUB_POINT_SAVE_RECV*)lpMsg);
					#endif
					break;
			}
			break;
		case 0x1C:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					#if(DATASERVER_SHOP==2 || DATASERVER_SHOP==4)
					gMuCashShop->GDMuCashShopPointRecv((SDHP_MU_CASH_SHOP_POINT_RECV*)lpMsg,index);
					#endif
					break;
				case 0x01:
					#if(DATASERVER_SHOP==2 || DATASERVER_SHOP==4)
					gMuCashShop->GDMuCashShopItemBuyRecv((SDHP_MU_CASH_SHOP_ITEM_BUY_RECV*)lpMsg,index);
					#endif
					break;
				case 0x02:
					#if(DATASERVER_SHOP==2 || DATASERVER_SHOP==4)
					gMuCashShop->GDMuCashShopRecievePointRecv((SDHP_MU_CASH_SHOP_RECIEVE_POINT_RECV*)lpMsg,index);
					#endif
					break;
				case 0x30:
					#if(DATASERVER_SHOP==2 || DATASERVER_SHOP==4)
					gMuCashShop->GDMuCashShopAddPointSaveRecv((SDHP_MU_CASH_SHOP_ADD_POINT_SAVE_RECV*)lpMsg);
					#endif
					break;
				case 0x31:
					#if(DATASERVER_SHOP==2 || DATASERVER_SHOP==4)
					gMuCashShop->GDMuCashShopSubPointSaveRecv((SDHP_MU_CASH_SHOP_SUB_POINT_SAVE_RECV*)lpMsg);
					#endif
					break;
			}
			break;
		case 0x1E:
			#if(DATASERVER_UPDATE>=201)
			GDCrywolfSyncRecv((SDHP_CRYWOLF_SYNC_RECV*)lpMsg);
			#endif
			break;
		case 0x1F:
			#if(DATASERVER_UPDATE>=201)
			GDCrywolfInfoRecv((SDHP_CRYWOLF_INFO_RECV*)lpMsg,index);
			#endif
			break;
		case 0x20:
			GDGlobalPostRecv((SDHP_GLOBAL_POST_RECV*)lpMsg,index);
			break;
		case 0x21:
			GDGlobalNoticeRecv((SDHP_GLOBAL_NOTICE_RECV*)lpMsg,index);
			break;
		case 0x2A:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					#if(DATASERVER_UPDATE<=401)
					gPeriodicItem->GDPeriodicItemRecv((SDHP_PERIODIC_ITEM_RECV*)lpMsg,index);
					#endif
					break;
				case 0x30:
					#if(DATASERVER_UPDATE<=401)
					gPeriodicItem->GDPeriodicItemSaveRecv((SDHP_PERIODIC_ITEM_SAVE_RECV*)lpMsg);
					#endif
					break;
			}
			break;
		case 0x2B:
			GDGlobalMessageRecv((SDHP_GLOBAL_MESSAGE_RECV*)lpMsg,index);
			break;
		case 0x30:
			GDCharacterInfoSaveRecv((SDHP_CHARACTER_INFO_SAVE_RECV*)lpMsg);
			break;
		case 0x31:
			GDInventoryItemSaveRecv((SDHP_INVENTORY_ITEM_SAVE_RECV*)lpMsg);
			break;
		case 0x33:
			GDOptionDataSaveRecv((SDHP_OPTION_DATA_SAVE_RECV*)lpMsg);
			break;
		case 0x34:
			GDPetItemInfoSaveRecv((SDHP_PET_ITEM_INFO_SAVE_RECV*)lpMsg);
			break;
		case 0x39:
			GDResetInfoSaveRecv((SDHP_RESET_INFO_SAVE_RECV*)lpMsg);
			break;
		case 0x3A:
			GDMasterResetInfoSaveRecv((SDHP_MASTER_RESET_INFO_SAVE_RECV*)lpMsg);
			break;
		case 0x3C:
			GDRankingDuelSaveRecv((SDHP_RANKING_DUEL_SAVE_RECV*)lpMsg);
			break;
		case 0x3D:
			GDRankingBloodCastleSaveRecv((SDHP_RANKING_BLOOD_CASTLE_SAVE_RECV*)lpMsg);
			break;
		case 0x3E:
			GDRankingChaosCastleSaveRecv((SDHP_RANKING_CHAOS_CASTLE_SAVE_RECV*)lpMsg);
			break;
		case 0x3F:
			GDRankingDevilSquareSaveRecv((SDHP_RANKING_DEVIL_SQUARE_SAVE_RECV*)lpMsg);
			break;
		case 0x40:
			GDRankingIllusionTempleSaveRecv((SDHP_RANKING_ILLUSION_TEMPLE_SAVE_RECV*)lpMsg);
			break;
		case 0x41:
			GDRankingUpdateSaveRecv((SDHP_RANKING_KILL_HERO_SAVE_RECV*)lpMsg);
			break;
		case 0x42:
			GDCreationCardSaveRecv((SDHP_CREATION_CARD_SAVE_RECV*)lpMsg);
			break;
		case 0x49:
			#if(DATASERVER_UPDATE>=201)
			GDCrywolfInfoSaveRecv((SDHP_CRYWOLF_INFO_SAVE_RECV*)lpMsg);
			#endif
			break;
		case 0x50:
			GDCustomMonsterKilLCountRecv((SDHP_MONSTER_KILL_COUNT_RECV*)lpMsg, index);
			break;
		case 0x70:
			GDConnectCharacterRecv((SDHP_CONNECT_CHARACTER_RECV*)lpMsg,index);
			break;
		case 0x71:
			GDDisconnectCharacterRecv((SDHP_DISCONNECT_CHARACTER_RECV*)lpMsg,index);
			break;
		case 0x72:
			GDGlobalWhisperRecv((SDHP_GLOBAL_WHISPER_RECV*)lpMsg,index);
			break;
		case 0x80:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x01:
					gCastleSiege->GDCastleSiegeStateRecv((SDHP_CASTLE_SIEGE_STATE_RECV*)lpMsg,index);
					break;
				case 0x03:
					gCastleSiege->GDCastleSiegeNpcBuyRecv((SDHP_CASTLE_SIEGE_NPC_BUY_RECV*)lpMsg,index);
					break;
				case 0x04:
					gCastleSiege->GDCastleSiegeNpcRepairRecv((SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV*)lpMsg,index);
					break;
				case 0x05:
					gCastleSiege->GDCastleSiegeNpcUpgradeRecv((SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV*)lpMsg,index);
					break;
				case 0x06:
					gCastleSiege->GDCastleSiegeTaxMoneyInfoRecv((SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV*)lpMsg,index);
					break;
				case 0x07:
					gCastleSiege->GDCastleSiegeTaxRateChangeRecv((SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV*)lpMsg,index);
					break;
				case 0x08:
					gCastleSiege->GDCastleSiegeMoneyOutRecv((SDHP_CASTLE_SIEGE_MONEY_OUT_RECV*)lpMsg,index);
					break;
				case 0x09:
					gCastleSiege->GDCastleSiegeDateChangeRecv((SDHP_CASTLE_SIEGE_DATE_CHANGE_RECV*)lpMsg);
					break;
				case 0x0A:
					gCastleSiege->GDCastleSiegeRegisterInfoRecv((SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV*)lpMsg,index);
					break;
				case 0x0B:
					gCastleSiege->GDCastleSiegeEndRecv((SDHP_CASTLE_SIEGE_END_RECV*)lpMsg);
					break;
				case 0x0C:
					gCastleSiege->GDCastleSiegeOwnerChangeRecv((SDHP_CASTLE_SIEGE_OWNER_CHANGE_RECV*)lpMsg);
					break;
				case 0x0D:
					gCastleSiege->GDCastleSiegeRegisterRecv((SDHP_CASTLE_SIEGE_REGISTER_RECV*)lpMsg,index);
					break;
				case 0x0E:
					gCastleSiege->GDCastleSiegeResetStateRecv((SDHP_CASTLE_SIEGE_RESET_STATE_RECV*)lpMsg, index);
					break;
				case 0x10:
					gCastleSiege->GDCastleSiegeRegisterMarkRecv((SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV*)lpMsg,index);
					break;
				case 0x12:
					gCastleSiege->GDCastleSiegeGiveUpGuildRecv((SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV*)lpMsg,index);
					break;
				case 0x16:
					gCastleSiege->GDCastleSiegeNpcRemoveRecv((SDHP_CASTLE_SIEGE_NPC_REMOVE_RECV*)lpMsg);
					break;
				case 0x17:
					gCastleSiege->GDCastleSiegeSyncStateRecv((SDHP_CASTLE_SIEGE_SYNC_STATE_RECV*)lpMsg);
					break;
				case 0x18:
					gCastleSiege->GDCastleSiegeAddTributeMoneyRecv((SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV*)lpMsg,index);
					break;
				case 0x19:
					gCastleSiege->GDCastleSiegeResetTaxInfoRecv((SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV*)lpMsg,index);
					break;
				case 0x1A:
					gCastleSiege->GDCastleSiegeParticipantClearRecv((SDHP_CASTLE_SIEGE_PARTICIPANT_CLEAR_RECV*)lpMsg);
					break;
				case 0x1B:
					gCastleSiege->GDCastleSiegeRegisteredClearRecv((SDHP_CASTLE_SIEGE_REGISTERED_CLEAR_RECV*)lpMsg);
					break;
			}
			break;
		case 0x81:
			gCastleSiege->GDCastleSiegeInitDataRecv((SDHP_CASTLE_SIEGE_INIT_DATA_RECV*)lpMsg,index);
			break;
		case 0x83:
			gCastleSiege->GDCastleSiegeRegisterListRecv((SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV*)lpMsg,index);
			break;
		case 0x84:
			gCastleSiege->GDCastleSiegeNpcCreateRecv((SDHP_CASTLE_SIEGE_NPC_CREATE_RECV*)lpMsg,index);
			break;
		case 0x85:
			gCastleSiege->GDCastleSiegeBuildGuildListRecv((SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV*)lpMsg,index);
			break;
		case 0x86:
			gCastleSiege->GDCastleSiegeLoadUnionListRecv((SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_RECV*)lpMsg,index);
			break;
		case 0x87:
			gCastleSiege->GDCastleSiegeSaveGuildListRecv((SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV*)lpMsg,index);
			break;
		case 0x88:
			gCastleSiege->GDCastleSiegeLoadGuildListRecv((SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV*)lpMsg,index);
			break;
		case 0x89:
			gCastleSiege->GDCastleSiegeNpcSaveRecv((SDHP_CASTLE_SIEGE_NPC_SAVE_RECV*)lpMsg);
			break;
		case 0x94:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					#if(DATASERVER_UPDATE<=603)
					gGoldenArcher->GDGoldenArcherCountRecv((SDHP_GOLDEN_ARCHER_COUNT_RECV*)lpMsg,index);
					#endif
					break;
				case 0x01:
					#if(DATASERVER_UPDATE<=603)
					gGoldenArcher->GDGoldenArcherRewardRecv((SDHP_GOLDEN_ARCHER_REWARD_RECV*)lpMsg,index);
					#endif
					break;
				case 0x30:
					#if(DATASERVER_UPDATE<=603)
					gGoldenArcher->GDGoldenArcherAddCountSaveRecv((SDHP_GOLDEN_ARCHER_ADD_COUNT_SAVE_RECV*)lpMsg);
					#endif
					break;
			}
			break;
		case 0xA0:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gGuild->GDGuildCreateRecv((SDHP_GUILD_CREATE_RECV*)lpMsg,index);
					break;
				case 0x01:
					gGuild->GDGuildDeleteRecv((SDHP_GUILD_DELETE_RECV*)lpMsg,index);
					break;
				case 0x02:
					gGuild->GDGuildMemberAddRecv((SDHP_GUILD_MEMBER_ADD_RECV*)lpMsg,index);
					break;
				case 0x03:
					gGuild->GDGuildMemberDelRecv((SDHP_GUILD_MEMBER_DEL_RECV*)lpMsg,index);
					break;
				case 0x04:
					gGuild->GDGuildAssignStatusRecv((SDHP_GUILD_ASSIGN_STATUS_RECV*)lpMsg);
					break;
				case 0x05:
					gGuild->GDGuildAssignTypeRecv((SDHP_GUILD_ASSIGN_TYPE_RECV*)lpMsg);
					break;
				case 0x06:
					gGuild->GDGuildNoticeRecv((SDHP_GUILD_NOTICE_RECV*)lpMsg);
					break;
				case 0x07:
					gGuild->GDGuildPostRecv((SDHP_GUILD_POST_RECV*)lpMsg);
					break;
				case 0x08:
					gGuild->GDGuildScoreRecv((SDHP_GUILD_SCORE_RECV*)lpMsg);
					break;
				case 0x09:
					gGuild->GDGuildUnionGetRecv((SDHP_GUILD_UNION_GET_RECV*)lpMsg,index);
					break;
				case 0x0A:
					gGuild->GDGuildUnionJoinRecv((SDHP_GUILD_UNION_JOIN_RECV*)lpMsg,index);
					break;
				case 0x0B:
					gGuild->GDGuildUnionBreakRecv((SDHP_GUILD_UNION_BREAK_RECV*)lpMsg,index);
					break;
				case 0x0C:
					gGuild->GDGuildAllianceKickRecv((SDHP_GUILD_ALLIANCE_KICK_RECV*)lpMsg,index);
					break;
			}
			break;
		case 0xB0:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gFriend->GDFriendListRecv((SDHP_FRIEND_LIST_RECV*)lpMsg,index);
					break;
				case 0x01:
					gFriend->GDFriendRequestRecv((SDHP_FRIEND_REQUEST_RECV*)lpMsg,index);
					break;
				case 0x02:
					gFriend->GDFriendResultRecv((SDHP_FRIEND_RESULT_RECV*)lpMsg,index);
					break;
				case 0x03:
					gFriend->GDFriendDeleteRecv((SDHP_FRIEND_DELETE_RECV*)lpMsg,index);
					break;
				case 0x04:
					gFriend->GDFriendMailRecv((SDHP_FRIEND_MAIL_RECV*)lpMsg,index);
					break;
				case 0x05:
					gFriend->GDFriendMailListRecv((SDHP_FRIEND_MAIL_LIST_RECV*)lpMsg,index);
					break;
				case 0x06:
					gFriend->GDFriendMailReadRecv((SDHP_FRIEND_MAIL_READ_RECV*)lpMsg,index);
					break;
				case 0x07:
					gFriend->GDFriendMailDeleteRecv((SDHP_FRIEND_MAIL_DELETE_RECV*)lpMsg,index);
					break;
				case 0x08:
					gFriend->GDFriendStatusRecv((SDHP_FRIEND_STATUS_RECV*)lpMsg,index);
					break;
			}
			break;
		case 0xC0:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gReconnect->GDReconnectInfoInsertRecv((SDHP_RECONNECT_INFO_INSERT_RECV*)lpMsg);
					break;
				case 0x01:
					gReconnect->GDReconnectInfoDeleteRecv((SDHP_RECONNECT_INFO_DELETE_RECV*)lpMsg);
					break;
			}
			break;
		case 0xF3:
			switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4])) // subcode
			{
			case 0xE7:
				GDAtualizarPatenteRecv((SDHP_UPDATE_PATENTE_RECV*)lpMsg, index);
				break;
			}
			break;
		case 0xF7:
			switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
			{
			case 0x04:
				GDCustomJewelBankRecv((SDHP_CUSTOM_JEWELBANK_RECV*)lpMsg);
				break;
			case 0x05:
				GDCustomJewelBankInfoRecv((SDHP_CUSTOM_JEWELBANK_INFO_RECV*)lpMsg, index);
				break;
			case 0x06:
				GDSetCoinRecv((SDHP_SETCOIN_RECV*)lpMsg);
				break;
			}
			break;
		case 0x68:
			GDDailyRewardInfoRecv(lpMsg, index);			
			break;
	}

	PROTECT_FINAL
}

void GDServerInfoRecv(SDHP_SERVER_INFO_RECV* lpMsg,int index) // OK
{
	if(gQueryManager->ExecQuery("SELECT 1 FROM GameServerInfo WHERE Number = 0") == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO GameServerInfo (Number,ItemCount,ZenCount,AceItemCount) VALUES (0,0,0,0)");
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
	}

	gServerManager[index].SetServerInfo(lpMsg->ServerName,lpMsg->ServerPort,lpMsg->ServerCode);

	gReconnect->ReconnectInfoListSend(lpMsg->ServerCode,index);
}

void GDCharacterListRecv(SDHP_CHARACTER_LIST_RECV* lpMsg,int index) // OK
{
	BYTE send[2048];

	SDHP_CHARACTER_LIST_SEND pMsg;

	pMsg.header.set(0x01,0);

	int size = sizeof(pMsg);

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	if(gQueryManager->ExecQuery("SELECT Id FROM AccountCharacter WHERE Id='%s'",lpMsg->account) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO AccountCharacter (Id) VALUES ('%s')",lpMsg->account);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
	}

	char CharacterName[5][11];

	memset(CharacterName,0,sizeof(CharacterName));

	gQueryManager->ExecQuery("SELECT * FROM AccountCharacter WHERE Id='%s'",lpMsg->account);

	gQueryManager->Fetch();

	pMsg.MoveCnt = (BYTE)gQueryManager->GetAsInteger("MoveCnt");

	pMsg.ExtClass = (BYTE)gQueryManager->GetAsInteger("ExtClass");

	#if(DATASERVER_UPDATE>=602)

	pMsg.ExtWarehouse = (BYTE)gQueryManager->GetAsInteger("ExtWarehouse");

	#endif

	gQueryManager->GetAsString("GameID1",CharacterName[0],sizeof(CharacterName[0]));

	gQueryManager->GetAsString("GameID2",CharacterName[1],sizeof(CharacterName[1]));

	gQueryManager->GetAsString("GameID3",CharacterName[2],sizeof(CharacterName[2]));

	gQueryManager->GetAsString("GameID4",CharacterName[3],sizeof(CharacterName[3]));

	gQueryManager->GetAsString("GameID5",CharacterName[4],sizeof(CharacterName[4]));

	gQueryManager->Close();

	pMsg.count = 0;

	SDHP_CHARACTER_LIST info;

	for(int n=0;n < 5;n++)
	{
		if(CharacterName[n][0] == 0)
		{
			continue;
		}

		if(gQueryManager->ExecQuery("SELECT cLevel,Class,Inventory,CtlCode FROM Character WHERE AccountID='%s' AND Name='%s'",lpMsg->account,CharacterName[n]) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
		{
			gQueryManager->Close();
		}
		else
		{
			info.slot = n;

			memcpy(info.name,CharacterName[n],sizeof(info.name));

			info.level = (WORD)gQueryManager->GetAsInteger("cLevel");

			info.Class = (BYTE)gQueryManager->GetAsInteger("Class");

			BYTE Inventory[12][16];

			gQueryManager->GetAsBinary("Inventory",Inventory[0],sizeof(Inventory));

			info.CtlCode = (BYTE)gQueryManager->GetAsInteger("CtlCode");

			gQueryManager->Close();

			memset(info.Inventory,0xFF,sizeof(info.Inventory));

			for(int i=0;i < 12;i++)
			{
				if(Inventory[i][0] == 0xFF && (Inventory[i][7] & 0x80) == 0x80 && (Inventory[i][9] & 0xF0) == 0xF0)
				{
					info.Inventory[0+(i*5)] = 0xFF;
					info.Inventory[1+(i*5)] = 0xFF;
					info.Inventory[2+(i*5)] = 0xFF;
					info.Inventory[3+(i*5)] = 0xFF;
					info.Inventory[4+(i*5)] = 0xFF;
				}
				else
				{
					info.Inventory[0+(i*5)] = Inventory[i][0];
					info.Inventory[1+(i*5)] = Inventory[i][1];
					info.Inventory[2+(i*5)] = Inventory[i][7];
					info.Inventory[3+(i*5)] = Inventory[i][8];
					info.Inventory[4+(i*5)] = Inventory[i][9];
				}
			}

			if(gQueryManager->ExecQuery("SELECT G_Status FROM GuildMember WHERE Name='%s'",CharacterName[n]) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
			{
				gQueryManager->Close();

				info.GuildStatus = 0xFF;
			}
			else
			{
				info.GuildStatus = (BYTE)gQueryManager->GetAsInteger("G_Status");

				gQueryManager->Close();
			}

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index,send,size);
}

void GDCharacterCreateRecv(SDHP_CHARACTER_CREATE_RECV* lpMsg,int index) // OK
{
	SDHP_CHARACTER_CREATE_SEND pMsg;

	pMsg.header.set(0x02,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	pMsg.slot = 0;

	pMsg.Class = lpMsg->Class;

	memset(pMsg.equipment, 0xFF, sizeof(pMsg.equipment));

	pMsg.level = 1;

	char CharacterName[5][11] = {0};

	if(gBadSyntax->CheckSyntax(lpMsg->name) == 0 || gQueryManager->ExecQuery("SELECT * FROM AccountCharacter WHERE Id='%s'",lpMsg->account) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.result = 0;
	}
	else
	{
		gQueryManager->GetAsString("GameID1",CharacterName[0],sizeof(CharacterName[0]));
		gQueryManager->GetAsString("GameID2",CharacterName[1],sizeof(CharacterName[1]));
		gQueryManager->GetAsString("GameID3",CharacterName[2],sizeof(CharacterName[2]));
		gQueryManager->GetAsString("GameID4",CharacterName[3],sizeof(CharacterName[3]));
		gQueryManager->GetAsString("GameID5",CharacterName[4],sizeof(CharacterName[4]));
		gQueryManager->Close();

		if(GetCharacterSlot(CharacterName,"",&pMsg.slot) == 0)
		{
			pMsg.result = 2;
		}
		else
		{
			if(gQueryManager->ExecQuery("EXEC WZ_CreateCharacter '%s','%s','%d'",lpMsg->account,lpMsg->name,lpMsg->Class) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
			{
				gQueryManager->Close();

				pMsg.result = 0;
			}
			else
			{
				pMsg.result = gQueryManager->GetResult(0);

				gQueryManager->Close();
			}

			if(pMsg.result == 1)
			{
				gQueryManager->ExecQuery("UPDATE AccountCharacter SET GameID%d='%s' WHERE Id='%s'",(pMsg.slot+1),lpMsg->name,lpMsg->account);
				gQueryManager->Close();
			}
		}
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
}

void GDCharacterDeleteRecv(SDHP_CHARACTER_DELETE_RECV* lpMsg,int index) // OK
{
	SDHP_CHARACTER_DELETE_SEND pMsg;

	pMsg.header.set(0x03,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	if(gQueryManager->ExecQuery("EXEC WZ_DeleteCharacter '%s','%s'",lpMsg->account,lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.result = 0;
	}
	else
	{
		pMsg.result = gQueryManager->GetResult(0);

		gQueryManager->Close();

		if(pMsg.result == 1)
		{
			char CharacterName[5][11] = {0};

			if(gQueryManager->ExecQuery("SELECT * FROM AccountCharacter WHERE Id='%s'",lpMsg->account) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
			{
				gQueryManager->Close();
			}
			else
			{
				gQueryManager->GetAsString("GameID1",CharacterName[0],sizeof(CharacterName[0]));
				gQueryManager->GetAsString("GameID2",CharacterName[1],sizeof(CharacterName[1]));
				gQueryManager->GetAsString("GameID3",CharacterName[2],sizeof(CharacterName[2]));
				gQueryManager->GetAsString("GameID4",CharacterName[3],sizeof(CharacterName[3]));
				gQueryManager->GetAsString("GameID5",CharacterName[4],sizeof(CharacterName[4]));
				gQueryManager->Close();

				BYTE slot;

				if(GetCharacterSlot(CharacterName,lpMsg->name,&slot) != 0)
				{
					gQueryManager->ExecQuery("UPDATE AccountCharacter SET GameID%d=NULL WHERE Id='%s'",(slot+1),lpMsg->account);
					gQueryManager->Close();
				}
			}
		}
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
}

void GDCharacterInfoRecv(SDHP_CHARACTER_INFO_RECV* lpMsg,int index) // OK
{
	SDHP_CHARACTER_INFO_SEND pMsg;

	pMsg.header.set(0x04,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	pMsg.result = ((CheckTextSyntax(lpMsg->name, sizeof(lpMsg->name)) == 0) ? 0 : 1);

	SDHP_DAILY_REWARD_INFO_SEND dailyreward = { 0 };

	dailyreward.header.set(0x68, sizeof(dailyreward));

	dailyreward.index = lpMsg->index;

	memcpy(dailyreward.name, lpMsg->name, sizeof(dailyreward.name));

	dailyreward.result = 0;

	for (int z = 0; z < 31; z++)
	{
		dailyreward.Status[z] = 0;
	}

	if(gQueryManager->ExecQuery("SELECT * FROM Character WHERE AccountID='%s' AND Name='%s'",lpMsg->account,lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.result = 0;
	}
	else
	{
		pMsg.Level = (WORD)gQueryManager->GetAsInteger("cLevel");

		pMsg.Class = (BYTE)gQueryManager->GetAsInteger("Class");

		pMsg.LevelUpPoint = gQueryManager->GetAsInteger("LevelUpPoint");

		pMsg.Experience = gQueryManager->GetAsInteger("Experience");

		pMsg.Strength = gQueryManager->GetAsInteger("Strength");

		pMsg.Dexterity = gQueryManager->GetAsInteger("Dexterity");

		pMsg.Vitality = gQueryManager->GetAsInteger("Vitality");

		pMsg.Energy = gQueryManager->GetAsInteger("Energy");

		pMsg.Leadership = gQueryManager->GetAsInteger("Leadership");

		pMsg.Patente = gQueryManager->GetAsInteger("Patente");

		gQueryManager->GetAsBinary("Inventory",pMsg.Inventory[0],sizeof(pMsg.Inventory));

		gQueryManager->GetAsBinary("MagicList",pMsg.Skill[0],sizeof(pMsg.Skill));

		pMsg.Money = gQueryManager->GetAsInteger("Money");

		pMsg.Life = (DWORD)gQueryManager->GetAsFloat("Life");

		pMsg.MaxLife = (DWORD)gQueryManager->GetAsFloat("MaxLife");

		pMsg.Mana = (DWORD)gQueryManager->GetAsFloat("Mana");

		pMsg.MaxMana = (DWORD)gQueryManager->GetAsFloat("MaxMana");

		pMsg.BP = (DWORD)gQueryManager->GetAsFloat("BP");

		pMsg.MaxBP = (DWORD)gQueryManager->GetAsFloat("MaxBP");

		#if(DATASERVER_UPDATE>=201)

		pMsg.Shield = (DWORD)gQueryManager->GetAsFloat("Shield");

		pMsg.MaxShield = (DWORD)gQueryManager->GetAsFloat("MaxShield");

		#endif

		pMsg.Map = (BYTE)gQueryManager->GetAsInteger("MapNumber");

		pMsg.X = (BYTE)gQueryManager->GetAsInteger("MapPosX");

		pMsg.Y = (BYTE)gQueryManager->GetAsInteger("MapPosY");

		pMsg.Dir = (BYTE)gQueryManager->GetAsInteger("MapDir");

		pMsg.PKCount = gQueryManager->GetAsInteger("PkCount");

		pMsg.PKLevel = gQueryManager->GetAsInteger("PkLevel");

		pMsg.PKTime = gQueryManager->GetAsInteger("PkTime");

		pMsg.CtlCode = (BYTE)gQueryManager->GetAsInteger("CtlCode");

		gQueryManager->GetAsBinary("Quest",pMsg.Quest,sizeof(pMsg.Quest));

		pMsg.ChatLimitTime = gQueryManager->GetAsInteger("ChatLimitTime");

		gQueryManager->GetAsBinary("EffectList",pMsg.Effect[0],sizeof(pMsg.Effect));

		pMsg.FruitAddPoint = (WORD)gQueryManager->GetAsInteger("FruitAddPoint");

		pMsg.FruitSubPoint = (WORD)gQueryManager->GetAsInteger("FruitSubPoint");

		#if(DATASERVER_UPDATE>=602)

		pMsg.ExtInventory = (BYTE)gQueryManager->GetAsInteger("ExtInventory");

		#endif

		pMsg.IsNewChar = gQueryManager->GetAsInteger("IsNewChar");

		pMsg.Married = gQueryManager->GetAsInteger("Married");

		gQueryManager->GetAsString("MarryCouple", pMsg.MarryName, sizeof(pMsg.MarryName));

		gQueryManager->Close();

		if (pMsg.IsNewChar != 0)
		{
			gQueryManager->ExecQuery("UPDATE Character SET IsNewChar='0' WHERE Name='%s'", lpMsg->name);

			gQueryManager->Close();
		}

		#if(DATASERVER_SHOP == 2)

		gQueryManager->ExecQuery("SELECT PcPoint FROM PcPointData WHERE AccountID='%s'",lpMsg->account);

		gQueryManager->Fetch();

		pMsg.Coin1 = gQueryManager->GetAsInteger("PcPoint");

		gQueryManager->Close();

		#endif

		gQueryManager->ExecQuery("EXEC WZ_GetResetInfo '%s','%s'",lpMsg->account,lpMsg->name);

		gQueryManager->Fetch();

		pMsg.Reset = gQueryManager->GetAsInteger("Reset");

		gQueryManager->Close();

		gQueryManager->ExecQuery("EXEC WZ_GetMasterResetInfo '%s','%s'",lpMsg->account,lpMsg->name);

		gQueryManager->Fetch();

		pMsg.MasterReset = gQueryManager->GetAsInteger("MasterReset");

		gQueryManager->Close();

		gQueryManager->ExecQuery("EXEC WZ_GetEventEntryInfo '%s','%s'",lpMsg->account,lpMsg->name);

		gQueryManager->Fetch();

		pMsg.BCCount = gQueryManager->GetAsInteger("BCCount");

		pMsg.CCCount = gQueryManager->GetAsInteger("CCCount");

		pMsg.DSCount = gQueryManager->GetAsInteger("DSCount");

		pMsg.DGCount = gQueryManager->GetAsInteger("DGCount");

		pMsg.ITCount = gQueryManager->GetAsInteger("ITCount");

		pMsg.IGCount = gQueryManager->GetAsInteger("IGCount");

		gQueryManager->Close();

		#if(DATASERVER_UPDATE>=801)

		GUILD_MATCHING_INFO GuildMatchingInfo;

		GUILD_MATCHING_JOIN_INFO GuildMatchingJoinInfo;

		GUILD_INFO* lpGuildInfo = gGuildManager->GetGuildMember(lpMsg->name);

		pMsg.UseGuildMatching = ((lpGuildInfo==0)?0:gGuildMatching->GetGuildMatchingInfo(&GuildMatchingInfo,lpGuildInfo->Name));

		pMsg.UseGuildMatchingJoin = ((lpGuildInfo!=0)?0:gGuildMatching->GetGuildMatchingJoinInfo(&GuildMatchingJoinInfo,lpMsg->name));

		#endif

		gQueryManager->ExecQuery("UPDATE AccountCharacter SET GameIDC='%s' WHERE Id='%s'",lpMsg->name,lpMsg->account);

		gQueryManager->Close();

		pMsg.result = 1;
	}

	if (gQueryManager->ExecQuery("SELECT * FROM DailyReward WHERE Acc='%s' AND month = MONTH(GETDATE())", lpMsg->account) == 0)
	{
		gQueryManager->Close();
	}
	else
	{
		while (gQueryManager->Fetch() != SQL_NO_DATA)
		{
			int day = (DWORD)gQueryManager->GetAsInteger("Date");
			if (day > 0 && day <= 31)
			{
				dailyreward.Status[day - 1] = 1;
			}
		}
		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index, (BYTE*)&dailyreward, sizeof(dailyreward));
}

void GDGetItemSerialRecv(SDHP_GET_ITEM_SERIAL_RECV* lpMsg,int aIndex) // OK
{
	SDHP_GET_ITEM_SERIAL_SEND pMsg;

	pMsg.header.set(0x06,sizeof(pMsg));

	pMsg.aIndex = lpMsg->aIndex;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	pMsg.index = lpMsg->index;

	pMsg.map = lpMsg->map;

	pMsg.x = lpMsg->x;

	pMsg.y = lpMsg->y;

	if(gQueryManager->ExecQuery("EXEC WZ_GetItemSerial") == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.Serial = 0;
	}
	else
	{
		pMsg.Serial = gQueryManager->GetResult(0);

		gQueryManager->Close();
	}

	gSocketManager->DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
}

void GDCreateItemRecv(SDHP_CREATE_ITEM_RECV* lpMsg,int index) // OK
{
	SDHP_CREATE_ITEM_SEND pMsg;

	pMsg.header.set(0x07,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	pMsg.X = lpMsg->X;

	pMsg.Y = lpMsg->Y;

	pMsg.Map = lpMsg->Map;

	if(gQueryManager->ExecQuery("EXEC WZ_GetItemSerial") == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.Serial = 0;
	}
	else
	{
		pMsg.Serial = gQueryManager->GetResult(0);

		gQueryManager->Close();
	}

	if(lpMsg->ItemIndex == 0x1A04 || lpMsg->ItemIndex == 0x1A05)
	{
		if(gQueryManager->ExecQuery("SELECT ItemSerial FROM T_PetItem_Info WHERE ItemSerial=%d",pMsg.Serial) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
		{
			gQueryManager->Close();
			gQueryManager->ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial,Pet_Level,Pet_Exp) VALUES (%d,%d,%d)",pMsg.Serial,1,0);
			gQueryManager->Close();
		}
		else
		{
			gQueryManager->Close();
		}
	}

	pMsg.ItemIndex = lpMsg->ItemIndex;

	pMsg.Level = lpMsg->Level;

	pMsg.Dur = lpMsg->Dur;

	pMsg.Option1 = lpMsg->Option1;

	pMsg.Option2 = lpMsg->Option2;

	pMsg.Option3 = lpMsg->Option3;

	pMsg.NewOption = lpMsg->NewOption;

	pMsg.LootIndex = lpMsg->LootIndex;

	pMsg.SetOption = lpMsg->SetOption;

	pMsg.JewelOfHarmonyOption = lpMsg->JewelOfHarmonyOption;

	pMsg.ItemOptionEx = lpMsg->ItemOptionEx;

	memcpy(pMsg.SocketOption,lpMsg->SocketOption,sizeof(pMsg.SocketOption));

	pMsg.SocketOptionBonus = lpMsg->SocketOptionBonus;

	pMsg.Duration = lpMsg->Duration;

	gSocketManager->DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
}

void GDOptionDataRecv(SDHP_OPTION_DATA_RECV* lpMsg,int index) // OK
{
	SDHP_OPTION_DATA_SEND pMsg;

	pMsg.header.set(0x08,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	if(gQueryManager->ExecQuery("SELECT * FROM OptionData WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		memset(pMsg.SkillKey,0xFF,sizeof(pMsg.SkillKey));

		pMsg.GameOption = 0xFF;
		pMsg.QKey = 0xFF;
		pMsg.WKey = 0xFF;
		pMsg.EKey = 0xFF;
		#if(DATASERVER_UPDATE>=401)
		pMsg.ChatWindow = 0xFF;
		#else
		pMsg.ChatWindow = 0x06;
		#endif
		#if(DATASERVER_UPDATE>=401)
		pMsg.RKey = 0xFF;
		pMsg.QWERLevel = 0xFFFFFFFF;
		#endif
		#if(DATASERVER_UPDATE>=701)
		pMsg.ChangeSkin = 0;
		#endif
	}
	else
	{
		gQueryManager->GetAsBinary("SkillKey",pMsg.SkillKey,sizeof(pMsg.SkillKey));

		pMsg.GameOption = (BYTE)gQueryManager->GetAsInteger("GameOption");
		pMsg.QKey = (BYTE)gQueryManager->GetAsInteger("Qkey");
		pMsg.WKey = (BYTE)gQueryManager->GetAsInteger("Wkey");
		pMsg.EKey = (BYTE)gQueryManager->GetAsInteger("Ekey");
		pMsg.ChatWindow = (BYTE)gQueryManager->GetAsInteger("ChatWindow");
		#if(DATASERVER_UPDATE>=401)
		pMsg.RKey = (BYTE)gQueryManager->GetAsInteger("Rkey");
		pMsg.QWERLevel = gQueryManager->GetAsInteger("QWERLevel");
		#endif
		#if(DATASERVER_UPDATE>=701)
		pMsg.ChangeSkin = (BYTE)gQueryManager->GetAsInteger("ChangeSkin");
		#endif

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
}

void GDPetItemInfoRecv(SDHP_PET_ITEM_INFO_RECV* lpMsg,int index) // OK
{
	BYTE send[4096];

	SDHP_PET_ITEM_INFO_SEND pMsg;

	pMsg.header.set(0x09,0);

	int size = sizeof(pMsg);

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	pMsg.type = lpMsg->type;

	pMsg.count = 0;

	SDHP_PET_ITEM_INFO2 info;

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_PET_ITEM_INFO1* lpInfo = (SDHP_PET_ITEM_INFO1*)(((BYTE*)lpMsg)+sizeof(SDHP_PET_ITEM_INFO_RECV)+(sizeof(SDHP_PET_ITEM_INFO1)*n));

		if(gQueryManager->ExecQuery("SELECT Pet_Level,Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%d",lpInfo->serial) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
		{
			gQueryManager->Close();
			gQueryManager->ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial,Pet_Level,Pet_Exp) VALUES (%d,%d,%d)",lpInfo->serial,1,0);
			gQueryManager->Close();

			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.level = 1;
			info.experience = 0;
		}
		else
		{
			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.level = gQueryManager->GetAsInteger("Pet_Level");
			info.experience = gQueryManager->GetAsInteger("Pet_Exp");

			gQueryManager->Close();
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gSocketManager->DataSend(index,send,size);
}

void GDCharacterNameCheckRecv(SDHP_CHARACTER_NAME_CHECK_RECV* lpMsg,int index) // OK
{
	#if(DATASERVER_UPDATE>=401)

	SDHP_CHARACTER_NAME_CHECK_SEND pMsg;

	pMsg.header.set(0x0A,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	pMsg.result = 0;

	gSocketManager->DataSend(index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void GDAcheronGuardianSyncRecv(SDHP_ACHERON_GUARDIAN_SYNC_RECV* lpMsg) // OK
{
	#if(DATASERVER_UPDATE>=801)

	SDHP_ACHERON_GUARDIAN_SYNC_SEND pMsg;

	pMsg.header.set(0x14,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.AcheronState = lpMsg->AcheronState;

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void GDArcaBattleSyncRecv(SDHP_ARCA_BATTLE_SYNC_RECV* lpMsg) // OK
{
	#if(DATASERVER_UPDATE>=702)

	SDHP_ARCA_BATTLE_SYNC_SEND pMsg;

	pMsg.header.set(0x15,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.ArcaState = lpMsg->ArcaState;

	pMsg.MinGuildMember = lpMsg->MinGuildMember;

	pMsg.MaxGuildMember = lpMsg->MaxGuildMember;

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void GDCrywolfSyncRecv(SDHP_CRYWOLF_SYNC_RECV* lpMsg) // OK
{
	#if(DATASERVER_UPDATE>=201)

	SDHP_CRYWOLF_SYNC_SEND pMsg;

	pMsg.header.set(0x1E,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.CrywolfState = lpMsg->CrywolfState;

	pMsg.OccupationState = lpMsg->OccupationState;

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void GDCrywolfInfoRecv(SDHP_CRYWOLF_INFO_RECV* lpMsg,int index) // OK
{
	#if(DATASERVER_UPDATE>=201)

	SDHP_CRYWOLF_INFO_SEND pMsg;

	pMsg.header.set(0x1F,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	if(gQueryManager->ExecQuery("EXEC WZ_CW_InfoLoad '%d'",lpMsg->MapServerGroup) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.CrywolfState = 0;

		pMsg.OccupationState = 0;
	}
	else
	{
		pMsg.CrywolfState = gQueryManager->GetAsInteger("CRYWOLF_STATE");

		pMsg.OccupationState = gQueryManager->GetAsInteger("CRYWOLF_OCCUFY");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));

	#endif
}

void GDGlobalPostRecv(SDHP_GLOBAL_POST_RECV* lpMsg,int index) // OK
{
	SDHP_GLOBAL_POST_SEND pMsg;

	pMsg.header.set(0x20,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.type = lpMsg->type;

	pMsg.code = lpMsg->code;

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	memcpy(pMsg.message,lpMsg->message,sizeof(pMsg.message));

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void GDGlobalNoticeRecv(SDHP_GLOBAL_NOTICE_RECV* lpMsg,int index) // OK
{
	SDHP_GLOBAL_NOTICE_SEND pMsg;

	pMsg.header.set(0x21,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.type = lpMsg->type;

	pMsg.count = lpMsg->count;

	pMsg.opacity = lpMsg->opacity;

	pMsg.delay = lpMsg->delay;

	pMsg.color = lpMsg->color;

	pMsg.speed = lpMsg->speed;

	memcpy(pMsg.message,lpMsg->message,sizeof(pMsg.message));

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void GDSNSDataRecv(SDHP_SNS_DATA_RECV* lpMsg,int index) // OK
{
	#if(DATASERVER_UPDATE>=801)

	SDHP_SNS_DATA_SEND pMsg;

	pMsg.header.set(0x24,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	if(gQueryManager->ExecQuery("SELECT Data FROM SNSData WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.result = 1;

		memset(pMsg.data,0xFF,sizeof(pMsg.data));
	}
	else
	{
		pMsg.result = 0;

		gQueryManager->GetAsBinary("Data",pMsg.data,sizeof(pMsg.data));

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));

	#endif
}

void GDGlobalMessageRecv(SDHP_GLOBAL_MESSAGE_RECV* lpMsg,int index) // OK
{
	SDHP_GLOBAL_MESSAGE_SEND pMsg;

	pMsg.header.set(0x2B,sizeof(pMsg));

	pMsg.MapServerGroup = lpMsg->MapServerGroup;

	pMsg.type = lpMsg->type;

	pMsg.color = lpMsg->color;

	memcpy(pMsg.message,lpMsg->message,sizeof(pMsg.message));

	for(int n=0;n < MAX_SERVER;n++)
	{
		if(gServerManager[n].CheckState() != SERVER_OFFLINE)
		{
			gSocketManager->DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void GDCustomMonsterKilLCountRecv(SDHP_MONSTER_KILL_COUNT_RECV* lpMsg, int index) // OK
{
	SDHP_MONSTER_KILL_COUNT_SEND pMsg;

	pMsg.header.set(0x50,sizeof(pMsg));

	pMsg.Index = lpMsg->Index;

	memcpy(pMsg.Account, lpMsg->Account, sizeof(pMsg.Account));

	memcpy(pMsg.Name, lpMsg->Name, sizeof(pMsg.Name));

	pMsg.MonsterClass = lpMsg->MonsterClass;

	if (gQueryManager->ExecQuery("SELECT * FROM MonsterKillCount WHERE Name='%s' and MonsterClass='%d'", lpMsg->Name, lpMsg->MonsterClass) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		pMsg.Count = 1;

		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO MonsterKillCount (Name,MonsterClass,MonsterKillCount) VALUES ('%s',%d,%d)", lpMsg->Name, lpMsg->MonsterClass, pMsg.Count);
		gQueryManager->Close();
	}
	else
	{
		pMsg.Count = (gQueryManager->GetAsInteger("MonsterKillCount") + 1);

		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE MonsterKillCount SET MonsterKillCount=%d WHERE Name='%s' and MonsterClass='%d'", pMsg.Count, lpMsg->Name, lpMsg->MonsterClass);
		gQueryManager->Close();
	}

	gSocketManager->DataSend(index, (BYTE*)&pMsg, pMsg.header.size);
}

void GDCharacterInfoSaveRecv(SDHP_CHARACTER_INFO_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->BindParameterAsBinary(1,lpMsg->Inventory[0],sizeof(lpMsg->Inventory));
	gQueryManager->BindParameterAsBinary(2,lpMsg->Skill[0],sizeof(lpMsg->Skill));
	gQueryManager->BindParameterAsBinary(3,lpMsg->Quest,sizeof(lpMsg->Quest));
	gQueryManager->BindParameterAsBinary(4,lpMsg->Effect[0],sizeof(lpMsg->Effect));
	#if(DATASERVER_UPDATE>=602)
	gQueryManager->ExecQuery("UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d,Experience=%d,Strength=%d,Dexterity=%d,Vitality=%d,Energy=%d,Leadership=%d,Inventory=?,MagicList=?,Money=%d,Life=%f,MaxLife=%f,Mana=%f,MaxMana=%f,BP=%f,MaxBP=%f,Shield=%f,MaxShield=%f,MapNumber=%d,MapPosX=%d,MapPosY=%d,MapDir=%d,PkCount=%d,PkLevel=%d,PkTime=%d,Quest=?,ChatLimitTime=%d,EffectList=?,FruitAddPoint=%d,FruitSubPoint=%d,ExtInventory=%d,Married=%d,MarryCouple='%s' WHERE AccountID='%s' AND Name='%s'", lpMsg->Level, lpMsg->Class, lpMsg->LevelUpPoint, lpMsg->Experience, lpMsg->Strength, lpMsg->Dexterity, lpMsg->Vitality, lpMsg->Energy, lpMsg->Leadership, lpMsg->Money, (float)lpMsg->Life, (float)lpMsg->MaxLife, (float)lpMsg->Mana, (float)lpMsg->MaxMana, (float)lpMsg->BP, (float)lpMsg->MaxBP, (float)lpMsg->Shield, (float)lpMsg->MaxShield, lpMsg->Map, lpMsg->X, lpMsg->Y, lpMsg->Dir, lpMsg->PKCount, lpMsg->PKLevel, lpMsg->PKTime, lpMsg->ChatLimitTime, lpMsg->FruitAddPoint, lpMsg->FruitSubPoint, lpMsg->ExtInventory, lpMsg->Married, lpMsg->MarryName, lpMsg->account, lpMsg->name);
	#elif(DATASERVER_UPDATE>=201)
	gQueryManager->ExecQuery("UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d,Experience=%d,Strength=%d,Dexterity=%d,Vitality=%d,Energy=%d,Leadership=%d,Inventory=?,MagicList=?,Money=%d,Life=%f,MaxLife=%f,Mana=%f,MaxMana=%f,BP=%f,MaxBP=%f,Shield=%f,MaxShield=%f,MapNumber=%d,MapPosX=%d,MapPosY=%d,MapDir=%d,PkCount=%d,PkLevel=%d,PkTime=%d,Quest=?,ChatLimitTime=%d,EffectList=?,FruitAddPoint=%d,FruitSubPoint=%d,Married=%d,MarryCouple='%s' WHERE AccountID='%s' AND Name='%s'",lpMsg->Level,lpMsg->Class,lpMsg->LevelUpPoint,lpMsg->Experience,lpMsg->Strength,lpMsg->Dexterity,lpMsg->Vitality,lpMsg->Energy,lpMsg->Leadership,lpMsg->Money,(float)lpMsg->Life,(float)lpMsg->MaxLife,(float)lpMsg->Mana,(float)lpMsg->MaxMana,(float)lpMsg->BP,(float)lpMsg->MaxBP,(float)lpMsg->Shield,(float)lpMsg->MaxShield,lpMsg->Map,lpMsg->X,lpMsg->Y,lpMsg->Dir,lpMsg->PKCount,lpMsg->PKLevel,lpMsg->PKTime, lpMsg->ChatLimitTime,lpMsg->FruitAddPoint,lpMsg->FruitSubPoint,lpMsg->Married,lpMsg->MarryName,lpMsg->account,lpMsg->name);
	#else
	gQueryManager->ExecQuery("UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d,Experience=%d,Strength=%d,Dexterity=%d,Vitality=%d,Energy=%d,Leadership=%d,Inventory=?,MagicList=?,Money=%d,Life=%f,MaxLife=%f,Mana=%f,MaxMana=%f,BP=%f,MaxBP=%f,MapNumber=%d,MapPosX=%d,MapPosY=%d,MapDir=%d,PkCount=%d,PkLevel=%d,PkTime=%d,Quest=?,ChatLimitTime=%d,EffectList=?,FruitAddPoint=%d,FruitSubPoint=%d,Married=%d,MarryCouple='%s' WHERE AccountID='%s' AND Name='%s'",lpMsg->Level,lpMsg->Class,lpMsg->LevelUpPoint,lpMsg->Experience,lpMsg->Strength,lpMsg->Dexterity,lpMsg->Vitality,lpMsg->Energy,lpMsg->Leadership,lpMsg->Money,(float)lpMsg->Life,(float)lpMsg->MaxLife,(float)lpMsg->Mana,(float)lpMsg->MaxMana,(float)lpMsg->BP,(float)lpMsg->MaxBP,lpMsg->Map,lpMsg->X,lpMsg->Y,lpMsg->Dir,lpMsg->PKCount,lpMsg->PKLevel,lpMsg->PKTime,lpMsg->ChatLimitTime,lpMsg->FruitAddPoint,lpMsg->FruitSubPoint,lpMsg->Married,lpMsg->MarryName,lpMsg->account,lpMsg->name);
	#endif
	gQueryManager->Close();

	#if(DATASERVER_UPDATE>=602)
	gQueryManager->ExecQuery("UPDATE AccountCharacter SET ExtWarehouse=%d WHERE Id='%s'",lpMsg->ExtWarehouse,lpMsg->account);
	gQueryManager->Close();
	#endif

	gQueryManager->ExecQuery("EXEC WZ_SetEventEntryInfo '%s','%s','%d','%d','%d','%d','%d','%d'",lpMsg->account,lpMsg->name,lpMsg->BCCount,lpMsg->CCCount,lpMsg->DSCount,lpMsg->DGCount,lpMsg->ITCount,lpMsg->IGCount);
	gQueryManager->Fetch();
	gQueryManager->Close();
}

void GDInventoryItemSaveRecv(SDHP_INVENTORY_ITEM_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->BindParameterAsBinary(1,lpMsg->Inventory[0],sizeof(lpMsg->Inventory));
	gQueryManager->ExecQuery("UPDATE Character SET Inventory=? WHERE AccountID='%s' AND Name='%s'",lpMsg->account,lpMsg->name);
	gQueryManager->Close();
}

void GDOptionDataSaveRecv(SDHP_OPTION_DATA_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager->ExecQuery("SELECT Name FROM OptionData WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->BindParameterAsBinary(1, lpMsg->SkillKey, sizeof(lpMsg->SkillKey));

		#if(DATASERVER_UPDATE>=701)
		gQueryManager->ExecQuery("INSERT INTO OptionData (Name,SkillKey,GameOption,Qkey,Wkey,Ekey,ChatWindow,Rkey,QWERLevel,ChangeSkin) VALUES ('%s',?,%d,%d,%d,%d,%d,%d,%d,%d)",lpMsg->name,lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->RKey,lpMsg->QWERLevel,lpMsg->ChangeSkin);
		#elif(DATASERVER_UPDATE>=401)
		gQueryManager->ExecQuery("INSERT INTO OptionData (Name,SkillKey,GameOption,Qkey,Wkey,Ekey,ChatWindow,Rkey,QWERLevel) VALUES ('%s',?,%d,%d,%d,%d,%d,%d,%d)",lpMsg->name,lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->RKey,lpMsg->QWERLevel);
		#else
		gQueryManager->ExecQuery("INSERT INTO OptionData (Name,SkillKey,GameOption,Qkey,Wkey,Ekey,ChatWindow) VALUES ('%s',?,%d,%d,%d,%d,%d)",lpMsg->name,lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow);
		#endif

		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->BindParameterAsBinary(1,lpMsg->SkillKey,sizeof(lpMsg->SkillKey));

		#if(DATASERVER_UPDATE>=701)
		gQueryManager->ExecQuery("UPDATE OptionData SET SkillKey=?,GameOption=%d,Qkey=%d,Wkey=%d,Ekey=%d,ChatWindow=%d,Rkey=%d,QWERLevel=%d,ChangeSkin=%d WHERE Name='%s'",lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->RKey,lpMsg->QWERLevel,lpMsg->ChangeSkin,lpMsg->name);
		#elif(DATASERVER_UPDATE>=401)
		gQueryManager->ExecQuery("UPDATE OptionData SET SkillKey=?,GameOption=%d,Qkey=%d,Wkey=%d,Ekey=%d,ChatWindow=%d,Rkey=%d,QWERLevel=%d WHERE Name='%s'",lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->RKey,lpMsg->QWERLevel,lpMsg->name);
		#else
		gQueryManager->ExecQuery("UPDATE OptionData SET SkillKey=?,GameOption=%d,Qkey=%d,Wkey=%d,Ekey=%d,ChatWindow=%d WHERE Name='%s'",lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->name);
		#endif

		gQueryManager->Close();
	}
}

void GDPetItemInfoSaveRecv(SDHP_PET_ITEM_INFO_SAVE_RECV* lpMsg) // OK
{
	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_PET_ITEM_INFO_SAVE* lpInfo = (SDHP_PET_ITEM_INFO_SAVE*)(((BYTE*)lpMsg)+sizeof(SDHP_PET_ITEM_INFO_SAVE_RECV)+(sizeof(SDHP_PET_ITEM_INFO_SAVE)*n));

		if(gQueryManager->ExecQuery("SELECT ItemSerial FROM T_PetItem_Info WHERE ItemSerial=%d",lpInfo->serial) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
		{
			gQueryManager->Close();
			gQueryManager->ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial,Pet_Level,Pet_Exp) VALUES (%d,%d,%d)",lpInfo->serial,lpInfo->level,lpInfo->experience);
			gQueryManager->Close();
		}
		else
		{
			gQueryManager->Close();
			gQueryManager->ExecQuery("UPDATE T_PetItem_Info SET Pet_Level=%d,Pet_Exp=%d WHERE ItemSerial=%d",lpInfo->level,lpInfo->experience,lpInfo->serial);
			gQueryManager->Close();
		}
	}
}

void GDResetInfoSaveRecv(SDHP_RESET_INFO_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_SetResetInfo '%s','%s','%d','%d','%d','%d'",lpMsg->account,lpMsg->name,lpMsg->Reset,lpMsg->ResetDay,lpMsg->ResetWek,lpMsg->ResetMon);
	gQueryManager->Fetch();
	gQueryManager->Close();
}

void GDMasterResetInfoSaveRecv(SDHP_MASTER_RESET_INFO_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("EXEC WZ_SetMasterResetInfo '%s','%s','%d','%d','%d','%d','%d'",lpMsg->account,lpMsg->name,lpMsg->Reset,lpMsg->MasterReset,lpMsg->MasterResetDay,lpMsg->MasterResetWek,lpMsg->MasterResetMon);
	gQueryManager->Fetch();
	gQueryManager->Close();
}

void GDRankingDuelSaveRecv(SDHP_RANKING_DUEL_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager->ExecQuery("SELECT Name FROM RankingDuel WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO RankingDuel (Name,WinScore,LoseScore) VALUES ('%s',%d,%d)",lpMsg->name,lpMsg->WinScore,lpMsg->LoseScore);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE RankingDuel SET WinScore=WinScore+%d,LoseScore=LoseScore+%d WHERE Name='%s'",lpMsg->WinScore,lpMsg->LoseScore,lpMsg->name);
		gQueryManager->Close();
	}
}

void GDRankingBloodCastleSaveRecv(SDHP_RANKING_BLOOD_CASTLE_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager->ExecQuery("SELECT Name FROM RankingBloodCastle WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO RankingBloodCastle (Name,Score) VALUES ('%s',%d)",lpMsg->name,lpMsg->score);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE RankingBloodCastle SET Score=Score+%d WHERE Name='%s'",lpMsg->score,lpMsg->name);
		gQueryManager->Close();
	}
}

void GDRankingChaosCastleSaveRecv(SDHP_RANKING_CHAOS_CASTLE_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager->ExecQuery("SELECT Name FROM RankingChaosCastle WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO RankingChaosCastle (Name,Score) VALUES ('%s',%d)",lpMsg->name,lpMsg->score);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE RankingChaosCastle SET Score=Score+%d WHERE Name='%s'",lpMsg->score,lpMsg->name);
		gQueryManager->Close();
	}
}

void GDRankingDevilSquareSaveRecv(SDHP_RANKING_DEVIL_SQUARE_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager->ExecQuery("SELECT Name FROM RankingDevilSquare WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO RankingDevilSquare (Name,Score) VALUES ('%s',%d)",lpMsg->name,lpMsg->score);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE RankingDevilSquare SET Score=Score+%d WHERE Name='%s'",lpMsg->score,lpMsg->name);
		gQueryManager->Close();
	}
}

void GDRankingIllusionTempleSaveRecv(SDHP_RANKING_ILLUSION_TEMPLE_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager->ExecQuery("SELECT Name FROM RankingIllusionTemple WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO RankingIllusionTemple (Name,Score) VALUES ('%s',%d)",lpMsg->name,lpMsg->score);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE RankingIllusionTemple SET Score=Score+%d WHERE Name='%s'",lpMsg->score,lpMsg->name);
		gQueryManager->Close();
	}
}

void GDCreationCardSaveRecv(SDHP_CREATION_CARD_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("UPDATE AccountCharacter SET ExtClass=%d WHERE Id='%s'",lpMsg->ExtClass,lpMsg->account);
	gQueryManager->Close();
}

void GDCrywolfInfoSaveRecv(SDHP_CRYWOLF_INFO_SAVE_RECV* lpMsg) // OK
{
	#if(DATASERVER_UPDATE>=201)

	gQueryManager->ExecQuery("EXEC WZ_CW_InfoSave '%d','%d','%d'",lpMsg->MapServerGroup,lpMsg->CrywolfState,lpMsg->OccupationState);
	gQueryManager->Fetch();
	gQueryManager->Close();

	#endif
}

void GDSNSDataSaveRecv(SDHP_SNS_DATA_SAVE_RECV* lpMsg) // OK
{
	#if(DATASERVER_UPDATE>=801)

	if(gQueryManager->ExecQuery("SELECT Name FROM SNSData WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->BindParameterAsBinary(1,lpMsg->data,sizeof(lpMsg->data));
		gQueryManager->ExecQuery("INSERT INTO SNSData (Name,Data) VALUES ('%s',?)",lpMsg->name);
		gQueryManager->Close();
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->BindParameterAsBinary(1,lpMsg->data,sizeof(lpMsg->data));
		gQueryManager->ExecQuery("UPDATE SNSData SET Data=? WHERE Name='%s'",lpMsg->name);
		gQueryManager->Close();
	}

	#endif
}

void GDConnectCharacterRecv(SDHP_CONNECT_CHARACTER_RECV* lpMsg,int index) // OK
{
	CHARACTER_INFO CharacterInfo;

	if(gCharacterManager->GetCharacterInfo(&CharacterInfo,lpMsg->name) != 0)
	{
		return;
	}

	strcpy_s(CharacterInfo.Name,lpMsg->name);

	strcpy_s(CharacterInfo.Account,lpMsg->account);

	CharacterInfo.UserIndex = lpMsg->index;

	CharacterInfo.GameServerCode = gServerManager[index].m_ServerCode;

	gCharacterManager->InsertCharacterInfo(CharacterInfo);

	gFriend->DGFriendStateSend(lpMsg->name,0);

	gGuild->MemberOnlineOffline(lpMsg->name,CharacterInfo.GameServerCode);
}

void GDDisconnectCharacterRecv(SDHP_DISCONNECT_CHARACTER_RECV* lpMsg,int index) // OK
{
	CHARACTER_INFO CharacterInfo;

	if(gCharacterManager->GetCharacterInfo(&CharacterInfo,lpMsg->name) == 0)
	{
		return;
	}

	if(CharacterInfo.UserIndex != lpMsg->index)
	{
		return;
	}

	if(CharacterInfo.GameServerCode != gServerManager[index].m_ServerCode)
	{
		return;
	}

	gCharacterManager->RemoveCharacterInfo(CharacterInfo);

	gFriend->DGFriendStateSend(lpMsg->name,1);

	gGuild->MemberOnlineOffline(lpMsg->name,-1);
}

void GDGlobalWhisperRecv(SDHP_GLOBAL_WHISPER_RECV* lpMsg,int index) // OK
{
	SDHP_GLOBAL_WHISPER_SEND pMsg;

	pMsg.header.set(0x72,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	CHARACTER_INFO CharacterInfo;

	if(gCharacterManager->GetCharacterInfo(&CharacterInfo,lpMsg->TargetName) == 0)
	{
		pMsg.result = 0;
	}
	else
	{
		pMsg.result = 1;
		DGGlobalWhisperEchoSend(CharacterInfo.GameServerCode,CharacterInfo.UserIndex,CharacterInfo.Account,CharacterInfo.Name,lpMsg->name,lpMsg->message);
	}

	memcpy(pMsg.TargetName,lpMsg->TargetName,sizeof(pMsg.TargetName));

	memcpy(pMsg.message,lpMsg->message,sizeof(pMsg.message));

	gSocketManager->DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
}

void DGGlobalWhisperEchoSend(WORD ServerCode,WORD index,char* account,char* name,char* SourceName,char* message) // OK
{
	SDHP_GLOBAL_WHISPER_ECHO_SEND pMsg;

	pMsg.header.set(0x73,sizeof(pMsg));

	pMsg.index = index;

	memcpy(pMsg.account,account,sizeof(pMsg.account));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.SourceName,SourceName,sizeof(pMsg.SourceName));

	memcpy(pMsg.message,message,sizeof(pMsg.message));

	CServerManager* lpServerManager = FindServerByCode(ServerCode);

	if(lpServerManager != 0)
	{
		gSocketManager->DataSend(lpServerManager->m_index,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void GDCustomJewelBankRecv(SDHP_CUSTOM_JEWELBANK_RECV* lpMsg) // OK
{
	int Chaos = 0;
	int Life = 0;
	int Soul = 0;
	int Bless = 0;
	int Creation = 0;
	int Guardian = 0;
	int Harmony = 0;
	int GemStone = 0;
	int LowStone = 0;
	int HighStone = 0;

	switch (lpMsg->type)
	{
	case 0:
		Chaos = lpMsg->count;
		break;
	case 1:
		Life = lpMsg->count;
		break;
	case 2:
		Soul = lpMsg->count;
		break;
	case 3:
		Bless = lpMsg->count;
		break;
	case 4:
		Creation = lpMsg->count;
		break;
	case 5:
		Guardian = lpMsg->count;
		break;
	case 6:
		Harmony = lpMsg->count;
		break;
	case 7:
		GemStone = lpMsg->count;
		break;
	case 8:
		LowStone = lpMsg->count;
		break;
	case 9:
		HighStone = lpMsg->count;
		break;
	}

	if (gQueryManager->ExecQuery("SELECT * FROM CustomJewelBank WHERE AccountID='%s'", lpMsg->account) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO CustomJewelBank (AccountID,Chaos,Life,Soul,Bless,Creation,Guardian,Harmony,GemStone,LowStone,HighStone) VALUES ('%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", lpMsg->account, Chaos, Life, Soul, Bless, Creation, Guardian, Harmony, GemStone, LowStone, HighStone);
		gQueryManager->Close();

	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE CustomJewelBank SET Chaos+=%d,Life+=%d,Soul+=%d,Bless+=%d,Creation+=%d,Guardian+=%d,Harmony+=%d,GemStone+=%d,LowStone+=%d,HighStone+=%d WHERE AccountID = '%s'", Chaos, Life, Soul, Bless, Creation, Guardian, Harmony, GemStone, LowStone, HighStone, lpMsg->account);
		gQueryManager->Close();
	}

}

void GDCustomJewelBankInfoRecv(SDHP_CUSTOM_JEWELBANK_INFO_RECV* lpMsg, int index)
{
	SDHP_CUSTOM_JEWELBANK_INFO_SEND pMsg = { 0 };

	pMsg.header.set(0xF7, 0x05, sizeof(pMsg));

	pMsg.index = lpMsg->index;

	if (gQueryManager->ExecQuery("SELECT * FROM CustomJewelBank WHERE AccountID='%s'", lpMsg->account) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		pMsg.Chaos = 0;
		pMsg.Life = 0;
		pMsg.Soul = 0;
		pMsg.Bless = 0;
		pMsg.Creation = 0;
		pMsg.Guardian = 0;
		pMsg.Harmony = 0;
		pMsg.GemStone = 0;
		pMsg.LowStone = 0;
		pMsg.HighStone = 0;
	}
	else
	{
		pMsg.Chaos = gQueryManager->GetAsInteger("Chaos"); ;
		pMsg.Life = gQueryManager->GetAsInteger("Life");
		pMsg.Soul = gQueryManager->GetAsInteger("Soul");
		pMsg.Bless = gQueryManager->GetAsInteger("Bless");
		pMsg.Creation = gQueryManager->GetAsInteger("Creation");
		pMsg.Guardian = gQueryManager->GetAsInteger("Guardian");
		pMsg.Harmony = gQueryManager->GetAsInteger("Harmony");
		pMsg.GemStone = gQueryManager->GetAsInteger("GemStone");
		pMsg.LowStone = gQueryManager->GetAsInteger("LowStone");
		pMsg.HighStone = gQueryManager->GetAsInteger("HighStone");

		gQueryManager->Close();
	}

	gSocketManager->DataSend(index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GDRankingUpdateSaveRecv(SDHP_RANKING_KILL_HERO_SAVE_RECV* lpMsg) // OK
{
	gQueryManager->ExecQuery("UPDATE Character SET %s = %s + %d WHERE Name='%s'", lpMsg->coluna, lpMsg->coluna,lpMsg->score, lpMsg->name);
	gQueryManager->Close();
}

void GDDailyRewardInfoRecv(BYTE* lpMsg, int index)
{
	SDHP_DAILY_REWARD_INFO_SEND* dailyreward = (SDHP_DAILY_REWARD_INFO_SEND*)lpMsg;

	for (int i = 0; i < 31; i++)
	{
		if (dailyreward->Status[i])
		{
			gQueryManager->ExecQuery("SELECT * FROM DailyReward WHERE acc='%s' AND date = %d AND month = MONTH(GETDATE())", dailyreward->acc, i + 1);

			// Verifica se há dados disponíveis
			if (gQueryManager->Fetch() == SQL_NO_DATA)
			{
				gQueryManager->Close(); // Fecha antes de executar uma nova consulta
				gQueryManager->ExecQuery("INSERT INTO DailyReward (acc, name, date) VALUES ('%s','%s', %d)", dailyreward->acc, dailyreward->name, i + 1);
			}

			gQueryManager->Close(); // Garante que o cursor seja fechado corretamente
		}
	}
}

void AtualizarPatenteComPrioridade(const char* nome, int novaPatente)
{
	if (novaPatente <= 0)
		return;

	// REMOVO A PATENTE DO ANTIGO USUARIO
	gQueryManager->Close();
	gQueryManager->ExecQuery("UPDATE Character SET Patente = 0 WHERE Patente = %d", novaPatente);
	gQueryManager->Close();

	// ATUALIZA A PATENTE DO NOVO USUARIO
	gQueryManager->Close();
	gQueryManager->ExecQuery("UPDATE Character SET Patente = %d WHERE Name = '%s'", novaPatente, nome);
	gQueryManager->Close();
}

void GDAtualizarPatenteRecv(SDHP_UPDATE_PATENTE_RECV* lpMsg, int index)
{
	AtualizarPatenteComPrioridade(lpMsg->nome, lpMsg->patente);
}

void GDSetCoinRecv(SDHP_SETCOIN_RECV* lpMsg) // OK
{

	if (gQueryManager->ExecQuery("SELECT * FROM MEMB_INFO WHERE memb___id = '%s'", lpMsg->account) == false || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("INSERT INTO MEMB_INFO (memb___id,Gold) VALUES ('%s',%d)", lpMsg->account, lpMsg->Gold);
		gQueryManager->Close();
		LogAdd(LOG_GREEN, "Update Gold %d - Account %s", lpMsg->Gold, lpMsg->account);
	}
	else
	{
		gQueryManager->Close();
		gQueryManager->ExecQuery("UPDATE MEMB_INFO SET Gold = Gold + %d WHERE memb___id = '%s'", lpMsg->Gold, lpMsg->account);
		gQueryManager->Fetch();
		gQueryManager->Close();

		LogAdd(LOG_GREEN,"Update Gold %d - Account %s", lpMsg->Gold, lpMsg->account);
	}

}