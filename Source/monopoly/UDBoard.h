#ifndef __UDBOARD_H__
#define __UDBOARD_H__

/*************************************************************
*
*   FILE:              UDBOARD.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

extern RECT viewRects[4];

#define UDBoard_2D_constructedBoardBufferMax 4 // Maximum boards with overlays to keep in memory buffers
struct UDBoardBuffer
{
  int       ViewLoaded;
  TYPE_Tick TimeLoaded;       //LE_TIME_TickCount, 60ths of a second program has run
  LE_DATA_DataId BoardBuffer; //Allocate space for 800x450 during init.
};
extern UDBoardBuffer UDBoard_2DLoadedViews[UDBoard_2D_constructedBoardBufferMax];


/************************************************************/
/* DEFINES                                                  */
/************************************************************/

#define UDBOARD_CAMERA_VIEWS TAB_mncp030001 - TAB_mncp020001

enum VIEWS_2D
{
  VIEW2D01_TOPDOWN_SQUARE = 0,
  VIEW2D02_TOPDOWN_SOCCER,
  VIEW2D03_TOPDOWN_STARWARS,

  VIEW2D04_3TILES01,
  VIEW2D05_3TILES02,
  VIEW2D06_3TILES03,
  VIEW2D07_3TILES04,
  VIEW2D08_3TILES05,
  VIEW2D09_3TILES06,
  VIEW2D10_3TILES07,
  VIEW2D11_3TILES08,
  VIEW2D12_3TILES09,
  VIEW2D13_3TILES10,
  VIEW2D14_3TILES11,
  VIEW2D15_3TILES12,

  VIEW2D16_CORNER_GO,
  VIEW2D17_CORNER_JAIL,
  VIEW2D18_CORNER_FREEPARKING,
  VIEW2D19_CORNER_GOTOJAIL,

  VIEW2D20_5TILES01,
  VIEW2D21_5TILES02,
  VIEW2D22_5TILES03,
  VIEW2D23_5TILES04,
  VIEW2D24_5TILES05,
  VIEW2D25_5TILES06,
  VIEW2D26_5TILES07,
  VIEW2D27_5TILES08,

  VIEW2D28_15TILES01,
  VIEW2D29_15TILES02,
  VIEW2D30_15TILES03,
  VIEW2D31_15TILES04,
  VIEW2D32_15TILES05,
  VIEW2D33_15TILES06,
  VIEW2D34_15TILES07,
  VIEW2D35_15TILES08,
  VIEW2D36_15TILES09,
  VIEW2D37_15TILES10,
  VIEW2D38_15TILES11,
  VIEW2D39_15TILES12,

  VIEW2DMAX
};

enum VIEW_SELECTION_TYPES
{
  VIEW_ROLLDICE = 0,
  VIEW_JAILCHOICE
};

enum VIEWPORTS
{
  VIEWPORT_MAIN = 0,
  VIEWPORT_STATUS,
  VIEWPORT_TRADE,
  VIEWPORT_OFF
};

/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/

extern D3DVALUE pt3DBoardLocations[42][3];

extern LE_DATA_DataId                 CurrentBoard; // Should be in display state, but cant check it out RK
extern Object_3D_Interpolation_data   CameraInterpolationData, CameraWaitingData;
extern BOOL                           CameraMoveWaiting, CameraIsFloatingIdle;

extern D3DVALUE CameraAngles2D[][10];


/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDBOARD_Initialize( void );
void DISPLAY_UDBOARD_Destroy( void );
void DISPLAY_UDBOARD_TickActions( int numberOfTicks );
void DISPLAY_UDBOARD_Show( void );

void UDBOARD_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );


void UDBOARD_SetBackdrop( int backdrop );

void UDBOARD_CompileBackdrop( int cameraview, int bufferindex );
void UDBOARD_SwitchToBackdrop( int cameraview );
void UDBOARD_PreLoadBackdrop( int cameraview1 );


void UDBOARD_SetCameraToPresetPostion( int camera );
void UDBOARD_ActivateDesiredCamera( int cameraIndex );
int UDBOARD_SelectAppropriateView( VIEW_SELECTION_TYPES type, int variable );

void UDBOARD_GetHighlightPosition(
  int nBoardTile,
  float *lpfXCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle);

void UDBOARD_SwitchToBoard( LE_DATA_DataId newBoard, BOOL UseLargeTextures );


#endif // __UDBOARD_H__
