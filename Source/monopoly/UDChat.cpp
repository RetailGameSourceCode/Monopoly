//abcdefghijklmnopqrstuvwxyz123467890bcdefghijklmnopqrstuvwxyz23467890cdefghijklmnopqrstuvwxyz3467890defghijklmnopqrstuvwxyz467890efghijklmnopqrstuvwxyz67890fghijklmnopqrstuvwxyz7890ghijklmnopqrstuvwxyz890hijklmnopqrstuvwxyz90ijklmnopqrstuvwxyz0jklmnopqrstuvwxyzklmnopqrstuvwxyz
/***************************************************************************
* chat.cpp -- Monopoly chat module
* "Hello children.  Say hello to Mr. Chat."
* This implements a chat window, and another window for the "fluff text".
* (Where'd that name come from?  I'm just using it 'cuz Star Wars Monopoly
* used it.  It's actually international/multilingual "canned" phrases)
*
* Currently expects the Lucida Sans Unicode (LSANSUNI.TTF) font
* to be in the same directory as the Monopoly executable.
***************************************************************************/
//bleeble: Temporarily commented out:
// Datafile handling, until artwork is finalized
#include "GameInc.h"
#include "../pc3d/PC3DHdr.h"

//add back if testing clickregions
//extern LE_DATA_DataId tmpoverlay;
//#define DrawBox(box) LE_GRAFIX_ColorArea(tmpoverlay, \
//  (short)box.left, (short)box.top, width(box), height(box), LEG_MCRA(0, 255, 255, 128));

static LANG_TextMessageNumber FunctionKeyMessageIndices[10] =
{
  TMN_FLUFF_excuse_me_i_think_its_your_turn,
  TMN_FLUFF_lets_hurry_up_folks,
  TMN_FLUFF_i_hate_when_that_happens,
  TMN_FLUFF_i_love_when_that_happens,
  TMN_FLUFF_wow_that_was_lucky,
  TMN_FLUFF_so_make_me_an_offer,
  TMN_FLUFF_sorry_i_dont_think_thats_worth_it,
  TMN_FLUFF_no,
  TMN_FLUFF_yes,
NULL//  TMN_FLUFF_i_dont_know
};
  /* Particularly popular messages are assigned to function keys. */
//bleeble: not yet

// "Maximum" and "number of" defines.
#define CHAT_KeepInputLines 100 //5 //100
#define CHAT_KeepOutputLines 512 //10 //512
#define NUMFLUFFTEXT (TMN_FLUFF_spectator - TMN_FLUFF_greetings)
#define NUM_ALPHA_CHANNEL 17
#define FONTMAX 14
#define FONTMIN 7
#define BTNOFFSET 5

#define FLUFFTEXTOFFSET TMN_FLUFF_greetings
#define BG 220 // white level of the textarea background

// Handy little defines
#define width(x) ((short)(x.right - x.left))
#define height(x) ((short)(x.bottom - x.top))
#define PointInRect(x, y, r) \
  (!((x < r.left) || (x >= r.right) || (y < r.top) || (y >= r.bottom)))

POINT CHAT_winloc = {10, 10};
POINT CHAT_winsize = {246, 99};  // Each +1 to allow -1 (can't have -0)
// If the offset is positive (or 0), the coordinate is relative to the top or left
// If the offset is negative, the coordinate is relative to the bottom or right
RECT CHAT_chatbar,        CHAT_chatbar_rel = {0, 0, -1, 18};    // Title bar
RECT CHAT_chatbox,        CHAT_chatbox_rel = {0, 18, -1, -1};   // Rest of window
RECT CHAT_textbox,        CHAT_textbox_rel = {6, 18, -20, -18}; // Text area
RECT CHAT_editbox,        CHAT_editbox_rel = {5, -18, -20, -4}; // Edit box
//RECT CHAT_editbox,        CHAT_editbox_rel = {5, -20, -23, -6}; // Edit box
RECT CHAT_upbtn,          CHAT_upbtn_rel = {-18, 18, -1, 35};
RECT CHAT_dnbtn,          CHAT_dnbtn_rel = {-18, -35, -1, -18};
RECT CHAT_scrollbar,      CHAT_scrollbar_rel = {-17, 35, -2, -35};
//RECT CHAT_scrollbar,      CHAT_scrollbar_rel = {-22, 34, -6, -38};
RECT CHAT_chatsbtn,       CHAT_chatsbtn_rel = {-21, 21, -5, 25};  // bleeble: notused
RECT CHAT_allbtn,         CHAT_allbtn_rel = {-76, 2, -60, 16};
//RECT CHAT_allbtn,         CHAT_allbtn_rel = {-94, 2, -78, 16};
RECT CHAT_optbtn,         CHAT_optbtn_rel = {-58, 2, -42, 16};
RECT CHAT_fluffbtn,       CHAT_fluffbtn_rel = {-40, 2, -24, 16};
RECT CHAT_shadebtn,       CHAT_shadebtn_rel = {-21, 1, -1, 18};
RECT CHAT_closebtn,       CHAT_closebtn_rel = {0, 0, 19, 18};
RECT CHAT_sizebtn,        CHAT_sizebtn_rel = {-20, -18, -1, -1};
RECT CHAT_optbox,         CHAT_optbox_rel = {-120, 16, -22, 68};
// BG transparency up/down
RECT CHAT_trnup1btn,      CHAT_trnup1btn_rel = {-120+7,  16+2, -120+23, 16+17};
RECT CHAT_trndn1btn,      CHAT_trndn1btn_rel = {-120+77, 16+2, -120+94, 16+17};
// Text transparency up/down
RECT CHAT_trnup2btn,      CHAT_trnup2btn_rel = {-120+7,  16+18, -120+23, 16+33};
RECT CHAT_trndn2btn,      CHAT_trndn2btn_rel = {-120+77, 16+18, -120+94, 16+33};
// BG+Text transparency up/down
//RECT CHAT_trnup3btn,      CHAT_trnup3btn_rel = {-120+3, 16+13, -120+19, 16+27};
//RECT CHAT_trndn3btn,      CHAT_trndn3btn_rel = {-120+79, 16+13, -120+96, 16+27};
// Text size up/down
RECT CHAT_txtupbtn,       CHAT_txtupbtn_rel = {-120+7,  16+34, -120+23, 16+49};
RECT CHAT_txtdnbtn,       CHAT_txtdnbtn_rel = {-120+77, 16+34, -120+94, 16+49};
/*
// BG transparency up/down
RECT CHAT_trnup1btn,      CHAT_trnup1btn_rel = {-120+21, 16+3, -120+37, 16+17};
RECT CHAT_trndn1btn,      CHAT_trndn1btn_rel = {-120+61, 16+3, -120+77, 16+17};
// Text transparency up/down
RECT CHAT_trnup2btn,      CHAT_trnup2btn_rel = {-120+21, 16+19, -120+37, 16+33};
RECT CHAT_trndn2btn,      CHAT_trndn2btn_rel = {-120+61, 16+19, -120+77, 16+33};
// BG+Text transparency up/down
RECT CHAT_trnup3btn,      CHAT_trnup3btn_rel = {-120+3, 16+13, -120+19, 16+27};
RECT CHAT_trndn3btn,      CHAT_trndn3btn_rel = {-120+79, 16+13, -120+96, 16+27};
// Text size up/down
RECT CHAT_txtupbtn,       CHAT_txtupbtn_rel = {-120+21, 16+35, -120+37, 16+49};
RECT CHAT_txtdnbtn,       CHAT_txtdnbtn_rel = {-120+61, 16+35, -120+77, 16+49};
*/
const short BarHeight = (short)(CHAT_chatbar_rel.bottom - CHAT_chatbar_rel.top);

// Window location, window size, and rectangle defines for fluff window.
// The _rel variables do not change.  They are used to change the non-_rel boxes.
POINT FLUFF_winloc = {265, 10};
POINT FLUFF_winsize = {246, 99};  // Each +1 to allow -1 (can't have -0)
// If the offset is positive (or 0), the coordinate is relative to the top or left
// If the offset is negative, the coordinate is relative to the bottom or right
RECT FLUFF_chatbar,   FLUFF_chatbar_rel = {0, 0, -1, 18};
RECT FLUFF_chatbox,   FLUFF_chatbox_rel = {0, 18, -1, -1};
RECT FLUFF_textbox,   FLUFF_textbox_rel = {6, 18, -20, -4};
RECT FLUFF_upbtn,     FLUFF_upbtn_rel = {-18, 18, -1, 35};
RECT FLUFF_dnbtn,     FLUFF_dnbtn_rel = {-18, -35, -1, -18};
RECT FLUFF_scrollbar, FLUFF_scrollbar_rel = {-17, 35, -2, -35};
//RECT FLUFF_scrollbar, FLUFF_scrollbar_rel = {-22, 34, -6, -38};
RECT FLUFF_chatsbtn,  FLUFF_chatsbtn_rel = {-21, 21, -5, 25}; // bleeble: notused
// Fluff category buttons.  Used for clicking on.
RECT FLUFF_grp1btn,   FLUFF_grp1btn_rel = {-143, 2, -123, 17};
RECT FLUFF_grp2btn,   FLUFF_grp2btn_rel = {-123, 2, -103, 17};//{-112, 2, -96, 16};
RECT FLUFF_grp3btn,   FLUFF_grp3btn_rel = {-103, 2, -83, 17};
RECT FLUFF_grp4btn,   FLUFF_grp4btn_rel = {-83, 2, -63, 17};
RECT FLUFF_grp5btn,   FLUFF_grp5btn_rel = {-63, 2, -43, 17};
RECT FLUFF_grp6btn,   FLUFF_grp6btn_rel = {-43, 2, -23, 17};
RECT FLUFF_shadebtn,  FLUFF_shadebtn_rel = {-21, 1, -1, 18};
RECT FLUFF_closebtn,  FLUFF_closebtn_rel = {0, 0, 19, 18};
RECT FLUFF_sizebtn,   FLUFF_sizebtn_rel = {-20, -18, -1, -1};

// Text related variables
int CHAT_InputLinesTop = 0;
int CHAT_OutputLinesTop = 0;
// The following two arrays point to entries in the two
// after them, to help reduce the pain of overflow.
wchar_t *CHAT_InputLinesPtr[CHAT_KeepInputLines];
wchar_t *CHAT_OutputLinesPtr[CHAT_KeepOutputLines];
wchar_t CHAT_InputLines[CHAT_KeepInputLines][CHAT_MAXCHAR];
wchar_t CHAT_OutputLines[CHAT_KeepOutputLines][CHAT_TOTALCHAR];
wchar_t CHAT_WrappedLines[64][1024/*CHAT_TOTALCHAR*/];  // Temporarily hold wrapped text
UNS16 CHAT_NumWrappedOutputLines[CHAT_KeepOutputLines];  // How many wrapped lines does each real line represent?
//BOOL CHAT_OutputLinePrivate[CHAT_KeepOutputLines];
//wchar_t *CHAT_FluffText[NUMFLUFFTEXT];                   // pointers to full list
//wchar_t *CHAT_MiniFluffText[NUMFLUFFTEXT];                // pointers to subset
wchar_t CHAT_SmallFluffText[NUMFLUFFTEXT][CHAT_MAXCHAR]; // temp "..." versions

BOOL CHAT_ToPlayer[RULE_MAX_PLAYERS];  // Who we sending to?
UNS16 NumInputLines = 0;   // How many we got so far?
UNS16 NumOutputLines = 0;  // How many we got so far?
UNS16 TotalWrappedOutputLines = 0;  // NumOutputLines is the number of (possibly big) lines we have.  This is how many wrapped lines we have.
INT16 InputLinesOffset = 0;   // Offset for display window
INT16 OutputLinesOffset = 0;  // Offset for history use
INT16 FluffLinesOffset = 0;   // Offset for display window
// When Num[In/Out]putLines reaches CHAT_Keep[In/Out]putLines, the next two
// variables start changing.  Rather than copying all the strings when we
// overflow, just adjust all the pointers.
INT16 InputLinesBeginIndex = 0;
INT16 OutputLinesBeginIndex = 0;
INT16 LinesInWindow = 0;       // How many lines can the chat window hold?
INT16 FluffLinesInWindow = 0;  // How many lines can the fluff window hold?
UNS16 CHAT_FontSize = 7;
UNS16 CHAT_FontHeight = 0;
// Current non-AI player on this machine.  (Actually, current is "recent"...
// If it's an AI's turn, this holds the last human player's index.  If there
// are no human players, it's RULE_SPECTATOR_PLAYER)
UNS8 CHAT_CurNonAIPlayer = 255;
INT16 CHAT_FluffNumber = -1;  // Fluff text selected.
INT16 CHAT_CountTime = 0;     // A little counter
INT16 CHAT_NumNonLocalHumans = 0;  // How many people can we send to?  (Don't talk to AIs :P)

// Fluff text category related variables
#define NUMCATEGORIES 6
int CategoryStart[NUMCATEGORIES] =
{ TMN_FLUFF_greetings+1, TMN_FLUFF_common_answers+1, TMN_FLUFF_game_play_and_pace+1, 
  TMN_FLUFF_trading+1, TMN_FLUFF_positive_comments+1, TMN_FLUFF_negative_comments+1 };
int NumStrings[NUMCATEGORIES] =
{ TMN_FLUFF_common_answers - TMN_FLUFF_greetings - 1,
  TMN_FLUFF_game_play_and_pace - TMN_FLUFF_common_answers - 1,
  TMN_FLUFF_trading - TMN_FLUFF_game_play_and_pace - 1,
  TMN_FLUFF_positive_comments - TMN_FLUFF_trading - 1,
  TMN_FLUFF_negative_comments - TMN_FLUFF_positive_comments - 1,
  TMN_FLUFF_spectator - TMN_FLUFF_negative_comments - 1 };
int CurrentCategory = 127;
int CurrentDispCategory = 128;
int CurrentDispRollover = 129;
int NumFluffText = 0;  // How many in the category being displayed?

char tmp[1000];  // bleeble: temporary
//#define CHAT_boxlevel 1229  // 1231 // cursor under box at 1235, over at 1234

// Window/UI state variables
int CHAT_boxlevel = 5230;
int FLUFF_boxlevel = 5225;
BOOL CHAT_BoxActive = FALSE;
BOOL CHAT_moving = FALSE;
BOOL CHAT_sizing = FALSE;
BOOL CHAT_scrolling = FALSE;
BOOL CHAT_shade = FALSE;
BOOL CHAT_options = FALSE;
BOOL CHAT_LMBDown = FALSE;
BOOL CHAT_EditBoxActive = FALSE;
// Note that FLUFF_BoxActive can be TRUE when CHAT_BoxActive is FALSE.
// If you're not sure CHAT_BoxActive is TRUE (ie: user clicked on chatbox
// button) check (CHAT_BoxActive && FLUFF_BoxActive)
BOOL FLUFF_BoxActive = FALSE;//TRUE;
BOOL FLUFF_moving = FALSE;
BOOL FLUFF_sizing = FALSE;
BOOL FLUFF_scrolling = FALSE;
BOOL FLUFF_shade = FALSE;

int CHAT_offsetx, CHAT_offsety;
// InEditBox tracks the state of the mouse pointer...
// If it is 0, the pointer is not in the edit box.
// If it is 2, the pointer is in the edit box.
// When inside the editbox, we must change from the Artlib
// pointer to a Windows pointer. (an ibar)  
#define NOTNEAREDITBOX 0
#define INEDITBOX 2
int InEditBox = NOTNEAREDITBOX;
int CHAT_TextAlpha = 10;
int CHAT_BGAlpha = 10;
short CHAT_AlphaValues[NUM_ALPHA_CHANNEL]={
  ALPHA_TRANSPARENT, ALPHA_OPAQUE06_25, ALPHA_OPAQUE12_50, ALPHA_OPAQUE18_75,
  ALPHA_OPAQUE25_00, ALPHA_OPAQUE31_25, ALPHA_OPAQUE37_50, ALPHA_OPAQUE43_75,
  ALPHA_OPAQUE50_00, ALPHA_OPAQUE56_25, ALPHA_OPAQUE62_50, ALPHA_OPAQUE68_75,
  ALPHA_OPAQUE75_00, ALPHA_OPAQUE81_25, ALPHA_OPAQUE87_50, ALPHA_OPAQUE93_75,
  ALPHA_OPAQUE100_0 };


//#define DAT_CHAT 10  // should be in gameinc.h, but probably going away anyway
//LE_DATA_DataId CHAT_datafile = LED_EI;
// Dynamically created/destroyed window regions.
LE_DATA_DataId CHAT_BarArea, CHAT_BarArea1;
LE_DATA_DataId CHAT_TextArea, CHAT_TextArea1;
LE_DATA_DataId CHAT_OptionArea;
LE_DATA_DataId FLUFF_BarArea, FLUFF_BarArea1;
LE_DATA_DataId FLUFF_TextArea, FLUFF_TextArea1;

// Icky Windows stuff
HWND CHAT_EditBox;
HFONT CHAT_EditFont = NULL;
WNDPROC EditBox_oldwinproc;
HCURSOR hOldCursor = NULL;
BOOL bNonGDI = FALSE;
long px = 0, py = 0;

/***************************************************************************
* AdjustRect - Create screen-relative rectangle (r1) from window-relative
* rectangle (r2), window location (p1), and window size (p2).
***************************************************************************/
inline void AdjustRect(RECT *r1, RECT r2, POINT p1, POINT p2)
{
  if (r2.left >= 0)   r1->left = r2.left + p1.x;     else r1->left = r2.left + p1.x + p2.x;
  if (r2.top >= 0)    r1->top = r2.top + p1.y;       else r1->top = r2.top + p1.y + p2.y;
  if (r2.right >= 0)  r1->right = r2.right + p1.x;   else r1->right = r2.right + p1.x + p2.x;
  if (r2.bottom >= 0) r1->bottom = r2.bottom + p1.y; else r1->bottom = r2.bottom + p1.y + p2.y;
}

/***************************************************************************
* AdjustRects - Create screen-relative rectangles from window-relative ones.
***************************************************************************/
inline void AdjustRects()
{
  AdjustRect(&CHAT_chatbar, CHAT_chatbar_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_chatbox, CHAT_chatbox_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_textbox, CHAT_textbox_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_editbox, CHAT_editbox_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_upbtn, CHAT_upbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_dnbtn, CHAT_dnbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_scrollbar, CHAT_scrollbar_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_chatsbtn, CHAT_chatsbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_allbtn, CHAT_allbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_optbtn, CHAT_optbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_fluffbtn, CHAT_fluffbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_shadebtn, CHAT_shadebtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_closebtn, CHAT_closebtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_sizebtn, CHAT_sizebtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_optbox, CHAT_optbox_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_trnup1btn, CHAT_trnup1btn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_trndn1btn, CHAT_trndn1btn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_trnup2btn, CHAT_trnup2btn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_trndn2btn, CHAT_trndn2btn_rel, CHAT_winloc, CHAT_winsize);
//  AdjustRect(&CHAT_trnup3btn, CHAT_trnup3btn_rel, CHAT_winloc, CHAT_winsize);
//  AdjustRect(&CHAT_trndn3btn, CHAT_trndn3btn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_txtupbtn, CHAT_txtupbtn_rel, CHAT_winloc, CHAT_winsize);
  AdjustRect(&CHAT_txtdnbtn, CHAT_txtdnbtn_rel, CHAT_winloc, CHAT_winsize);
  // Coordinates determined by the font size
  CHAT_editbox.top = CHAT_editbox.bottom - CHAT_FontHeight;
  CHAT_textbox.bottom = CHAT_editbox.top - 1;

  LinesInWindow =
    (((CHAT_textbox.bottom - CHAT_textbox.top) -
     ((CHAT_textbox.bottom - CHAT_textbox.top) % CHAT_FontHeight))
      / CHAT_FontHeight);
}

/***************************************************************************
* AdjustFluffRects - Create screen-relative rects from window-relative ones.
***************************************************************************/
inline void AdjustFluffRects()
{
  AdjustRect(&FLUFF_chatbar, FLUFF_chatbar_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_chatbox, FLUFF_chatbox_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_textbox, FLUFF_textbox_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_upbtn, FLUFF_upbtn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_dnbtn, FLUFF_dnbtn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_scrollbar, FLUFF_scrollbar_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_chatsbtn, FLUFF_chatsbtn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_grp1btn, FLUFF_grp1btn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_grp2btn, FLUFF_grp2btn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_grp3btn, FLUFF_grp3btn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_grp4btn, FLUFF_grp4btn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_grp5btn, FLUFF_grp5btn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_grp6btn, FLUFF_grp6btn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_shadebtn, FLUFF_shadebtn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_closebtn, FLUFF_closebtn_rel, FLUFF_winloc, FLUFF_winsize);
  AdjustRect(&FLUFF_sizebtn, FLUFF_sizebtn_rel, FLUFF_winloc, FLUFF_winsize);

  FluffLinesInWindow =
    (((FLUFF_textbox.bottom - FLUFF_textbox.top) -
     ((FLUFF_textbox.bottom - FLUFF_textbox.top) % CHAT_FontHeight))
      / CHAT_FontHeight);
}

/***************************************************************************
* ScrollTextAreaUpOne - Scroll chat text area up one line.
***************************************************************************/
inline BOOL ScrollTextAreaUpOne()
{
  if (OutputLinesOffset != 0)
    { OutputLinesOffset--; CHAT_RefreshTextArea(); }
  return(TRUE);
}

/***************************************************************************
* ScrollTextAreaDownOne - Scroll chat text area down one line.
***************************************************************************/
inline BOOL ScrollTextAreaDownOne()
{
  if (OutputLinesOffset != TotalWrappedOutputLines - 1)
    { OutputLinesOffset++; CHAT_RefreshTextArea(); }
  return(TRUE);
}

/***************************************************************************
* ScrollFluffTextAreaUpOne - Scroll fluff text area up one line.
***************************************************************************/
inline BOOL ScrollFluffTextAreaUpOne()
{
  if (FluffLinesOffset != 0)
    { FluffLinesOffset--; CHAT_RefreshFluffTextArea(); }
  return(TRUE);
}

/***************************************************************************
* ScrollFluffTextAreaDownOne - Scroll fluff text area down one line.
***************************************************************************/
inline BOOL ScrollFluffTextAreaDownOne()
{
  if (FluffLinesOffset != NumFluffText - 1)
    { FluffLinesOffset++; CHAT_RefreshFluffTextArea(); }
  return(TRUE);
}

/***************************************************************************
* ShowRollover - Display the rollover in the chat window title on mouseover
***************************************************************************/
void ShowRollover(int num)
{
  // Find out the name of the most recent (incl. current) local
  // non-AI player, and put their name in the chat box title.
  // If nobody qualifies, stick in "Spectator".
  int i = UICurrentGameState.CurrentPlayer;
  int j = i;
  while (SlotIsALocalHumanPlayer[i] == 0)
  {
    i--;
    if (i < 0)   i = UICurrentGameState.NumberOfPlayers - 1;
    if (i == j)  { i = RULE_SPECTATOR_PLAYER; break; }
  }

  if ((CurrentDispRollover != num) || (i != CHAT_CurNonAIPlayer)) {
    CurrentDispRollover = num;
    LE_GRAFIX_ColorArea(CHAT_BarArea1, 0, 0,
      width(CHAT_chatbar), height(CHAT_chatbar), LEG_MCR(0, 255, 0));
    LE_FONTS_SetSettings(0);
    LE_FONTS_GetSettings(8);
    if (num < 100)
      // Show the category the mouse is over in blue
      LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(192,192,255, 255),
        LANG_GetTextMessage(TMN_ALL_PLAYERS+num));
    else {
      // Show the chatting player's name in white
      // Only change the text if the player number changed
      LE_FONTS_SetSettings(0);
      LE_FONTS_GetSettings(8);
      LE_GRAFIX_ColorArea(CHAT_BarArea1, 0, 0,
        width(CHAT_chatbar), height(CHAT_chatbar), LEG_MCR(0, 255, 0));
      if (i == RULE_SPECTATOR_PLAYER)
        LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
          LANG_GetTextMessage(TMN_FLUFF_spectator));
      else
        LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
          UICurrentGameState.Players[i].name);
      LE_GRAFIX_InvalidateRect(&CHAT_chatbar);
      LE_FONTS_GetSettings(0);
      CHAT_CurNonAIPlayer = i;
    }
    LE_FONTS_GetSettings(0);
    LE_GRAFIX_InvalidateRect(&FLUFF_chatbar);
  }
}

/***************************************************************************
* GetCategory - Switch the current fluff text category
***************************************************************************/
void GetCategory(int num)
{
  if (CurrentCategory != num) {
    // Set indices and copy out proper range of strings
    CurrentCategory = num;
    FluffLinesOffset = 0;
    NumFluffText = NumStrings[CurrentCategory];
//    for (int i = 0; i < NumFluffText; i++)
//      CHAT_MiniFluffText[i] =
//        LANG_GetTextMessage(CategoryStart[CurrentCategory] + i);

    SetWindowText(CHAT_EditBox, "");
    CHAT_FluffNumber = -1;
  }
  // Update display
  CHAT_WordWrapAll();
  CHAT_RefreshFluffTextArea();
  // Update title
  LE_GRAFIX_ColorArea(FLUFF_BarArea1, 0, 0,
    width(FLUFF_chatbar), height(FLUFF_chatbar), LEG_MCR(0, 255, 0));
  LE_FONTS_SetSettings(0);
  LE_FONTS_GetSettings(8);
  LE_FONTS_Print(FLUFF_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
    LANG_GetTextMessage(CategoryStart[CurrentCategory]-1));
  LE_FONTS_GetSettings(0);
  LE_GRAFIX_InvalidateRect(&FLUFF_chatbar);
}

/***************************************************************************
* ShowCategory - Display the category in the fluff window title on mouseover
***************************************************************************/
void ShowCategory(int num)
{
  if (CurrentDispCategory != num) {
    CurrentDispCategory = num;
    LE_GRAFIX_ColorArea(FLUFF_BarArea1, 0, 0,
      width(FLUFF_chatbar), height(FLUFF_chatbar), LEG_MCR(0, 255, 0));
    LE_FONTS_SetSettings(0);
    LE_FONTS_GetSettings(8);
    if (num < 100)
      // Show the category the mouse is over in blue
      LE_FONTS_Print(FLUFF_BarArea1, 23, 3, LEG_MCRA(192,192,255, 255),
        LANG_GetTextMessage(CategoryStart[CurrentDispCategory]-1));
    else
      // Show the category that's selected in white
      LE_FONTS_Print(FLUFF_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
        LANG_GetTextMessage(CategoryStart[CurrentCategory]-1));
    LE_FONTS_GetSettings(0);
    LE_GRAFIX_InvalidateRect(&FLUFF_chatbar);
  }
}

/***************************************************************************
* CHAT_wcscmp - Pass this to qsort to sort Unicode strings. (see below)
***************************************************************************/
//bleeble: not currently used
int CHAT_wcscmp(const void *a, const void *b)
{
  wchar_t **a1 = (wchar_t **)a, **b1 = (wchar_t **)b;
  return(wcscmp(*a1, *b1));
}

/***************************************************************************
* EditBox_WinProc - Our own window handling routine for the edit box.
* Note that using a Windows edit box rather than putting something together
* with Artlib gives us a lot of nice functionality, including being able
* to input international characters with Windows keymaps, etc.  However,
* since the edit box is a separate window and not the main Artlib window,
* the cursor that Artlib draws appears *under* the edit box, and "freezes"
* when the pointer enters the edit box.  There's a lot of code below to
* change between the Windows pointer and Artlib pointer so that this
* doesn't happen.  Anyway, since the edit box is a different window from
* the main one, Artlib doesn't handle the Windows messages, so here we
* take the Windows messages and make up stuff for UDCHAT_ProcessMessage.
***************************************************************************/
LRESULT CALLBACK EditBox_WinProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)
{
  LE_QUEUE_MessageRecord UIMessageRec;

  // Update Artlib's key handling, but not for spacebar, since
  // as well as sticking a space in the chat edit box, it
  // would activate the default Monopoly action button.
  // bleeble: Anything else to keep to ourselves?
  if (((message == WM_KEYDOWN) || (message == WM_KEYUP)) &&
       (uParam != VK_SPACE))
    LI_KEYBRD_UpdateKeybrd(message, uParam, lParam);
  else if (message == WM_ERASEBKGND)
    return 0;
  // Convert edit box-relative coords to screen coords in following cases
  else if (message == WM_LBUTTONDOWN) {
    // If user clicks in editbox, deactivate fluff text selection
    if (CHAT_FluffNumber != -1) {
      CHAT_FluffNumber = -1;
      SetWindowText(CHAT_EditBox, "");
      CHAT_RefreshFluffTextArea();
    }
    UIMessageRec.messageCode = UIMSG_MOUSE_LEFT_DOWN;
    UIMessageRec.numberA = LOWORD(lParam) + CHAT_editbox.left;
    UIMessageRec.numberB = HIWORD(lParam) + CHAT_editbox.top;
    UDCHAT_ProcessMessage(&UIMessageRec);
  } else if (message == WM_LBUTTONUP) {
    UIMessageRec.messageCode = UIMSG_MOUSE_LEFT_UP;
    UIMessageRec.numberA = LOWORD(lParam) + CHAT_editbox.left;
    UIMessageRec.numberB = HIWORD(lParam) + CHAT_editbox.top;
    UDCHAT_ProcessMessage(&UIMessageRec);
  } else if (message == WM_MOUSEMOVE) {
    UIMessageRec.messageCode = UIMSG_MOUSE_MOVED;
    UIMessageRec.numberA = LOWORD(lParam) + CHAT_editbox.left;
    UIMessageRec.numberB = HIWORD(lParam) + CHAT_editbox.top;
    UDCHAT_ProcessMessage(&UIMessageRec);
  }
  // Call the previous handler to let Windows do its thing.
  return CallWindowProc(EditBox_oldwinproc, hWnd, message, uParam, lParam);
}

/***************************************************************************
* CHAT_RefreshTextArea - Refresh the text box with user message text.
***************************************************************************/
void CHAT_RefreshTextArea()
{
  int j, pos, tmpnum = 0, lineindex, wrapindex;
  double fraction;

  if ((CHAT_BoxActive) && (NumOutputLines)/*(NumInputLines)*/)
  {
    // Clear the text area, then print the new text
    LE_GRAFIX_ColorArea(CHAT_TextArea1,
      (short)CHAT_textbox_rel.left, (short)CHAT_textbox_rel.top - BarHeight,
      width(CHAT_textbox), height(CHAT_textbox), LEG_MCR(0, 255, 0));

// bleeble: linear search for now
    // To find the text to print, we actually have to find where the index
    // lies in the sum of the CHAT_NumWrappedOutputLines array.  (from 0 to i)
    // The first line in the window may actually be a wrapped line.  Blah.
    for (lineindex = 0; lineindex </*=*/ NumOutputLines; lineindex++)
      if ((tmpnum + CHAT_NumWrappedOutputLines[lineindex]) > OutputLinesOffset)  break;
      else  tmpnum += CHAT_NumWrappedOutputLines[lineindex];

    // Wrap the line we have to display, and display/wrap
    // new lines until the window is full or we reach the end
    CHAT_WordWrap(CHAT_OutputLinesPtr[lineindex], width(CHAT_textbox), (wchar_t **)CHAT_WrappedLines, 9);
    wrapindex = OutputLinesOffset - tmpnum;
    for (j = 0; j < LinesInWindow; j++)
    {
      LE_FONTS_SetSettings(0);
      LE_FONTS_GetSettings(9);
      LE_FONTS_Print(CHAT_TextArea1, (short)CHAT_textbox_rel.left,
        (short)(CHAT_textbox_rel.top - BarHeight + (j * CHAT_FontHeight)),
        LEG_MCRA(16, 16, 16, CHAT_AlphaValues[CHAT_TextAlpha]), CHAT_WrappedLines[wrapindex]);
      LE_FONTS_GetSettings(0);
      wrapindex++;
      // Ran out of wrapped lines, wrap a new one
      if (wrapindex >= CHAT_NumWrappedOutputLines[lineindex])
      {
        wrapindex = 0;
        lineindex++;
        if (lineindex >= NumOutputLines)  break;
        CHAT_WordWrap(CHAT_OutputLinesPtr[lineindex], width(CHAT_textbox),
          (wchar_t **)CHAT_WrappedLines, 9);
      }
    }

    // Update the position of the scroll bar
    fraction = (double)OutputLinesOffset / TotalWrappedOutputLines;
    pos = (int)(fraction * (CHAT_scrollbar.bottom - CHAT_scrollbar.top - 5));
    // Clear scroll bar area
    LE_GRAFIX_ColorArea(CHAT_TextArea1, (short)(CHAT_scrollbar_rel.left+CHAT_winsize.x),
      CHAT_scrollbar_rel.top - BarHeight, width(CHAT_scrollbar), height(CHAT_scrollbar),
      LEG_MCRA(0, 255, 0, CHAT_AlphaValues[CHAT_TextAlpha]));
    // Draw little scrolly box
    LE_GRAFIX_ColorArea(CHAT_TextArea1, (short)(CHAT_scrollbar_rel.left+CHAT_winsize.x+2), // thumb
      CHAT_scrollbar_rel.top + pos - BarHeight + 1, width(CHAT_scrollbar)-4, 4,
      LEG_MCRA(255, 255, 255, CHAT_AlphaValues[CHAT_TextAlpha]));

    LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
  }
}

/***************************************************************************
* CHAT_RefreshFluffTextArea - Refresh the text box with fluff text.
***************************************************************************/
void CHAT_RefreshFluffTextArea(void)
{
  int i, pos;
  double fraction;
  RECT tmprect;
  char string[CHAT_MAXCHAR];

  if ((CHAT_BoxActive) && (FLUFF_BoxActive))
  {
    LE_FONTS_SetSettings(0);
    LE_FONTS_GetSettings(9);
    // Clear the text area, then print the new text
    LE_GRAFIX_ColorArea(FLUFF_TextArea1,
      (short)FLUFF_textbox_rel.left, (short)FLUFF_textbox_rel.top - BarHeight,
      width(FLUFF_textbox), height(FLUFF_textbox), LEG_MCR(0, 255, 0));

    // Printing the fluff text isn't as hard as the chat window, since
    // in keeping with the select-a-line-of-text model, we truncate
    // strings with "..." rather than wrapping them (done in GetCategory)
    for (i = FluffLinesOffset; i < FluffLinesOffset + FluffLinesInWindow; i++)
      if (i < NumFluffText)
        LE_FONTS_Print(FLUFF_TextArea1, (short)FLUFF_textbox_rel.left,
          FLUFF_textbox_rel.top - BarHeight + ((i - FluffLinesOffset) * CHAT_FontHeight),
          LEG_MCRA(16, 16, 190, CHAT_AlphaValues[CHAT_TextAlpha]), CHAT_SmallFluffText[i]);

    // Update the position of the scroll bar
    fraction = (double)FluffLinesOffset / NumFluffText;
    pos = (int)(fraction * (FLUFF_scrollbar.bottom - FLUFF_scrollbar.top - 5));
    // Clear scroll bar area
    LE_GRAFIX_ColorArea(FLUFF_TextArea1, (short)(FLUFF_scrollbar_rel.left+FLUFF_winsize.x),
      FLUFF_scrollbar_rel.top - BarHeight, width(FLUFF_scrollbar), height(FLUFF_scrollbar),
      LEG_MCRA(0, 255, 0, CHAT_AlphaValues[CHAT_TextAlpha]));
    // Draw little scrolly box
    LE_GRAFIX_ColorArea(FLUFF_TextArea1, (short)FLUFF_scrollbar_rel.left+FLUFF_winsize.x+2, // thumb
      FLUFF_scrollbar_rel.top + pos - BarHeight + 1, width(FLUFF_scrollbar)-4, 4,
      LEG_MCRA(255, 255, 255, CHAT_AlphaValues[CHAT_TextAlpha]));

    if (CHAT_FluffNumber >= NumFluffText)
      CHAT_FluffNumber = NumFluffText - 1;
    // Draw the selection bar if it currently exists and is visible
    if ((CHAT_FluffNumber != -1) &&
        (((CHAT_FluffNumber - FluffLinesOffset) >= 0) &&
         ((CHAT_FluffNumber - FluffLinesOffset) < FluffLinesInWindow)))
    {
      tmprect.top = FLUFF_textbox_rel.top + ((CHAT_FluffNumber - FluffLinesOffset) * CHAT_FontHeight);
      tmprect.bottom = tmprect.top + CHAT_FontHeight;
      LE_GRAFIX_ColorArea(FLUFF_TextArea1, (short)FLUFF_textbox_rel.left,
        FLUFF_textbox_rel.top - BarHeight + ((CHAT_FluffNumber - FluffLinesOffset) * CHAT_FontHeight),
        width(FLUFF_textbox), CHAT_FontHeight,
        LEG_MCRA(16, 16, 255, CHAT_AlphaValues[CHAT_TextAlpha]));
      LE_FONTS_Print(FLUFF_TextArea1, (short)FLUFF_textbox_rel.left,
        FLUFF_textbox_rel.top - BarHeight + ((CHAT_FluffNumber - FluffLinesOffset) * CHAT_FontHeight),
        LEG_MCRA(0, 255, 255, CHAT_AlphaValues[CHAT_TextAlpha]),
        CHAT_SmallFluffText[CHAT_FluffNumber]);

      // Stick the fluff text in the edit box
//      wcstombs(string, CHAT_MiniFluffText[CHAT_FluffNumber], CHAT_MAXCHAR);
      wcstombs(string, LANG_GetTextMessage(CategoryStart[CurrentCategory] + CHAT_FluffNumber), CHAT_MAXCHAR);
      SetWindowText(CHAT_EditBox, string);
      SendMessage(CHAT_EditBox, EM_SETSEL, 0, 0); // set cursor at beginning of line
      SendMessage(CHAT_EditBox, EM_SETREADONLY, (WPARAM)TRUE, 0);
    } else
      SendMessage(CHAT_EditBox, EM_SETREADONLY, (WPARAM)FALSE, 0);
    LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
    LE_FONTS_GetSettings(0);
  }
}

/***************************************************************************
* UDCHAT_ProcessFluffMessage - Process fluff window messages
***************************************************************************/
void UDCHAT_SendFluffText()
{
  int i;

  // The user clicked again on the fluff text line.
  // Clear the selection and edit box and send the message.
  SetWindowText(CHAT_EditBox, "");

  // If all players selected, then send to RULE_ALL_PLAYERS,
  // else send sequentially to selected players
// begin bleeble: this is gonna change...  make separate
// function, and add player subsets to mess.cpp
  BOOL allflag = TRUE;
  int num = 0, flag = 0;
  RULE_NetworkAddressRecord addr, addrs[RULE_MAX_PLAYERS];

  for (i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
    if ((!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0) && (!CHAT_ToPlayer[i]))
      allflag = FALSE;
  if (allflag)
  {
    MESS_SendAction (ACTION_TEXT_CHAT, CHAT_CurNonAIPlayer,
      RULE_BANK_PLAYER, RULE_ALL_PLAYERS, 0,
    CHAT_FluffNumber+CategoryStart[CurrentCategory]-CategoryStart[0]+1, 0, NULL, 0, 0);
  } else {
    for (i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
    {
      if ((!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0) && (CHAT_ToPlayer[i]))
      {
        // Don't send to same computer more than once
        MESS_GetAddressOfPlayer(i, &addr);
        flag = 1;
        for (int p = 0; p < num; p++)
        {
          if (addr.networkSystem == addrs[p].networkSystem)
          {
            if (addr.networkSystem == NS_DIRECTPLAY)
            {
              for (int r = 0; r < 4; r++)
                if (addr.address.directPlay[r] != addrs[p].address.directPlay[r])
                  break;
              if (r == 4)
                flag = 0;
            } else {
              for (int r = 0; r < 20; r++)
                if (addr.address.winSock[r] != addrs[p].address.winSock[r])
                  break;
                if (r == 20)
                  flag = 0;
            }
          }
        }
        if (flag)
        {
          MESS_SendAction (ACTION_TEXT_CHAT, CHAT_CurNonAIPlayer,
            RULE_BANK_PLAYER, i, 0,
          CHAT_FluffNumber+CategoryStart[CurrentCategory]-CategoryStart[0]+1, 0, NULL, 0, 0);
          addrs[num].networkSystem = addr.networkSystem;
          for (int q = 0; q < 20; q++)
            addrs[num].address.directPlay[q] = addr.address.directPlay[q];
          num++;
        }
      }
    }
    CHAT_ReceiveMessage(NULL,
      CHAT_FluffNumber+CategoryStart[CurrentCategory]-CategoryStart[0]+1, CHAT_CurNonAIPlayer, CHAT_CurNonAIPlayer);
  }
// end bleeble: this is gonna change...  make separate
// function, and add player subsets to mess.cpp

// (bleeble: diff queue per person?)
  // Copy the fluff text to our input queue
  if (NumInputLines == CHAT_KeepInputLines)
  {
    // If queue full, adjust pointers and start overwriting oldest stuff
    CHAT_InputLinesTop++;
    if (CHAT_InputLinesTop == CHAT_KeepInputLines)
      CHAT_InputLinesTop = 0;

    wchar_t *tmp = CHAT_InputLinesPtr[0];
    for (i = 0; i < CHAT_KeepInputLines; i++)
      CHAT_InputLinesPtr[i] = CHAT_InputLinesPtr[i + 1];
    CHAT_InputLinesPtr[CHAT_KeepInputLines-1] = tmp;

    wcscpy(CHAT_InputLinesPtr[NumInputLines-1], LANG_GetTextMessage(CategoryStart[CurrentCategory] + CHAT_FluffNumber));
  } else {
    wcscpy(CHAT_InputLinesPtr[NumInputLines], LANG_GetTextMessage(CategoryStart[CurrentCategory] + CHAT_FluffNumber));
    NumInputLines++;
  }
  // Deselect item and refresh windows
  CHAT_FluffNumber = -1;
  CHAT_RefreshTextArea();
  CHAT_RefreshFluffTextArea();
}

/***************************************************************************
* UDCHAT_ProcessFluffMessage - Process fluff window messages
***************************************************************************/
BOOL UDCHAT_ProcessFluffMessage(LE_QUEUE_MessagePointer UIMessagePntr)
{
  register int x = UIMessagePntr->numberA, y = UIMessagePntr->numberB;
  int j = 0, pos, tmpx, tmpy;
  RECT tmprect;
  double fraction;

  if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) {
    //** Handle LMB Down messages ******************************************
    // --- Bring window to front ---  // no longer needed
/*
    if ((PointInRect(x, y, FLUFF_chatbar)) || (PointInRect(x, y, FLUFF_chatbox))) {
      if (FLUFF_boxlevel < CHAT_boxlevel) {
        CHAT_Close();
        int tmp = CHAT_boxlevel;
        CHAT_boxlevel = FLUFF_boxlevel;
        FLUFF_boxlevel = tmp;
        CHAT_Open();
      }
    }
*/
    if (!FLUFF_shade) {  // Only handle items if they're displayed
      // --- Handle the scroll bar ---
      if (PointInRect(x, y, FLUFF_scrollbar)) {
        FLUFF_scrolling = TRUE;
        pos = y;
        if (pos < FLUFF_scrollbar.top)     pos = FLUFF_scrollbar.top;
        if (pos > FLUFF_scrollbar.bottom)  pos = FLUFF_scrollbar.bottom;
        fraction = (double)(pos - FLUFF_scrollbar.top) /
                   (double)(height(FLUFF_scrollbar));
        FluffLinesOffset = (int)(fraction * (NumFluffText - 1));
        CHAT_RefreshFluffTextArea();
        return(TRUE);
      }
      // --- Handle the scroll bar's up button ---
      else if (PointInRect(x, y, FLUFF_upbtn))  ScrollFluffTextAreaUpOne();
      // --- Handle the scroll bar's down button ---
      else if (PointInRect(x, y, FLUFF_dnbtn))  ScrollFluffTextAreaDownOne();
      // --- Handle resizing the window ---
      else if (PointInRect(x, y, FLUFF_sizebtn)) {
        FLUFF_sizing = TRUE;
        CHAT_offsetx = FLUFF_winloc.x + FLUFF_winsize.x - x;
        CHAT_offsety = FLUFF_winloc.y + FLUFF_winsize.y - y;
        return(TRUE);
      }
      // --- Handle selecting a line of fluff text ---
      else if (PointInRect(x, y, FLUFF_textbox)) {
        tmprect.left = FLUFF_textbox.left;
        tmprect.right = FLUFF_textbox.right;
        for (int i = 0; i < FluffLinesInWindow; i++)
        {
          tmprect.top = FLUFF_textbox.top + (i * CHAT_FontHeight);
          tmprect.bottom = tmprect.top + CHAT_FontHeight;
          if (PointInRect(x, y, tmprect))
          {
            if (CHAT_FluffNumber == i + FluffLinesOffset)
            {
              UDCHAT_SendFluffText();
              return(TRUE); //break;
            } else {
              // User clicked once on the string.  "Select" it by
              // drawing a rectangle, then print the text on top of it.
              if (i + FluffLinesOffset < NumFluffText)
              {
                CHAT_FluffNumber = i + FluffLinesOffset;
                CHAT_RefreshFluffTextArea();
                return(TRUE); //break;
              }
            }
          }
        }
      }
    } // End if (!FLUFF_shade)

    // *** Handle Fluff title bar buttons from here on
    // --- Handle the shade button ---
    if (PointInRect(x, y, FLUFF_shadebtn)) {
      FLUFF_shade = !FLUFF_shade;
      if (FLUFF_shade) {
        LE_SEQNCR_Stop(FLUFF_TextArea, FLUFF_boxlevel);
        LE_SEQNCR_Stop(FLUFF_TextArea1, FLUFF_boxlevel+1);
      } else {
        LE_SEQNCR_StartXY(FLUFF_TextArea, FLUFF_boxlevel, FLUFF_chatbox.left, FLUFF_chatbox.top);
        LE_SEQNCR_StartXY(FLUFF_TextArea1, FLUFF_boxlevel+1, FLUFF_chatbox.left, FLUFF_chatbox.top);
        CHAT_RefreshFluffTextArea();
      }
      return(TRUE);
    }
    // --- Handle closing the window ---
    else if (PointInRect(x, y, FLUFF_closebtn)) {
      CHAT_Close();
      FLUFF_BoxActive = !FLUFF_BoxActive;
      CHAT_Open();
      return(TRUE);
    // --- Handle selecting a fluff category ---
    } else if (PointInRect(x, y, FLUFF_grp1btn)) {
      GetCategory(0);
      return(TRUE);
    } else if (PointInRect(x, y, FLUFF_grp2btn)) {
      GetCategory(1);
      return(TRUE);
    } else if (PointInRect(x, y, FLUFF_grp3btn)) {
      GetCategory(2);
      return(TRUE);
    } else if (PointInRect(x, y, FLUFF_grp4btn)) {
      GetCategory(3);
      return(TRUE);
    } else if (PointInRect(x, y, FLUFF_grp5btn)) {
      GetCategory(4);
      return(TRUE);
    } else if (PointInRect(x, y, FLUFF_grp6btn)) {
      GetCategory(5);
      return(TRUE);
    } else if (PointInRect(x, y, FLUFF_chatbar)) {
      // --- Handle the chat bar ("window title") ---
      FLUFF_moving = TRUE;
      CHAT_offsetx = x - FLUFF_chatbar.left;
      CHAT_offsety = y - FLUFF_chatbar.top;
      return(TRUE);
    }
  }

  //** Handle mouse moved messages ***************************************
  else if (UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED)
  {
    if (FLUFF_moving) { // Handle moving the window
      ShowWindow(CHAT_EditBox, SW_HIDE);
      FLUFF_winloc.x = x - CHAT_offsetx;
      FLUFF_winloc.y = y - CHAT_offsety;
      AdjustFluffRects();
      LE_SEQNCR_MoveXY(FLUFF_BarArea, FLUFF_boxlevel, FLUFF_chatbar.left, FLUFF_chatbar.top);
      LE_SEQNCR_MoveXY(FLUFF_BarArea1, FLUFF_boxlevel+1, FLUFF_chatbar.left, FLUFF_chatbar.top);
      if (!FLUFF_shade) {
        LE_SEQNCR_MoveXY(FLUFF_TextArea, FLUFF_boxlevel, FLUFF_chatbox.left, FLUFF_chatbox.top);
        LE_SEQNCR_MoveXY(FLUFF_TextArea1, FLUFF_boxlevel+1, FLUFF_chatbox.left, FLUFF_chatbox.top);
      }
      return(TRUE);
    }

    if (!FLUFF_shade) {
      if (FLUFF_scrolling) { // Scrollbar handling
        pos = y;
        if (pos < FLUFF_scrollbar.top)     pos = FLUFF_scrollbar.top;
        if (pos > FLUFF_scrollbar.bottom)  pos = FLUFF_scrollbar.bottom;
        fraction = (double)(pos - FLUFF_scrollbar.top) /
                   (double)(height(FLUFF_scrollbar));
        FluffLinesOffset = (int)(fraction * (NumFluffText - 1));
        CHAT_RefreshFluffTextArea();
        return(TRUE);
      } else if (FLUFF_sizing) { // Handle sizing the window
        tmpx = x + CHAT_offsetx - FLUFF_winloc.x;
        tmpy = y + CHAT_offsety - FLUFF_winloc.y;
        if (tmpx < 246)   tmpx = 246;
        if (tmpx > 1601)  tmpx = 1601;
        if (tmpy < 99)    tmpy = 99;
        if (tmpy > 1201)  tmpy = 1201;
        // Only allow window dimensions divisible by 5 (disregarding
        // the title bar) for border drawing.  (+1 since winsize is +1)
        tmpx = ((tmpx / 5) * 5)+1;
        tmpy = (((tmpy-BarHeight-1) / 5) * 5)+BarHeight+1;
        if ((tmpx != FLUFF_winsize.x) || (tmpy != FLUFF_winsize.y))
        {
          FLUFF_winsize.x = tmpx;
          FLUFF_winsize.y = tmpy;
          AdjustFluffRects();
          CHAT_Close();
          CHAT_WordWrapAll();
          CHAT_Open();
        }
        return(TRUE);
      }
    }
  }

  //** Handle LMB Up messages ********************************************
  // TIM - Russ changed this to left_down from left_up - all regular mouse stuff I have changed this way,
  // I am mentioning it here because this code is a bit more complicated - let me know if this causes any problems.
  // RUSS - Tim changed it back.  Guess what happens when you can't trap LMB up messages when you're
  // moving or sizing the window?  Yup.  You're stuck moving it for eternity.
  else if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP) {
    if (FLUFF_moving) {
      if (!CHAT_shade) {
        MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
          width(CHAT_editbox), height(CHAT_editbox), TRUE);
        ShowWindow(CHAT_EditBox, SW_SHOW);
      }
      FLUFF_moving = FALSE;
      return(TRUE);
    }

    if (!FLUFF_shade) {
      if (FLUFF_scrolling) {
        FLUFF_scrolling = FALSE;
        return(TRUE);
      } else if (FLUFF_sizing) {
        MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
          width(CHAT_editbox), height(CHAT_editbox), TRUE);
        ShowWindow(CHAT_EditBox, SW_SHOW);
        FLUFF_sizing = FALSE;
        return(TRUE);
      }
    }
  }

  // Don't allow others to see this message if the pointer is in the
  // chat box.  (ie: If you click in the chat window, the message
  // won't "fall through" to the Monopoly button underneath it.
//bleeble: Any events I shouldn't be stealing?  I give back timer ones...
  if (((UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)) &&
      (!FLUFF_shade) && (PointInRect(x, y, FLUFF_chatbox)))
    return(TRUE);
  if (((UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)) &&
      (PointInRect(x, y, FLUFF_chatbar)))
    return(TRUE);
  return(FALSE);
}

/***************************************************************************
* UDCHAT_ProcessChatMessage - Process chat window messages
***************************************************************************/
BOOL UDCHAT_ProcessChatMessage(LE_QUEUE_MessagePointer UIMessagePntr)
{
  register int x = UIMessagePntr->numberA, y = UIMessagePntr->numberB;
  int i, j = 0;
  int pos;
  int tmpx, tmpy;
  RECT tmprect;
  double fraction;
  char astring[CHAT_MAXCHAR];
  wchar_t ustring[CHAT_MAXCHAR];

  if (UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED)
  {
    if ((!CHAT_moving) && (!CHAT_sizing) && (!CHAT_scrolling) &&
        (!FLUFF_moving) && (!FLUFF_sizing) && (!FLUFF_scrolling) && (!CHAT_shade))
    {
      // Icky.  This if changes the pointer to Windows/Artlib pointer
      // based on where it is on the screen. See EditBox_WinProc function comments.
      if (PointInRect(x, y, CHAT_editbox)) // In EditBox
      {
        if (InEditBox == NOTNEAREDITBOX) {
          InEditBox = INEDITBOX;
          hOldCursor = SetCursor(LoadCursor(NULL, IDC_IBEAM));
//          LE_MOUSE_MousePointerOff();
#if CE_ARTLIB_EnableSystemSequencer
            LE_SEQNCR_AddMouseSubSequence (LE_DATA_EmptyItem);
#endif
          ShowCursor(TRUE);
        }
      } else { // Not near EditBox
        if (!CHAT_LMBDown) {
          if (InEditBox == INEDITBOX) {
            if (bNonGDI) {
              RECT rect;
              rect.left = px;        rect.top = py;
              rect.right = px + 20;  rect.bottom = py + 35;
              LE_GRAFIX_InvalidateRect(&rect);
            }
            InEditBox = NOTNEAREDITBOX;
            hOldCursor = SetCursor(LoadCursor(NULL, IDC_ARROW));
            if (hOldCursor != NULL) {
              ShowCursor(FALSE);
              LE_SEQNCR_AddMouseSubSequenceTheWorks(
                LED_IFT(DAT_MAIN, TAB_pointer), 100, 0, 0, TRUE);
              SetCursor(hOldCursor);
            }
          }
        }
      }
    }
  }

  if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) {
    //** Handle LMB Down messages ******************************************
    // --- Bring window to front ---  // not needed
/*
    if (CHAT_boxlevel < FLUFF_boxlevel) {
      CHAT_Close();
      int tmp = CHAT_boxlevel;
      CHAT_boxlevel = FLUFF_boxlevel;
      FLUFF_boxlevel = tmp;
      CHAT_Open();
     }
*/
    // *** Handle Options box buttons, if it's open ***
    if ((CHAT_options) && (PointInRect(x, y, CHAT_optbox))) {
      if ((PointInRect(x, y, CHAT_txtdnbtn)) && (CHAT_FontSize < FONTMAX)) {
        // --- Handle the text size down button ---
        CHAT_Close();
        LE_FONTS_SetSettings(0);
        LE_FONTS_GetSettings(9);
        CHAT_FontSize++;
        LE_FONTS_SetSize(CHAT_FontSize);
        CHAT_FontHeight = LE_FONTS_GetStringHeight(A_T("Xy"));
        LE_FONTS_SetSettings(9);

        // Set the font for the edit box
        if (CHAT_EditFont != NULL)
          DeleteObject(CHAT_EditFont);
        CHAT_EditFont = CreateFontIndirect(LE_FONTS_GetLogfont());
        SendMessage(CHAT_EditBox, WM_SETFONT, (WPARAM)CHAT_EditFont, MAKELPARAM(TRUE, 0));
        SendMessage(CHAT_EditBox, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELONG(1, 1));
        SendMessage(CHAT_EditBox, EM_LIMITTEXT, CHAT_MAXCHAR - 1, 0);
        CHAT_editbox.top = CHAT_editbox.bottom - CHAT_FontHeight;
        CHAT_textbox.bottom = CHAT_editbox.top - 1;
        LE_FONTS_GetSettings(0);
        CHAT_WordWrapAll();
        CHAT_Open();
      } else if ((PointInRect(x, y, CHAT_txtupbtn)) && (CHAT_FontSize > FONTMIN)) {
        // --- Handle the text size up button ---
        CHAT_Close();
        LE_FONTS_SetSettings(0);
        LE_FONTS_GetSettings(9);
        CHAT_FontSize--;
        LE_FONTS_SetSize(CHAT_FontSize);
        CHAT_FontHeight = LE_FONTS_GetStringHeight(A_T("Xy"));
        LE_FONTS_SetSettings(9);

        // Set the font for the edit box
        if (CHAT_EditFont != NULL)
          DeleteObject(CHAT_EditFont);
        CHAT_EditFont = CreateFontIndirect(LE_FONTS_GetLogfont());
        SendMessage(CHAT_EditBox, WM_SETFONT, (WPARAM)CHAT_EditFont, MAKELPARAM(TRUE, 0));
        SendMessage(CHAT_EditBox, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELONG(1, 1));
        SendMessage(CHAT_EditBox, EM_LIMITTEXT, CHAT_MAXCHAR - 1, 0);
        CHAT_editbox.top = CHAT_editbox.bottom - CHAT_FontHeight;
        CHAT_textbox.bottom = CHAT_editbox.top - 1;
        LE_FONTS_GetSettings(0);
        CHAT_WordWrapAll();
        CHAT_Open();
      } else if ((PointInRect(x, y, CHAT_trnup1btn)) && (CHAT_BGAlpha > 0)) {
        // --- Handle the BG transparency down button ---
        CHAT_BGAlpha--;
        LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
        LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
        if (FLUFF_BoxActive) {
          LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
          LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
        }
      } else if ((PointInRect(x, y, CHAT_trndn1btn)) && (CHAT_BGAlpha < NUM_ALPHA_CHANNEL - 1)) {
        // --- Handle the BG transparency down button ---
        CHAT_BGAlpha++;
        LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
        LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
        if (FLUFF_BoxActive) {
          LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
          LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
        }
      } else if ((PointInRect(x, y, CHAT_trnup2btn)) && (CHAT_TextAlpha > 1)) {
        // --- Handle the text transparency down button ---
        CHAT_TextAlpha--;
        LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
        LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
        if (FLUFF_BoxActive) {
          LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
          LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
        }
      } else if ((PointInRect(x, y, CHAT_trndn2btn)) && (CHAT_TextAlpha < NUM_ALPHA_CHANNEL - 1)) {
        // --- Handle the text transparency down button ---
        CHAT_TextAlpha++;
        LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
        LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
        if (FLUFF_BoxActive) {
          LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
          LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
        }
/*
      } else if (PointInRect(x, y, CHAT_trnup3btn)) {
        // --- Handle the BG+text transparency down button ---
        if (CHAT_TextAlpha > 1) {
          CHAT_TextAlpha--;
          LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
          if (FLUFF_BoxActive)  LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
        }
        if (CHAT_BGAlpha > 0) {
          CHAT_BGAlpha--;
          LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
          if (FLUFF_BoxActive)  LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
        }
        LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
        LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
      } else if (PointInRect(x, y, CHAT_trndn3btn)) {
        // --- Handle the BG+text transparency down button ---
        if (CHAT_TextAlpha < NUM_ALPHA_CHANNEL - 1) {
          CHAT_TextAlpha++;
          LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
          if (FLUFF_BoxActive) LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea1, CHAT_AlphaValues[CHAT_TextAlpha]);
        }
        if (CHAT_BGAlpha < NUM_ALPHA_CHANNEL - 1) {
          CHAT_BGAlpha++;
          LE_GRAFIX_ChangeObjectAlpha(CHAT_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
          if (FLUFF_BoxActive) LE_GRAFIX_ChangeObjectAlpha(FLUFF_TextArea, CHAT_AlphaValues[CHAT_BGAlpha]);
        }
        LE_GRAFIX_InvalidateRect(&CHAT_chatbox);
        LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
*/
      }
      return(TRUE);
    }

    // --- Handle clicking on users to send privately to ---
    tmprect.top = CHAT_chatbar.top + 3;
    tmprect.bottom = tmprect.top + 12;
    for (j = 0, i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
    {
      if ((!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0)) {
//                width(CHAT_chatbar) - 1 - ((j + BTNOFFSET) * 19), 2, 19, 14, LEG_MCR(220,220,220));
        tmprect.left = CHAT_chatbar.left + width(CHAT_chatbar) - ((j + BTNOFFSET) * 19) + 4;
        tmprect.right = tmprect.left + 17;
        if (PointInRect(x, y, tmprect)) {
          CHAT_ToPlayer[i] = !CHAT_ToPlayer[i]; // Timer code will update display
          return(TRUE);
        }
        j++;
      }
    }

    if (!CHAT_shade) {
      // --- Handle the scroll bar ---
      if (PointInRect(x, y, CHAT_scrollbar))
      {
        CHAT_scrolling = TRUE;
        pos = y;
        if (pos < CHAT_scrollbar.top)     pos = CHAT_scrollbar.top;
        if (pos > CHAT_scrollbar.bottom)  pos = CHAT_scrollbar.bottom;
        fraction = (double)(pos - CHAT_scrollbar.top) /
                   (double)(height(CHAT_scrollbar));
        OutputLinesOffset = (int)(fraction * (TotalWrappedOutputLines - 1));
        CHAT_RefreshTextArea();
        return(TRUE);
      }
      // --- Handle the scroll bar's up button ---
      else if (PointInRect(x, y, CHAT_upbtn))  ScrollTextAreaUpOne();
      // --- Handle the scroll bar's down button ---
      else if (PointInRect(x, y, CHAT_dnbtn))  ScrollTextAreaDownOne();
      // --- Handle resizing the window ---
      else if (PointInRect(x, y, CHAT_sizebtn))
      {
        CHAT_sizing = TRUE;
        CHAT_offsetx = CHAT_winloc.x + CHAT_winsize.x - x;
        CHAT_offsety = CHAT_winloc.y + CHAT_winsize.y - y;
        return(TRUE);
      }
    }

    // --- Handle the options button ---
    if (PointInRect(x, y, CHAT_optbtn))
    {
      CHAT_options = !CHAT_options;
      if (CHAT_options) {
        if (CHAT_shade) {
          CHAT_shade = FALSE;
          LE_SEQNCR_StartXY(CHAT_TextArea, CHAT_boxlevel, CHAT_chatbox.left, CHAT_chatbox.top);
          LE_SEQNCR_StartXY(CHAT_TextArea1, CHAT_boxlevel+1, CHAT_chatbox.left, CHAT_chatbox.top);
          CHAT_RefreshTextArea();
          MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
            width(CHAT_editbox), height(CHAT_editbox), TRUE);
          ShowWindow(CHAT_EditBox, SW_SHOW);
        }
        LE_SEQNCR_StartXY(CHAT_OptionArea, CHAT_boxlevel+2, CHAT_optbox.left, CHAT_optbox.top);
      } else {
        LE_SEQNCR_Stop(CHAT_OptionArea, CHAT_boxlevel+2);
      }
      return(TRUE);
    }

    // --- Handle the shade button ---
    if (PointInRect(x, y, CHAT_shadebtn))
    {
      CHAT_shade = !CHAT_shade;
      if (CHAT_shade) {
        LE_SEQNCR_Stop(CHAT_TextArea, CHAT_boxlevel);
        LE_SEQNCR_Stop(CHAT_TextArea1, CHAT_boxlevel+1);
        if (CHAT_options) {
          LE_SEQNCR_Stop(CHAT_OptionArea, CHAT_boxlevel+2);
          CHAT_options = FALSE;
        }
        ShowWindow(CHAT_EditBox, SW_HIDE);
        CHAT_options = FALSE;
      } else {
        LE_SEQNCR_StartXY(CHAT_TextArea, CHAT_boxlevel, CHAT_chatbox.left, CHAT_chatbox.top);
        LE_SEQNCR_StartXY(CHAT_TextArea1, CHAT_boxlevel+1, CHAT_chatbox.left, CHAT_chatbox.top);
        if (CHAT_options)
          LE_SEQNCR_StartXY(CHAT_OptionArea, CHAT_boxlevel+2, CHAT_optbox.left, CHAT_optbox.top);
        CHAT_RefreshTextArea();
        MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
          width(CHAT_editbox), height(CHAT_editbox), TRUE);
        ShowWindow(CHAT_EditBox, SW_SHOW);
      }
      return(TRUE);
    // --- Handle closing the window ---
    } else if (PointInRect(x, y, CHAT_closebtn)) {
      CHAT_Close();
      return(TRUE);
    } else if (PointInRect(x, y, CHAT_allbtn)) {
      // --- Handle the "all" button ---
      // If everybody is already on, turn them all off.
      BOOL notallflag = FALSE;
      for (i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
        if ((!CHAT_ToPlayer[i]) && (!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0))
          { notallflag = TRUE; break; }
      for (i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
        CHAT_ToPlayer[i] = notallflag;
      return(TRUE);
    }
    // --- Handle the fluff text button ---
    else if (PointInRect(x, y, CHAT_fluffbtn))
    {
      CHAT_Close();
      FLUFF_BoxActive = !FLUFF_BoxActive;
      CHAT_Open();
    } else if (PointInRect(x, y, CHAT_chatbar)) {
      // --- Handle the chat bar ("window title") ---
      CHAT_moving = TRUE;
      CHAT_offsetx = x - CHAT_chatbar.left;
      CHAT_offsety = y - CHAT_chatbar.top;
      return(TRUE);
    }
  }

  //** Handle mouse moved messages ***************************************
  else if (UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED)
  {
    if (CHAT_moving) { // Handle moving the window
      ShowWindow(CHAT_EditBox, SW_HIDE);
      CHAT_winloc.x = x - CHAT_offsetx;
      CHAT_winloc.y = y - CHAT_offsety;
      AdjustRects();
      LE_SEQNCR_MoveXY(CHAT_BarArea, CHAT_boxlevel, CHAT_chatbar.left, CHAT_chatbar.top);
      LE_SEQNCR_MoveXY(CHAT_BarArea1, CHAT_boxlevel+1, CHAT_chatbar.left, CHAT_chatbar.top);
      if (!CHAT_shade)
      {
        LE_SEQNCR_MoveXY(CHAT_TextArea, CHAT_boxlevel, CHAT_chatbox.left, CHAT_chatbox.top);
        LE_SEQNCR_MoveXY(CHAT_TextArea1, CHAT_boxlevel+1, CHAT_chatbox.left, CHAT_chatbox.top);
        if (CHAT_options)
          LE_SEQNCR_MoveXY(CHAT_OptionArea, CHAT_boxlevel+2, CHAT_optbox.left, CHAT_optbox.top);
        MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
          width(CHAT_editbox), height(CHAT_editbox), TRUE);
      }
      return(TRUE);
    }
    if (!CHAT_shade) {
      if (CHAT_scrolling) { // Scrollbar handling
        pos = y;
        if (pos < CHAT_scrollbar.top)     pos = CHAT_scrollbar.top;
        if (pos > CHAT_scrollbar.bottom)  pos = CHAT_scrollbar.bottom;
        fraction = (double)(pos - CHAT_scrollbar.top) /
                   (double)(height(CHAT_scrollbar));
        OutputLinesOffset = (int)(fraction * (TotalWrappedOutputLines - 1));
        CHAT_RefreshTextArea();
        return(TRUE);
      } else if (CHAT_sizing) { // Handle sizing the window
        ShowWindow(CHAT_EditBox, SW_HIDE);
        tmpx = x + CHAT_offsetx - CHAT_winloc.x;
        tmpy = y + CHAT_offsety - CHAT_winloc.y;
        if (tmpx < 246)   tmpx = 246;
        if (tmpx > 1601)  tmpx = 1601;
        if (tmpy < 99)    tmpy = 99;
        if (tmpy > 1201)  tmpy = 1201;
        // Only allow window dimensions divisible by 5 (disregarding
        // the title bar) for border drawing.  (+1 since winsize is +1)
        tmpx = ((tmpx / 5) * 5)+1;
        tmpy = (((tmpy-BarHeight-1) / 5) * 5)+BarHeight+1;
        if ((tmpx != CHAT_winsize.x) || (tmpy != CHAT_winsize.y))
        {
          CHAT_winsize.x = tmpx;
          CHAT_winsize.y = tmpy;
          AdjustRects();
          CHAT_Close();
          MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
            width(CHAT_editbox), height(CHAT_editbox), TRUE);
          CHAT_WordWrapAll();
          CHAT_Open();
          if (CHAT_options)
            LE_SEQNCR_MoveXY(CHAT_OptionArea, CHAT_boxlevel+2, CHAT_optbox.left, CHAT_optbox.top);
        }
        return(TRUE);
      }
    }
  }

  //** Handle LMB Up messages ********************************************
  else if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP)// changed to down  // changed back to up
  {
    if (CHAT_moving) {
      if (!CHAT_shade) {
        MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
          width(CHAT_editbox), height(CHAT_editbox), TRUE);
        ShowWindow(CHAT_EditBox, SW_SHOW);
      }
      CHAT_moving = FALSE;
      return(TRUE);
    }
    if (!CHAT_shade) {
      if (CHAT_scrolling) {
        CHAT_scrolling = FALSE;
        return(TRUE);
      } else if (CHAT_sizing) {
        MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
          width(CHAT_editbox), height(CHAT_editbox), TRUE);
        ShowWindow(CHAT_EditBox, SW_SHOW);
        CHAT_sizing = FALSE;
        return(TRUE);
      }
    }
  }

  //** Handle keyboard released messages *********************************
  else if (UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED)
  {
    if (CHAT_FluffNumber != -1)
      UDCHAT_SendFluffText();

    if (!CHAT_shade) {
      // Scroll the text area
      if (x == LE_KEYBRD_PAGEUP)         ScrollTextAreaUpOne();
      else if (x == LE_KEYBRD_PAGEDOWN)  ScrollTextAreaDownOne();
    }

    // Edit box-only key bindings
    if (CHAT_EditBoxActive)
    {
      // Scroll the input history (edit box)
      if ((x == LE_KEYBRD_UP) &&
          (InputLinesOffset < NumInputLines))
      {
        // Scroll up history list
        InputLinesOffset++;
        wcstombs(astring, CHAT_InputLinesPtr[NumInputLines - InputLinesOffset], CHAT_MAXCHAR);
        SetWindowText(CHAT_EditBox, astring);
        SendMessage(CHAT_EditBox, EM_SETSEL, CHAT_MAXCHAR, CHAT_MAXCHAR);  // set cursor at end of line
      } else if ((x == LE_KEYBRD_DOWN) &&
                 (InputLinesOffset > 1)) {
        // Scroll down history list
        InputLinesOffset--;
        wcstombs(astring, CHAT_InputLinesPtr[NumInputLines - InputLinesOffset], CHAT_MAXCHAR);
        SetWindowText(CHAT_EditBox, astring);
        SendMessage(CHAT_EditBox, EM_SETSEL, CHAT_MAXCHAR, CHAT_MAXCHAR);  // set cursor at end of line
      } else if (x == LE_KEYBRD_RET) {
        // Send the message and clear the edit box
        GetWindowText(CHAT_EditBox, astring, CHAT_MAXCHAR);
        SetWindowText(CHAT_EditBox, "");
        // Don't send blank messages!
        // The user can still send space-filled messages, if they're clumsy or annoying.
        if (astring[0] != 0)
        {
          wchar_t *MemoryPntr;
          RULE_CharHandle MemoryHandle;
          mbstowcs(ustring, astring, CHAT_MAXCHAR);
          // Copy to input history
          if (NumInputLines == CHAT_KeepInputLines)
          {
            // If queue full, adjust pointers and start overwriting oldest stuff
            CHAT_InputLinesTop++;
            if (CHAT_InputLinesTop == CHAT_KeepInputLines)
              CHAT_InputLinesTop = 0;
            wchar_t *tmp = CHAT_InputLinesPtr[0];
            for (i = 0; i < CHAT_KeepInputLines; i++)
              CHAT_InputLinesPtr[i] = CHAT_InputLinesPtr[i + 1];
            CHAT_InputLinesPtr[CHAT_KeepInputLines-1] = tmp;
            wcscpy(CHAT_InputLinesPtr[NumInputLines-1], ustring);
          } else {
            wcscpy(CHAT_InputLinesPtr[NumInputLines], ustring);
            NumInputLines++;
          }

          // If all players selected, then send to RULE_ALL_PLAYERS,
          // else send sequentially to selected players
// begin bleeble: this is gonna change...  make separate
// function, and add player subsets to mess.cpp
          BOOL allflag = TRUE;
          int num = 0, flag = 0;
          RULE_NetworkAddressRecord addr, addrs[RULE_MAX_PLAYERS];

          for (i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
            if ((!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0) && (!CHAT_ToPlayer[i]))
              allflag = FALSE;
          if (allflag) {
            MemoryHandle = RULE_AllocateHandle(wcslen(ustring)*2+2);
            if (MemoryHandle == NULL) return(TRUE);
            MemoryPntr = (unsigned short *)RULE_LockHandle(MemoryHandle);
            if (MemoryPntr == NULL) {
              RULE_FreeHandle(MemoryHandle);
              return(TRUE);
            }
            wcscpy(MemoryPntr, ustring);
            RULE_UnlockHandle(MemoryHandle);

            MESS_SendAction (ACTION_TEXT_CHAT, CHAT_CurNonAIPlayer,
              RULE_BANK_PLAYER, RULE_ALL_PLAYERS, 0, 0, 0, NULL, 0, MemoryHandle);
          } else {
            for (i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
            {
              if ((!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0) && (CHAT_ToPlayer[i]))
              {
                // Don't send to same computer more than once
                MESS_GetAddressOfPlayer(i, &addr);
                flag = 1;
                for (int p = 0; p < num; p++)
                {
                  if (addr.networkSystem == addrs[p].networkSystem)
                  {
                    if (addr.networkSystem == NS_DIRECTPLAY)
                    {
                      for (int r = 0; r < 4; r++)
                        if (addr.address.directPlay[r] != addrs[p].address.directPlay[r])
                          break;
                      if (r == 4)
                        flag = 0;
                    } else {
                      for (int r = 0; r < 20; r++)
                        if (addr.address.winSock[r] != addrs[p].address.winSock[r])
                          break;
                      if (r == 20)
                        flag = 0;
                    }
                  }
                }
                if (flag)
                {
                  MemoryHandle = RULE_AllocateHandle(wcslen(ustring)*2+2);
                  if (MemoryHandle == NULL) return(TRUE);
                  MemoryPntr = (unsigned short *)RULE_LockHandle(MemoryHandle);
                  if (MemoryPntr == NULL) {
                    RULE_FreeHandle(MemoryHandle);
                    return(TRUE);
                  }
                  wcscpy(MemoryPntr, ustring);
                  RULE_UnlockHandle(MemoryHandle);

                  MESS_SendAction (ACTION_TEXT_CHAT, CHAT_CurNonAIPlayer,
                    RULE_BANK_PLAYER, i, 0, 0, 0, NULL, 0, MemoryHandle);
                  addrs[num].networkSystem = addr.networkSystem;
                  for (int q = 0; q < 20; q++)
                    addrs[num].address.directPlay[q] = addr.address.directPlay[q];
                  num++;
                }
              }
            }
            CHAT_ReceiveMessage(ustring, 0, CHAT_CurNonAIPlayer, CHAT_CurNonAIPlayer);
          }
// end bleeble: this is gonna change...  make separate
// function, and add player subsets to mess.cpp
        }
      }
      return(TRUE);
    }
  }

  // Don't allow others to see this message if the pointer is in the
  // chat box.  (ie: If you click in the chat window, the message
  // won't "fall through" to the Monopoly button underneath it.
//bleeble: Any events I shouldn't be stealing?  I give back timer ones...
  if (((UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)) &&
      (!CHAT_shade) && (PointInRect(x, y, CHAT_chatbox)))
    return(TRUE);
  if (((UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP) ||
       (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)) &&
      (PointInRect(x, y, CHAT_chatbar)))
    return(TRUE);
  return(FALSE);
}

/***************************************************************************
* UDCHAT_ProcessMessage - Handle all of the messages for the chat box.
* Mostly checks mouse clicks in rectangles.
* Note that TRUE is returned if other message handlers should NOT be called
* (ie: "I handled the message"), FALSE is returned if other message handlers
* should still be called.  This is so that if the user clicks on the chat
* window, the message doesn't "fall through" to Monopoly buttons.
* However, for timing messages, FALSE is returned even though this function
* does something with them, to allow other message processors to see them.
***************************************************************************/
BOOL UDCHAT_ProcessMessage(LE_QUEUE_MessagePointer UIMessagePntr)
{
  register int x = LE_MOUSE_MouseX, y = LE_MOUSE_MouseY;
  int j = 0;

  CHAT_EditBoxActive = (GetActiveWindow() == CHAT_EditBox);
  // Why our own LMBDown var?  Artlib doesn't handle events in the edit box
  if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)     CHAT_LMBDown = TRUE;
  else if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_UP)  CHAT_LMBDown = FALSE;

  //** Handle Timer messages *********************************************
  if ((UIMessagePntr->messageCode == UIMSG_TIMER_REACHED_ZERO) &&
      (UIMessagePntr->numberA == MAIN_GAME_TIMER))
  {
    if (!MESS_NetworkMode)
      CHAT_Close();
    if (CHAT_BoxActive) {
      CHAT_CountTime++;
      if (CHAT_CountTime == 4) {  // Only do a few times a second.
        CHAT_CountTime = 0;

        if ((!CHAT_moving) && (!CHAT_sizing) && (!CHAT_scrolling) &&
            (!FLUFF_moving) && (!FLUFF_sizing) && (!FLUFF_scrolling)) {
          if (bNonGDI) {
            // Get a DC to the screen (where our window is) and
            // Get a DC to the buffer on the non-GDI device (where we need to copy it)
            HDC shDC, dhDC;
            Surface *pSurface = pc3D::GetPrimarySurface();
            shDC = GetDC(NULL);
            dhDC = pSurface->GetDC();
            BitBlt(dhDC, CHAT_editbox.left, CHAT_editbox.top, width(CHAT_editbox), height(CHAT_editbox), shDC, CHAT_editbox.left, CHAT_editbox.top, SRCCOPY);

            if (InEditBox == INEDITBOX)
            {
              POINT pt;
              HCURSOR MouseCursorCur;
              static HCURSOR MouseCursor;
              static ICONINFO IconInfo;
              // Now draw the mouse on the backbuffer
              MouseCursorCur = GetCursor();
              if (MouseCursorCur != MouseCursor)
              {
                MouseCursor = MouseCursorCur;
                GetIconInfo(MouseCursor, &IconInfo);
                if (IconInfo.hbmMask)
                  DeleteObject(IconInfo.hbmMask);
                if (IconInfo.hbmColor)
                  DeleteObject(IconInfo.hbmColor);
              }

              GetCursorPos(&pt);
              pt.x -= IconInfo.xHotspot;
              pt.y -= IconInfo.yHotspot;

              RECT rect;
              rect.left = px;        rect.top = py;
              rect.right = px + 20;  rect.bottom = py + 35;
              LE_GRAFIX_InvalidateRect(&rect);
              px = pt.x;  py = pt.y;

              //BitBlt(dhDC, CHAT_editbox.left, CHAT_editbox.top, width(CHAT_editbox), height(CHAT_editbox), shDC, CHAT_editbox.left, CHAT_editbox.top, SRCCOPY);
              DrawIcon(dhDC, pt.x, pt.y, MouseCursor);
            }
            ReleaseDC(NULL, shDC);
            pSurface->ReleaseDC(dhDC);
          }

          // Scroll the display while the mouse is pressed in the arrow buttons
          //if (LE_MOUSE_MousePressed[LEFT_BUTTON]) {
          if (CHAT_LMBDown) {
            if (!CHAT_shade) {
              // --- Handle the scroll bar's up button ---
              if (PointInRect(x, y, CHAT_upbtn))       ScrollTextAreaUpOne();
              // --- Handle the scroll bar's down button ---
              else if (PointInRect(x, y, CHAT_dnbtn))  ScrollTextAreaDownOne();
            }
            if (!FLUFF_shade) {
              // (Special: Don't handle fluff buttons if
              //   chat window is on top of it.  blah)
              // --- Handle the scroll bar's up button ---
              if ((PointInRect(x, y, FLUFF_upbtn)) &&
                  !(PointInRect(x, y, CHAT_chatbar)) &&
                  !(PointInRect(x, y, CHAT_chatbox)))
                ScrollFluffTextAreaUpOne();
              // --- Handle the scroll bar's down button ---
              else if ((PointInRect(x, y, FLUFF_dnbtn)) &&
                       !(PointInRect(x, y, CHAT_chatbar)) &&
                       !(PointInRect(x, y, CHAT_chatbox)))
                ScrollFluffTextAreaDownOne();
            }
//tmptmptmp
/*
            if (PointInRect(x, y, CHAT_fluffbtn))
            {
              CHAT_Close();
//              FLUFF_BoxActive = !FLUFF_BoxActive;
              CHAT_Open();
            }
*/
          } // End LMB pressed

          // Temporarily show the user what category the button means in the title
          // bar. (writing out all the categories in full takes a lot of space)
          if (PointInRect(x, y, CHAT_allbtn)) {
            ShowRollover(0);
          } else if (PointInRect(x, y, CHAT_optbtn)) {
            ShowRollover(2);
          } else if (PointInRect(x, y, CHAT_fluffbtn)) {
            ShowRollover(1);
          } else {
            ShowRollover(255);
          }

          // Temporarily show the user what category the button means in the title
          // bar. (writing out all the categories in full takes a lot of space)
          if (FLUFF_BoxActive) {
            if (PointInRect(x, y, FLUFF_grp1btn)) {
              ShowCategory(0);
            } else if (PointInRect(x, y, FLUFF_grp2btn)) {
              ShowCategory(1);
            } else if (PointInRect(x, y, FLUFF_grp3btn)) {
              ShowCategory(2);
            } else if (PointInRect(x, y, FLUFF_grp4btn)) {
              ShowCategory(3);
            } else if (PointInRect(x, y, FLUFF_grp5btn)) {
              ShowCategory(4);
            } else if (PointInRect(x, y, FLUFF_grp6btn)) {
              ShowCategory(5);
            } else {
              ShowCategory(255);
            }
          }
        }

/*
        // Find out the name of the most recent (incl. current) local
        // non-AI player, and put their name in the chat box title.
        // If nobody qualifies, stick in "Spectator".
        int i = UICurrentGameState.CurrentPlayer;
        int j = i;
        while (SlotIsALocalHumanPlayer[i] == 0)
        {
          i--;
          if (i < 0)   i = UICurrentGameState.NumberOfPlayers - 1;
          if (i == j)  { i = RULE_SPECTATOR_PLAYER; break; }
        }

        // Only change the text if the player number changed
        if (i != CHAT_CurNonAIPlayer)
        {
          LE_FONTS_SetSettings(0);
          LE_FONTS_GetSettings(8);
          LE_GRAFIX_ColorArea(CHAT_BarArea1, 0, 0,
            width(CHAT_chatbar), height(CHAT_chatbar), LEG_MCR(0, 255, 0));
          if (i == RULE_SPECTATOR_PLAYER)
            LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
              LANG_GetTextMessage(TMN_FLUFF_spectator));
          else
            LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
              UICurrentGameState.Players[i].name);
          LE_GRAFIX_InvalidateRect(&CHAT_chatbar);
          LE_FONTS_GetSettings(0);
          CHAT_CurNonAIPlayer = i;
        }
*/

        // bleeble: temporary - show characters one can talk privately to
        // instead of drawing it every fraction of a second, draw when it changes
//grib
        for (j = UICurrentGameState.NumberOfPlayers - 1; j >= 0; j--)
          LE_GRAFIX_ColorArea(CHAT_BarArea,
            width(CHAT_chatbar) - ((j + BTNOFFSET) * 19) + 3, 2, 19, 14, LEG_MCR(95,95,95));//LEG_MCR(127,127,127));
        j = 0;
        for (int i = UICurrentGameState.NumberOfPlayers - 1; i >= 0; i--)
        {
          if ((!SlotIsALocalPlayer[i]) && (UICurrentGameState.Players[i].AIPlayerLevel == 0))
          {
            LE_GRAFIX_ShowTCB(CHAT_BarArea,
              width(CHAT_chatbar) - ((j + BTNOFFSET) * 19) + 4, 3,
              LED_IFT(DAT_MAIN, TAB_cnclr00 + UICurrentGameState.Players[i].colour));
            if (CHAT_ToPlayer[i])
              LE_GRAFIX_ShowTCB(CHAT_BarArea,
                width(CHAT_chatbar) - ((j + BTNOFFSET) * 19) + 8, 5,
                LED_IFT(DAT_MAIN, TAB_cntlkfc0 + UICurrentGameState.Players[i].colour));
            j++;
          }
        }
        if (CHAT_NumNonLocalHumans != j) {
          CHAT_NumNonLocalHumans = j;
          if (CHAT_BoxActive)
            { CHAT_Close(); CHAT_Open(); }
        }
        LE_GRAFIX_InvalidateRect(&CHAT_chatbar);
      }
    }
    return(FALSE);  // Let other handlers see the timer message too
  }

/* // Chat window always on top of fluff now
  if (FLUFF_boxlevel > CHAT_boxlevel) {
    if (FLUFF_BoxActive)
      if (UDCHAT_ProcessFluffMessage(UIMessagePntr))
        return(TRUE);
    if (CHAT_BoxActive)
      if (UDCHAT_ProcessChatMessage(UIMessagePntr))
        return(TRUE);
  } else {
*/
    if (CHAT_BoxActive)
      if (UDCHAT_ProcessChatMessage(UIMessagePntr))
        if ((!FLUFF_moving) && (!FLUFF_sizing) && (!FLUFF_scrolling))
          return(TRUE);
    if ((CHAT_BoxActive) && (FLUFF_BoxActive))
      if (UDCHAT_ProcessFluffMessage(UIMessagePntr))
        return(TRUE);
//  }
  return(FALSE);
}

/***************************************************************************
* CHAT_WordWrapAll - Wrap/truncate lines based on current sizes of windows.
***************************************************************************/
void CHAT_WordWrapAll(void)
{
  int i, j;
  TotalWrappedOutputLines = 0;

  // Wrap the chat window text
  for (i = 0; i < NumOutputLines; i++) {
    CHAT_NumWrappedOutputLines[i] =
      CHAT_WordWrap(CHAT_OutputLinesPtr[i], width(CHAT_textbox),
      (wchar_t **)CHAT_WrappedLines, 9);
    TotalWrappedOutputLines += CHAT_NumWrappedOutputLines[i];
  }

  // Update counts (bring OutputLinesOffset back in line if it went out)
  if (OutputLinesOffset > (TotalWrappedOutputLines - 1))
    OutputLinesOffset = (TotalWrappedOutputLines - 1);
  LinesInWindow =
    (((CHAT_textbox.bottom - CHAT_textbox.top) -
     ((CHAT_textbox.bottom - CHAT_textbox.top) % CHAT_FontHeight))
      / CHAT_FontHeight);
  FluffLinesInWindow =
    (((FLUFF_textbox.bottom - FLUFF_textbox.top) -
     ((FLUFF_textbox.bottom - FLUFF_textbox.top) % CHAT_FontHeight))
      / CHAT_FontHeight);

  // Truncate the fluff window text (stick on "...")
  for (i = 0; i < NumFluffText; i++)
  {
    wcscpy(CHAT_SmallFluffText[i], LANG_GetTextMessage(CategoryStart[CurrentCategory] + i));
    CHAT_WordWrap(CHAT_SmallFluffText[i], width(FLUFF_textbox), NULL, 9);
    wcscpy(CHAT_SmallFluffText[i], CHAT_WrappedLines[0]);
    if (wcslen(LANG_GetTextMessage(CategoryStart[CurrentCategory] + i)) != wcslen(CHAT_SmallFluffText[i]))
    {
      j = wcslen(CHAT_SmallFluffText[i]);
      j--; CHAT_SmallFluffText[i][j] = L'.';
      j--; CHAT_SmallFluffText[i][j] = L'.';
      j--; CHAT_SmallFluffText[i][j] = L'.';
    }
  }
}

/***************************************************************************
* CHAT_WordWrap - Wrap a line based on the current size of the chat window.
***************************************************************************/
int CHAT_WordWrap(wchar_t *line, int width, wchar_t **wrapped, int font)
{
  int j, s, n, i = 0;
  wchar_t tmpmsg[1024/*CHAT_MAXCHAR+RULE_MAX_PLAYER_NAME_LENGTH+2*/]; // + name + ": "

//wrapped = (unsigned short **)(&CHAT_WrappedLines[0][0]);
#define wrapped CHAT_WrappedLines  // bleeble: ug. had problems passing this... fixfixfix
  // Word wrap the string.  (We need to get the font to
  // see how big strings will be in this font)
  LE_FONTS_SetSettings(0);
  LE_FONTS_GetSettings(font);
  wcscpy(wrapped[0], line);
  // While the line is too big, chop it up to make it fit
  while (LE_FONTS_GetStringWidth(wrapped[i]) > width)
  {
    // Step one: Try to break on a space
    wcscpy(tmpmsg, wrapped[i]);
    n = wcslen(wrapped[i]) - 1;
    while (LE_FONTS_GetStringWidth(wrapped[i]) > width) {
      while ((wrapped[i][n] != L' ') && (n > 0))
        n--;  // Find a space
      s = n + 1;  // Set the start for the next line
      while ((wrapped[i][n] == L' ') && (n > 0))
        n--;  // Skip past all the spaces
      wrapped[i][n+1] = 0;
    }

    // Step two: No spaces found to break on...  No more Mr. Nice Guy.
    // Break at the first character that fits the max amt of text on a line.
    if (wcslen(wrapped[i]) == 1)
    {
      wcscpy(wrapped[i], tmpmsg);
      n = wcslen(wrapped[i]) - 1;
      while (LE_FONTS_GetStringWidth(wrapped[i]) > width) {
        n--;
        wrapped[i][n+1] = 0;
      }
      s = n + 1;
    }

    // Let _ represent a non-breaking space.
    for (j = wcslen(wrapped[i]) - 1; j >= 0; j--)
      if (wrapped[i][j] == A_T('_'))
        wrapped[i][j] = A_T(' ');
    // Increment the index and copy the trailing part of the text to the queue.
    i++;
    wcscpy(wrapped[i], &tmpmsg[s]);
  }
  i++;
  LE_FONTS_GetSettings(0);
  return(i);
}

/***************************************************************************
* CHAT_ReceiveMessage - Add a message to the received queue, and display it
* if the chat box is open and not in windowshade mode.  This routine is
* called by ProcessMessageToPlayer (Userifce.cpp) when getting a message
* through the network, and UDCHAT_ProcessMessage (this file), so the user
* has a history of what they said.
***************************************************************************/
void CHAT_ReceiveMessage(wchar_t *msg, long id, long player_from, long player_to)
{
  wchar_t ustring[CHAT_TOTALCHAR];

  if (msg == NULL)
    msg = LANG_GetTextMessage(FLUFFTEXTOFFSET + id); // msg is NULL, so we got a fluff text message.
//    msg = CHAT_FluffText[id]; // msg is NULL, so we got a fluff text message.
  if (player_to != RULE_ALL_PLAYERS)
  {
    // This message didn't go to everyone, so stick in "(Private) "
    if (player_from != RULE_SPECTATOR_PLAYER)
      Asprintf(ustring, A_T("%s: %s %s"),
        UICurrentGameState.Players[player_from].name, LANG_GetTextMessage(TMN_FLUFF_private), msg);
    else
      Asprintf(ustring, A_T("%s: %s %s"),
        LANG_GetTextMessage(TMN_FLUFF_spectator), LANG_GetTextMessage(TMN_FLUFF_private), msg);
  } else {
    // This message went to everyone
    if (player_from != RULE_SPECTATOR_PLAYER)
      Asprintf(ustring, A_T("%s: %s"),
        UICurrentGameState.Players[player_from].name, msg);
    else
      Asprintf(ustring, A_T("%s: %s"),
        LANG_GetTextMessage(TMN_FLUFF_spectator), msg);
  }

  // Copy string to history, and keep track of how many wrapped lines it represents
  if (NumOutputLines == CHAT_KeepOutputLines)
  {
    // If queue full, adjust pointers and start overwriting oldest stuff
    CHAT_OutputLinesTop++;
    if (CHAT_OutputLinesTop == CHAT_KeepOutputLines)
      CHAT_OutputLinesTop = 0;

    wchar_t *tmp = CHAT_OutputLinesPtr[0];
    int tmpint = CHAT_NumWrappedOutputLines[0];
    for (int i = 0; i < CHAT_KeepOutputLines; i++)
    {
      CHAT_OutputLinesPtr[i] = CHAT_OutputLinesPtr[i + 1];
      CHAT_NumWrappedOutputLines[i] = CHAT_NumWrappedOutputLines[i + 1];
    }
    CHAT_OutputLinesPtr[CHAT_KeepOutputLines-1] = tmp;
    CHAT_NumWrappedOutputLines[CHAT_KeepOutputLines-1] = tmpint;

    wcscpy(CHAT_OutputLinesPtr[NumOutputLines-1], ustring);
    TotalWrappedOutputLines -= CHAT_NumWrappedOutputLines[NumOutputLines-1];
    CHAT_NumWrappedOutputLines[NumOutputLines-1] =
      CHAT_WordWrap(ustring, width(CHAT_textbox), (wchar_t **)CHAT_WrappedLines, 9);
    TotalWrappedOutputLines += CHAT_NumWrappedOutputLines[NumOutputLines-1];
  } else {
    wcscpy(CHAT_OutputLinesPtr[NumOutputLines], ustring);
    CHAT_NumWrappedOutputLines[NumOutputLines] =
      CHAT_WordWrap(ustring, width(CHAT_textbox), (wchar_t **)CHAT_WrappedLines, 9);
    TotalWrappedOutputLines += CHAT_NumWrappedOutputLines[NumOutputLines];
    NumOutputLines++;
  }

// bleeble: only change if user hasn't scrolled display, maybe?
  OutputLinesOffset = max(TotalWrappedOutputLines - LinesInWindow, 0);
  CHAT_RefreshTextArea();
  InputLinesOffset = 0;
  if (!CHAT_BoxActive)  CHAT_Open();
  else if (CHAT_shade) {
    CHAT_Close();
    CHAT_shade = FALSE;
    CHAT_Open();
  }
}

/***************************************************************************
* CHAT_Toggle - If the chat box is open, close it.  If closed, open it.
***************************************************************************/
void CHAT_Toggle(void)
{
//  CHAT_BoxActive = !CHAT_BoxActive;
  if (CHAT_BoxActive)  CHAT_Close();
  else                 CHAT_Open();
}

/***************************************************************************
* CHAT_Open - Open/display the chat box on the screen.
***************************************************************************/
BOOL CHAT_Open(void)
{
  LE_QUEUE_MessageRecord msg;

if (!CHAT_BoxActive) {
  // Start all of the items (Create the transparent ones)
  // Create the top "drag bar" tab
  CHAT_BarArea = LE_GRAFIX_ObjectCreate(
    width(CHAT_chatbar), height(CHAT_chatbar), LE_GRAFIX_ObjectNotTransparent);
  LE_GRAFIX_ColorArea(CHAT_BarArea, 0, 0,
    width(CHAT_chatbar), height(CHAT_chatbar), LEG_MCR(0,255,255));//LEG_MCR(127,127,127));
  LE_SEQNCR_StartXY(CHAT_BarArea, CHAT_boxlevel, CHAT_chatbar.left, CHAT_chatbar.top);

  // Create the tab to appear above the top tab (with chatter's name)
  CHAT_BarArea1 = LE_GRAFIX_ObjectCreate(
    width(CHAT_chatbar) - (short)(5 + (6 * 18) + ((CHAT_NumNonLocalHumans - 1) * 18)),
    height(CHAT_chatbar), LE_GRAFIX_ObjectTransparent);
  LE_GRAFIX_ColorArea(CHAT_BarArea1, 0, 0,
    width(CHAT_chatbar), height(CHAT_chatbar), LEG_MCR(0, 255, 0));
  LE_SEQNCR_StartXY(CHAT_BarArea1, CHAT_boxlevel+1, CHAT_chatbar.left, CHAT_chatbar.top);

  // Create the main box "background" (border tiles are placed here)
  CHAT_TextArea = LE_GRAFIX_ObjectCreateAlpha(width(CHAT_chatbox),
    height(CHAT_chatbox), LE_GRAFIX_ObjectTransparent, CHAT_AlphaValues[CHAT_BGAlpha]);
  LE_GRAFIX_ColorArea(CHAT_TextArea, 0, 0, width(CHAT_chatbox),
    height(CHAT_chatbox), LEG_MCRA(BG, BG, BG, CHAT_AlphaValues[CHAT_BGAlpha]));
  if (!CHAT_shade)
    LE_SEQNCR_StartXY(CHAT_TextArea, CHAT_boxlevel, CHAT_chatbox.left, CHAT_chatbox.top);

/*bleeble: Add this back when we get approved art*/
  LE_GRAFIX_ShowTCB(CHAT_TextArea, 0, CHAT_winsize.y-18-19, LED_IFT(DAT_MAIN, TAB_cnbtmlft));
  for (int i = 0; i < ((CHAT_winsize.y-19) / 5)-1; i++) {
    LE_GRAFIX_ShowTCB(CHAT_TextArea, 0, (5 * i), LED_IFT(DAT_MAIN, TAB_cnleftbr));
    LE_GRAFIX_ShowTCB(CHAT_TextArea, CHAT_winsize.x-1-17, (5 * i), LED_IFT(DAT_MAIN, TAB_cnrghtbr));
  }
  for (i = 1; i < (CHAT_winsize.x / 5)-1; i++) {
    LE_GRAFIX_ShowTCB(CHAT_BarArea, (5 * i), 0, LED_IFT(DAT_MAIN, TAB_cntopbar));
    LE_GRAFIX_ShowTCB(CHAT_TextArea, (5 * i), CHAT_winsize.y-3-19, LED_IFT(DAT_MAIN, TAB_cnbtmbr));
  }
  LE_GRAFIX_ShowTCB(CHAT_TextArea, CHAT_winsize.x-1-19, CHAT_winsize.y-18-19, LED_IFT(DAT_MAIN, TAB_cnbtmrgt));

  LE_GRAFIX_ShowTCB(CHAT_BarArea, 0, 0, LED_IFT(DAT_MAIN, TAB_cntoplft));
  LE_GRAFIX_ShowTCB(CHAT_BarArea, CHAT_winsize.x-21-1, 0, LED_IFT(DAT_MAIN, TAB_cntoprgt));
//*/

  // Create the main box "foreground" in which text and buttons will be placed
  CHAT_TextArea1 = LE_GRAFIX_ObjectCreateAlpha(width(CHAT_chatbox),
    height(CHAT_chatbox), LE_GRAFIX_ObjectTransparent, CHAT_AlphaValues[CHAT_TextAlpha]);
  LE_GRAFIX_ColorArea(CHAT_TextArea1, 0, 0, width(CHAT_chatbox),
    height(CHAT_chatbox), LEG_MCRA(0, 255, 0, CHAT_AlphaValues[CHAT_TextAlpha]));
  if (!CHAT_shade)
    LE_SEQNCR_StartXY(CHAT_TextArea1, CHAT_boxlevel+1, CHAT_chatbox.left, CHAT_chatbox.top);

    LE_GRAFIX_ShowTCB(CHAT_TextArea, CHAT_winsize.x-18, 0, LED_IFT(DAT_MAIN, TAB_cnarup));
    LE_GRAFIX_ShowTCB(CHAT_TextArea, CHAT_winsize.x-18, CHAT_winsize.y-27-26, LED_IFT(DAT_MAIN, TAB_cnardwn));
///////////////////////////////
  LE_GRAFIX_ShowTCB(CHAT_BarArea, (short)(CHAT_allbtn.left-CHAT_winloc.x)+3, (short)(CHAT_allbtn.top-CHAT_winloc.y), LED_IFT(DAT_MAIN, TAB_cnclrall));
  LE_GRAFIX_ShowTCB(CHAT_BarArea, (short)(CHAT_optbtn.left-CHAT_winloc.x), (short)(CHAT_optbtn.top-CHAT_winloc.y), LED_IFT(DAT_MAIN, TAB_cnoption));
  LE_GRAFIX_ShowTCB(CHAT_BarArea, (short)(CHAT_fluffbtn.left-CHAT_winloc.x), (short)(CHAT_fluffbtn.top-CHAT_winloc.y), LED_IFT(DAT_MAIN, TAB_cnmsgs));
//  LE_GRAFIX_ShowTCB(CHAT_BarArea, CHAT_shadebtn.left-10, CHAT_shadebtn.top-10, LED_IFT(DAT_MAIN, TAB_cndropdn));
///////////////////////////////

  LE_GRAFIX_InvalidateRect(&CHAT_chatbar);
  LE_GRAFIX_InvalidateRect(&CHAT_chatbox);

  if (CHAT_options)
    LE_SEQNCR_StartXY(CHAT_OptionArea, CHAT_boxlevel+2, CHAT_optbox.left, CHAT_optbox.top);

  if (FLUFF_BoxActive) {
    // Create the top "drag bar" tab
    FLUFF_BarArea = LE_GRAFIX_ObjectCreate(
      width(FLUFF_chatbar), height(FLUFF_chatbar), LE_GRAFIX_ObjectNotTransparent);
    LE_GRAFIX_ColorArea(FLUFF_BarArea, 0, 0,
      width(FLUFF_chatbar), height(FLUFF_chatbar), LEG_MCR(127,127,127));
    LE_SEQNCR_StartXY(FLUFF_BarArea, FLUFF_boxlevel, FLUFF_chatbar.left, FLUFF_chatbar.top);

    // Create the tab to appear above the top tab (with fluff category)
    FLUFF_BarArea1 = LE_GRAFIX_ObjectCreate(
      width(FLUFF_chatbar) - (short)(2 + (7 * 20)),
      height(FLUFF_chatbar), LE_GRAFIX_ObjectTransparent);
    LE_GRAFIX_ColorArea(FLUFF_BarArea1, 0, 0,
      width(FLUFF_chatbar), height(FLUFF_chatbar), LEG_MCR(0, 255, 0));
    LE_SEQNCR_StartXY(FLUFF_BarArea1, FLUFF_boxlevel+1, FLUFF_chatbar.left, FLUFF_chatbar.top);

    LE_FONTS_SetSettings(0);
    LE_FONTS_GetSettings(8);
    LE_GRAFIX_ColorArea(FLUFF_BarArea1, 0, 0,
      width(FLUFF_chatbar), height(FLUFF_chatbar), LEG_MCR(0, 255, 0));
    LE_FONTS_Print(FLUFF_BarArea1, 15, 2, LEG_MCRA(220,220,220, 255),
      LANG_GetTextMessage(CategoryStart[CurrentCategory]-1));
    LE_FONTS_GetSettings(0);

    // Create the main box "background" (border tiles are placed here)
    FLUFF_TextArea = LE_GRAFIX_ObjectCreateAlpha(width(FLUFF_chatbox),
      height(FLUFF_chatbox), LE_GRAFIX_ObjectTransparent, CHAT_AlphaValues[CHAT_BGAlpha]);
    LE_GRAFIX_ColorArea(FLUFF_TextArea, 0, 0, width(FLUFF_chatbox),
      height(FLUFF_chatbox), LEG_MCRA(BG, BG, BG, CHAT_AlphaValues[CHAT_BGAlpha]));
    if (!FLUFF_shade)
      LE_SEQNCR_StartXY(FLUFF_TextArea, FLUFF_boxlevel, FLUFF_chatbox.left, FLUFF_chatbox.top);

/*bleeble: add this back when we get approved art*/
    LE_GRAFIX_ShowTCB(FLUFF_TextArea, 0, FLUFF_winsize.y-18-19, LED_IFT(DAT_MAIN, TAB_cnbtmlft));
    for (i = 0; i < ((FLUFF_winsize.y-19) / 5)-1; i++) {
      LE_GRAFIX_ShowTCB(FLUFF_TextArea, 0, (5 * i), LED_IFT(DAT_MAIN, TAB_cnleftbr));
      LE_GRAFIX_ShowTCB(FLUFF_TextArea, FLUFF_winsize.x-1-17, (5 * i), LED_IFT(DAT_MAIN, TAB_cnrghtbr));
    }

    for (i = 1; i < (FLUFF_winsize.x / 5)-1; i++) {
      LE_GRAFIX_ShowTCB(FLUFF_BarArea, (5 * i), 0, LED_IFT(DAT_MAIN, TAB_cntopbar));
      LE_GRAFIX_ShowTCB(FLUFF_TextArea, (5 * i), FLUFF_winsize.y-3-19, LED_IFT(DAT_MAIN, TAB_cnbtmbr));
    }
    LE_GRAFIX_ShowTCB(FLUFF_TextArea, FLUFF_winsize.x-1-19, FLUFF_winsize.y-18-19, LED_IFT(DAT_MAIN, TAB_cnbtmrgt));

    LE_GRAFIX_ShowTCB(FLUFF_BarArea, 0, 0, LED_IFT(DAT_MAIN, TAB_cntoplft));
////    LE_GRAFIX_ShowTCB(FLUFF_BarArea, FLUFF_winsize.x-21-1, 0, LED_IFT(DAT_MAIN, TAB_cntoprgt));
    LE_GRAFIX_ShowTCB(FLUFF_BarArea, FLUFF_winsize.x-21-1, 0, LED_IFT(DAT_MAIN, TAB_cndropdn));
//*/

    // Create the main box "foreground" in which text and buttons will be placed
    FLUFF_TextArea1 = LE_GRAFIX_ObjectCreateAlpha(width(FLUFF_chatbox),
      height(FLUFF_chatbox), LE_GRAFIX_ObjectTransparent, CHAT_AlphaValues[CHAT_TextAlpha]);
    LE_GRAFIX_ColorArea(FLUFF_TextArea1, 0, 0, width(FLUFF_chatbox),
      height(FLUFF_chatbox), LEG_MCRA(0, 255, 0, CHAT_AlphaValues[CHAT_TextAlpha]));
    if (!FLUFF_shade)
      LE_SEQNCR_StartXY(FLUFF_TextArea1, FLUFF_boxlevel+1, FLUFF_chatbox.left, FLUFF_chatbox.top);

    LE_GRAFIX_ShowTCB(FLUFF_TextArea, FLUFF_winsize.x-18, 0, LED_IFT(DAT_MAIN, TAB_cnarup));
    LE_GRAFIX_ShowTCB(FLUFF_TextArea, FLUFF_winsize.x-18, FLUFF_winsize.y-27-26, LED_IFT(DAT_MAIN, TAB_cnardwn));

///////////////////////////////
  LE_GRAFIX_ShowTCB(FLUFF_BarArea, (short)(FLUFF_grp1btn.left-FLUFF_winloc.x), (short)(FLUFF_grp1btn.top-FLUFF_winloc.y), LED_IFT(DAT_MAIN, TAB_cngreetn));
  LE_GRAFIX_ShowTCB(FLUFF_BarArea, (short)(FLUFF_grp2btn.left-FLUFF_winloc.x), (short)(FLUFF_grp2btn.top-FLUFF_winloc.y), LED_IFT(DAT_MAIN, TAB_cncomans));
  LE_GRAFIX_ShowTCB(FLUFF_BarArea, (short)(FLUFF_grp3btn.left-FLUFF_winloc.x), (short)(FLUFF_grp3btn.top-FLUFF_winloc.y), LED_IFT(DAT_MAIN, TAB_cngmply));
  LE_GRAFIX_ShowTCB(FLUFF_BarArea, (short)(FLUFF_grp4btn.left-FLUFF_winloc.x), (short)(FLUFF_grp4btn.top-FLUFF_winloc.y), LED_IFT(DAT_MAIN, TAB_cntrade));
  LE_GRAFIX_ShowTCB(FLUFF_BarArea, (short)(FLUFF_grp5btn.left-FLUFF_winloc.x), (short)(FLUFF_grp5btn.top-FLUFF_winloc.y), LED_IFT(DAT_MAIN, TAB_cnposcom));
  LE_GRAFIX_ShowTCB(FLUFF_BarArea, (short)(FLUFF_grp6btn.left-FLUFF_winloc.x), (short)(FLUFF_grp6btn.top-FLUFF_winloc.y), LED_IFT(DAT_MAIN, TAB_cnnegcom));
///////////////////////////////

    LE_GRAFIX_InvalidateRect(&FLUFF_chatbar);
    LE_GRAFIX_InvalidateRect(&FLUFF_chatbox);
  }

//bleeble: getcuraiplayer
/* this should be handled by the timer
  LE_FONTS_SetSettings(0);
  LE_FONTS_GetSettings(8);
  if (CHAT_CurNonAIPlayer == RULE_SPECTATOR_PLAYER)
    LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
      LANG_GetTextMessage(TMN_FLUFF_spectator));
  else
    LE_FONTS_Print(CHAT_BarArea1, 23, 3, LEG_MCRA(220,220,220, 255),
      UICurrentGameState.Players[CHAT_CurNonAIPlayer].name);
  LE_GRAFIX_InvalidateRect(&CHAT_chatbar);
  LE_FONTS_GetSettings(0);
*/

  MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
    width(CHAT_editbox), height(CHAT_editbox), TRUE);
  if ((!CHAT_shade) && (!CHAT_sizing) && (!FLUFF_sizing))  // Don't show editbox while resizing window
    ShowWindow(CHAT_EditBox, SW_SHOW);

  CHAT_BoxActive = TRUE;
  CHAT_RefreshTextArea();
  if (FLUFF_BoxActive)  GetCategory(CurrentCategory);

  // Show correct pointer on window open
  if (!((CHAT_sizing) || (FLUFF_sizing))) {
    msg.messageCode = UIMSG_MOUSE_MOVED;
    msg.numberA = LE_MOUSE_MouseX;
    msg.numberB = LE_MOUSE_MouseY;
    UDCHAT_ProcessChatMessage(&msg);
  }
}
  return(TRUE);
}

/***************************************************************************
* CHAT_Close - Remove the chat box from the screen.
*              Calling CHAT_Open will put it back.  (Where it was closed)
***************************************************************************/
void CHAT_Close(void)
{
  LE_QUEUE_MessageRecord msg;

if (CHAT_BoxActive) {
  CHAT_BoxActive = FALSE;
  // Stop all of the items.
  // (Free the dynamic ones, to be recreated on CHAT_Open)
// bleeble: process user commands so the stops will hopefully be
// called before frees.  putting PUC after the stops causes
// ugly flickering.  fixfixfix
  LE_SEQNCR_ProcessUserCommands();
  LE_SEQNCR_Stop(CHAT_BarArea, CHAT_boxlevel);
  LE_SEQNCR_Stop(CHAT_BarArea1, CHAT_boxlevel+1);
  if (!CHAT_shade) {
    LE_SEQNCR_Stop(CHAT_TextArea, CHAT_boxlevel);
    LE_SEQNCR_Stop(CHAT_TextArea1, CHAT_boxlevel+1);
  }
  if (FLUFF_BoxActive) {
    LE_SEQNCR_Stop(FLUFF_BarArea, FLUFF_boxlevel);
    LE_SEQNCR_Stop(FLUFF_BarArea1, FLUFF_boxlevel+1);
    if (!FLUFF_shade) {
      LE_SEQNCR_Stop(FLUFF_TextArea, FLUFF_boxlevel);
      LE_SEQNCR_Stop(FLUFF_TextArea1, FLUFF_boxlevel+1);
    }
  }

  if (CHAT_options)
    LE_SEQNCR_Stop(CHAT_OptionArea, CHAT_boxlevel+2);
  if (!CHAT_shade)
    ShowWindow(CHAT_EditBox, SW_HIDE);

  LE_DATA_FreeRuntimeDataID(CHAT_BarArea);
  LE_DATA_FreeRuntimeDataID(CHAT_BarArea1);
  LE_DATA_FreeRuntimeDataID(CHAT_TextArea);
  LE_DATA_FreeRuntimeDataID(CHAT_TextArea1);

  if (FLUFF_BoxActive) {
    LE_DATA_FreeRuntimeDataID(FLUFF_BarArea);
    LE_DATA_FreeRuntimeDataID(FLUFF_BarArea1);
    LE_DATA_FreeRuntimeDataID(FLUFF_TextArea);
    LE_DATA_FreeRuntimeDataID(FLUFF_TextArea1);
  }

  // Show correct pointer on window close
  if (!((CHAT_sizing) || (FLUFF_sizing))) {
    msg.messageCode = UIMSG_MOUSE_MOVED;
    msg.numberA = msg.numberB = 4096;
    UDCHAT_ProcessChatMessage(&msg);
  }
}
}


/***************************************************************************
* CHAT_InitializeSystem - To be called on startup.  Get everything we need.
***************************************************************************/
BOOL CHAT_InitializeSystem(void)
{
  const DDCAPS *pCaps = pc3D::GetDDrawDriver()->GetHardwareCaps();
  if (!(pCaps->dwCaps2 & DDCAPS2_CANRENDERWINDOWED))
    bNonGDI = TRUE;

  // Be friendly and talk to everybody
// bleeble: what if people join?
  for (int i = 0; i < RULE_MAX_PLAYERS; i++)
    CHAT_ToPlayer[i] = TRUE;

  // Set up pointer arrays
  for (i = 0; i < CHAT_KeepInputLines; i++)
    CHAT_InputLinesPtr[i] = CHAT_InputLines[i];
  for (i = 0; i < CHAT_KeepOutputLines; i++)
    CHAT_OutputLinesPtr[i] = CHAT_OutputLines[i];
//  for (i = 0; i < NUMFLUFFTEXT; i++)
//    CHAT_FluffText[i] = LANG_GetTextMessage(FLUFFTEXTOFFSET + i);
//  qsort(CHAT_FluffText, NumFluffText, sizeof(wchar_t *), &CHAT_wcscmp);

  CurrentCategory = 0;
  NumFluffText = NumStrings[CurrentCategory];
//  for (i = 0; i < NumFluffText; i++)
//    CHAT_MiniFluffText[i] =
//      LANG_GetTextMessage(CategoryStart[CurrentCategory] + i);

  // Set up the font, and save it in a font slot so we don't mess
  // up people who are expecting the font to be something else
  // Slot 8 is for the title, slot 9 is for the size-changing chat text
  LE_FONTS_SetSettings(0);
  LE_FONTS_SetSize(CHAT_FontSize);
  CHAT_FontHeight = LE_FONTS_GetStringHeight(A_T("Xy"));
  AdjustRects();  // Initialize the rectangle positions.
  AdjustFluffRects();  // Initialize the rectangle positions.
  // We need this indirect font to send to Windows to set the edit box font
  CHAT_EditFont = CreateFontIndirect(LE_FONTS_GetLogfont());
  LE_FONTS_SetSettings(8);
  LE_FONTS_SetSettings(9);
  LE_FONTS_GetSettings(0);

  // Create the edit box
  if (IsWindow(CHAT_EditBox))
    DestroyWindow(CHAT_EditBox);
  CHAT_EditBox = CreateWindow("EDIT", "", WS_POPUP | ES_AUTOHSCROLL,
    CHAT_editbox.left, CHAT_editbox.top, width(CHAT_editbox), height(CHAT_editbox),
    LE_MAIN_HwndMainWindow, NULL, LE_MAIN_Hinst, NULL);
  if (CHAT_EditBox == NULL)
    return(FALSE);

  // Set up the new handler for the editbox
  EditBox_oldwinproc = (WNDPROC)GetWindowLong(CHAT_EditBox, GWL_WNDPROC);
  SetWindowLong(CHAT_EditBox, GWL_WNDPROC, (long)(&EditBox_WinProc));

  // Set editbox settings
  SendMessage(CHAT_EditBox, WM_SETFONT, (WPARAM)CHAT_EditFont, MAKELPARAM(TRUE, 0));
  SendMessage(CHAT_EditBox, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELONG(1, 1));
  SendMessage(CHAT_EditBox, EM_LIMITTEXT, CHAT_MAXCHAR - 1, 0);

  ShowWindow(CHAT_EditBox, SW_HIDE);
  MoveWindow(CHAT_EditBox, CHAT_editbox.left, CHAT_editbox.top,
    width(CHAT_editbox), height(CHAT_editbox), FALSE);

  // The Options window doesn't change size, so create on startup
  CHAT_OptionArea = LE_GRAFIX_ObjectCreate(width(CHAT_optbox),
    height(CHAT_optbox), LE_GRAFIX_ObjectNotTransparent);

//  LE_GRAFIX_ColorArea(CHAT_OptionArea, 0, 0, width(CHAT_optbox),
//    height(CHAT_optbox), LEG_MCR(0, 128, 189));
  LE_GRAFIX_ShowTCB(CHAT_OptionArea, 0, 0, LED_IFT(DAT_MAIN, TAB_cntrspy));
  return(TRUE);
}

/***************************************************************************
* CHAT_CleanAndRemoveSystem - To be called on shutdown. Close/free everything.
***************************************************************************/
void CHAT_CleanAndRemoveSystem(void)
{
  // Close the box and data file if open, and deallocate stuff for
  // the dynamic graphics, edit box, font, etc.
  if (CHAT_BoxActive) {
    CHAT_Close();
    CHAT_BoxActive = FLUFF_BoxActive = FALSE;
  }
  if (CHAT_OptionArea != LED_EI) {
    LE_DATA_FreeRuntimeDataID(CHAT_OptionArea);
    CHAT_OptionArea = LED_EI;
  }
  if (CHAT_EditBox != NULL) {
    DestroyWindow(CHAT_EditBox);
    CHAT_EditBox = NULL;
  }
  if (CHAT_EditFont != NULL) {
    DeleteObject(CHAT_EditFont);
    CHAT_EditFont = NULL;
  }
}
