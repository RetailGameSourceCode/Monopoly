#ifndef __UDPIECES_H__
#define __UDPIECES_H__

/*************************************************************
*
*   FILE:             UDPIECES.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

#define TOKEN1_XOFFSET    19.0f
#define TOKEN1_ZOFFSET    30.0f

#define HOUSE_XLENGTH     9.0f
#define HOUSE_ZWIDTH      11.0f
#define HOTEL_XLENGTH     18.0f
#define HOTEL_ZWIDTH      12.0f

#define PROPERTY_XWIDTH   38.0f
#define PROPERTY_ZHEIGHT  64.5f
#define SWATCH_ZHEIGHT    12.5f
#define BORDER_WIDTH      1.5f

#define HOUSE_SPACING     ((PROPERTY_XWIDTH - (4.0f * HOUSE_XLENGTH)) / 5.0f)

#define HOUSE0_XOFFSET    (HOUSE_SPACING + (HOUSE_XLENGTH * 0.5f))
#define HOUSE1_XOFFSET    ((HOUSE_SPACING * 2) + (HOUSE_XLENGTH * 1.5f))
#define HOUSE2_XOFFSET    ((HOUSE_SPACING * 3) + (HOUSE_XLENGTH * 2.5f))
#define HOUSE3_XOFFSET    ((HOUSE_SPACING * 4) + (HOUSE_XLENGTH * 3.5f))
#define HOUSE_ZOFFSET     (PROPERTY_ZHEIGHT - (SWATCH_ZHEIGHT / 2.0f))

#define HOTEL_XOFFSET     (PROPERTY_XWIDTH / 2.0f)
#define HOTEL_ZOFFSET     (PROPERTY_ZHEIGHT - (SWATCH_ZHEIGHT / 2.0f))


/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/
enum UDPIECES_TokenMoveCamEffects
{
  TOKEN_CAM_EFFECT_FirstSeqSwoopPAT = 0,
  TOKEN_CAM_EFFECT_
};

/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDPIECES_Initialize( void );
void DISPLAY_UDPIECES_Destroy( void );
void DISPLAY_UDPIECES_TickActions( int numberOfTicks );
void DISPLAY_UDPIECES_Show( void );

void UDPIECES_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDPIECES_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );

void UDPIECES_GetTokenOrientation(
  int nBoardTile,
  float *lpfXCoordinate,
  float *lpfYCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle);

void UDPIECES_GetTokenRestingIdleOrientation(
  int nBoardTile,
  int nRestingPosition,
  int nToken,
  float *lpfXCoordinate,
  float *lpfYCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle);

void UDPIECES_GetTokenActualOrientation(
  int nPlayer,
  float *lpfXCoordinate,
  float *lpfYCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle);

void UDPIECES_GetHotelPosition(
  int nBoardTile,
  float *lpfXCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle);

void UDPIECES_GetHousePosition(
  int nBoardTile,
  int nHouse,
  float *lpfXCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle);


int UDPIECES_PickCameraFor3Squares( int square );
int UDPIECES_PickCameraFor15Squares( int square );
void UDPIECES_PlanMoveAnim( int player, int rule_movecommand, int SQbefore, int SQafter );

void UDPIECES_StartOfGameSetup( void );

void UDPIECES_UpdateShadows( void );

#endif // __UDPIECES_H__
