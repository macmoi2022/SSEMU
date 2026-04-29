// GuildManager.h: interface for the CGuildManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_GUILD_MEMBER 80
#define MAX_GUILD_UNION 5
#define MAX_GUILD_RIVAL 2

enum eGuildStatus
{
	GUILD_STATUS_NONE = -1,
	GUILD_STATUS_MEMBER = 0,
	GUILD_STATUS_BATTLE_MASTER = 32,
	GUILD_STATUS_OFFICE_MASTER = 64,
	GUILD_STATUS_MASTER = 128,
};

enum eGuildRelation
{
	GUILD_RELATION_NONE = 0,
	GUILD_RELATION_UNION = 1,
	GUILD_RELATION_RIVAL = 2,
};

struct GUILD_MEMBER_INFO
{
	void Clear()
	{
		memset(this->Name,0,sizeof(this->Name));

		this->Server = -1;

		this->Status = GUILD_STATUS_NONE;
	}

	bool IsEmpty()
	{
		return ((this->Name[0]==0)?1:0);
	}

	char Name[11];
	int Server;
	int Status;
};

struct GUILD_INFO
{
	GUILD_INFO()
	{
		this->Index = 0;

		this->Score = 0;

		memset(this->Name,0,sizeof(this->Name));

		memset(this->Master,0,sizeof(this->Master));

		memset(this->Notice,0,sizeof(this->Notice));

		memset(this->Mark,0,sizeof(this->Mark));

		for(int n=0;n < MAX_GUILD_MEMBER;n++){this->Member[n].Clear();}

		this->Type = 0;

		this->Union = 0;

		this->Rival = 0;
	}

	BYTE GetMemberCount()
	{
		BYTE count = 0;

		for(int n=0;n < MAX_GUILD_MEMBER;n++)
		{
			if(this->Member[n].IsEmpty() == 0)
			{
				count++;
			}
		}

		return count;
	}

	DWORD Index;
	DWORD Score;
	char Name[9];
	char Master[11];
	char Notice[60];
	BYTE Mark[32];
	GUILD_MEMBER_INFO Member[MAX_GUILD_MEMBER];
	BYTE Type;
	DWORD Union;
	DWORD Rival;
};

class CGuildManager
{
	CGuildManager();
	virtual ~CGuildManager();
	SingletonInstance(CGuildManager)
public:
	void Init();
	void DelGuild(int index);
	GUILD_INFO* GetGuild(int index);
	GUILD_INFO* GetGuild(char* name);
	GUILD_INFO* GetGuildMember(char* member);
	GUILD_MEMBER_INFO* GetMemberInfo(char* member);
	BYTE CreateGuild(char* name,char* master,BYTE* mark,int type);
	BYTE RemoveGuild(char* name);
	BYTE AddGuildMember(char* name,char* member,int server);
	BYTE DelGuildMember(char* member);
	BYTE AddGuildRelationship(int guild01,int guild02,int relation);
	BYTE DelGuildRelationship(int guild01,int relation);
	BYTE SetGuildRelationship(char* guild01,char* guild02);
	BYTE GetUnionList(DWORD index,DWORD* list);
	BYTE GetRivalList(DWORD index,DWORD* list);
	bool CheckGuildOnCS(char* name);
	bool SetGuildScore(char* name,int score);
	bool SetGuildNotice(char* name,char* notice);
	bool SetGuildType(char* name, int type);
	bool SetMemberStatus(char* member, int status);
private:
	std::map<int,GUILD_INFO*> m_GuildList;
};

#define gGuildManager SingNull(CGuildManager)