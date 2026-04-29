// NpcTalk.cpp: implementation of the CNpcTalk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NpcTalk.h"
#include "BloodCastle.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "CastleSiegeWeapon.h"
#include "ChaosBox.h"
#include "CustomLukeHelper.h"
#include "DevilSquare.h"
#include "DoubleGoer.h"
#include "DSProtocol.h"
#include "Duel.h"
#include "EffectManager.h"
#include "GameMain.h"
#include "GensSystem.h"
#include "GuildManager.h"
#include "GoldenArcher.h"
#include "IllusionTemple.h"
#include "ImperialGuardian.h"
#include "ItemBagManager.h"
#include "KanturuEntranceNPC.h"
#include "Map.h"
#include "ReadFile.h"
#include "MonsterSchedule.h"
#include "Notice.h"
#include "Quest.h"
#include "QuestWorld.h"
#include "ServerInfo.h"
#include "ShopManager.h"
#include "Trade.h"
#include "Util.h"
#include "Warehouse.h"
#include "ScriptLoader.h"
#include "CustomNPCCommand.h"
#include "ObjectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpcTalk::CNpcTalk() // OK
{
	this->m_NpcTalkInfo.clear();
}

CNpcTalk::~CNpcTalk() // OK
{

}

void CNpcTalk::Load(char* path) // OK
{
	CReadFile* lpReadFile = new CReadFile;

	if(lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_NpcTalkInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			NPC_TALK_INFO info;

			memset(&info,0,sizeof(info));

			info.MonsterClass = lpReadFile->GetNumber();

			info.Map = lpReadFile->GetAsNumber();

			info.X = lpReadFile->GetAsNumber();

			info.Y = lpReadFile->GetAsNumber();

			strcpy_s(info.Message,lpReadFile->GetAsString());

			this->m_NpcTalkInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CNpcTalk::NpcTalk(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Interface.use != 0)
	{
		return 1;
	}

	if(gQuest->NpcTalk(lpNpc,lpObj) != 0)
	{
		return 1;
	}

	for(std::vector<NPC_TALK_INFO>::iterator it = this->m_NpcTalkInfo.begin(); it != this->m_NpcTalkInfo.end(); it++)
	{
		SYSTEMTIME SystemTime;

		GetLocalTime(&SystemTime);

		if (it->MonsterClass != lpNpc->Class)
		{
			continue;
		}

		if (it->Map != -1 && it->Map != lpNpc->Map)
		{
			continue;
		}

		if (it->X != -1 && it->X != lpNpc->X)
		{
			continue;
		}

		if (it->Y != -1 && it->Y != lpNpc->Y)
		{
			continue;
		}

		GCChatTargetSend(lpObj,lpNpc->Index,it->Message);
		
		break;
	}

	if (gCustomNpcCommand->GetNpcCommand(lpObj, lpNpc) != 0)
	{
		return 1;
	}

	switch(lpNpc->Class)
	{
		case 205:
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 206:
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 207: 
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 208:
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 209: 
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 216:
			this->NpcCastleSiegeCrown(lpNpc,lpObj);
			break;
		case 217:
			this->NpcCastleSiegeCrownSwitch(lpNpc,lpObj);
			break;
		case 218:
			this->NpcCastleSiegeCrownSwitch(lpNpc,lpObj);
			break;
		case 219:
			this->NpcCastleSiegeGateSwitch(lpNpc,lpObj);
			break;
		case 220:
			this->NpcCastleSiegeGuard(lpNpc,lpObj);
			break;
		case 221:
			this->NpcCastleSiegeWeaponOffense(lpNpc,lpObj);
			break;
		case 222:
			this->NpcCastleSiegeWeaponDefense(lpNpc,lpObj);
			break;
		case 223:
			this->NpcCastleSiegeSenior(lpNpc,lpObj);
			break;
		case 224:
			this->NpcCastleSiegeGuardsman(lpNpc,lpObj);
			break;
		case 226:
			this->NpcTrainer(lpNpc,lpObj);
			break;
		case 232:
			this->NpcAngelKing(lpNpc,lpObj);
			break;
		case 233:
			this->NpcAngelMessenger(lpNpc,lpObj);
			break;
		case 234:
			this->NpcServerTransfer(lpNpc,lpObj);
			break;
		case 236:
			this->NpcGoldenArcher(lpNpc,lpObj);
			break;
		case 237:
			this->NpcCharon(lpNpc,lpObj);
			break;
		case 238:
			this->NpcChaosGoblin(lpNpc,lpObj);
			break;
		case 240:
			this->NpcWarehouse(lpNpc,lpObj);
			break;
		case 241:
			this->NpcGuildMaster(lpNpc,lpObj);
			break;
		case 256:
			this->NpcLehap(lpNpc,lpObj);
			break;
		case 257:
			this->NpcElfBuffer(lpNpc,lpObj);
			break;
		case 367:
			this->NpcKanturuGate(lpNpc,lpObj);
			break;
		case 368:
			this->NpcElpis(lpNpc,lpObj);
			break;
		case 369:
			this->NpcOsbourne(lpNpc,lpObj);
			break;
		case 370:
			this->NpcJerridon(lpNpc,lpObj);
			break;
		case 371:
			this->NpcLeoTheHelper(lpNpc,lpObj);
			break;
		case 375:
			this->NpcChaosCardMaster(lpNpc,lpObj);
			break;
		case 380:
			this->NpcStoneStatue(lpNpc,lpObj);
			break;
		case 383:
			this->NpcYellowStorage(lpNpc,lpObj);
			break;
		case 384:
			this->NpcBlueStorage(lpNpc,lpObj);
			break;
		case 385:
			this->NpcMirageTheMummy(lpNpc,lpObj);
			break;
		case 450:
			this->NpcGovernorBlossom(lpNpc,lpObj);
			break;
		case 452:
			this->NpcSeedMaster(lpNpc,lpObj);
			break;
		case 453:
			this->NpcSeedResearcher(lpNpc,lpObj);
			break;
		case 465:
			this->NpcSantaClaus(lpNpc,lpObj);
			break;
		case 467:
			this->NpcSnowman(lpNpc,lpObj);
			break;
		case 468:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 469:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 470:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 471:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 472:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 473:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 474:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 475:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 478:
			this->NpcDelgado(lpNpc,lpObj);
			break;
		case 479:
			this->NpcTitus(lpNpc,lpObj);
			break;
		case 522:
			this->NpcJerryTheAdviser(lpNpc,lpObj);
			break;
		case 540:
			this->NpcSartiganTheAngel(lpNpc,lpObj);
			break;
		case 541:
			this->NpcSilverRewardChest(lpNpc,lpObj);
			break;
		case 542:
			this->NpcGoldenRewardChest(lpNpc,lpObj);
			break;
		case 543:
			this->NpcGensDuprianSteward(lpNpc,lpObj);
			break;
		case 544:
			this->NpcGensVarnertSteward(lpNpc,lpObj);
			break;
		case 546:
			this->NpcRaul(lpNpc,lpObj);
			break;
		case 547:
			this->NpcJulia(lpNpc,lpObj);
			break;
		case 566:
			this->NpcTercia(lpNpc,lpObj);
			break;
		case 567:
			this->NpcVeina(lpNpc,lpObj);
			break;
		case 568:
			this->NpcZyro(lpNpc,lpObj);
			break;
		case 579:
			this->NpcDavid(lpNpc,lpObj);
			break;
		case 580:
			this->NpcSloveCaptain(lpNpc,lpObj);
			break;
		case 581:
			this->NpcDeruvish(lpNpc,lpObj);
			break;
		case 582:
			this->NpcAdniel(lpNpc,lpObj);
			break;
		case 584:
			this->NpcLesnar(lpNpc,lpObj);
			break;
		case 651:
			this->NpcPersonalShopBoard(lpNpc,lpObj);
			break;
		default:
			return 0;
	}

	return 1;
}

void CNpcTalk::NpcCrywolfAltar(LPOBJ lpNpc,LPOBJ lpObj) // OK
{

}

void CNpcTalk::NpcCastleSiegeCrown(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiege->NpcCastleSiegeCrown(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeCrownSwitch(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiege->NpcCastleSiegeCrownSwitch(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeGateSwitch(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiege->NpcCastleSiegeGateSwitch(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeGuard(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiege->NpcCastleSiegeGuard(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeWeaponOffense(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiegeWeapon->NpcCastleSiegeWeaponOffense(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeWeaponDefense(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiegeWeapon->NpcCastleSiegeWeaponDefense(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeSenior(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	if(gCastleSiegeSync->GetState() == CS_STATE_START)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,241);
		return;
	}

	if(gCastleSiege->CheckGuildOwnCastle(lpObj->GuildName) == 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,242);
		return;
	}

	if(lpObj->GuildRank != GUILD_STATUS_OFFICE_MASTER && lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,243);
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 12;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = false;

	lpObj->CastleInvolved = 0;

	#if(GAMESERVER_UPDATE<=301)

	lpObj->CastleSeniorMixOpen = true;

	#endif

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcCastleSiegeGuardsman(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(gCastleSiegeSync->GetState() == CS_STATE_START)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetText(244,lpObj->Lang));
		return;
	}

	if(lpObj->Guild == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetText(248,lpObj->Lang));
		return;
	}
	
	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_COMMON;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 13;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcTrainer(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRAINER;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 7;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcAngelKing(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gBloodCastle->NpcAngelKing(lpNpc,lpObj);
}

void CNpcTalk::NpcAngelMessenger(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gBloodCastle->NpcAngelMessenger(lpNpc,lpObj);
}

void CNpcTalk::NpcServerTransfer(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Guild != 0)
	{
		GCServerCommandSend(lpObj->Index,6,0,0,0);
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRANSFER;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 5;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcGoldenArcher(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_GOLDEN_ARCHER;
	lpObj->Interface.state = 0;

	gGoldenArcher->CGGoldenArcherCountRecv(lpObj->Index);

	#endif
}

void CNpcTalk::NpcCharon(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gDevilSquare->NpcCharon(lpNpc,lpObj);
}

void CNpcTalk::NpcChaosGoblin(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 3;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcWarehouse(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Interface.type == INTERFACE_TRADE || lpObj->Interface.type == INTERFACE_CHAOS_BOX || lpObj->Interface.type == INTERFACE_PERSONAL_SHOP)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_WAREHOUSE;
	lpObj->Interface.state = 0;

	gWarehouse->GDWarehouseItemSend(lpObj->Index,lpObj->Account);
}

void CNpcTalk::NpcGuildMaster(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Guild != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,245);
		return;
	}

	if(lpObj->Level < gServerInfo->m_GuildCreateMinLevel[lpObj->AccountLevel])
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,246);
		return;
	}

	if(lpObj->Reset < gServerInfo->m_GuildCreateMinReset[lpObj->AccountLevel])
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,247);
		return;
	}

	#if(GAMESERVER_UPDATE>=702)

	if(gArcaBattle->GetState() > AB_STATE_SIGN1 && gArcaBattle->GetState() < AB_STATE_CLEAR)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,248);
		return;
	}

	#endif

	GCGuildMasterQuestionSend(lpObj->Index);
}

void CNpcTalk::NpcLehap(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_COMMON;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 9;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcElfBuffer(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gQuestWorld->NpcElfBuffer(lpNpc,lpObj);

	#else

	if(lpObj->Level > gServerInfo->m_ElfBufferMaxLevel[lpObj->AccountLevel])
	{
		GCServerCommandSend(lpObj->Index,13,0,0,0);
		return;
	}

	if(lpObj->Reset > gServerInfo->m_ElfBufferMaxReset[lpObj->AccountLevel])
	{
		GCServerCommandSend(lpObj->Index,13,0,0,0);
		return;
	}

	gEffectManager->AddEffect(lpObj, 0, EFFECT_ELF_BUFFER, 1800, (gServerInfo->m_ElfBufferDamageConstA + (lpObj->Level / gServerInfo->m_ElfBufferDamageConstB)), (gServerInfo->m_ElfBufferDefenseConstA + (lpObj->Level / gServerInfo->m_ElfBufferDefenseConstB)), 0, 0);

	#endif
}

void CNpcTalk::NpcKanturuGate(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	gKanturuEntranceNPC->CGKanturuEnterInfoRecv(lpObj->Index);

	#endif
}

void CNpcTalk::NpcElpis(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 17;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcOsbourne(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 18;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcJerridon(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 19;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcLeoTheHelper(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	GCServerCommandSend(lpObj->Index,15,1,0,0);

	#endif
}

void CNpcTalk::NpcChaosCardMaster(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 21;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcStoneStatue(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE<=802)
	gIllusionTemple->NpcStoneStatue(lpNpc,lpObj);
	#endif
}

void CNpcTalk::NpcYellowStorage(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE<=802)
	gIllusionTemple->NpcYellowStorage(lpNpc,lpObj);
	#endif
}

void CNpcTalk::NpcBlueStorage(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE<=802)
	gIllusionTemple->NpcBlueStorage(lpNpc,lpObj);
	#endif
}

void CNpcTalk::NpcMirageTheMummy(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE<=802)
	gIllusionTemple->NpcMirageTheMummy(lpNpc,lpObj);
	#endif
}

void CNpcTalk::NpcGovernorBlossom(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 22;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcSeedMaster(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 23;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);

	#endif
}

void CNpcTalk::NpcSeedResearcher(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 24;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);

	#endif
}

void CNpcTalk::NpcSantaClaus(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	GCServerCommandSend(lpObj->Index,16,0,0,0);

	#endif
}

void CNpcTalk::NpcSnowman(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	GCServerCommandSend(lpObj->Index,17,0,0,0);

	#endif
}

void CNpcTalk::NpcChristmas(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	if(lpNpc->Class == 470)
	{
		lpObj->Life = lpObj->MaxLife+lpObj->AddLife;
		GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);
		return;
	}

	if(lpNpc->Class == 471)
	{
		lpObj->Mana = lpObj->MaxMana+lpObj->AddMana;
		GCManaSend(lpObj->Index,0xFF,(int)lpObj->Mana,lpObj->BP);
		return;
	}
	
	switch(lpNpc->Class)
	{
		case 468:
			gEffectManager->AddEffect(lpObj,0,EFFECT_CHRISTMAS4,1800,30,0,0,0);
			break;
		case 469:
			gEffectManager->AddEffect(lpObj,0,EFFECT_CHRISTMAS5,1800,100,0,0,0);
			break;
		case 472:
			gEffectManager->AddEffect(lpObj,0,EFFECT_CHRISTMAS2,1800,500,0,0,0);
			break;
		case 473:
			gEffectManager->AddEffect(lpObj,0,EFFECT_CHRISTMAS3,1800,500,0,0,0);
			break;
		case 474:
			gEffectManager->AddEffect(lpObj,0,EFFECT_CHRISTMAS6,1800,15,0,0,0);
			break;
		case 475:
			gEffectManager->AddEffect(lpObj,0,EFFECT_CHRISTMAS7,1800,10,0,0,0);
			break;
	}

	#endif
}

void CNpcTalk::NpcDelgado(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=402)

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 32;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::NpcTitus(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=402)

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_COMMON;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 33;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	gDuel->GCDuelNpcInfoSend(lpObj->Index);

	#endif
}

void CNpcTalk::NpcJerryTheAdviser(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gImperialGuardian->NpcJerryTheAdviser(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcSartiganTheAngel(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gDoubleGoer->NpcSartiganTheAngel(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcSilverRewardChest(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gDoubleGoer->NpcSilverRewardChest(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcGoldenRewardChest(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gDoubleGoer->NpcGoldenRewardChest(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcGensDuprianSteward(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gGensSystem->NpcGensDuprianSteward(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcGensVarnertSteward(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gGensSystem->NpcGensVarnertSteward(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcRaul(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_COMMON;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 9;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::NpcJulia(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 37;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::NpcTercia(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gQuestWorld->NpcElfBuffer(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcVeina(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gQuestWorld->NpcElfBuffer(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcZyro(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	gQuestWorld->NpcElfBuffer(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcDavid(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=602)

	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 38;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);

	#endif
}

void CNpcTalk::NpcSloveCaptain(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 39;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::NpcDeruvish(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	gQuestWorld->NpcElfBuffer(lpNpc,lpObj);

	#endif
}

void CNpcTalk::NpcAdniel(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	if(lpObj->Store.Open != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 41;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);

	#endif
}

void CNpcTalk::NpcLesnar(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=702)

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 40;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::NpcPersonalShopBoard(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=802)

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 48;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::CGNpcTalkRecv(PMSG_NPC_TALK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use != 0)
	{
		return;
	}

	if(lpObj->Live == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpNpc = &gObj[bIndex];

	if(lpNpc->Type != OBJECT_NPC || lpObj->Map != lpNpc->Map)
	{
		return;
	}

	if(gObjCalcDistance(lpObj,lpNpc) > 5)
	{
		return;
	}

	gQuestWorldObjective->QuestWorldObjectiveSpecial(lpObj,QUEST_WORLD_OBJECTIVE_NPC,lpNpc->Class);

	if(gMonsterSchedule->CheckEnableNpc(lpObj,lpNpc) != 0)
	{
		return;
	}

	if (gScriptLoader->OnNpcTalk(bIndex, aIndex) != 0)
	{
		return;
	}

	if(gCustomLukeHelper->NpcLukeHelper(lpNpc,lpObj) != 0)
	{
		return;
	}

	if(this->NpcTalk(lpNpc,lpObj) != 0)
	{
		return;
	}

	if(SHOP_RANGE(lpNpc->ShopNumber) == 0)
	{
		return;
	}

	if(gServerInfo->m_PKLimitFree == 0 && lpObj->PKLevel > 4 && gServerInfo->m_PKLimitShop != 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetText(184,lpObj->Lang));
		return;
	}

	if(gShopManager->GetItemCountByIndex(lpNpc->ShopNumber) == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetText(180,lpObj->Lang));
		return;
	}

	if(gShopManager->CheckShopGameMasterLevel(lpNpc,lpObj) == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetText(181,lpObj->Lang));
		return;
	}

	if(gShopManager->CheckShopAccountLevel(lpNpc,lpObj) == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetText(182,lpObj->Lang));
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_SHOP;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	lpObj->TargetShopNumber = lpNpc->ShopNumber;

	gShopManager->GCShopItemListSendByIndex(lpNpc->ShopNumber,aIndex);

	gShopManager->GCShopItemPriceSendByIndex(lpNpc->ShopNumber,aIndex);

	GCTaxInfoSend(aIndex,2,gCastleSiegeSync->GetTaxRateStore(aIndex));
}

void CNpcTalk::CGNpcTalkCloseRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use == 0 || lpObj->Interface.type == INTERFACE_NONE || (lpObj->Interface.type == INTERFACE_CHAOS_BOX && lpObj->CastleSeniorMixOpen == 0))
	{
		return;
	}

	switch(lpObj->Interface.type)
	{
		case INTERFACE_TRADE:
			gTrade->CGTradeCancelButtonRecv(aIndex);
			break;
		case INTERFACE_SHOP:
			if(((lpObj->Interface.state==1)?(lpObj->Interface.state=0):1)==0){return;}
			lpObj->TargetShopNumber = -1;
			break;
		case INTERFACE_WAREHOUSE:
			gWarehouse->CGWarehouseClose(aIndex);
			break;
		case INTERFACE_TRAINER:
			gChaosBox->ChaosBoxInit(lpObj);
			gObjInventoryCommit(aIndex);
			break;
		#if(GAMESERVER_UPDATE>=501)
		case INTERFACE_QUEST_WORLD:
			lpObj->QuestWorldMonsterClass = -1;
			break;
		#endif
		case INTERFACE_CHAOS_BOX:
			lpObj->CastleSeniorMixOpen = 0;
			break;
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
}

void CNpcTalk::CGNpcLeoTheHelperRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	this->GDNpcLeoTheHelperSend(aIndex);

	#endif
}

void CNpcTalk::CGNpcSnowmanRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	gObjMoveGate(aIndex,22);

	#endif
}

void CNpcTalk::CGNpcSantaClausRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	this->GDNpcSantaClausSend(aIndex);

	#endif
}

void CNpcTalk::CGNpcJuliaRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Map == MAP_LOREN_MARKET)
	{
		gObjMoveGate(lpObj->Index,17);
	}
	else
	{
		gObjMoveGate(lpObj->Index,333);
	}

	#endif
}

void CNpcTalk::DGNpcLeoTheHelperRecv(SDHP_NPC_LEO_THE_HELPER_RECV* lpMsg) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGNpcLeoTheHelperRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(lpMsg->status != 0)
	{
		GCServerCommandSend(lpObj->Index,15,0,0,0);
		return;
	}

	gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_LEO_HELPER,-1,-1,lpObj,lpObj->Map,lpObj->X,lpObj->Y);

	this->GDNpcLeoTheHelperSaveSend(lpObj->Index,1);

	#endif
}

void CNpcTalk::DGNpcSantaClausRecv(SDHP_NPC_SANTA_CLAUS_RECV* lpMsg) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGNpcSantaClausRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(lpMsg->status != 0)
	{
		GCServerCommandSend(lpObj->Index,16,2,0,0);
		return;
	}

	gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_SANTA_CLAUS,-1,-1,lpObj,lpObj->Map,lpObj->X,lpObj->Y);

	this->GDNpcSantaClausSaveSend(lpObj->Index,1);

	#endif
}

void CNpcTalk::GDNpcLeoTheHelperSend(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_NPC_LEO_THE_HELPER_SEND pMsg;

	pMsg.header.set(0x0E,0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::GDNpcSantaClausSend(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_NPC_SANTA_CLAUS_SEND pMsg;

	pMsg.header.set(0x0E,0x01,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::GDNpcLeoTheHelperSaveSend(int aIndex,BYTE status) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	LPOBJ lpObj = &gObj[aIndex];

	SDHP_NPC_LEO_THE_HELPER_SAVE_SEND pMsg;

	pMsg.header.set(0x0E,0x30,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.status = status;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CNpcTalk::GDNpcSantaClausSaveSend(int aIndex,BYTE status) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	LPOBJ lpObj = &gObj[aIndex];

	SDHP_NPC_SANTA_CLAUS_SAVE_SEND pMsg;

	pMsg.header.set(0x0E,0x31,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.status = status;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}