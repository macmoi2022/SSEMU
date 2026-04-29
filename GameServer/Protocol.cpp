#include "stdafx.h"
#include "Protocol.h"
#include "Attack.h"
#include "BloodCastle.h"
#include "CustomPackage.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "CastleSiegeWeapon.h"
#include "ChaosBox.h"
#include "ChaosCastle.h"
#include "CommandManager.h"
#include "ConnectionManager.h"
#include "Crywolf.h"
#include "CustomAttack.h"
#include "CustomHealthBar.h"
#include "DarkSpirit.h"
#include "DefaultClassInfo.h"
#include "DevilSquare.h"
#include "DoubleGoer.h"
#include "DSProtocol.h"
#include "Duel.h"
#include "EffectManager.h"
#include "EventEntryLevel.h"
#include "Filter.h"
#include "Friend.h"
#include "Fruit.h"
#include "GameMain.h"
#include "GameMaster.h"
#include "GensSystem.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GoldenArcher.h"
#include "HackPacketCheck.h"
#include "Helper.h"
#include "IllusionTemple.h"
#include "ImperialGuardian.h"
#include "InventoryEquipment.h"
#include "ItemManager.h"
#include "JewelMix.h"
#include "JSProtocol.h"
#include "KanturuEntranceNPC.h"
#include "Log.h"
#include "LuckyCoin.h"
#include "Map.h"
#include "MapManager.h"
#include "MapServerManager.h"
#include "MasterSkillTree.h"
#include "Move.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "ObjectManager.h"
#include "Party.h"
#include "PersonalShop.h"
#include "Quest.h"
#include "QuestWorld.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Trade.h"
#include "Util.h"
#include "Viewport.h"
#include "Warehouse.h"
#include "MuCashShop.h"
#include "PcPoint.h"
#include "CustomMove.h"
#include "CustomJewelBank.h"
#include "CustomLuckyWheel.h"
#include "DailyReward.h"
#include "TheftEvent.h"
#include "DeathEvent.h"
#include "RoletaEvent.h"

void ProtocolCore(BYTE head,BYTE* lpMsg,int size,int aIndex,int encrypt,int serial) // OK
{
	PROTECT_START

	if(gObj[aIndex].Type == OBJECT_USER && gHackPacketCheck->CheckPacketHack(aIndex,head,((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]),encrypt,serial) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	switch(head)
	{
		case PROTOCOL_CODE1:
			CGMoveRecv((PMSG_MOVE_RECV*)lpMsg,aIndex);
			break;
		case PROTOCOL_CODE2:
			gAttack->CGAttackRecv((PMSG_ATTACK_RECV*)lpMsg,aIndex);
			break;
		case PROTOCOL_CODE3:
			CGPositionRecv((PMSG_POSITION_RECV*)lpMsg,aIndex);
			break;
		case PROTOCOL_CODE4:
			gSkillManager->CGMultiSkillAttackRecv((PMSG_MULTI_SKILL_ATTACK_RECV*)lpMsg,aIndex,0);
			break;
		case 0x00:
			CGChatRecv((PMSG_CHAT_RECV*)lpMsg,aIndex);
			break;
		case 0x02:
			CGChatWhisperRecv((PMSG_CHAT_WHISPER_RECV*)lpMsg,aIndex);
			break;
		case 0x03:
			CGMainCheckRecv((PMSG_MAIN_CHECK_RECV*)lpMsg,aIndex);
			break;
		case 0x04:
			gCustomAttack->CustomAttackClose(aIndex);
			break;
		case 0x06:
			CGWindowsEventOpen(aIndex);
			break;
		case 0x0E:
			CGLiveClientRecv((PMSG_LIVE_CLIENT_RECV*)lpMsg,aIndex);
			break;
		case 0x18:
			CGActionRecv((PMSG_ACTION_RECV*)lpMsg,aIndex);
			break;
		case 0x19:
			gSkillManager->CGSkillAttackRecv((PMSG_SKILL_ATTACK_RECV*)lpMsg,aIndex);
			break;
		case 0x1B:
			gSkillManager->CGSkillCancelRecv((PMSG_SKILL_CANCEL_RECV*)lpMsg,aIndex);
			break;
		case 0x1C:
			gMove->CGTeleportRecv((PMSG_TELEPORT_RECV*)lpMsg,aIndex);
			break;
		case 0x1E:
			gSkillManager->CGDurationSkillAttackRecv((PMSG_DURATION_SKILL_ATTACK_RECV*)lpMsg,aIndex);
			break;
		case 0x22:
			gItemManager->CGItemGetRecv((PMSG_ITEM_GET_RECV*)lpMsg,aIndex);
			break;
		case 0x23:
			gItemManager->CGItemDropRecv((PMSG_ITEM_DROP_RECV*)lpMsg,aIndex);
			break;
		case 0x24:
			gItemManager->CGItemMoveRecv((PMSG_ITEM_MOVE_RECV*)lpMsg,aIndex);
			break;
		case 0x26:
			gItemManager->CGItemUseRecv((PMSG_ITEM_USE_RECV*)lpMsg,aIndex);
			break;
		case 0x30:
			gNpcTalk->CGNpcTalkRecv((PMSG_NPC_TALK_RECV*)lpMsg,aIndex);
			break;
		case 0x31:
			gNpcTalk->CGNpcTalkCloseRecv(aIndex);
			break;
		case 0x32:
			gItemManager->CGItemBuyRecv((PMSG_ITEM_BUY_RECV*)lpMsg,aIndex);
			break;
		case 0x33:
			gItemManager->CGItemSellRecv((PMSG_ITEM_SELL_RECV*)lpMsg,aIndex);
			break;
		case 0x34:
			gItemManager->CGItemRepairRecv((PMSG_ITEM_REPAIR_RECV*)lpMsg,aIndex);
			break;
		case 0x36:
		{
			if (lpObj->InEvent == true || TheftEvent.PlayerStruct[lpObj->Index]._InEvent == true || DeathEvent.PlayerStruct[lpObj->Index]._InEvent == true || RoletaEvent.PlayerStruct[lpObj->Index]._InEvent == true)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Proibido dar trade no evento.");
				return;
			}

			gTrade->CGTradeRequestRecv((PMSG_TRADE_REQUEST_RECV*)lpMsg, aIndex);
		}
		break;
		case 0x37:
			gTrade->CGTradeResponseRecv((PMSG_TRADE_RESPONSE_RECV*)lpMsg,aIndex);
			break;
		case 0x3A:
			gTrade->CGTradeMoneyRecv((PMSG_TRADE_MONEY_RECV*)lpMsg,aIndex);
			break;
		case 0x3C:
			gTrade->CGTradeOkButtonRecv((PMSG_TRADE_OK_BUTTON_RECV*)lpMsg,aIndex);
			break;
		case 0x3D:
			gTrade->CGTradeCancelButtonRecv(aIndex);
			break;
		case 0x3F:
			switch(lpMsg[3])
			{
				case 0x01:
					gPersonalShop->CGPShopSetItemPriceRecv((PMSG_PSHOP_SET_ITEM_PRICE_RECV*)lpMsg,aIndex);
					break;
				case 0x02:
					gPersonalShop->CGPShopOpenRecv((PMSG_PSHOP_OPEN_RECV*)lpMsg,aIndex);
					break;
				case 0x03:
					gPersonalShop->CGPShopCloseRecv(aIndex);
					break;
				case 0x05:
					gPersonalShop->CGPShopItemListRecv((PMSG_PSHOP_ITEM_LIST_RECV*)lpMsg,aIndex);
					break;
				case 0x06:
					gPersonalShop->CGPShopBuyItemRecv((PMSG_PSHOP_BUY_ITEM_RECV*)lpMsg,aIndex);
					break;
				case 0x07:
					gPersonalShop->CGPShopLeaveRecv((PMSG_PSHOP_LEAVE_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0x40:
		{
			if (lpObj->InEvent == true || TheftEvent.PlayerStruct[lpObj->Index]._InEvent == true || DeathEvent.PlayerStruct[lpObj->Index]._InEvent == true || RoletaEvent.PlayerStruct[lpObj->Index]._InEvent == true)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Proibido formar party no evento.");
				return;
			}

			gParty->CGPartyRequestRecv((PMSG_PARTY_REQUEST_RECV*)lpMsg, aIndex);
		}
		break;
		case 0x41:
			gParty->CGPartyRequestResultRecv((PMSG_PARTY_REQUEST_RESULT_RECV*)lpMsg,aIndex);
			break;
		case 0x42:
			gParty->CGPartyListRecv(aIndex);
			break;
		case 0x43:
			gParty->CGPartyDelMemberRecv((PMSG_PARTY_DEL_MEMBER_RECV*)lpMsg,aIndex);
			break;
		case 0x4A:
			#if(GAMESERVER_UPDATE>=601)
			gSkillManager->CGRageFighterSkillAttackRecv((PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0x4B:
			#if(GAMESERVER_UPDATE>=601)
			gSkillManager->CGSkillDarkSideRecv((PMSG_SKILL_DARK_SIDE_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0x4C:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=801)
					gMiningSystem->CGMiningStartRecv((PMSG_MINING_START_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x01:
					#if(GAMESERVER_UPDATE>=801)
					gMiningSystem->CGMiningSuccessRecv((PMSG_MINING_SUCCESS_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=801)
					gMiningSystem->CGMiningFailureRecv((PMSG_MINING_FAILURE_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0x4D:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=802)
					gEventInventory->CGEventItemGetRecv((PMSG_EVENT_ITEM_GET_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x01:
					#if(GAMESERVER_UPDATE>=802)
					gEventInventory->CGEventItemDropRecv((PMSG_EVENT_ITEM_DROP_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0F:
					#if(GAMESERVER_UPDATE>=802)
					gEventInventory->CGEventInventoryOpenRecv((PMSG_EVENT_INVENTORY_OPEN_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x10:
					#if(GAMESERVER_UPDATE>=802)
					gMuRummy->CGReqMuRummyStart((_tagPMSG_REQ_MURUMMY_INFO*)lpMsg,aIndex);
					#endif
					break;
				case 0x11:
					#if(GAMESERVER_UPDATE>=802)
					gMuRummy->CGReqCardReveal((_tagPMSG_REQ_REVEAL_CARD*)lpMsg,aIndex);
					#endif
					break;
				case 0x12:
					#if(GAMESERVER_UPDATE>=802)
					gMuRummy->CGReqCardMove((_tagPMSG_REQ_CARD_MOVE*)lpMsg,aIndex);
					#endif
					break;
				case 0x13:
					#if(GAMESERVER_UPDATE>=802)
					gMuRummy->CGReqCardReMove((_tagPMSG_REQ_CARD_REMOVE*)lpMsg,aIndex);
					#endif
					break;
				case 0x14:
					#if(GAMESERVER_UPDATE>=802)
					gMuRummy->CGReqCardMatch((_tagPMSG_REQ_CARD_MATCH*)lpMsg,aIndex);
					#endif
					break;
				case 0x15:
					#if(GAMESERVER_UPDATE>=802)
					gMuRummy->CGReqMuRummyEnd((_tagPMSG_REQ_MURUMMY_END*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0x4E:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=803)
					gMuunSystem->CGMuunItemGetRecv((PMSG_MUUN_ITEM_GET_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x08:
					#if(GAMESERVER_UPDATE>=803)
					gMuunSystem->CGMuunItemUseRecv((PMSG_MUUN_ITEM_USE_RECV*)lpMsg,aIndex);
					#endif
				case 0x09:
					#if(GAMESERVER_UPDATE>=803)
					gMuunSystem->CGMuunItemSellRecv((PMSG_MUUN_ITEM_SELL_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0x50:
			gGuild->CGGuildRequestRecv((PMSG_GUILD_REQUEST_RECV*)lpMsg,aIndex);
			break;
		case 0x51:
			gGuild->CGGuildResultRecv((PMSG_GUILD_RESULT_RECV*)lpMsg,aIndex);
			break;
		case 0x52:
			gGuild->CGGuildListRecv(aIndex);
			break;
		case 0x53:
			gGuild->CGGuildDeleteRecv((PMSG_GUILD_DELETE_RECV*)lpMsg,aIndex);
			break;
		case 0x54:
			gGuild->CGGuildMasterOpenRecv((PMSG_GUILD_MASTER_OPEN_RECV*)lpMsg,aIndex);
			break;
		case 0x55:
			gGuild->CGGuildCreateRecv((PMSG_GUILD_CREATE_RECV*)lpMsg,aIndex);
			break;
		case 0x57:
			gGuild->CGGuildMasterCancelRecv(aIndex);
			break;
		case 0x61:
			gGuild->CGGuildWarRequestResultRecv((PMSG_GUILD_WAR_REQUEST_RESULT_RECV*)lpMsg,aIndex);
			break;
		case 0x66:
			gGuild->CGGuildViewportRequestRecv((PMSG_GUILD_VIEWPORT_REQUEST_RECV*)lpMsg,aIndex);
			break;
		case 0x81:
			gWarehouse->CGWarehouseMoneyRecv((PMSG_WAREHOUSE_MONEY_RECV*)lpMsg,aIndex);
			break;
		case 0x82:
			gWarehouse->CGWarehouseClose(aIndex);
			break;
		case 0x83:
			gWarehouse->CGWarehousePasswordRecv((PMSG_WAREHOUSE_PASSWORD_RECV*)lpMsg,aIndex);
			break;
		case 0x86:
			gChaosBox->CGChaosMixRecv((PMSG_CHAOS_MIX_RECV*)lpMsg,aIndex);
			break;
		case 0x87:
			gChaosBox->CGChaosMixCloseRecv(aIndex);
			break;
		case 0x88:
			gChaosBox->CGChaosMixRateRecv((PMSG_CHAOS_MIX_RATE_RECV*)lpMsg,aIndex);
			break;
		case 0x8E:
			#if(GAMESERVER_UPDATE>=501)
			gMove->CGTeleportMoveRecv((PMSG_TELEPORT_MOVE_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0x90:
			gDevilSquare->CGDevilSquareEnterRecv((PMSG_DEVIL_SQUARE_ENTER_RECV*)lpMsg,aIndex);
			break;
		case 0x91:
			CGEventRemainTimeRecv((PMSG_EVENT_REMAIN_TIME_RECV*)lpMsg,aIndex);
			break;
		case 0x95:
			#if(GAMESERVER_UPDATE<=603)
			gGoldenArcher->CGGoldenArcherRegisterRecv((PMSG_GOLDEN_ARCHER_COUNT_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0x96:
			#if(GAMESERVER_UPDATE<=603)
			gGoldenArcher->CGGoldenArcherRewardRecv((PMSG_GOLDEN_ARCHER_REWARD_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0x97:
			#if(GAMESERVER_UPDATE<=603)
			gGoldenArcher->CGGoldenArcherCloseRecv(aIndex);
			#endif
			break;
		case 0x9A:
			gBloodCastle->CGBloodCastleEnterRecv((PMSG_BLOOD_CASTLE_ENTER_RECV*)lpMsg,aIndex);
			break;
		case 0xA0:
			gQuest->CGQuestInfoRecv(aIndex);
			break;
		case 0xA2:
			gQuest->CGQuestStateRecv((PMSG_QUEST_STATE_RECV*)lpMsg,aIndex);
			break;
		case 0xA7:
			CGPetItemCommandRecv((PMSG_PET_ITEM_COMMAND_RECV*)lpMsg,aIndex);
			break;
		case 0xA9:
			CGPetItemInfoRecv((PMSG_PET_ITEM_INFO_RECV*)lpMsg,aIndex);
			break;
		case 0xAA:
			#if(GAMESERVER_UPDATE>=402)
			switch(lpMsg[3])
			{
				case 0x01:
					gDuel->CGDuelStartRecv((PMSG_DUEL_START_RECV*)lpMsg,aIndex);
					break;
				case 0x02:
					gDuel->CGDuelOkRecv((PMSG_DUEL_OK_RECV*)lpMsg,aIndex);
					break;
				case 0x03:
					gDuel->CGDuelEndRecv(aIndex);
					break;
				case 0x07:
					gDuel->CGDuelWatchRecv((PMSG_DUEL_WATCH_RECV*)lpMsg,aIndex);
					break;
				case 0x09:
					gDuel->CGDuelButtonRecv((PMSG_DUEL_BUTTON_RECV*)lpMsg,aIndex);
					break;
			}
			#else
			if (lpObj->InEvent == true || TheftEvent.PlayerStruct[lpObj->Index]._InEvent == true || DeathEvent.PlayerStruct[lpObj->Index]._InEvent == true || RoletaEvent.PlayerStruct[lpObj->Index]._InEvent == true)
			{
				gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Proibido iniciar o duelo no evento.");
				return;
			}

			gDuel->CGDuelStartRecv((PMSG_DUEL_START_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0xAB:
			#if(GAMESERVER_UPDATE<=401)
			gDuel->CGDuelEndRecv(aIndex);
			#endif
			break;
		case 0xAC:
			#if(GAMESERVER_UPDATE<=401)
			gDuel->CGDuelOkRecv((PMSG_DUEL_OK_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0xAE:
			#if(GAMESERVER_UPDATE>=603)
			gHelper->CGHelperDataRecv((PMSG_HELPER_DATA_RECV*)lpMsg,aIndex);
			#endif
			break;
		case 0xAF:
			switch(lpMsg[3])
			{
				case 0x01:
					gChaosCastle->CGChaosCastleEnterRecv((PMSG_CHAOS_CASTLE_ENTER_RECV*)lpMsg,aIndex);
					break;
				case 0x02:
					gChaosCastle->CGChaosCastlePositionRecv((PMSG_CHAOS_CASTLE_POSITION_RECV*)lpMsg,aIndex);
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=803)
					gChaosCastleFinal->CGChaosCastleFinalRankingSend(aIndex);
					#endif
					break;
				case 0x05:
					#if(GAMESERVER_UPDATE>=803)
					gChaosCastleFinal->CGChaosCastleFinalEnterRecv(aIndex);
					#endif
					break;
				case 0x06:
					#if(GAMESERVER_UPDATE>=803)
					gChaosCastleFinal->CGChaosCastleFinalPositionRecv((PMSG_CHAOS_CASTLE_FINAL_POSITION_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x07:
					#if(GAMESERVER_UPDATE>=803)
					gChaosCastleFinal->GCChaosCastleFinalRankingRecv(aIndex);
					#endif
					break;
				case 0x08:
					#if(GAMESERVER_UPDATE>=803)
					gChaosCastleFinal->CGChaosCastleFinalClose(aIndex);
					#endif
					break;
			}
			break;
		case 0xB0:
			gSkillManager->CGSkillTeleportAllyRecv((PMSG_SKILL_TELEPORT_ALLY_RECV*)lpMsg,aIndex);
			break;
		case 0xB1:
			switch(lpMsg[3])
			{
				case 0x01:
					CGMapServerMoveAuthRecv((PMSG_MAP_SERVER_MOVE_AUTH_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0xB2:
			switch(lpMsg[3])
			{
				case 0x00:
					gCastleSiege->CGCastleSiegeStateRecv(aIndex);
					break;
				case 0x01:
					gCastleSiege->CGCastleSiegeRegisterRecv(aIndex);
					break;
				case 0x02:
					gCastleSiege->CGCastleSiegeGiveUpGuildRecv((PMSG_CASTLE_SIEGE_GIVEUP_GUILD_RECV*)lpMsg,aIndex);
					break;
				case 0x03:
					gCastleSiege->CGCastleSiegeRegisterGuildInfoRecv(aIndex);
					break;
				case 0x04:
					gCastleSiege->CGCastleSiegeRegisterMarkRecv(aIndex);
					break;
				case 0x05:
					gCastleSiege->CGCastleSiegeNpcBuyRecv((PMSG_CASTLE_SIEGE_NPC_BUY_RECV*)lpMsg,aIndex);
					break;
				case 0x06:
					gCastleSiege->CGCastleSiegeNpcRepairRecv((PMSG_CASTLE_SIEGE_NPC_REPAIR_RECV*)lpMsg,aIndex);
					break;
				case 0x07:
					gCastleSiege->CGCastleSiegeNpcUpgradeRecv((PMSG_CASTLE_SIEGE_NPC_UPGRADE_RECV*)lpMsg,aIndex);
					break;
				case 0x08:
					gCastleSiege->CGCastleSiegeTaxMoneyInfoRecv(aIndex);
					break;
				case 0x09:
					gCastleSiege->CGCastleSiegeTaxRateChangeRecv((PMSG_CS_TAX_RATE_CHANGE_RECV*)lpMsg,aIndex);
					break;
				case 0x10:
					gCastleSiege->CGCastleSiegeMoneyOutRecv((PMSG_CASTLE_SIEGE_MONEY_OUT_RECV*)lpMsg,aIndex);
					break;
				case 0x12:
					gCastleSiege->CGCastleSiegeGateOperateRecv((PMSG_CASTLE_SIEGE_GATE_OPERATE_RECV*)lpMsg,aIndex);
					break;
				case 0x1B:
					gCastleSiege->CGCastleSiegeMiniMapRequestRecv(aIndex);
					break;
				case 0x1C:
					gCastleSiege->CGCastleSiegeMiniMapCloseRecv(aIndex);
					break;
				case 0x1D:
					gCastleSiege->CGCastleSiegeCommandRequestRecv((PMSG_CASTLE_SIEGE_COMMAND_REQUEST_RECV*)lpMsg,aIndex);
					break;
				case 0x1F:
					gCastleSiege->CGCastleSiegeHuntZoneAllowRecv((PMSG_CASTLE_SIEGE_HUNT_ZONE_ALLOW_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0xB3:
			gCastleSiege->CGCastleSiegeNpcListRecv((PMSG_CASTLE_SIEGE_NPC_LIST_RECV*)lpMsg,aIndex);
			break;
		case 0xB4:
			gCastleSiege->CGCastleSiegeGuildRegisterListRecv(aIndex);
			break;
		case 0xB5:
			gCastleSiege->CGCastleSiegeGuildListRecv(aIndex);
			break;
		case 0xB7:
			switch(lpMsg[3])
			{
				case 0x01:
					gCastleSiegeWeapon->CGCastleSiegeWeaponUseRecv((PMSG_CASTLE_SIEGE_WEAPON_USE_RECV*)lpMsg,aIndex);
					break;
				case 0x04:
					gCastleSiegeWeapon->CGCastleSiegeWeaponDamageRecv((PMSG_CASTLE_SIEGE_WEAPON_DAMAGE_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0xB9:
			switch(lpMsg[3])
			{
				case 0x02:
					gCastleSiege->CGCastleSiegeCastleOwnerMarkRecv(aIndex);
					break;
				case 0x05:
					gCastleSiege->CGCastleSiegeLandOfTrialsMoveRecv((PMSG_CASTLE_SIEGE_LAND_OF_TRIAL_MOVE_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0xBC:
			switch(lpMsg[3])
			{
				case 0x00:
					gJewelMix->CGJewelMixRecv((PMSG_JEWEL_MIX_RECV*)lpMsg,aIndex);
					break;
				case 0x01:
					gJewelMix->CGJewelUnMixRecv((PMSG_JEWEL_UNMIX_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0xBD:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=201)
					gCrywolf->CGCrywolfInfoRecv(aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=201)
					gCrywolf->CGCrywolfAltarContractRecv((PMSG_CRYWOLF_ALTAR_CONTRACT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x09:
					#if(GAMESERVER_UPDATE>=201)
					gCrywolf->CGCrywolfChaosRateRecv(aIndex);
					#endif
					break;
			}
			break;
		case 0xBF:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=301 && GAMESERVER_UPDATE<=802)
					gIllusionTemple->CGIllusionTempleEnterRecv((PMSG_ILLUSION_TEMPLE_ENTER_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x02:
					#if(GAMESERVER_UPDATE>=301)
					#if(GAMESERVER_UPDATE>=803)
					gIllusionTempleRenewal->CGIllusionTempleRenewalSkillRecv((PMSG_ILLUSION_TEMPLE_RENEWAL_SKILL_RECV*)lpMsg,aIndex);
					#else
					gIllusionTemple->CGIllusionTempleSkillRecv((PMSG_ILLUSION_TEMPLE_SKILL_RECV*)lpMsg,aIndex);
					#endif
					#endif
					break;
				case 0x05:
					#if(GAMESERVER_UPDATE>=301 && GAMESERVER_UPDATE<=802)
					gIllusionTemple->CGIllusionTempleButtonClickRecv(aIndex);
					#endif
					break;
				case 0x0B:
					#if(GAMESERVER_UPDATE>=402)
					gLuckyCoin->CGLuckyCoinCountRecv((PMSG_LUCKY_COIN_COUNT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0C:
					#if(GAMESERVER_UPDATE>=402)
					gLuckyCoin->CGLuckyCoinRegisterRecv((PMSG_LUCKY_COIN_REGISTER_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0D:
					#if(GAMESERVER_UPDATE>=402)
					gLuckyCoin->CGLuckyCoinExchangeRecv((PMSG_LUCKY_COIN_EXCHANGE_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0E:
					#if(GAMESERVER_UPDATE>=501)
					gDoubleGoer->CGDoubleGoerEnterRecv((PMSG_DOUBLE_GOER_ENTER_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x17:
					#if(GAMESERVER_UPDATE>=501)
					gNpcTalk->CGNpcJuliaRecv(aIndex);
					#endif
					break;
				case 0x20:
					#if(GAMESERVER_UPDATE>=501)
					gInventoryEquipment->CGInventoryEquipmentRecv((PMSG_INVENTORY_EQUIPMENT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x51:
					#if(GAMESERVER_UPDATE>=603)
					gHelper->CGHelperStartRecv((PMSG_HELPER_START_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x6A:
					#if(GAMESERVER_UPDATE>=803)
					gIllusionTempleRenewal->CGIllusionTempleRenewalStoneTalkRecv((PMSG_ILLUSION_TEMPLE_RENEWAL_STONE_TALK_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x6B:
					#if(GAMESERVER_UPDATE>=803)
					gIllusionTempleRenewal->CGIllusionTempleRenewalButtonClickRecv(aIndex);
					#endif
					break;
				case 0x70:
					#if(GAMESERVER_UPDATE>=803)
					gIllusionTempleRenewal->CGIllusionTempleRenewalEnterRecv(aIndex);
					#endif
					break;
			}
			break;
		case 0xC0:
			gFriend->CGFriendListRecv(aIndex);
			break;
		case 0xC1:
			gFriend->CGFriendRequestRecv((PMSG_FRIEND_REQUEST_RECV*)lpMsg,aIndex);
			break;
		case 0xC2:
			gFriend->CGFriendResultRecv((PMSG_FRIEND_RESULT_RECV*)lpMsg,aIndex);
			break;
		case 0xC3:
			gFriend->CGFriendDeleteRecv((PMSG_FRIEND_DELETE_RECV*)lpMsg,aIndex);
			break;
		case 0xC4:
			gFriend->CGFriendStateRecv((PMSG_FRIEND_STATE_RECV*)lpMsg,aIndex);
			break;
		case 0xC5:
			gFriend->CGFriendMailRecv((PMSG_FRIEND_MAIL_RECV*)lpMsg,aIndex);
			break;
		case 0xC7:
			gFriend->CGFriendMailReadRecv((PMSG_FRIEND_MAIL_READ_RECV*)lpMsg,aIndex);
			break;
		case 0xC8:
			gFriend->CGFriendMailDeleteRecv((PMSG_FRIEND_MAIL_DELETE_RECV*)lpMsg,aIndex);
			break;
		case 0xC9:
			gFriend->CGFriendMailListRecv(aIndex);
			break;
		case 0xCA:
			gFriend->CGFriendChatCreateRecv((PMSG_FRIEND_CHAT_CREATE_RECV*)lpMsg,aIndex);
			break;
		case 0xCB:
			gFriend->CGFriendChatInviteRecv((PMSG_FRIEND_CHAT_INVITE_RECV*)lpMsg,aIndex);
			break;
		case 0xD0:
			switch(lpMsg[3])
			{
#if(GAMESERVER_UPDATE>=301)
				case 0x05:
					#if(GAMESERVER_SHOP==1)
					gPcPoint->CGPcPointItemBuyRecv((PMSG_PC_POINT_ITEM_BUY_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x06:
					#if(GAMESERVER_SHOP==1)
					gPcPoint->CGPcPointOpenRecv((PMSG_PC_POINT_OPEN_RECV*)lpMsg,aIndex);
					#endif
#endif
					break;
				case 0x07:
					#if(GAMESERVER_UPDATE>=301)
					gQuest->CGQuestNpcWarewolfRecv(aIndex);
					#endif
					break;
				case 0x08:
					#if(GAMESERVER_UPDATE>=301)
					gQuest->CGQuestNpcKeeperRecv(aIndex);
					#endif
					break;
				case 0x09:
					#if(GAMESERVER_UPDATE>=301)
					gNpcTalk->CGNpcLeoTheHelperRecv(aIndex);
					#endif
					break;
				case 0x0A:
					#if(GAMESERVER_UPDATE>=401)
					gNpcTalk->CGNpcSnowmanRecv(aIndex);
					#endif
					break;
				case 0x10:
					#if(GAMESERVER_UPDATE>=401)
					gNpcTalk->CGNpcSantaClausRecv(aIndex);
					#endif
					break;
			}
			break;
		case 0xD1:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=201)
					gKanturuEntranceNPC->CGKanturuEnterInfoRecv(aIndex);
					#endif
					break;
				case 0x01:
					#if(GAMESERVER_UPDATE>=201)
					gKanturuEntranceNPC->CGKanturuEnterRecv(aIndex);
					#endif
					break;
			}
			break;
		#if(GAMESERVER_SHOP==3)
		case 0xD2:
			switch(lpMsg[3])
			{
				case 0x01:
					#if(GAMESERVER_SHOP==3)
					gCashShop->CGCashShopPointRecv(aIndex);
					#endif
					break;
				case 0x02:
					#if(GAMESERVER_SHOP==3)
					gCashShop->CGCashShopOpenRecv((PMSG_CASH_SHOP_OPEN_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_SHOP==3)
					gCashShop->CGCashShopItemBuyRecv((PMSG_CASH_SHOP_ITEM_BUY_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x04:
					#if(GAMESERVER_SHOP==3)
					gCashShop->CGCashShopItemGifRecv((PMSG_CASH_SHOP_ITEM_GIF_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x05:
					#if(GAMESERVER_SHOP==3)
					gCashShop->CGCashShopItemNumRecv((PMSG_CASH_SHOP_ITEM_NUM_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0B:
					#if(GAMESERVER_SHOP==3)
					gCashShop->CGCashShopItemUseRecv((PMSG_CASH_SHOP_ITEM_USE_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		#endif
		case 0xE1:
			gGuild->CGGuildAssignStatusRecv((PMSG_GUILD_ASSIGN_STATUS_RECV*)lpMsg,aIndex);
			break;
		case 0xE2:
			gGuild->CGGuildAssignTypeRecv((PMSG_GUILD_ASSIGN_TYPE_RECV*)lpMsg,aIndex);
			break;
		case 0xE5:
			gGuild->CGGuildUnionRequestRecv((PMSG_GUILD_UNION_REQUEST_RECV*)lpMsg,aIndex);
			break;
		case 0xE6:
			gGuild->CGGuildUnionResultRecv((PMSG_GUILD_UNION_RESULT_RECV*)lpMsg,aIndex);
			break;
		case 0xE7:
			switch(lpMsg[3])
			{
				case 0x01:
					#if(GAMESERVER_UPDATE>=802)
					gMiniMap->CGMiniMapStartPartyInfoRecv(aIndex);
					#endif
					break;
				case 0x02:
					#if(GAMESERVER_UPDATE>=802)
					gMiniMap->CGMiniMapClosePartyInfoRecv(aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=802)
					gMiniMap->CGMiniMapInfoRecv((PMSG_MINI_MAP_INFO_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0xE9:
			gGuild->CGGuildUnionListRecv(aIndex);
			break;
		case 0xEB:
			gGuild->CGGuildAllianceKickRecv((PMSG_GUILD_ALLIANCE_KICK_RECV*)lpMsg,aIndex);
			break;
		case 0xEC:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=701)
					gPentagramSystem->CGPentagramJewelInsertRecv((PMSG_PENTAGRAM_JEWEL_INSERT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x01:
					#if(GAMESERVER_UPDATE>=701)
					gPentagramSystem->CGPentagramJewelRemoveRecv((PMSG_PENTAGRAM_JEWEL_REMOVE_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x02:
					#if(GAMESERVER_UPDATE>=701)
					gPentagramSystem->CGPentagramJewelRefineRecv((PMSG_PENTAGRAM_JEWEL_REFINE_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=701)
					gPentagramSystem->CGPentagramJewelUpgradeRecv((PMSG_PENTAGRAM_JEWEL_UPGRADE_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x31:
					#if(GAMESERVER_UPDATE>=802)
					gPersonalShop->CGPShopSearchRecv((PMSG_PSHOP_SEARCH_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x33:
					#if(GAMESERVER_UPDATE>=802)
					gPersonalShop->CGPShopSearchLogRecv((PMSG_PSHOP_SEARCH_LOG_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0xED:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingListRecv((PMSG_GUILD_MATCHING_LIST_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x01:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingListSearchRecv((PMSG_GUILD_MATCHING_LIST_SEARCH_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x02:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingInsertRecv((PMSG_GUILD_MATCHING_INSERT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingCancelRecv((PMSG_GUILD_MATCHING_CANCEL_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x04:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingJoinInsertRecv((PMSG_GUILD_MATCHING_JOIN_INSERT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x05:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingJoinCancelRecv((PMSG_GUILD_MATCHING_JOIN_CANCEL_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x06:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingJoinAcceptRecv((PMSG_GUILD_MATCHING_JOIN_ACCEPT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x07:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingJoinListRecv((PMSG_GUILD_MATCHING_JOIN_LIST_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x08:
					#if(GAMESERVER_UPDATE>=801)
					gGuildMatching->CGGuildMatchingJoinInfoRecv((PMSG_GUILD_MATCHING_JOIN_INFO_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0xEF:
			switch(lpMsg[3])
			{
				case 0x00:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingInsertRecv((PMSG_PARTY_MATCHING_INSERT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x01:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingListRecv((PMSG_PARTY_MATCHING_LIST_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x02:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingJoinInsertRecv((PMSG_PARTY_MATCHING_JOIN_INSERT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingJoinInfoRecv((PMSG_PARTY_MATCHING_JOIN_INFO_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x04:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingJoinListRecv((PMSG_PARTY_MATCHING_JOIN_LIST_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x05:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingJoinAcceptRecv((PMSG_PARTY_MATCHING_JOIN_ACCEPT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x06:
					#if(GAMESERVER_UPDATE>=801)
					gPartyMatching->CGPartyMatchingJoinCancelRecv((PMSG_PARTY_MATCHING_JOIN_CANCEL_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0xF1:
			switch(lpMsg[3])
			{
				case 0x01:
					CGConnectAccountRecv((PMSG_CONNECT_ACCOUNT_RECV*)lpMsg,aIndex);
					break;
				case 0x02:
					CGCloseClientRecv((PMSG_CLOSE_CLIENT_RECV*)lpMsg,aIndex);
					break;
			}
			break;
		case 0xF2:
			switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
			{
			case 0x05:
				gPackage->RecvPackageType((CG_SENDPACKAGE_RECV*)lpMsg, aIndex);
				break;
			}
			break;
		case 0xF3:
			switch(lpMsg[3])
			{
				case 0x00:
					CGCharacterListRecv(aIndex);
					break;
				case 0x01:
					CGCharacterCreateRecv((PMSG_CHARACTER_CREATE_RECV*)lpMsg,aIndex);
					break;
				case 0x02:
					CGCharacterDeleteRecv((PMSG_CHARACTER_DELETE_RECV*)lpMsg,aIndex);
					break;
				case 0x03:
					CGCharacterInfoRecv((PMSG_CHARACTER_INFO_RECV*)lpMsg,aIndex);
					break;
				case 0x06:
					CGLevelUpPointRecv((PMSG_LEVEL_UP_POINT_RECV*)lpMsg,aIndex);
					break;
				case 0x09:
					gConnectionManager->CGHardwareIdRecv((PMSG_HARDWARE_ID_INFO_RECV*)lpMsg,aIndex);
					break;
				case 0x0A:
					gCustomAttack->CGCustomAttackToggleRecv((PMSG_CUSTOM_ATTACK_TOGGLE_RECV*)lpMsg,aIndex);
					break;
				case 0x12:
					CGCharacterMoveViewportEnableRecv(aIndex);
					break;
				case 0x15:
					#if(GAMESERVER_UPDATE>=401)
					CGCharacterNameCheckRecv((PMSG_CHARACTER_NAME_CHECK_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x21:
					#if(GAMESERVER_UPDATE>=701)
					CGOptionChangeSkinRecv((PMSG_OPTION_CHANGE_SKIN_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x30:
					CGOptionDataRecv((PMSG_OPTION_DATA_RECV*)lpMsg,aIndex);
					break;
				case 0x31:
					#if(GAMESERVER_UPDATE>=701)
					CGClientSecurityBreachRecv((PMSG_CLIENT_SECURITY_BREACH_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x52:
					#if(GAMESERVER_UPDATE>=401)
					gMasterSkillTree->CGMasterSkillRecv((PMSG_MASTER_SKILL_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0xF5:
					gCustomJewelBank->JewelBankRecv((PSBMSG_JEWELBANK_RECV*)lpMsg, aIndex);
					break;
				case 0xF6:
					gCustomJewelBank->JewelBankWithDrawRecv((PSBMSG_JEWELBANKWITHDRAW_RECV*)lpMsg, aIndex);
					break;
				case 0xF7:
					gWarehouse->WarehouseChangeRecv((PMSG_WAREHOUSE_CHANGE_RECV*)lpMsg, aIndex);
					break;
			}
			break;
		#if(GAMESERVER_SHOP==2)
		case 0xF5:
			switch(lpMsg[3])
			{
				case 0x01:
					#if(GAMESERVER_SHOP==2)
					gMuCashShop->CGMuCashShopOpenRecv((PMSG_MU_CASH_SHOP_OPEN_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_SHOP==2)
					gMuCashShop->GDMuCashPointSend(aIndex);
					#endif
					break;
				case 0x05:
					#if(GAMESERVER_SHOP==2)
					gMuCashShop->CGMuCashShopItemRecv((PMSG_MU_CASH_SHOP_ITEM_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x07:
					#if(GAMESERVER_SHOP==2)
					gMuCashShop->CGMuCashShopItemBuyRecv((PMSG_MU_CASH_SHOP_BUY_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		#endif
		case 0xF6:
			switch(lpMsg[3])
			{
				case 0x0A:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldDialogRecv((PMSG_QUEST_WORLD_DIALOG_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0B:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldAcceptRecv((PMSG_QUEST_WORLD_ACCEPT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0D:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldFinishRecv((PMSG_QUEST_WORLD_FINISH_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0F:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldCancelRecv((PMSG_QUEST_WORLD_CANCEL_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x10:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldButtonRecv((PMSG_QUEST_WORLD_BUTTON_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x1B:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldDetailRecv((PMSG_QUEST_WORLD_DETAIL_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x30:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldListRecv(aIndex);
					#endif
					break;
				case 0x31:
					#if(GAMESERVER_UPDATE>=501)
					gQuestWorld->CGQuestWorldNpcElfBufferRecv(aIndex);
					#endif
					break;
			}
			break;
		case 0xF7:
			switch(lpMsg[3])
			{
				case 0x01:
					#if(GAMESERVER_UPDATE>=501)
					gImperialGuardian->CGImperialGuardianEnterRecv((PMSG_IMPERIAL_GUARDIAN_ENTER_RECV*)lpMsg,aIndex);
					#endif
					break;
			}
			break;
		case 0xF8:
			switch(lpMsg[3])
			{
				case 0x01:
					#if(GAMESERVER_UPDATE>=501)
					gGensSystem->CGGensSystemInsertRecv((PMSG_GENS_SYSTEM_INSERT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x03:
					#if(GAMESERVER_UPDATE>=501)
					gGensSystem->CGGensSystemDeleteRecv((PMSG_GENS_SYSTEM_DELETE_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x09:
					#if(GAMESERVER_UPDATE>=501)
					gGensSystem->CGGensSystemRewardRecv((PMSG_GENS_SYSTEM_REWARD_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x0B:
					#if(GAMESERVER_UPDATE>=501)
					gGensSystem->CGGensSystemMemberRecv((PMSG_GENS_SYSTEM_MEMBER_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x20:
					#if(GAMESERVER_UPDATE>=701)
					CGAcheronEnterRecv(aIndex);
					#endif
					break;
				case 0x30:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleGuildMasterRegisterRecv(aIndex);
					#endif
					break;
				case 0x32:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleGuildMemberRegisterRecv(aIndex);
					#endif
					break;
				case 0x34:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleEnterRecv((PMSG_ARCA_BATTLE_ENTER_RESULT_RECV*)lpMsg,aIndex);
					#endif
					break;
				case 0x36:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleTrophiesRecv(aIndex);
					#endif
					break;
				case 0x41:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleMemberCountRecv(aIndex);
					#endif
					break;
				case 0x43:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleRegisterMarkRecv(aIndex);
					#endif
					break;
				case 0x45:
					#if(GAMESERVER_UPDATE>=702)
					gArcaBattle->CGArcaBattleMarkRankRecv(aIndex);
					#endif
					break;
				case 0x4B:
					#if(GAMESERVER_UPDATE>=801)
					CGAcheronGuardianEnterRecv(aIndex);
					#endif
					break;
			}
			break;
		case 0xFB:
			switch (lpMsg[3])
			{
			case 0x02:
				DailyReward->CGRequest(lpMsg, aIndex);
				break;
			case 0x16:
				gLuckyWheel->Start(lpObj);
				break;
			}
			break;
	}

	PROTECT_FINAL
}

void CGChatRecv(PMSG_CHAT_RECV* lpMsg,int aIndex) // OK
{
#pragma message("Bug Fix =>  Fix Inject Date:17/05/2025 ")

	char* SendText = &lpMsg->message[0];

	if (SendText[0] == '@' && SendText[1] == '>')
	{
		for (unsigned int i = 0; i < strlen(SendText); i++)
		{
			if (SendText[i] == '\'')
			{
				SendText[i] = '"';
			}
		}
	}

	if(gMapManager->GetMapChatEnable(gObj[aIndex].Map) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	char name[11] = {0};

	memcpy(name,lpMsg->name,sizeof(lpMsg->name));

	if(strcmp(name,lpObj->Name) != 0)
	{
		return;
	}

	lpMsg->message[(sizeof(lpMsg->message)-1)] = 0;

	gLog->Output(LOG_CHAT,"[General][%s][%s] - (Message: %s)",lpObj->Account,lpObj->Name,lpMsg->message);

	//CustomMove
	{
		CUSTOMMOVE_INFO CustomMoveInfo;

		if (gCustomMove->GetInfoByName(lpMsg->message, &CustomMoveInfo) != 0)
		{
			gCustomMove->GetMove(lpObj, CustomMoveInfo.Index);
		}

	}

	gFilter->CheckSyntax(lpMsg->message);

	if(lpMsg->message[0] == '/')
	{
		gCommandManager->ManagementCore(lpObj,lpMsg->message);
		return;
	}

	if(lpObj->ChatLimitTime > 0)
	{
		gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,269,lpObj->ChatLimitTime);
		return;
	}

	if((lpObj->Penalty & 1) == 1)
	{
		return;
	}

	if(lpObj->Permission[11] != 0)
	{
		return;
	}

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,sizeof(pMsg));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	memcpy(pMsg.message,lpMsg->message,sizeof(pMsg.message));

	if(lpMsg->message[0] == '!' && lpObj->Authority >= AUTHORITY_GAME_MASTER)
	{
		gNotice->GCNoticeSendToAll(0,0,0,0,0,0,"%s",&lpMsg->message[1]);
	}
	else if(lpMsg->message[0] == '~')
	{
		if(OBJECT_RANGE(lpObj->PartyNumber) != 0)
		{
			for(int n=0;n < MAX_PARTY_USER;n++)
			{
				if(OBJECT_RANGE(gParty->m_PartyInfo[lpObj->PartyNumber].Index[n]) != 0)
				{
					DataSend(gParty->m_PartyInfo[lpObj->PartyNumber].Index[n],(BYTE*)&pMsg,pMsg.header.size);
				}
			}
		}
	}
	else if(lpMsg->message[0] == '@')
	{
		if(lpObj->Guild != 0)
		{
			if(lpMsg->message[1] == '>')
			{
				if(lpObj->GuildRank == GUILD_STATUS_MASTER)
				{
					gGuild->GDGuildGlobalNoticeSend(lpObj->Guild->Name,&lpMsg->message[2]);
				}
			}
			else if(lpMsg->message[1] == '@')
			{
				if(lpObj->Guild->GuildUnion != 0)
				{
					gGuild->GDGuildGlobalChatSend(1,lpObj->Guild->Name,lpObj->Name,lpMsg->message);
				}
			}
			else
			{
				gGuild->GDGuildGlobalChatSend(0,lpObj->Guild->Name,lpObj->Name,lpMsg->message);
			}
		}
	}
	#if(GAMESERVER_UPDATE>=501)
	else if(lpMsg->message[0] == '$')
	{
		if(lpObj->GensFamily != 0)
		{
			for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
			{
				if(gObj[n].GensFamily == lpObj->GensFamily)
				{
					DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
				}
			}
		}
	}
	#endif
	else
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CGChatWhisperRecv(PMSG_CHAT_WHISPER_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	char name[11] = {0};

	memcpy(name,lpMsg->name,sizeof(lpMsg->name));

	if(lpObj->ChatLimitTime > 0)
	{
		gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,269,lpObj->ChatLimitTime);
		return;
	}

	if((lpObj->Penalty & 1) == 1)
	{
		return;
	}

	if(lpObj->Permission[11] != 0)
	{
		return;
	}

	gFilter->CheckSyntax(lpMsg->message);

	LPOBJ lpTarget = gObjFind(name);

	if(lpTarget == 0)
	{
		GDGlobalWhisperSend(aIndex,name,lpMsg->message);
		return;
	}

	if(aIndex == lpTarget->Index)
	{
		gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,270);
		return;
	}

	lpMsg->message[(sizeof(lpMsg->message)-1)] = 0;

	gLog->Output(LOG_CHAT,"[Whisper][%s][%s] - (Name: %s,Message: %s)",lpObj->Account,lpObj->Name,name,lpMsg->message);

	GCChatWhisperSend(lpTarget->Index,lpObj->Name,lpMsg->message);
}

void CGMainCheckRecv(PMSG_MAIN_CHECK_RECV* lpMsg,int aIndex) // OK
{
	
}

void CGWindowsEventOpen(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	lpObj->WindowTimeOpen = true;
}

void CGLiveClientRecv(PMSG_LIVE_CLIENT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED && lpObj->Connected != OBJECT_ONLINE)
	{
		return;
	}

	if(gServerInfo->m_CheckLatencyHack != 0 && abs((int)((GetTickCount()-lpObj->ServerTickCount)-(lpMsg->TickCount-lpObj->ClientTickCount))) > gServerInfo->m_CheckLatencyHackTolerance)
	{
		gLog->Output(LOG_HACK,"[%s][%s] Client Latency Hack Detected [%d][%d]",lpObj->Account,lpObj->Name,(GetTickCount()-lpObj->ServerTickCount),(lpMsg->TickCount-lpObj->ClientTickCount));
	
		if(gServerInfo->m_CheckLatencyHackAction == 1)
		{
			gObjUserKill(aIndex);
		}
		else if(gServerInfo->m_CheckLatencyHackAction == 2)
		{
			GCCloseClientSend(aIndex,0);
		}

		return;
	}

	if(lpObj->Connected == OBJECT_ONLINE && gServerInfo->m_CheckSpeedHack != 0 && lpMsg->PhysiSpeed > lpObj->PhysiSpeed && abs((int)(lpObj->PhysiSpeed-lpMsg->PhysiSpeed)) > gServerInfo->m_CheckSpeedHackTolerance)
	{
		gLog->Output(LOG_HACK,"[%s][%s] Client Speed Hack Detected [%d][%d]",lpObj->Account,lpObj->Name,lpObj->PhysiSpeed,lpMsg->PhysiSpeed);

		if(gServerInfo->m_CheckSpeedHackAction == 1)
		{
			gObjUserKill(aIndex);
		}
		else if(gServerInfo->m_CheckSpeedHackAction == 2)
		{
			GCCloseClientSend(aIndex,0);
		}

		return;
	}

	if(lpObj->Connected == OBJECT_ONLINE && gServerInfo->m_CheckSpeedHack != 0 && lpMsg->MagicSpeed > lpObj->MagicSpeed && abs((int)(lpObj->MagicSpeed-lpMsg->MagicSpeed)) > gServerInfo->m_CheckSpeedHackTolerance)
	{
		gLog->Output(LOG_HACK,"[%s][%s] Client Speed Hack Detected [%d][%d]",lpObj->Account,lpObj->Name,lpObj->MagicSpeed,lpMsg->MagicSpeed);

		if(gServerInfo->m_CheckSpeedHackAction == 1)
		{
			gObjUserKill(aIndex);
		}
		else if(gServerInfo->m_CheckSpeedHackAction == 2)
		{
			GCCloseClientSend(aIndex,0);
		}

		return;
	}

	lpObj->ConnectTickCount = GetTickCount();
}

void CGPositionRecv(PMSG_POSITION_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Teleport != 0)
	{
		return;
	}

	if(gObjCheckMapTile(lpObj,2) != 0)
	{
		return;
	}

	if(lpObj->WindowTimeOpen == true)
	{
		lpObj->WindowTimeOpen = false;
	}

	#if(GAMESERVER_UPDATE>=803)

	if((CC_MAP_RANGE(lpObj->Map) != 0 || CF_MAP_RANGE(lpObj->Map) != 0) && (GetTickCount()-lpObj->ChaosCastleBlowTime) < 1000)
	{
		return;
	}

	#else

	if(CC_MAP_RANGE(lpObj->Map) != 0 && (GetTickCount()-lpObj->ChaosCastleBlowTime) < 1000)
	{
		return;
	}

	#endif

	if(lpMsg->x < (lpObj->X-15) || lpMsg->x > (lpObj->X+15) || lpMsg->y < (lpObj->Y-15) || lpMsg->y > (lpObj->Y+15))
	{
		return;
	}

	gMap[lpObj->Map].DelStandAttr(lpObj->OldX,lpObj->OldY);

	lpObj->X = lpMsg->x;
	lpObj->Y = lpMsg->y;
	lpObj->TX = lpMsg->x;
	lpObj->TY = lpMsg->y;
	lpObj->OldX = lpMsg->x;
	lpObj->OldY = lpMsg->y;

	gMap[lpObj->Map].SetStandAttr(lpObj->TX,lpObj->TY);

	PMSG_POSITION_SEND pMsg;

	pMsg.header.set(PROTOCOL_CODE3,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.x = (BYTE)lpObj->TX;

	pMsg.y = (BYTE)lpObj->TY;

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index,(BYTE*)&pMsg,pMsg.header.size);
			}
		}
	}
}

void CGActionRecv(PMSG_ACTION_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	lpObj->Dir = lpMsg->dir;

	lpObj->ActionNumber = lpMsg->action;

	if(lpObj->ActionNumber == ACTION_SIT1)
	{
		lpObj->ViewState = VIEW_STATE_ACTION_1;
		lpObj->Rest = lpObj->ActionNumber;
	}
	else if(lpObj->ActionNumber == ACTION_POSE1)
	{
		lpObj->ViewState = VIEW_STATE_ACTION_2;
		lpObj->Rest = lpObj->ActionNumber;
	}
	else if(lpObj->ActionNumber == ACTION_HEALING1)
	{
		lpObj->ViewState = VIEW_STATE_ACTION_3;
		lpObj->Rest = lpObj->ActionNumber;
	}

	PMSG_ACTION_SEND pMsg;

	pMsg.header.set(0x18,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.dir = lpMsg->dir;

	pMsg.action = lpMsg->action;

	pMsg.target[0] = lpMsg->index[0];

	pMsg.target[1] = lpMsg->index[1];
	
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index,(BYTE*)&pMsg,pMsg.header.size);
			}
		}
	}
}

void CGEventRemainTimeRecv(PMSG_EVENT_REMAIN_TIME_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_EVENT_REMAIN_TIME_SEND pMsg;

	pMsg.header.set(0x91,sizeof(pMsg));

	pMsg.EventType = lpMsg->EventType;

	pMsg.RemainTimeH = 0;

	pMsg.EnteredUser = 0;

	pMsg.RemainTimeL = 0;

	switch(lpMsg->EventType)
	{
		case 1:
		{
			lpMsg->ItemLevel = gEventEntryLevel->GetDSLevel(&gObj[aIndex]);

			if(DS_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
			{
				if(gDevilSquare->GetState(lpMsg->ItemLevel) == DS_STATE_READY)
				{
					pMsg.EnteredUser = gDevilSquare->GetEnteredUserCount(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.RemainTimeH = gDevilSquare->GetRemainTime(lpMsg->ItemLevel);
				}
			}
			else
			{
				GCNewMessageSend(aIndex,0,0,"%s",gMessage->GetText(35,lpObj->Lang));
				return;
			}
		}break;
		case 2:
		{
			lpMsg->ItemLevel = gEventEntryLevel->GetBCLevel(&gObj[aIndex]);

			if(BC_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
			{
				if(gBloodCastle->GetState(lpMsg->ItemLevel) == BC_STATE_READY)
				{
					pMsg.EnteredUser = gBloodCastle->GetEnteredUserCount(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.RemainTimeH = gBloodCastle->GetRemainTime(lpMsg->ItemLevel);
				}
			}
			else
			{
				GCNewMessageSend(aIndex,0,0,"%s",gMessage->GetText(7,lpObj->Lang));
				return;
			}
		}break;
		case 4:
		{
			lpMsg->ItemLevel = gEventEntryLevel->GetCCLevel(&gObj[aIndex]);

			if(CC_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
			{
				if(gChaosCastle->GetState(lpMsg->ItemLevel) == CC_STATE_READY)
				{
					pMsg.EnteredUser = gChaosCastle->GetEnteredUserCount(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.RemainTimeL = gChaosCastle->GetRemainTime(lpMsg->ItemLevel);
				}
			}
			else
			{
				GCNewMessageSend(aIndex,0,0,"%s",gMessage->GetText(22,lpObj->Lang));
				return;
			}
		}break;
		case 5:
		{
			lpMsg->ItemLevel = gEventEntryLevel->GetITLevel(&gObj[aIndex]);

			if(IT_LEVEL_RANGE(lpMsg->ItemLevel) != 0)
			{
				if(gIllusionTemple->GetState(lpMsg->ItemLevel) == IT_STATE_READY)
				{
					pMsg.EnteredUser = gIllusionTemple->GetEnteredUserCount(lpMsg->ItemLevel);
				}
				else
				{
					pMsg.RemainTimeH = gIllusionTemple->GetRemainTime(lpMsg->ItemLevel);
				}
			}
			else
			{
				GCNewMessageSend(aIndex,0,0,"%s",gMessage->GetText(45,lpObj->Lang));
				return;
			}
		}break;
	}

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CGPetItemCommandRecv(PMSG_PET_ITEM_COMMAND_RECV* lpMsg,int aIndex) // OK
{
	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpMsg->type != 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	gDarkSpirit[aIndex].ChangeCommand(lpMsg->command,bIndex);
}

void CGPetItemInfoRecv(PMSG_PET_ITEM_INFO_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpMsg->type != 0 && lpMsg->type != 1)
	{
		return;
	}

	if(lpMsg->flag == 0) // Inventory
	{
		if(INVENTORY_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->Inventory[lpMsg->slot].m_PetItemLevel,lpObj->Inventory[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->Inventory[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 1) // Warehouse
	{
		if(WAREHOUSE_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(lpObj->LoadWarehouse == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->Warehouse[lpMsg->slot].m_PetItemLevel,lpObj->Warehouse[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->Warehouse[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 2) // Trade
	{
		if(TRADE_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_TRADE || lpObj->Interface.state == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->Trade[lpMsg->slot].m_PetItemLevel,lpObj->Trade[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->Trade[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 3) // Target Trade
	{
		if(TRADE_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_TRADE || lpObj->Interface.state == 0)
		{
			return;
		}

		if(OBJECT_RANGE(lpObj->TargetNumber) == 0)
		{
			return;
		}

		LPOBJ lpTarget = &gObj[lpObj->TargetNumber];

		if(lpTarget->Interface.use == 0 || lpTarget->Interface.type != INTERFACE_TRADE || lpTarget->Interface.state == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpTarget->Trade[lpMsg->slot].m_PetItemLevel,lpTarget->Trade[lpMsg->slot].m_PetItemExp,(BYTE)lpTarget->Trade[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 4) // Chaos Box
	{
		if(CHAOS_BOX_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpObj->ChaosBox[lpMsg->slot].m_PetItemLevel,lpObj->ChaosBox[lpMsg->slot].m_PetItemExp,(BYTE)lpObj->ChaosBox[lpMsg->slot].m_Durability);
	}
	else if(lpMsg->flag == 5) // Target Personal Shop
	{
		if(INVENTORY_SHOP_RANGE(lpMsg->slot) == 0)
		{
			return;
		}

		if(OBJECT_RANGE(lpObj->Store.DealerIndex) == 0)
		{
			return;
		}

		LPOBJ lpTarget = &gObj[lpObj->Store.DealerIndex];

		GCPetItemInfoSend(aIndex,lpMsg->type,lpMsg->flag,lpMsg->slot,lpTarget->Inventory[lpMsg->slot].m_PetItemLevel,lpTarget->Inventory[lpMsg->slot].m_PetItemExp,(BYTE)lpTarget->Inventory[lpMsg->slot].m_Durability);
	}
}

void CGMapServerMoveAuthRecv(PMSG_MAP_SERVER_MOVE_AUTH_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_CONNECTED)
	{
		CloseClient(aIndex);
		return;
	}

	if(memcmp(gServerInfo->m_ServerVersion,lpMsg->ClientVersion,sizeof(lpMsg->ClientVersion)) != 0)
	{
		GCMapServerMoveAuthSend(aIndex,6);
		return;
	}

	if(memcmp(gServerInfo->m_ServerSerial,lpMsg->ClientSerial,sizeof(lpMsg->ClientSerial)) != 0)
	{
		GCMapServerMoveAuthSend(aIndex,6);
		return;
	}

	if(lpObj->LoginMessageSend == 0)
	{
		lpObj->LoginMessageSend++;
		lpObj->LoginMessageCount++;
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->ClientTickCount = lpMsg->TickCount;
		lpObj->ServerTickCount = GetTickCount();

		char account[11] = {0};

		PacketArgumentDecrypt(account,lpMsg->account,sizeof(account));

		lpObj->MapServerMoveRequest = 1;

		GJMapServerMoveAuthSend(aIndex,account,lpMsg->name,lpMsg->AuthCode1,lpMsg->AuthCode2,lpMsg->AuthCode3,lpMsg->AuthCode4);
	}
}

void CGMoveRecv(PMSG_MOVE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->RegenOk > 0)
	{
		return;
	}

	if(lpObj->Teleport != 0)
	{
		return;
	}

	if((GetTickCount()-lpObj->LastMoveTime) < 100)
	{
		return;
	}

	if(gObjCheckMapTile(lpObj,1) != 0)
	{
		return;
	}

	if(gEffectManager->CheckStunEffect(lpObj) != 0 || gEffectManager->CheckImmobilizeEffect(lpObj) != 0)
	{
		return;
	}

	if(lpObj->WindowTimeOpen == true)
	{
		lpObj->WindowTimeOpen = false;
	}

	if(lpObj->SkillSummonPartyTime != 0)
	{
		lpObj->SkillSummonPartyTime = 0;
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,272);
	}

	lpObj->LastMoveTime = GetTickCount();
	lpObj->Dir = lpMsg->path[0] >> 4;
	lpObj->Rest = 0;
	lpObj->PathCur = 0;
	lpObj->PathCount = lpMsg->path[0] & 0x0F;

	memset(lpObj->PathX,0,sizeof(lpObj->PathX));

	memset(lpObj->PathY,0,sizeof(lpObj->PathY));

	memset(lpObj->PathOri,0,sizeof(lpObj->PathOri));

	lpObj->TX = lpMsg->x;
	lpObj->TY = lpMsg->y;
	lpObj->PathCur = ((lpObj->PathCount>0)?1:0);
	lpObj->PathCount = ((lpObj->PathCount>0)?(lpObj->PathCount+1):lpObj->PathCount);
	lpObj->PathStartEnd = 1;
	lpObj->PathX[0] = lpMsg->x;
	lpObj->PathY[0] = lpMsg->y;
	lpObj->PathDir[0] = lpObj->Dir;

	for(int n=1;n < lpObj->PathCount;n++)
	{
		if((n%2) == 0)
		{
			lpObj->TX = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+0];
			lpObj->TY = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+1];
			lpObj->PathX[n] = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+0];
			lpObj->PathY[n] = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)] & 0x0F)*2)+1];
			lpObj->PathOri[n-1] = lpMsg->path[((n+1)/2)] & 0x0F;
			lpObj->PathDir[n+0] = lpMsg->path[((n+1)/2)] & 0x0F;
		}
		else
		{
			lpObj->TX = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)]/0x10)*2)+0];
			lpObj->TY = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)]/0x10)*2)+1];
			lpObj->PathX[n] = lpObj->PathX[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)]/0x10)*2)+0];
			lpObj->PathY[n] = lpObj->PathY[n-1]+RoadPathTable[((lpMsg->path[((n+1)/2)]/0x10)*2)+1];
			lpObj->PathOri[n-1] = lpMsg->path[((n+1)/2)]/0x10;
			lpObj->PathDir[n+0] = lpMsg->path[((n+1)/2)]/0x10;
		}
	}

	if(lpObj->TX < (lpObj->X-15) || lpObj->TX > (lpObj->X+15) || lpObj->TY < (lpObj->Y-15) || lpObj->TY > (lpObj->Y+15))
	{
		lpObj->PathCur = 0;
		lpObj->PathCount = 0;
		lpObj->PathStartEnd = 0;
		memset(lpObj->PathX,0,sizeof(lpObj->PathX));
		memset(lpObj->PathY,0,sizeof(lpObj->PathY));
		memset(lpObj->PathOri,0,sizeof(lpObj->PathOri));
		gObjSetPosition(lpObj->Index,lpObj->X,lpObj->Y);
		return;
	}
	
	if(lpObj->PathCount > 0 && (gMap[lpObj->Map].CheckAttr(lpObj->TX,lpObj->TY,MAP_ATTR_LOCK1) != 0 || gMap[lpObj->Map].CheckAttr(lpObj->TX,lpObj->TY,MAP_ATTR_LOCK2) != 0))
	{
		lpObj->PathCur = 0;
		lpObj->PathCount = 0;
		lpObj->PathStartEnd = 0;
		memset(lpObj->PathX,0,sizeof(lpObj->PathX));
		memset(lpObj->PathY,0,sizeof(lpObj->PathY));
		memset(lpObj->PathOri,0,sizeof(lpObj->PathOri));
		gObjSetPosition(lpObj->Index,lpObj->X,lpObj->Y);
		return;
	}

	int nx = lpObj->X;
	int ny = lpObj->Y;

	if(lpObj->Type == OBJECT_USER && lpObj->Map == MAP_CASTLE_SIEGE)
	{
		bool result = 0;

		for(int n = 0; n < MAX_MAP_SIZE; n++)
		{
			if(nx > lpObj->TX) { nx -= 1; }
			if(nx < lpObj->TX) { nx += 1; }
			if(ny > lpObj->TY) { ny -= 1; }
			if(ny < lpObj->TY) { ny += 1; }

			if(gMap[lpObj->Map].CheckAttr(nx,ny,MAP_ATTR_LOCK3) != 0)
			{
				result = 1;
				break;
			}

			if (lpObj->TX == nx && lpObj->TY == ny)
			{
				break;
			}
		}

		if(result == 1)
		{
			lpObj->Rest = 1;
			lpObj->PathCur = 0;
			lpObj->PathCount = 0;
			lpObj->PathStartEnd = 0;
			gObjSetPosition(lpObj->Index,lpObj->X,lpObj->Y);

			PMSG_ACTION_SEND pMsg;

			pMsg.header.set(0x18,sizeof(pMsg));

			pMsg.index[0] = SET_NUMBERHB(lpObj->Index);

			pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

			pMsg.action = ACTION_STAND1;

			pMsg.dir = lpObj->Dir;

			pMsg.target[0] = 0;

			pMsg.target[1] = 0;

			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}
	}

	gMap[lpObj->Map].DelStandAttr(lpObj->OldX,lpObj->OldY);

	lpObj->X = lpMsg->x;
	lpObj->Y = lpMsg->y;
	lpObj->TX = lpObj->TX;
	lpObj->TY = lpObj->TY;
	lpObj->OldX = lpObj->TX;
	lpObj->OldY = lpObj->TY;
	lpObj->ViewState = VIEW_STATE_NONE;

	gMap[lpObj->Map].SetStandAttr(lpObj->TX,lpObj->TY);

	PMSG_MOVE_SEND pMsg;

	pMsg.header.set(PROTOCOL_CODE1,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.x = (BYTE)lpObj->TX;

	pMsg.y = (BYTE)lpObj->TY;

	pMsg.dir = lpObj->Dir << 4;

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(lpObj->VpPlayer2[n].state != OBJECT_EMPTY && lpObj->VpPlayer2[n].state != OBJECT_DIECMD && lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index,(BYTE*)&pMsg,pMsg.header.size);
			}
		}
	}

	#if(GAMESERVER_UPDATE>=803)

	if(IT_MAP_RANGE(lpObj->Map) != 0)
	{
		gIllusionTempleRenewal->StoneStatueCancel(lpObj);
	}

	#endif
}

void CGConnectAccountRecv(PMSG_CONNECT_ACCOUNT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_CONNECTED)
	{
		CloseClient(aIndex);
		return;
	}

	if(memcmp(gServerInfo->m_ServerVersion,lpMsg->ClientVersion,sizeof(lpMsg->ClientVersion)) != 0)
	{
		GCConnectAccountSend(aIndex,6);
		return;
	}

	if(memcmp(gServerInfo->m_ServerSerial,lpMsg->ClientSerial,sizeof(lpMsg->ClientSerial)) != 0)
	{
		GCConnectAccountSend(aIndex,6);
		return;
	}

	if(lpObj->LoginMessageSend == 0)
	{
		lpObj->LoginMessageSend++;
		lpObj->LoginMessageCount++;
		lpObj->ConnectTickCount = GetTickCount();
		lpObj->ClientTickCount = lpMsg->TickCount;
		lpObj->ServerTickCount = GetTickCount();

		char account[11] = {0};

		PacketArgumentDecrypt(account,lpMsg->account,(sizeof(account)-1));

		char password[11] = {0};

		PacketArgumentDecrypt(password,lpMsg->password,(sizeof(password)-1));

		lpObj->MapServerMoveRequest = 0;
		lpObj->LastServerCode = -1;
		lpObj->DestMap = -1;
		lpObj->DestX = 0;
		lpObj->DestY = 0;

		memcpy(lpObj->Password,password,sizeof(lpObj->Password));

		GJConnectAccountSend(aIndex,account,password,lpObj->IpAddr,lpObj->HardwareId);
	}
}

void CGCloseClientRecv(PMSG_CLOSE_CLIENT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED && lpObj->Connected != OBJECT_ONLINE)
	{
		return;
	}

	gObjectManager->CharacterGameCloseSet(aIndex,lpMsg->type);
}

void CGCharacterListRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	GDCharacterListSend(aIndex);
}

void CGCharacterCreateRecv(PMSG_CHARACTER_CREATE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	PMSG_CHARACTER_CREATE_SEND pMsg;

	pMsg.header.set(0xF3,0x01,sizeof(pMsg));

	pMsg.result = 0;

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	if(gServerInfo->m_CharacterCreateSwitch == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	char name[11] = {0};

	memcpy(name,lpMsg->name,sizeof(lpMsg->name));

	if(CheckNameSyntax(name) == 0)
	{
		gLog->Output(LOG_HACK,"[CheckTextSymbol][%s] Character Create error. [%s]",lpObj->Account,name);
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->Class != DB_CLASS_DW && lpMsg->Class != DB_CLASS_DK && lpMsg->Class != DB_CLASS_FE && lpMsg->Class != DB_CLASS_MG && lpMsg->Class != DB_CLASS_DL && lpMsg->Class != DB_CLASS_SU && lpMsg->Class != DB_CLASS_RF)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->Class == DB_CLASS_SU && (lpObj->ClassCode & CARD_CODE_SU) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->Class == DB_CLASS_MG && (lpObj->ClassCode & CARD_CODE_MG) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->Class == DB_CLASS_DL && (lpObj->ClassCode & CARD_CODE_DL) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->Class == DB_CLASS_RF && (lpObj->ClassCode & CARD_CODE_RF) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	GDCharacterCreateSend(aIndex,name,lpMsg->Class);
}

void CGCharacterDeleteRecv(PMSG_CHARACTER_DELETE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	if(lpObj->EnableDelCharacter == 0)
	{
		return;
	}

	PMSG_CHARACTER_DELETE_SEND pMsg;

	pMsg.header.set(0xF3,0x02,sizeof(pMsg));

	pMsg.result = 0;

	if(gServerInfo->m_CharacterDeleteSwitch == 0)
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(gObjCheckPersonalCode(aIndex,lpMsg->PersonalCode) == 0)
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	//itemLock
	if (lpObj->Lock > 0)
	{
		gNotice->GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 2, 0, gMessage->GetMessage(1778));
		pMsg.result = 2;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	char name[11] = {0};

	memcpy(name,lpMsg->name,sizeof(lpMsg->name));

	GDCharacterDeleteSend(aIndex,name);
}

void CGCharacterInfoRecv(PMSG_CHARACTER_INFO_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	lpObj->EnableDelCharacter = 0;
	
	char name[11] = {0};

	memcpy(name,lpMsg->name,sizeof(lpMsg->name));

	GDCharacterInfoSend(aIndex,name);
}

void CGLevelUpPointRecv(PMSG_LEVEL_UP_POINT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_LEVEL_UP_POINT_SEND pMsg;

	pMsg.header.set(0xF3,0x06,sizeof(pMsg));

	pMsg.result = 0;
	
	if(gObjectManager->CharacterLevelUpPointAdd(lpObj,lpMsg->type,1) != 0)
	{
		pMsg.result = 16+lpMsg->type;

		switch(lpMsg->type)
		{
			case 2: // Vitality
				pMsg.MaxLifeAndMana = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));
				break;
			case 3: // Energy
				pMsg.MaxLifeAndMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));
				break;
		}

		pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));

		#if(GAMESERVER_UPDATE>=201)
		pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));
		#endif

		#if(GAMESERVER_EXTRA==1)
		pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
		pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
		pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
		pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
		#if(GAMESERVER_UPDATE>=201)
		pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
		#endif
		pMsg.ViewStrength = lpObj->Strength;
		pMsg.ViewDexterity = lpObj->Dexterity;
		pMsg.ViewVitality = lpObj->Vitality;
		pMsg.ViewEnergy = lpObj->Energy;
		pMsg.ViewLeadership = lpObj->Leadership;
		#endif
	}

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CGCharacterMoveViewportEnableRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	lpObj->RegenOk = ((lpObj->RegenOk==1)?2:lpObj->RegenOk);
}

void CGCharacterNameCheckRecv(PMSG_CHARACTER_NAME_CHECK_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected != OBJECT_LOGGED)
	{
		return;
	}

	char name[11] = {0};

	memcpy(name,lpMsg->name,sizeof(lpMsg->name));

	GDCharacterNameCheckSend(aIndex,name);

	#endif
}

void CGOptionChangeSkinRecv(PMSG_OPTION_CHANGE_SKIN_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	lpObj->ChangeSkin = lpMsg->ChangeSkin;

	gObjViewportListProtocolCreate(lpObj);

	gObjectManager->CharacterUpdateMapEffect(lpObj);

	#endif
}

void CGOptionDataRecv(PMSG_OPTION_DATA_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	#if(GAMESERVER_UPDATE>=401)
	GDOptionDataSaveSend(aIndex,lpMsg->SkillKey,lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,lpMsg->RKey,lpMsg->QWERLevel);
	#else
	GDOptionDataSaveSend(aIndex,lpMsg->SkillKey,lpMsg->GameOption,lpMsg->QKey,lpMsg->WKey,lpMsg->EKey,lpMsg->ChatWindow,0,0);
	#endif
}

void CGClientSecurityBreachRecv(PMSG_CLIENT_SECURITY_BREACH_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	gLog->Output(LOG_HACK,"[ClientSecurityBreach][%s][%s] (Code: %d)",lpObj->Account,lpObj->Name,MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->code[0],lpMsg->code[1]),MAKE_NUMBERW(lpMsg->code[2],lpMsg->code[3])));

	#endif
}

void CGAcheronEnterRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_ACHERON_ENTER_SEND pMsg;

	pMsg.header.set(0xF8,0x21,sizeof(pMsg));

	if(gItemManager->GetInventoryItemCount(lpObj,GET_ITEM(13,146),0) == 0)
	{
		pMsg.result = 1;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	gItemManager->DeleteInventoryItemCount(lpObj,GET_ITEM(13,146),0,1);

	gObjMoveGate(aIndex,417);

	#endif
}

void CGAcheronGuardianEnterRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=801)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Teleport != 0)
	{
		return;
	}

	if(lpObj->UsePartyMatching != 0 || lpObj->UsePartyMatchingJoin != 0)
	{
		PMSG_PARTY_MATCHING_ERROR_SEND pMsg;

		pMsg.header.set(0xEF,0x09,sizeof(pMsg));

		pMsg.result = -1;

		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

		return;
	}

	if(gAcheronGuardian->GetState() != AG_STATE_START)
	{
		PMSG_ACHERON_ENTER_SEND pMsg;

		pMsg.header.set(0xF8,0x4C,sizeof(pMsg));

		pMsg.result = 1;

		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	gObjMoveGate(aIndex,426);

	#endif
}

void CGSNSDataRecv(PMSG_SNS_DATA_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=801)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	GDSNSDataSaveSend(aIndex,lpMsg->data);

	#endif
}

void CGSNSDataLogRecv(PMSG_SNS_DATA_LOG_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=801)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LogAdd(LOG_BLACK,"[SNSDataLog][%s][%s] - (Code: %d/%d/%d)",lpObj->Account,lpObj->Name,lpMsg->code[0],lpMsg->code[1],lpMsg->code[2]);

	#endif
}

void GCChatTargetSend(LPOBJ lpObj,int aIndex,char* message) // OK
{
	int size = strlen(message);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_TARGET_SEND pMsg;

	pMsg.header.set(0x01,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	memcpy(pMsg.message,message,size);

	pMsg.message[size] = 0;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCChatWhisperSend(int aIndex,char* name,char* message) // OK
{
	int size = strlen(message);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_WHISPER_SEND pMsg;

	pMsg.header.set(0x02,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,message,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMainCheckSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_MAIN_CHECK_SEND pMsg;

	pMsg.header.set(0x03,sizeof(pMsg));

	pMsg.key = (((GetLargeRand()%64)*16) | ((512 & 0x3F0)*64) | ((512 & 0x0F)) ^ 0xB479);

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCEventStateSend(int aIndex,BYTE state,BYTE event) // OK
{
	PMSG_EVENT_STATE_SEND pMsg;

	pMsg.header.set(0x0B,sizeof(pMsg));

	pMsg.state = state;
	
	pMsg.event = event;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCEventStateSendToAll(int Map,BYTE state,BYTE event) // OK
{
	PMSG_EVENT_STATE_SEND pMsg;

	pMsg.header.set(0x0B,sizeof(pMsg));

	pMsg.state = state;

	pMsg.event = event;

	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if(gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		if(Map == -1 || gObj[n].Map == Map)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void GCServerMsgSend(int aIndex,BYTE msg) // OK
{
	PMSG_SERVER_MSG_SEND pMsg;

	pMsg.header.set(0x0C,sizeof(pMsg));

	pMsg.MsgNumber = msg;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCWeatherSend(int aIndex,BYTE weather) // OK
{
	PMSG_WEATHER_SEND pMsg;

	pMsg.header.set(0x0F,sizeof(pMsg));
	
	pMsg.weather = weather;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCDamageSend(int aIndex,int bIndex,BYTE flag,int damage,int type,int ShieldDamage) // OK
{
	PMSG_DAMAGE_SEND pMsg;

	pMsg.header.set(PROTOCOL_CODE2,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(bIndex) | (flag*0x80);
	pMsg.index[1] = SET_NUMBERLB(bIndex);

	pMsg.damage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(damage));
	pMsg.damage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(damage));

	#if(GAMESERVER_UPDATE>=701)

	pMsg.type[0] = SET_NUMBERHB(type);
	pMsg.type[1] = SET_NUMBERLB(type);

	#else

	pMsg.type = type;

	#endif

	#if(GAMESERVER_UPDATE>=201)
	
	pMsg.ShieldDamage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(ShieldDamage));
	pMsg.ShieldDamage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(ShieldDamage));
	
	#endif

	#if(GAMESERVER_UPDATE>=701)

	pMsg.attribute = 0;

	#endif

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewCurHP = (DWORD)gObj[bIndex].Life;
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewCurSD = (DWORD)gObj[bIndex].Shield;
	#endif
	pMsg.ViewDamageHP = damage;
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewDamageSD = ShieldDamage;
	#endif
	#endif

	if(gObj[aIndex].Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	if(gObj[bIndex].Type == OBJECT_USER)
	{
		DataSend(bIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void GCUserDieSend(LPOBJ lpObj,int aIndex,int skill,int bIndex) // OK
{
	PMSG_USER_DIE_SEND pMsg;

	pMsg.header.set(0x17,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	#if(GAMESERVER_UPDATE>=401)
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	#else
	pMsg.skill = skill;
	#endif

	pMsg.killer[0] = SET_NUMBERHB(bIndex);
	pMsg.killer[1] = SET_NUMBERLB(bIndex);

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCActionSend(LPOBJ lpObj,BYTE action,int aIndex,int bIndex) // OK
{
	PMSG_ACTION_SEND pMsg;

	pMsg.header.set(0x18,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.dir = lpObj->Dir;

	pMsg.action = action;

	pMsg.target[0] = SET_NUMBERHB(bIndex);

	pMsg.target[1] = SET_NUMBERLB(bIndex);

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMoneySend(int aIndex,DWORD money) // OK
{
	PMSG_ITEM_GET_SEND pMsg;

	pMsg.header.setE(0x22,sizeof(pMsg));

	pMsg.result = 0xFE;

	memset(pMsg.ItemInfo,0,sizeof(pMsg.ItemInfo));

	pMsg.ItemInfo[0] = SET_NUMBERHB(SET_NUMBERHW(money));
	pMsg.ItemInfo[1] = SET_NUMBERLB(SET_NUMBERHW(money));
	pMsg.ItemInfo[2] = SET_NUMBERHB(SET_NUMBERLW(money));
	pMsg.ItemInfo[3] = SET_NUMBERLB(SET_NUMBERLW(money));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCLifeSend(int aIndex,BYTE type,int life,int shield) // OK
{
	PMSG_LIFE_SEND pMsg;

	pMsg.header.set(0x26,sizeof(pMsg));

	pMsg.type = type;

	pMsg.life[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(life));
	pMsg.life[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(life));

	pMsg.flag = 0;

	#if(GAMESERVER_UPDATE>=201)
	pMsg.shield[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(shield));
	pMsg.shield[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(shield));
	#endif

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewHP = life;
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewSD = shield;
	#endif
	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCManaSend(int aIndex,BYTE type,int mana,int bp) // OK
{
	PMSG_MANA_SEND pMsg;

	pMsg.header.set(0x27,sizeof(pMsg));

	pMsg.type = type;

	pMsg.mana[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(mana));
	pMsg.mana[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(mana));

	pMsg.bp[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(bp));
	pMsg.bp[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(bp));

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewMP = mana;
	pMsg.ViewBP = bp;
	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCItemUseSpecialTimeSend(int aIndex,BYTE number,int time) // OK
{
	PMSG_ITEM_SPECIAL_TIME_SEND pMsg;

	pMsg.header.setE(0x29,sizeof(pMsg));

	pMsg.number = number;

	pMsg.time = time;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMapAttrSend(int aIndex,BYTE type,BYTE attr,BYTE flag,BYTE count,PMSG_MAP_ATTR* lpInfo) // OK
{
	BYTE send[256];

	PMSG_MAP_ATTR_SEND pMsg;

	pMsg.header.set(0x46,0);

	int size = sizeof(pMsg);

	pMsg.type = type;

	pMsg.attr = attr;

	pMsg.flag = flag;

	pMsg.count = count;

	PMSG_MAP_ATTR info;

	for(int n=0;n < count;n++)
	{
		info.x = lpInfo[n].x;
		info.y = lpInfo[n].y;
		info.tx = lpInfo[n].tx;
		info.ty = lpInfo[n].ty;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);
	}

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}

void GCPartyItemInfoSend(int aIndex,CItem* lpItem) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return;
	}

	PMSG_PARTY_ITEM_INFO_SEND pMsg;

	pMsg.header.set(0x47,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.ItemInfo = lpItem->m_Index & 0x1FFF;

	if(lpItem->m_Option1 != 0)
	{
		pMsg.ItemInfo |= 0x2000;
	}

	if(lpItem->m_Option2 != 0)
	{
		pMsg.ItemInfo |= 0x4000;
	}

	if(lpItem->m_Option3 != 0)
	{
		pMsg.ItemInfo |= 0x8000;
	}

	if(lpItem->IsExcItem() != 0)
	{
		pMsg.ItemInfo |= 0x10000;
	}

	if(lpItem->IsSetItem() != 0)
	{
		pMsg.ItemInfo |= 0x20000;
	}

	if(lpItem->Is380Item() != 0)
	{
		pMsg.ItemInfo |= 0x40000;
	}

	pMsg.level = (BYTE)lpItem->m_Level;

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		int index = gParty->m_PartyInfo[lpObj->PartyNumber].Index[n];

		if(OBJECT_RANGE(index) != 0)
		{
			DataSend(index,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void GCEffectInfoSend(int aIndex,BYTE effect) // OK
{
	PMSG_EFFECT_INFO_SEND pMsg;

	pMsg.header.set(0x48,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.effect = effect;

	#if(GAMESERVER_UPDATE>=201)

	if(effect == 17)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	#endif

	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void GCGuildMasterQuestionSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Interface.use != 0)
	{
		return;
	}

	PBMSG_HEAD pMsg;

	pMsg.set(0x54,sizeof(pMsg));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.size);
}

void GCMonsterSkillSend(LPOBJ lpObj,LPOBJ lpTarget,int skill) // OK
{
	PMSG_MONSTER_SKILL_SEND pMsg;

	pMsg.header.set(0x69,sizeof(pMsg));

	#if(GAMESERVER_UPDATE>=401)
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	#else
	pMsg.skill = skill;
	#endif

	pMsg.index = lpObj->Index;

	pMsg.target = lpTarget->Index;

	if(lpObj->Index == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMonsterDieSend(int aIndex,int bIndex,int experience,int damage,BYTE flag) // OK
{
	PMSG_REWARD_EXPERIENCE_SEND pMsg;

	pMsg.header.setE(0x9C,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(bIndex) | (flag*0x80);
	pMsg.index[1] = SET_NUMBERLB(bIndex);

	pMsg.experience[0] = SET_NUMBERHW(experience);
	pMsg.experience[1] = SET_NUMBERLW(experience);

	pMsg.damage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(damage));
	pMsg.damage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(damage));

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewDamageHP = damage;
	pMsg.ViewExperience = gObj[aIndex].Experience;
	pMsg.ViewNextExperience = gObj[aIndex].NextExperience;
	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCRewardExperienceSend(int aIndex,int experience) // OK
{
	PMSG_REWARD_EXPERIENCE_SEND pMsg;

	pMsg.header.setE(0x9C,sizeof(pMsg));

	pMsg.index[0] = 0xFF;
	pMsg.index[1] = 0xFF;

	pMsg.experience[0] = SET_NUMBERHW(experience);
	pMsg.experience[1] = SET_NUMBERLW(experience);

	pMsg.damage[0] = 0;
	pMsg.damage[1] = 0;

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewDamageHP = 0;
	pMsg.ViewExperience = gObj[aIndex].Experience;
	pMsg.ViewNextExperience = gObj[aIndex].NextExperience;
	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCEventEnterCountSend(int aIndex,BYTE EventType,BYTE EnterCount) // OK
{
	PMSG_EVENT_ENTER_COUNT_SEND pMsg;

	pMsg.header.setE(0x9F,sizeof(pMsg));

	pMsg.EventType = EventType;

	pMsg.EnterCount = EnterCount;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCPetItemInfoSend(int aIndex,BYTE type,BYTE flag,BYTE slot,BYTE level,DWORD experience,BYTE durability) // OK
{
	PMSG_PET_ITEM_INFO_SEND pMsg;

	pMsg.header.set(0xA9,sizeof(pMsg));

	pMsg.type = type;

	pMsg.flag = flag;

	pMsg.slot = slot;

	pMsg.level = level;

	pMsg.experience = experience;

	#if(GAMESERVER_UPDATE>=401)

	pMsg.durability = durability;

	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMapServerMoveAuthSend(int aIndex,BYTE result) // OK
{
	PMSG_MAP_SERVER_MOVE_AUTH_SEND pMsg;

	pMsg.header.set(0xB1,0x01,sizeof(pMsg));

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCTaxInfoSend(int aIndex,BYTE type,BYTE rate) // OK
{
	PMSG_TAX_INFO_SEND pMsg;

	pMsg.header.set(0xB2,0x1A,sizeof(pMsg));

	pMsg.type = type;

	pMsg.rate = rate;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCCreationStateSend(int aIndex,BYTE state) // OK
{
	PMSG_CREATION_STATE_SEND pMsg;

	pMsg.header.set(0xB9,0x01,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.state = state;

	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void GCMonsterAreaSkillSend(int aIndex,int MonsterClass,int sx,int sy,int tx,int ty) // OK
{
	PMSG_MONSTER_AREA_SKILL_SEND pMsg;

	pMsg.header.set(0xBD,0x0C,sizeof(pMsg));

	pMsg.MonsterClass[0] = SET_NUMBERHB(MonsterClass);

	pMsg.MonsterClass[1] = SET_NUMBERLB(MonsterClass);

	pMsg.sx = sx;

	pMsg.sy = sy;

	pMsg.tx = tx;

	pMsg.ty = ty;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCElementalDamageSend(int aIndex,int bIndex,BYTE attribute,int damage) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	PMSG_ELEMENTAL_DAMAGE_SEND pMsg;

	pMsg.header.set(0xD8,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(bIndex);

	pMsg.index[1] = SET_NUMBERLB(bIndex);

	pMsg.attribute = attribute;

	pMsg.damage = damage;

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewCurHP = (DWORD)gObj[bIndex].Life;
	pMsg.ViewCurSD = (DWORD)gObj[bIndex].Shield;
	#endif

	if(gObj[aIndex].Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	if(gObj[bIndex].Type == OBJECT_USER)
	{
		DataSend(bIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	#endif
}

void GCCharacterCreationEnableSend(int aIndex,BYTE flag,BYTE result) // OK
{
	PMSG_CHARACTER_CREATION_ENABLE_SEND pMsg;

	pMsg.header.set(0xDE,sizeof(pMsg));

	pMsg.flag = flag;
	pMsg.result = result;

	#if(GAMESERVER_EXTRA==1)
	pMsg.ExperienceMultiplierConstA = gServerInfo->m_ExperienceMultiplierConstA;
	pMsg.ExperienceMultiplierConstB = gServerInfo->m_ExperienceMultiplierConstB;
	pMsg.CharacterMaxLevel = gServerInfo->m_MaxLevel;
	pMsg.CharacterDeleteMaxLevel = gServerInfo->m_CharacterDeleteMaxLevel;
	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCLifeUpdateSend(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	if(lpObj->Type != OBJECT_MONSTER)
	{
		return;
	}

	PMSG_LIFE_UPDATE_SEND pMsg;

	pMsg.header.set(0xEC,0x10,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	pMsg.MaxHP[0] = SET_NUMBERHB(SET_NUMBERHW((lpObj->MaxLife+lpObj->AddLife)));
	pMsg.MaxHP[1] = SET_NUMBERHB(SET_NUMBERLW((lpObj->MaxLife+lpObj->AddLife)));
	pMsg.MaxHP[2] = SET_NUMBERLB(SET_NUMBERHW((lpObj->MaxLife+lpObj->AddLife)));
	pMsg.MaxHP[3] = SET_NUMBERLB(SET_NUMBERLW((lpObj->MaxLife+lpObj->AddLife)));

	pMsg.CurHP[0] = SET_NUMBERHB(SET_NUMBERHW((lpObj->Life)));
	pMsg.CurHP[1] = SET_NUMBERHB(SET_NUMBERLW((lpObj->Life)));
	pMsg.CurHP[2] = SET_NUMBERLB(SET_NUMBERHW((lpObj->Life)));
	pMsg.CurHP[3] = SET_NUMBERLB(SET_NUMBERLW((lpObj->Life)));

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void GCConnectClientSend(int aIndex,BYTE result) // OK
{
	PMSG_CONNECT_CLIENT_SEND pMsg;

	pMsg.header.set(0xF1,0x00,sizeof(pMsg));

	pMsg.result = result;

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.ClientVersion[0] = gServerInfo->m_ServerVersion[0];

	pMsg.ClientVersion[1] = gServerInfo->m_ServerVersion[1];

	pMsg.ClientVersion[2] = gServerInfo->m_ServerVersion[2];

	pMsg.ClientVersion[3] = gServerInfo->m_ServerVersion[3];

	pMsg.ClientVersion[4] = gServerInfo->m_ServerVersion[4];

	#if(GAMESERVER_EXTRA==1)
	
	pMsg.ServerCode = gServerInfo->m_ServerCode;

	#endif

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	gObj[aIndex].ConnectTickCount = GetTickCount();
}

void GCConnectAccountSend(int aIndex,BYTE result) // OK
{
	PMSG_CONNECT_ACCOUNT_SEND pMsg;

	pMsg.header.set(0xF1,0x01,sizeof(pMsg));

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCConnectAccountSend(int aIndex,BYTE result,SOCKET socket) // OK
{
	PMSG_CONNECT_ACCOUNT_SEND pMsg;

	pMsg.header.set(0xF1,0x01,sizeof(pMsg));

	pMsg.result = result;

	DataSendSocket(socket,(BYTE*)&pMsg,pMsg.header.size);
}

void GCCloseClientSend(int aIndex,BYTE result) // OK
{
	PMSG_CLOSE_CLIENT_SEND pMsg;

	pMsg.header.setE(0xF1,0x02,sizeof(pMsg));

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCCharacterRegenSend(LPOBJ lpObj) // OK
{
	PMSG_CHARACTER_REGEN_SEND pMsg;

	pMsg.header.setE(0xF3,0x04,sizeof(pMsg));

	pMsg.X = (BYTE)lpObj->X;

	pMsg.Y = (BYTE)lpObj->Y;

	pMsg.Map = lpObj->Map;

	pMsg.Dir = lpObj->Dir;

	pMsg.Life = GET_MAX_WORD_VALUE(lpObj->Life);

	pMsg.Mana = GET_MAX_WORD_VALUE(lpObj->Mana);
	
	#if(GAMESERVER_UPDATE>=201)
	
	pMsg.Shield = GET_MAX_WORD_VALUE(lpObj->Shield);

	#endif

	pMsg.BP = GET_MAX_WORD_VALUE(lpObj->BP);

	#if(GAMESERVER_UPDATE>=401)

	if(gMasterSkillTree->CheckMasterLevel(lpObj) == 0)
	{
		pMsg.Experience[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->Experience)));
		pMsg.Experience[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->Experience)));
		pMsg.Experience[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->Experience)));
		pMsg.Experience[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->Experience)));
		pMsg.Experience[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->Experience)));
	}
	else
	{
		pMsg.Experience[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterExperience)));
		pMsg.Experience[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterExperience)));
		pMsg.Experience[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterExperience)));
		pMsg.Experience[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterExperience)));
		pMsg.Experience[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterExperience)));
	}

	#else

	pMsg.Experience = lpObj->Experience;

	#endif

	pMsg.Money = lpObj->Money;

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewCurHP = (DWORD)(lpObj->Life);
	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);
	pMsg.ViewCurBP = (DWORD)(lpObj->BP);
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);
	#endif
	#endif

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void GCLevelUpSend(LPOBJ lpObj) // OK
{
	PMSG_LEVEL_UP_SEND pMsg;

	pMsg.header.set(0xF3,0x05,sizeof(pMsg));

	pMsg.Level = lpObj->Level;

	pMsg.LevelUpPoint = lpObj->LevelUpPoint;

	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));

	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));

	#if(GAMESERVER_UPDATE>=201)
	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));
	#endif

	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));

	pMsg.FruitAddPoint = lpObj->FruitAddPoint;

	pMsg.MaxFruitAddPoint = gFruit->GetMaxFruitPoint(lpObj);

	pMsg.FruitSubPoint = lpObj->FruitSubPoint;

	pMsg.MaxFruitSubPoint = gFruit->GetMaxFruitPoint(lpObj);

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
	#endif
	pMsg.ViewExperience = lpObj->Experience;
	pMsg.ViewNextExperience = lpObj->NextExperience;
	#endif

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCEffectInfoSend(lpObj->Index,16);
}

void GCMonsterDamageSend(int aIndex,int damage,int ShieldDamage) // OK
{
	PMSG_MONSTER_DAMAGE_SEND pMsg;

	pMsg.header.set(0xF3,0x07,sizeof(pMsg));

	pMsg.damage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(damage));

	pMsg.damage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(damage));

	#if(GAMESERVER_UPDATE>=201)
	
	pMsg.ShieldDamage[0] = SET_NUMBERHB(GET_MAX_WORD_VALUE(ShieldDamage));

	pMsg.ShieldDamage[1] = SET_NUMBERLB(GET_MAX_WORD_VALUE(ShieldDamage));
	
	#endif

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewCurHP = (DWORD)gObj[aIndex].Life;
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewCurSD = (DWORD)gObj[aIndex].Shield;
	#endif
	pMsg.ViewDamageHP = damage;
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewDamageSD = ShieldDamage;
	#endif
	#endif

	if(gObj[aIndex].Type == OBJECT_USER)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void GCPKLevelSend(int aIndex,int PKLevel) // OK
{
	PMSG_PK_LEVEL_SEND pMsg;

	pMsg.header.set(0xF3,0x08,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	pMsg.PKLevel = PKLevel;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void GCSummonLifeSend(int aIndex,int life,int MaxLife) // OK
{
	if(MaxLife <= 0)
	{
		return;
	}

	PMSG_SUMMON_LIFE_SEND pMsg;

	pMsg.header.set(0xF3,0x20,sizeof(pMsg));

	pMsg.life = (life*100)/MaxLife;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCTimeViewSend(int aIndex,int time) // OK
{
	PMSG_TIME_VIEW_SEND pMsg;

	pMsg.header.set(0xF3,0x22,sizeof(pMsg));

	pMsg.time = time;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCMathAuthenticatorSend(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	PMSG_MATH_AUTHENTICATOR_SEND pMsg;

	pMsg.header.set(0xF3,0x32,sizeof(pMsg));

	pMsg.function = GetLargeRand()%6;

	pMsg.value = (float)(1+(GetLargeRand()%9));

	memset(pMsg.result,0,sizeof(pMsg.result));

	MAPM mapm = pMsg.value;

	switch(pMsg.function)
	{
		case 0:
			mapm = mapm.sin();
			break;
		case 1:
			mapm = mapm.cos();
			break;
		case 2:
			mapm = mapm.tan();
			break;
		case 3:
			mapm = mapm.sqrt();
			break;
		case 4:
			mapm = mapm.log();
			break;
		case 5:
			mapm = mapm.exp();
			break;
	}

	mapm.toString(pMsg.result,5);

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void GCServerCommandSend(int aIndex,int type,int x,int y,int Viewport) // OK
{
	PMSG_SERVER_COMMAND_SEND pMsg;

	pMsg.header.set(0xF3,0x40,sizeof(pMsg));

	pMsg.type = type;

	pMsg.x = x;

	pMsg.y = y;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	if(Viewport != 0){MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);}
}

void GCNewCharacterInfoSend(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_EXTRA==1)

	PMSG_NEW_CHARACTER_INFO_SEND pMsg;

	pMsg.header.set(0xF3,0xE0,sizeof(pMsg));

	pMsg.Level = lpObj->Level;
	pMsg.LevelUpPoint = lpObj->LevelUpPoint;
	pMsg.Experience = lpObj->Experience;
	pMsg.NextExperience = lpObj->NextExperience;
	pMsg.Strength = GET_MAX_WORD_VALUE(lpObj->Strength);
	pMsg.Dexterity = GET_MAX_WORD_VALUE(lpObj->Dexterity);
	pMsg.Vitality = GET_MAX_WORD_VALUE(lpObj->Vitality);
	pMsg.Energy = GET_MAX_WORD_VALUE(lpObj->Energy);
	pMsg.Leadership = GET_MAX_WORD_VALUE(lpObj->Leadership);
	pMsg.Life = GET_MAX_WORD_VALUE(lpObj->Life);
	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));
	pMsg.Mana = GET_MAX_WORD_VALUE(lpObj->Mana);
	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));
	pMsg.BP = GET_MAX_WORD_VALUE(lpObj->BP);
	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));
	#if(GAMESERVER_UPDATE>=201)
	pMsg.Shield = GET_MAX_WORD_VALUE(lpObj->Shield);
	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));
	#endif
	pMsg.FruitAddPoint = lpObj->FruitAddPoint;
	pMsg.MaxFruitAddPoint = gFruit->GetMaxFruitPoint(lpObj);
	pMsg.FruitSubPoint = lpObj->FruitSubPoint;
	pMsg.MaxFruitSubPoint = gFruit->GetMaxFruitPoint(lpObj);

	//EXTRA
	pMsg.ViewClass = (WORD)lpObj->Class;
	pMsg.ViewReset = (DWORD)(lpObj->Reset);
	pMsg.ViewMReset = (DWORD)(lpObj->MasterReset);
	pMsg.ViewAccountLevel = (DWORD)(lpObj->AccountLevel);
	char* VipName[] = { gMessage->GetMessage(746), gMessage->GetMessage(747), gMessage->GetMessage(748), gMessage->GetMessage(749) };
	memcpy(pMsg.ViewVipName, VipName[lpObj->AccountLevel], sizeof(pMsg.ViewVipName));
	memcpy(pMsg.ViewAccountExpireDate, lpObj->AccountExpireDate, sizeof(pMsg.ViewAccountExpireDate));
#if(GAMESERVER_SHOP == 2)
	pMsg.ViewCoin1 = (DWORD)(lpObj->Coin1);
#endif
	pMsg.ViewAuthority = (DWORD)(lpObj->Authority);
	pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
	pMsg.ViewCurHP = (DWORD)(lpObj->Life);
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewCurBP = (DWORD)(lpObj->BP);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	#if(GAMESERVER_UPDATE>=201)
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
	#endif
	pMsg.ViewStrength = lpObj->Strength;
	pMsg.ViewDexterity = lpObj->Dexterity;
	pMsg.ViewVitality = lpObj->Vitality;
	pMsg.ViewEnergy = lpObj->Energy;
	pMsg.ViewLeadership = lpObj->Leadership;

	#if(GAMESERVER_UPDATE>=401)
	pMsg.ViewMasterExperience = lpObj->MasterExperience;
	pMsg.ViewMasterNextExperience = lpObj->MasterNextExperience;
	#endif

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void GCNewCharacterCalcSend(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_EXTRA==1)

	PMSG_NEW_CHARACTER_CALC_SEND pMsg;

	pMsg.header.set(0xF3,0xE1,sizeof(pMsg));

	pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);

	pMsg.ViewCurHP = (DWORD)(lpObj->Life);

	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);

	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);

	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);

	pMsg.ViewCurBP = (DWORD)(lpObj->BP);

	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);

	#if(GAMESERVER_UPDATE>=201)
	
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);

	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);

	#endif

	pMsg.ViewStrength = lpObj->Strength;

	pMsg.ViewDexterity = lpObj->Dexterity;

	pMsg.ViewVitality = lpObj->Vitality;

	pMsg.ViewEnergy = lpObj->Energy;

	pMsg.ViewLeadership = lpObj->Leadership;

	pMsg.ViewAddStrength = lpObj->AddStrength;

	pMsg.ViewAddDexterity = lpObj->AddDexterity;

	pMsg.ViewAddVitality = lpObj->AddVitality;

	pMsg.ViewAddEnergy = lpObj->AddEnergy;

	pMsg.ViewAddLeadership = lpObj->AddLeadership;

	gAttack->GetPreviewPhysiDamage(lpObj,&pMsg.ViewPhysiDamageMin,&pMsg.ViewPhysiDamageMax,&pMsg.ViewMulPhysiDamage,&pMsg.ViewDivPhysiDamage);

	gAttack->GetPreviewMagicDamage(lpObj,&pMsg.ViewMagicDamageMin,&pMsg.ViewMagicDamageMax,&pMsg.ViewMulMagicDamage,&pMsg.ViewDivMagicDamage,&pMsg.ViewMagicDamageRate);

	gAttack->GetPreviewCurseDamage(lpObj,&pMsg.ViewCurseDamageMin,&pMsg.ViewCurseDamageMax,&pMsg.ViewMulCurseDamage,&pMsg.ViewDivCurseDamage,&pMsg.ViewCurseDamageRate);

	pMsg.ViewPhysiSpeed = lpObj->PhysiSpeed;

	pMsg.ViewMagicSpeed = lpObj->MagicSpeed;

	pMsg.FruitAddPoint = lpObj->FruitAddPoint;
	
	pMsg.MaxFruitAddPoint = gFruit->GetMaxFruitPoint(lpObj);
	
	pMsg.FruitSubPoint = lpObj->FruitSubPoint;
	
	pMsg.MaxFruitSubPoint = gFruit->GetMaxFruitPoint(lpObj);

	pMsg.ViewAttackSuccessRate = lpObj->AttackSuccessRate;

	pMsg.ViewAttackSuccessRate += lpObj->EffectOption.AddAttackSuccessRate;

	pMsg.ViewAttackSuccessRate += (pMsg.ViewAttackSuccessRate*lpObj->EffectOption.MulAttackSuccessRate)/100;

	pMsg.ViewAttackSuccessRate -= (pMsg.ViewAttackSuccessRate*lpObj->EffectOption.DivAttackSuccessRate)/100;

	pMsg.ViewAttackSuccessRatePvP = lpObj->AttackSuccessRatePvP;

	gAttack->GetPreviewDefense(lpObj,&pMsg.ViewDefense);

	pMsg.ViewDefenseSuccessRate = lpObj->DefenseSuccessRate;

	pMsg.ViewDefenseSuccessRate += lpObj->EffectOption.AddDefenseSuccessRate;

	pMsg.ViewDefenseSuccessRate += (pMsg.ViewDefenseSuccessRate*lpObj->EffectOption.MulDefenseSuccessRate)/100;

	pMsg.ViewDefenseSuccessRate -= (pMsg.ViewDefenseSuccessRate*lpObj->EffectOption.DivDefenseSuccessRate)/100;

	pMsg.ViewDefenseSuccessRatePvP = lpObj->DefenseSuccessRatePvP;

	gAttack->GetPreviewDamageMultiplier(lpObj,&pMsg.ViewDamageMultiplier,&pMsg.ViewRFDamageMultiplierA,&pMsg.ViewRFDamageMultiplierB,&pMsg.ViewRFDamageMultiplierC);

	gDarkSpirit[lpObj->Index].GetPreviewAttackDamage(lpObj,&pMsg.ViewDarkSpiritAttackDamageMin,&pMsg.ViewDarkSpiritAttackDamageMax,&pMsg.ViewDarkSpiritAttackSpeed,&pMsg.ViewDarkSpiritAttackSuccessRate);

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void GCNewHealthBarSend(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_EXTRA==1)

	#if(GAMESERVER_UPDATE<=603)

	if(gMapManager->GetMapMonsterHPBar(lpObj->Map) == 0)
	{
		return;
	}

	BYTE send[4096];

	PMSG_NEW_HEALTH_BAR_SEND pMsg;

	pMsg.header.set(0xF3,0xE2,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_NEW_HEALTH_BAR info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND && lpObj->VpPlayer[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(lpTarget->Live == 0 || lpTarget->State != OBJECT_PLAYING || OBJECT_RANGE(lpTarget->SummonIndex) != 0)
		{
			continue;
		}

		if(gCustomHealthBar->CheckHealthBar(lpTarget->Class,lpTarget->Map) == 0)
		{
			continue;
		}

		info.index = lpTarget->Index;

		info.type = (BYTE)lpTarget->Type;

		info.hprate = (BYTE)((lpTarget->Life*100)/(lpTarget->MaxLife+lpTarget->AddLife));

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);

	#endif

	#endif
}

void GCNewGensBattleInfoSend(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_EXTRA==1)

	#if(GAMESERVER_UPDATE>=501)

	PMSG_NEW_GENS_BATTLE_INFO_SEND pMsg;

	pMsg.header.set(0xF3,0xE3,sizeof(pMsg));

	pMsg.GensBattleMapCount = gGensSystem->GetGensBattleMap(pMsg.GensBattleMap);

	pMsg.GensMoveIndexCount = gGensSystem->GetGensMoveIndex(pMsg.GensMoveIndex);

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif

	#endif
}

void GCNewMessageSend(int aIndex,int type,int color,char* message,...) // OK
{
	#if(GAMESERVER_EXTRA==1)

	char buff[128];

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	#if(GAMESERVER_UPDATE==603)

	gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,"%s",buff);

	#else

	PMSG_NEW_MESSAGE_SEND pMsg;

	pMsg.header.set(0xF3,0xE4,sizeof(pMsg));

	pMsg.type = type;

	pMsg.color = color;

	memcpy(pMsg.message,buff,sizeof(pMsg.message));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	#endif

	#endif
}

void GCNewMessageSend(int aIndex,int type,int color,int message,...) // OK
{
	#if(GAMESERVER_EXTRA==1)

	char buff[128] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,gMessage->GetText(message,gObj[aIndex].Lang),arg);
	va_end(arg);

	#if(GAMESERVER_UPDATE==603)

	gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,"%s",buff);

	#else

	PMSG_NEW_MESSAGE_SEND pMsg;

	pMsg.header.set(0xF3,0xE4,sizeof(pMsg));

	pMsg.type = type;

	pMsg.color = color;

	memcpy(pMsg.message,buff,sizeof(pMsg.message));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	#endif

	#endif
}

void GCNewMessageSendToAll(int type,int color,char* message,...) // OK
{
	#if(GAMESERVER_EXTRA==1)

	char buff[128];

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	#if(GAMESERVER_UPDATE==603)

	gNotice->GCNoticeSendToAll(1,0,0,0,0,0,"%s",buff);

	#else

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(n,type,color,"%s",buff);
		}
	}

	#endif

	#endif
}

void GCWindowsNameSend(int aIndex,char* message,...) // OK
{
	char buff[128] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	PMSG_WINDOW_NAME_SEND pMsg;

	pMsg.header.set(0xF3,0xE8,sizeof(pMsg));

	memcpy(pMsg.title,buff,sizeof(pMsg.title));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void GCFireworksSend(LPOBJ lpObj, int x, int y) // OK
{
	PMSG_FIREWORKS_SEND pMsg;

	pMsg.header.set(0xF3, 0x40, sizeof(pMsg));

	pMsg.type = 0;

	pMsg.x = x;

	pMsg.y = y;

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}

void GCNewAdvancedCalcSend(LPOBJ lpObj) // OK
{
#if(GAMESERVER_EXTRA==1)

#if(GAMESERVER_UPDATE == 101)

	PMSG_NEW_ADVANCED_CALC_SEND pMsg;
	pMsg.header.set(0xF3, 0xE3, sizeof(pMsg));

	// Customs Info
	pMsg.ViewTotalDamageReflect = lpObj->DamageReflect;
	pMsg.ViewCriticalDamageRate = lpObj->CriticalDamageRate;
	pMsg.ViewCriticalDamagePVM = lpObj->CriticalDamageRate * gServerInfo->m_DamageRatePvM[lpObj->Class];
	pMsg.ViewCriticalDamagePVP = lpObj->CriticalDamageRate * gServerInfo->m_DamageRatePvP[lpObj->Class];
	pMsg.ViewExellentDamageRate = lpObj->ExcellentDamageRate;
	pMsg.ViewExellentDamagePVM = lpObj->ExcellentDamageRate * gServerInfo->m_DamageRatePvM[lpObj->Class];
	pMsg.ViewExellentDamagePVP = lpObj->ExcellentDamageRate * gServerInfo->m_DamageRatePvP[lpObj->Class];
	pMsg.ViewDoubleDamageRate = lpObj->DoubleDamageRate;
	pMsg.ViewTripleDamageRate = lpObj->TripleDamageRate;
	pMsg.ViewIgnoreDefenceRate = lpObj->IgnoreDefenseRate;
	pMsg.ViewIncreaseDamagePvP = lpObj->AttackSuccessRatePvP;
	pMsg.ViewIncreaseDefencePvP = lpObj->DefenseSuccessRatePvP;
	pMsg.ViewMoneyRate = lpObj->MoneyAmountDropRate;

	pMsg.ViewDamageReductionRate = 0;
	for (int n = 0; n < MAX_DAMAGE_REDUCTION; n++)
	{
		pMsg.ViewDamageReductionRate += lpObj->DamageReduction[n];
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

#endif

#if(GAMESERVER_UPDATE >= 201)

	PMSG_NEW_ADVANCED_CALC_SEND pMsg;
	pMsg.header.set(0xF3, 0xE3, sizeof(pMsg));

	// Customs Info
	pMsg.ViewTotalDamageReflect = lpObj->DamageReflect;
	pMsg.ViewCriticalDamageRate = lpObj->CriticalDamageRate;
	pMsg.ViewCriticalDamagePVM = lpObj->CriticalDamageRate * gServerInfo->m_DamageRatePvM[lpObj->Class];
	pMsg.ViewCriticalDamagePVP = lpObj->CriticalDamageRate * gServerInfo->m_DamageRatePvP[lpObj->Class];
	pMsg.ViewExellentDamageRate = lpObj->ExcellentDamageRate;
	pMsg.ViewExellentDamagePVM = lpObj->ExcellentDamageRate * gServerInfo->m_DamageRatePvM[lpObj->Class];
	pMsg.ViewExellentDamagePVP = lpObj->ExcellentDamageRate * gServerInfo->m_DamageRatePvP[lpObj->Class];
	pMsg.ViewDoubleDamageRate = lpObj->DoubleDamageRate;
	pMsg.ViewTripleDamageRate = lpObj->TripleDamageRate;
	pMsg.ViewShieldSkillDamageReductionRate = lpObj->ShieldDamageReduction;
	pMsg.ViewSDDamageReductionRate = lpObj->DamageReduction[DAMAGE_REDUCTION_JOH_ITEM];
	pMsg.ViewSDDecreaseDamageRate = lpObj->DecreaseShieldGaugeRate;
	pMsg.ViewIgnoreDefenceRate = lpObj->IgnoreDefenseRate;
	pMsg.ViewIgnoreSDRate = lpObj->IgnoreShieldGaugeRate;
	pMsg.ViewIncreaseDamagePvP = lpObj->AttackSuccessRatePvP;
	pMsg.ViewIncreaseDefencePvP = lpObj->DefenseSuccessRatePvP;
	pMsg.ViewMoneyRate = lpObj->MoneyAmountDropRate;

	pMsg.ViewDamageReductionRate = 0;
	for (int n = 0; n < MAX_DAMAGE_REDUCTION; n++)
	{
		pMsg.ViewDamageReductionRate += lpObj->DamageReduction[n];
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

#endif

#endif
}