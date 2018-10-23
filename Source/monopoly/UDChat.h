#ifndef __CHAT_H__
#define __CHAT_H__

/*************************************************************
*   FILE:             CHAT.H
*   (C) Copyright 99 Artech Digital Entertainments.
*                    All rights reserved.
*   Monopoly Game External Chat Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
inline void AdjustRect(RECT *r1, RECT r2, POINT p1, POINT p2);
inline void AdjustRects();
inline void AdjustFluffRects();
inline BOOL ScrollTextAreaUpOne();
inline BOOL ScrollTextAreaDownOne();
inline BOOL ScrollFluffTextAreaUpOne();
inline BOOL ScrollFluffTextAreaDownOne();
inline void DrawButton(LE_DATA_DataId tab, RECT box, DWORD colour, int offset);
inline void DrawFButton(LE_DATA_DataId tab, RECT box, DWORD colour, int offset);
inline void DrawOptionButton(RECT box, DWORD colour);
void GetCategory(int num);
void ShowCategory(int num);
int CHAT_wcscmp(const void *a, const void *b);
LRESULT CALLBACK EditBox_WinProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam);
void CHAT_RefreshTextArea(void);
void CHAT_RefreshFluffTextArea(void);
BOOL UDCHAT_ProcessFluffMessage(LE_QUEUE_MessagePointer UIMessagePntr);
BOOL UDCHAT_ProcessChatMessage(LE_QUEUE_MessagePointer UIMessagePntr);
BOOL UDCHAT_ProcessMessage(LE_QUEUE_MessagePointer UIMessagePntr);
void CHAT_WordWrapAll(void);
int CHAT_WordWrap(wchar_t *line, int width, wchar_t **wrapped, int flag);
void CHAT_ReceiveMessage(wchar_t * msg, long id, long player_from, long player_to);
void CHAT_Toggle(void);
BOOL CHAT_Open(void);
void CHAT_Close(void);
BOOL CHAT_InitializeSystem(void);
void CHAT_CleanAndRemoveSystem(void);
void CHAT_ReceiveVoiceMessage(BYTE *wave_data, DWORD wave_size, long player_from);
void CHAT_SendVoiceChat(long ChatToPlayer, long ChatFromPlayer);
void CHAT_ToggleVoice(void);

#define CHAT_MAXCHAR 256  // max length of input string, including NULL
#define CHAT_TOTALCHAR CHAT_MAXCHAR+RULE_MAX_PLAYER_NAME_LENGTH+64+2  // bigger, for name, ": ", and "(Private) "
extern wchar_t CHAT_WrappedLines[64][1024/*CHAT_TOTALCHAR*/];  // Temporarily hold wrapped text
/************************************************************/

#endif // __CHAT_H__
