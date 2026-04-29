#pragma once

#include "ServerDisplayer.h"
#include "User.h"

int SafeGetItem(int index);
float GetRoundValue(float value);
BYTE GetNewOptionCount(BYTE NewOption);
BYTE GetSocketOptionCount(BYTE SocketOption[5]);
BYTE GetPathPacketDirPos(int px,int py);
DWORD GenerateCheckSum(char* out_buff,int size,int key);
void PacketArgumentDecrypt(char* out_buff,char* in_buff,int size);
void ErrorMessageBox(char* message,...);
void LogAdd(eLogColor color,char* text,...);
bool DataSend(int aIndex,BYTE* lpMsg,DWORD size);
void DataSendAll(BYTE* lpMsg,int size);
bool DataSendSocket(SOCKET socket,BYTE* lpMsg,DWORD size);
void MsgSendV2(LPOBJ lpObj,BYTE* lpMsg,int size);
void CloseClient(int aIndex);

void PostMessage_1(char* name, char* message, char* text);
void PostMessage_2(char* name, char* message, char* text);
void PostMessage_3(char* name, char* message, char* text);
void PostMessage_4(char* name, char* message, char* text);
void PostMessage1(char* name,int index,char* message);
void PostMessage2(char* name,int index,char* message);
void PostMessage3(char* name,int index,char* message);
void PostMessage4(char* name,int index,char* message);
void SetLargeRand();
long GetLargeRand();
bool IsValidItem(int index);
bool IsValidMonster(int index);
char* CharToLower(char* text);
char* CharReverse(char* text);
bool CheckNameSyntax(char* text);

extern short RoadPathTable[MAX_ROAD_PATH_TABLE];


enum ePATENTE {
              
    RANK0 = 0,
    RANK1 = 1,
    RANK2 = 2,
    RANK3 = 3,
    RANK4 = 4,
    RANK5 = 5,
    RANK6 = 6,
    RANK7 = 7,
    RANK8 = 8,
};