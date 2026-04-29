// GuildManager.cpp: implementation of the CGuildManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildManager.h"
#include "Guild.h"
#include "QueryManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuildManager::CGuildManager() // OK
{
	this->m_GuildList.clear();
}

CGuildManager::~CGuildManager() // OK
{
	for(std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.begin(); it != this->m_GuildList.end(); ++it)
    {
        delete it->second;
    }

    this->m_GuildList.clear();
}

void CGuildManager::Init() // OK
{
	this->m_GuildList.clear();

	if(gQueryManager->ExecQuery("SELECT * FROM Guild") != 0)
	{
		GUILD_INFO* lpGuild;

		while(gQueryManager->Fetch() != SQL_NO_DATA)
		{
			lpGuild = new GUILD_INFO();

			lpGuild->Index = gQueryManager->GetAsInteger("Number");

			lpGuild->Score = gQueryManager->GetAsInteger("G_Score");

			gQueryManager->GetAsString("G_Name",lpGuild->Name,sizeof(lpGuild->Name));

			gQueryManager->GetAsString("G_Master",lpGuild->Master,sizeof(lpGuild->Master));

			gQueryManager->GetAsString("G_Notice",lpGuild->Notice,sizeof(lpGuild->Notice));

			gQueryManager->GetAsBinary("G_Mark",lpGuild->Mark,sizeof(lpGuild->Mark));

			lpGuild->Type = gQueryManager->GetAsInteger("G_Type");

			lpGuild->Rival = gQueryManager->GetAsInteger("G_Rival");

			lpGuild->Union = gQueryManager->GetAsInteger("G_Union");

			memcpy(lpGuild->Member[0].Name,lpGuild->Master,sizeof(lpGuild->Member[0].Name));

			lpGuild->Member[0].Status = GUILD_STATUS_MASTER;

			this->m_GuildList[lpGuild->Index] = lpGuild;
		}
	}

	gQueryManager->Close();

	if(gQueryManager->ExecQuery("SELECT * FROM GuildMember") != 0)
	{
		while(gQueryManager->Fetch() != SQL_NO_DATA)
		{
			char GuildName[9] = {0};

			char MemberName[11] = {0};

			gQueryManager->GetAsString("G_Name",GuildName,sizeof(GuildName));

			gQueryManager->GetAsString("Name",MemberName,sizeof(MemberName));

			int MemberStatus = gQueryManager->GetAsInteger("G_Status");

			GUILD_INFO* lpGuild = this->GetGuild(GuildName);

			if(lpGuild != 0)
			{
				if(strcmp(lpGuild->Master,MemberName) != 0)
				{
					for(int n=1;n < MAX_GUILD_MEMBER;n++)
					{
						if(lpGuild->Member[n].IsEmpty() != 0)
						{
							memcpy(lpGuild->Member[n].Name,MemberName,sizeof(lpGuild->Member[n].Name));
							lpGuild->Member[n].Status = MemberStatus;
							break;
						}
					}
				}
			}
		}
	}

	gQueryManager->Close();
}

void CGuildManager::DelGuild(int index) // OK
{
	std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.find(index);

	if(it != this->m_GuildList.end()) 
	{
		delete it->second;
		this->m_GuildList.erase(it);
	}
}

GUILD_INFO* CGuildManager::GetGuild(int index) // OK
{
	std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.find(index);

	if(it != this->m_GuildList.end()) 
	{
		return it->second;
	}

	return 0;
}

GUILD_INFO* CGuildManager::GetGuild(char* name) // OK
{
	for(std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.begin(); it != this->m_GuildList.end(); it++)
	{
		if(it->second->Name[0] == name[0])
		{
			if(_stricmp(it->second->Name,name) == 0)
			{
				return it->second;
			}
		}
	}

	return 0;
}

GUILD_INFO* CGuildManager::GetGuildMember(char* member) // OK
{
	for(std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.begin(); it != this->m_GuildList.end(); it++)
	{
		for(int n=0;n < MAX_GUILD_MEMBER;n++)
		{
			if(it->second->Member[n].Name[0] == member[0])
			{
				if(strcmp(it->second->Member[n].Name,member) == 0)
				{
					return it->second;
				}
			}
		}
	}

    return 0;
}

GUILD_MEMBER_INFO* CGuildManager::GetMemberInfo(char* member) // OK
{
	for(std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.begin(); it != this->m_GuildList.end(); it++)
	{
		for(int n=0;n < MAX_GUILD_MEMBER;n++)
		{
			if(it->second->Member[n].Name[0] == member[0])
			{
				if(strcmp(it->second->Member[n].Name,member) == 0)
				{
					return &it->second->Member[n];
				}
			}
		}
	}

    return 0;
}

BYTE CGuildManager::CreateGuild(char* name,char* master,BYTE* mark,int type) // OK
{
	if(this->GetGuild(name) != 0)
	{
		return 3;
	}

	if(gQueryManager->ExecQuery("WZ_GuildCreate '%s','%s'",name,master) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		return 0;
	}
	else
	{
		if(gQueryManager->GetResult(0) == 0)
		{
			gQueryManager->Close();

			gQueryManager->BindParameterAsBinary(1,mark,32);

			gQueryManager->ExecQuery("UPDATE Guild SET G_Mark=? WHERE G_Name='%s'",name);

			gQueryManager->Close();

			gQueryManager->ExecQuery("UPDATE GuildMember SET G_Status=%d WHERE Name='%s'",GUILD_STATUS_MASTER,master);

			gQueryManager->Close();

			GUILD_INFO* lpGuild = new GUILD_INFO();

			gQueryManager->ExecQuery("SELECT Number FROM Guild WHERE G_Name='%s'",name);

			gQueryManager->Fetch();

			lpGuild->Index = gQueryManager->GetAsInteger("Number");

			gQueryManager->Close();

			memcpy(lpGuild->Name,name,sizeof(lpGuild->Name));

			memcpy(lpGuild->Master,master,sizeof(lpGuild->Master));

			memcpy(lpGuild->Mark,mark,sizeof(lpGuild->Mark));

			lpGuild->Type = type;

			memcpy(lpGuild->Member[0].Name,master,sizeof(lpGuild->Member[0].Name));

			lpGuild->Member[0].Status = GUILD_STATUS_MASTER;

			this->m_GuildList[lpGuild->Index] = lpGuild;

			return 1;
		}
		else
		{
			gQueryManager->Close();

			return 2;
		}
	}
}

BYTE CGuildManager::RemoveGuild(char* name) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(name);

	if(lpGuild == 0)
	{
		return 3;
	}

	if(this->CheckGuildOnCS(name) != 0)
	{
		return 2;
	}

	this->DelGuildRelationship(lpGuild->Index,1);

	this->DelGuildRelationship(lpGuild->Index,2);

	this->DelGuild(lpGuild->Index);

	gQueryManager->ExecQuery("DELETE FROM Guild WHERE G_Name='%s'",name);

	gQueryManager->Close();

	gQueryManager->ExecQuery("DELETE FROM GuildMember WHERE G_Name='%s'",name);

	gQueryManager->Close();

	#if(DATASERVER_UPDATE>=702)

	gQueryManager->ExecQuery("DELETE FROM ArcaBattleGuildEmblem WHERE GuildName='%s'",name);

	gQueryManager->Close();

	gQueryManager->ExecQuery("DELETE FROM ArcaBattleGuildMaster WHERE GuildName='%s'", name);

	gQueryManager->Close();

	gQueryManager->ExecQuery("DELETE FROM ArcaBattleGuildMember WHERE GuildName='%s'", name);

	gQueryManager->Close();

	gQueryManager->ExecQuery("DELETE FROM ArcaBattleWinner WHERE GuildName='%s'", name);

	gQueryManager->Close();

	#endif

	return 1;
}

BYTE CGuildManager::AddGuildMember(char* name,char* member,int server) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(name);

	if(lpGuild == 0)
	{
		return 0;
	}

	if(this->GetMemberInfo(member) != 0)
	{
		return 3;
	}

	if(gQueryManager->ExecQuery("SELECT * FROM GuildMember WHERE Name='%s'",member) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();

		for(int n=1;n < MAX_GUILD_MEMBER;n++)
		{
			if(lpGuild->Member[n].IsEmpty() != 0)
			{
				strcpy_s(lpGuild->Member[n].Name,member);
				lpGuild->Member[n].Server = server;
				lpGuild->Member[n].Status = GUILD_STATUS_MEMBER;
				
				gQueryManager->ExecQuery("INSERT INTO GuildMember (Name,G_Name,G_Level,G_Status) VALUES ('%s','%s',1,%d)",member,name,GUILD_STATUS_MEMBER);

				gQueryManager->Close();

				return 1;
			}
		}

		return 2;
	}
	else
	{
		gQueryManager->Close();

		return 4;
	}
}

BYTE CGuildManager::DelGuildMember(char* member) // OK
{
	GUILD_MEMBER_INFO* lpMember = this->GetMemberInfo(member);

	if(lpMember == 0)
	{
		return 3;
	}

	lpMember->Clear();

	gQueryManager->ExecQuery("DELETE FROM GuildMember WHERE Name='%s'",member);

	gQueryManager->Close();

	#if(DATASERVER_UPDATE>=702)

	gQueryManager->ExecQuery("DELETE FROM ArcaBattleGuildMember WHERE Name='%s'",member);

	gQueryManager->Close();

	#endif

	return 1;
}

BYTE CGuildManager::AddGuildRelationship(int guild01,int guild02,int relation)  // OK
{
	GUILD_INFO* lpGuildA = this->GetGuild(guild01);

	GUILD_INFO* lpGuildB = this->GetGuild(guild02);

	if(lpGuildA == 0 || lpGuildB == 0)
	{
		return 0;
	}

	if(this->CheckGuildOnCS(lpGuildA->Name) != 0 || this->CheckGuildOnCS(lpGuildB->Name) != 0)
	{
		return 16;
	}

	if(relation == GUILD_RELATION_UNION)
	{
		if(lpGuildA->Union > 0 || lpGuildA->Rival > 0)
		{
			return 0;
		}

		DWORD UnionList[MAX_GUILD_UNION];

		if(this->GetUnionList(lpGuildB->Index,UnionList) >= MAX_GUILD_UNION)
		{
			return 0;
		}

		gQueryManager->ExecQuery("UPDATE Guild SET G_Union=%d WHERE Number=%d",lpGuildB->Index,lpGuildB->Index);
		
		gQueryManager->Close();

		gQueryManager->ExecQuery("UPDATE Guild SET G_Union=%d WHERE Number=%d",lpGuildB->Index,lpGuildA->Index);
	
		gQueryManager->Close();

		lpGuildA->Union = lpGuildB->Index;

		lpGuildB->Union = lpGuildB->Index;

		gGuild->UnionInfoSend(lpGuildB->Index,relation);
	}
	else if (relation == GUILD_RELATION_RIVAL)
	{
		if(lpGuildA->Rival > 0 || lpGuildB->Rival > 0)
		{
			return 0;
		}

		if(lpGuildA->Union > 0 && lpGuildA->Union != lpGuildA->Index)
		{
			return 0;
		}

		if(lpGuildB->Union > 0 && lpGuildB->Union != lpGuildB->Index)
		{
			return 0;
		}

		gQueryManager->ExecQuery("UPDATE Guild SET G_Rival=%d WHERE Number=%d",lpGuildA->Index,lpGuildB->Index);
		
		gQueryManager->Close();

		gQueryManager->ExecQuery("UPDATE Guild SET G_Rival=%d WHERE Number=%d",lpGuildB->Index,lpGuildA->Index);
		
		gQueryManager->Close();

		lpGuildA->Rival = lpGuildB->Index;

		lpGuildB->Rival = lpGuildA->Index;

		gGuild->UnionInfoSend(lpGuildA->Index,relation);

		gGuild->UnionInfoSend(lpGuildB->Index,relation);
	}

	return 1;
}

BYTE CGuildManager::DelGuildRelationship(int guild01,int relation)  // OK
{
	GUILD_INFO* lpGuildA = this->GetGuild(guild01);

	if(lpGuildA == 0)
	{
		return 0;
	}

	if(this->CheckGuildOnCS(lpGuildA->Name) != 0)
	{
		return 16;
	}

	GUILD_INFO* lpBuildB = 0;

	if(relation == GUILD_RELATION_UNION)
	{
		if(lpGuildA->Index == lpGuildA->Union)
		{
			return 0;
		}

		if((lpBuildB=this->GetGuild(lpGuildA->Union)) == 0)
		{
			return 0;
		}

		DWORD count = 0;

		DWORD UnionList[MAX_GUILD_UNION];

		if((count=this->GetUnionList(lpBuildB->Index,UnionList)) == 0)
		{
			return 0;
		}

		if(count == 2)
		{
			gQueryManager->ExecQuery("UPDATE Guild SET G_Union=0 WHERE Number IN (%d,%d)",UnionList[0],UnionList[1]);
			
			gQueryManager->Close();
			
			lpGuildA->Union = 0;
			
			lpBuildB->Union = 0;
			
			gGuild->UnionInfoSend(lpBuildB->Index,1);
			
			gGuild->UnionClearSend(lpBuildB->Index);
		}
		else
		{
			gQueryManager->ExecQuery("UPDATE Guild SET G_Union=0 WHERE Number=%d",lpGuildA->Index);
			
			gQueryManager->Close();
			
			lpGuildA->Union = 0;
			
			gGuild->UnionInfoSend(lpBuildB->Index,relation);
		}
	}
	else if(relation == GUILD_RELATION_RIVAL)
	{
		if((lpBuildB=this->GetGuild(lpGuildA->Rival)) == 0)
		{
			return 0;
		}

		gQueryManager->ExecQuery("UPDATE Guild SET G_Rival=0 WHERE Number IN (%d,%d)",lpGuildA->Index,lpBuildB->Index);
		
		gQueryManager->Close();

		lpGuildA->Rival = 0;

		lpBuildB->Rival = 0;
			
		gGuild->UnionInfoSend(lpGuildA->Index,relation);
		
		gGuild->UnionInfoSend(lpBuildB->Index,relation);
	}

	return 1;
}

BYTE CGuildManager::SetGuildRelationship(char* guild01,char* guild02)  // OK
{
	GUILD_INFO* lpGuildA = this->GetGuild(guild01);

	GUILD_INFO* lpGuildB = this->GetGuild(guild02);

	if(lpGuildA == 0 || lpGuildB == 0)
	{
		return 0;
	}

	if(lpGuildA->Union != lpGuildB->Index || lpGuildB->Union != lpGuildB->Index)
	{
		return 0;
	}

	if(this->CheckGuildOnCS(lpGuildA->Name) != 0 || this->CheckGuildOnCS(lpGuildB->Name) != 0)
	{
		return 16;
	}

	DWORD list[MAX_GUILD_UNION];

	BYTE count = this->GetUnionList(lpGuildB->Index,list);

	if(count == 2)
	{
		gQueryManager->ExecQuery("UPDATE Guild SET G_Union=0 WHERE Number IN (%d,%d)",lpGuildA->Index,lpGuildB->Index);
		
		gQueryManager->Close();
		
		lpGuildA->Union = 0;

		lpGuildB->Union = 0;

		gGuild->UnionInfoSend(lpGuildB->Index,GUILD_RELATION_UNION);

		gGuild->UnionClearSend(lpGuildB->Index);
	}
	else
	{
		gQueryManager->ExecQuery("UPDATE Guild SET G_Union=0 WHERE Number=%d",lpGuildA->Index);

		gQueryManager->Close();

		lpGuildA->Union = 0;

		gGuild->UnionInfoSend(lpGuildB->Index,GUILD_RELATION_UNION);
	}

	return 1;
}

BYTE CGuildManager::GetUnionList(DWORD index,DWORD* list) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(index);

	if(lpGuild == 0 || lpGuild->Union == 0)
	{
		return 0;
	}

	BYTE count = 0;

	list[count++] = lpGuild->Index;

	for(std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.begin(); it != this->m_GuildList.end(); it++)
	{
		if(it->second->Index != lpGuild->Index && it->second->Union == lpGuild->Index)
		{
			list[count++] = it->second->Index;
		}
	}

	return count;
}

BYTE CGuildManager::GetRivalList(DWORD index,DWORD* list) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(index);

	if(lpGuild == 0 || lpGuild->Rival == 0)
	{
		return 0;
	}

	BYTE count = 0;

	list[count++] = lpGuild->Rival;

	for(std::map<int,GUILD_INFO*>::iterator it = this->m_GuildList.begin(); it != this->m_GuildList.end(); it++)
	{
		if(it->second->Index != lpGuild->Index && it->second->Union == lpGuild->Rival)
		{
			list[count++] = it->second->Index;
		}
	}

	return count;
}

bool CGuildManager::CheckGuildOnCS(char* name) // OK
{
	bool result = 0;

	if(gQueryManager->ExecQuery("WZ_CS_CheckSiegeGuildList '%s'",name) == 0 || gQueryManager->Fetch() == SQL_NO_DATA)
	{
		gQueryManager->Close();
	}
	else
	{
		if(gQueryManager->GetResult(0) != 0)
		{
			result = 1;
		}

		gQueryManager->Close();
	}

	return result;
}

bool CGuildManager::SetGuildScore(char* name,int score) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(name);

	if(lpGuild == 0)
	{
		return 0;
	}

	if(gQueryManager->ExecQuery("UPDATE Guild SET G_Score=%d WHERE G_Name='%s'",score,name) == 0)
	{
		gQueryManager->Close();

		return 0;
	}
	else
	{
		gQueryManager->Close();

		lpGuild->Score = score;

		return 1;
	}
}

bool CGuildManager::SetGuildNotice(char* name,char* notice) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(name);

	if(lpGuild == 0)
	{
		return 0;
	}

	gQueryManager->BindParameterAsString(1,notice,60);

	if(gQueryManager->ExecQuery("UPDATE Guild SET G_Notice=? WHERE G_Name='%s'",name) == 0)
	{
		gQueryManager->Close();

		return 0;
	}
	else
	{
		gQueryManager->Close();

		memcpy(lpGuild->Notice,notice,sizeof(lpGuild->Notice));

		return 1;
	}
}

bool CGuildManager::SetGuildType(char* name,int type) // OK
{
	GUILD_INFO* lpGuild = this->GetGuild(name);

	if(lpGuild == 0)
	{
		return 0;
	}

	if(gQueryManager->ExecQuery("UPDATE Guild SET G_Type=%d WHERE G_Name='%s'",type,name) == 0)
	{
		gQueryManager->Close();

		return 0;
	}
	else
	{
		gQueryManager->Close();

		lpGuild->Type = type;

		return 1;
	}
}

bool CGuildManager::SetMemberStatus(char* member,int status) // OK
{
	GUILD_MEMBER_INFO* lpMember = this->GetMemberInfo(member);

	if(lpMember == 0)
	{
		return 0;
	}

	if(gQueryManager->ExecQuery("UPDATE GuildMember SET G_Status=%d WHERE Name='%s'",status,member) == 0)
	{
		gQueryManager->Close();

		return 0;
	}
	else
	{
		gQueryManager->Close();

		lpMember->Status = status;

		return 1;
	}
}