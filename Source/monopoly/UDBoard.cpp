/*****************************************************************************
 *
 * FILE:        UDBOARD.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to Camera moves.
 *    Specifically, the board, property & mortgage indicators and lights.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/

#include "gameinc.h"
//#include "TexInfo.h"

#if CE_ARTLIB_3DUsingOldFrame
#define USE_OLD_FRAME
#endif

#include "..\PC3D\PC3DHdr.h"
#include "..\pc3d\l_material.h"

#if CE_ARTLIB_3DUsingOldFrame

#include "..\pc3d\hmdstuff.h"
#include "..\pc3d\hmdload.h"
typedef meshx     MESHType;
#define SETTIMBANK  HMD_SetCurrentTimBank

// PC3d hack
extern "C" void HMD_SetCurrentTimBank( int bank );

#else // CE_ARTLIB_3DUsingOldFrame

#include "..\PC3D\GlobalData.h"
#include "..\Pc3D\NewMesh.h"
typedef Mesh     MESHType;
#define SETTIMBANK  SetCurrentTIMBank

#endif // CE_ARTLIB_3DUsingOldFrame

/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

#define UDBOARD_LIGHT_BOARDREFLECTION 0
#define UDBOARD_LIGHT_SUN             1
#define UDBOARD_LIGHT_SPOTLIGHT       2

int                           UDBoard_CurrentBufferDisplayed;
UDBoardBuffer                 UDBoard_2DLoadedViews[UDBoard_2D_constructedBoardBufferMax];
LE_DATA_DataId                Board2DTrade;
Object_3D_Interpolation_data  CameraInterpolationData, CameraWaitingData;
BOOL                          CameraMoveWaiting, CameraIsFloatingIdle;
LE_DATA_DataId                CurrentBoard;

// Define the 3D board view areas for main, status & trade screens.  The last is small for use when there is nothing to render (like turning 3D almost off)
RECT viewRects[4] = { { 0, 0, 800, 450 } , { 0, 0, 400, 225 }, { 200, 0, 600, 225 }, { 0, 0, 1, 1 } };
//RECT viewRects[4] = { { 0, 0, 800, 450 } , { 0, 0, 400, 225 }, { 300, 50, 400, 125 }, { 0, 0, 1, 1 } };

//extern "C" {
//	HMD_LoadBMPForTIM( unsigned long x, unsigned long y, unsigned long w, unsigned long h, char *name );
//}
//D3DVECTOR mAmbientFull( 0.76f, 0.76f, 0.76f ), mAmbientHalf( 0.40f, 0.40f, 0.40f );
D3DVECTOR mAmbientFull( 0.84f, 0.84f, 0.84f ), mAmbientHalf( 0.53f, 0.53f, 0.53f );

D3DVALUE pt3DBoardLocations[42][3] =
{
  0.0f,0.0f, 64.5f,
  0.0f,0.0f,104.0f,
  0.0f,0.0f,143.5f,
  0.0f,0.0f,183.0f,
  0.0f,0.0f,222.5f,
  0.0f,0.0f,262.0f,
  0.0f,0.0f,301.5f,
  0.0f,0.0f,341.0f,
  0.0f,0.0f,380.5f,
  0.0f,0.0f,420.0f,

   64.5f,0.0f,486.0f,
  104.0f,0.0f,486.0f,
  143.5f,0.0f,486.0f,
  183.0f,0.0f,486.0f,
  222.5f,0.0f,486.0f,
  262.0f,0.0f,486.0f,
  301.5f,0.0f,486.0f,
  341.0f,0.0f,486.0f,
  380.5f,0.0f,486.0f,
  420.0f,0.0f,486.0f,

  486.0f,0.0f,421.5f,
  486.0f,0.0f,382.0f,
  486.0f,0.0f,342.5f,
  486.0f,0.0f,303.0f,
  486.0f,0.0f,263.5f,
  486.0f,0.0f,224.0f,
  486.0f,0.0f,184.5f,
  486.0f,0.0f,145.0f,
  486.0f,0.0f,105.5f,
  486.0f,0.0f, 66.0f,

  421.5f,0.0f,0.0f,
  382.0f,0.0f,0.0f,
  342.5f,0.0f,0.0f,
  303.0f,0.0f,0.0f,
  263.5f,0.0f,0.0f,
  224.0f,0.0f,0.0f,
  184.5f,0.0f,0.0f,
  145.0f,0.0f,0.0f,
  105.5f,0.0f,0.0f,
   66.0f,0.0f,0.0f,

  64.5f,0.0f,466.5f,		//jail
  64.5f,0.0f,104.0f			//off board
};


D3DVALUE CameraAngles2D[][10] = // Note: while unit vectors might be proper, they are not required
{//  loc x    loc y     loc z     forward x     y            z              up vector xyz,   rollangle
  //{  243.0f, 1200.0f,  243.0f,    0.0f,       -1.0f,        0.0f       ,   1.0f,0.0f,0.0f,   0.0f},// converts xzy
  {  242.9f, 1200.0f,  243.0f,    0.0045f,    -0.99999f,    0.0f       ,   1.0f,0.0f,0.0f,   0.0f},// converts xzy - testing to see if slight angle stabilizes camera a bit during transitions.
  { -420.0f,  605.0f,  243.0f,    0.70710678f,-0.70710678f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  { -370.0f,  645.0f, -370.0f,    0.54167522f,-0.64278761f, 0.54167522f,   0.0f,1.0f,0.0f,   0.0f},

  { -139.0f,   78.0f,  124.5f,    0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  { -139.0f,   78.0f,  243.0f,    0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  { -139.0f,   78.0f,  361.5f,    0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  124.5f,   78.0f,  625.0f,    0.0f,       -0.35836795f,-0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  243.0f,   78.0f,  625.0f,    0.0f,       -0.35836795f,-0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  361.5f,   78.0f,  625.0f,    0.0f,       -0.35836795f,-0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  625.0f,   78.0f,  361.5f,   -0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  625.0f,   78.0f,  243.0f,   -0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  625.0f,   78.0f,  124.5f,   -0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  361.5f,   78.0f, -139.0f,    0.0f,       -0.35836795f, 0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  243.0f,   78.0f, -139.0f,    0.0f,       -0.35836795f, 0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  124.5f,   78.0f, -139.0f,    0.0f,       -0.35836795f, 0.93358043f,   0.0f,1.0f,0.0f,   0.0f},

  { -105.0f,  143.0f, -105.0f,    0.577096f,  -0.577858f,   0.577096f  ,   0.0f,1.0f,0.0f,   0.0f},
  { -105.0f,  143.0f,  591.0f,    0.577096f,  -0.577858f,  -0.577096f  ,   0.0f,1.0f,0.0f,   0.0f},
  {  591.0f,  143.0f,  591.0f,   -0.577096f,  -0.577858f,  -0.577096f  ,   0.0f,1.0f,0.0f,   0.0f},
  {  591.0f,  143.0f, -105.0f,   -0.577096f,  -0.577858f,   0.577096f  ,   0.0f,1.0f,0.0f,   0.0f},

  { -226.0f,  130.0f,  164.0f,    0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  { -226.0f,  130.0f,  322.0f,    0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  164.0f,  130.0f,  712.0f,    0.0f,       -0.35836795f,-0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  322.0f,  130.0f,  712.0f,    0.0f,       -0.35836795f,-0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  712.0f,  130.0f,  322.0f,   -0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  712.0f,  130.0f,  164.0f,   -0.93358043f,-0.35836795f, 0.0f       ,   0.0f,1.0f,0.0f,   0.0f},
  {  322.0f,  130.0f, -226.0f,    0.0f,       -0.35836795f, 0.93358043f,   0.0f,1.0f,0.0f,   0.0f},
  {  164.0f,  130.0f, -226.0f,    0.0f,       -0.35836795f, 0.93358043f,   0.0f,1.0f,0.0f,   0.0f},


  { -291.0f,  255.0f, -126.0f,    0.612372f,  -0.5f,        0.612372f,     0.0f,1.0f,0.0f,   0.0f},
  { -280.0f,  285.0f,  337.0f,    0.81915204f,-0.57357644f, 0.0f,          0.0f,1.0f,0.0f,   0.0f},
  { -209.0f,  227.0f,  650.0f,    0.717968f,  -0.5f,       -0.484275f,     0.0f,1.0f,0.0f,   0.0f},

  { -126.0f,  255.0f,  777.0f,    0.612372f,  -0.5f,       -0.612372f,     0.0f,1.0f,0.0f,   0.0f},//
  {  337.0f,  285.0f,  766.0f,    0.0f,       -0.57357644f,-0.81915204f,   0.0f,1.0f,0.0f,   0.0f},
  {  650.0f,  227.0f,  695.0f,   -0.484275f,  -0.5f,       -0.717968f,     0.0f,1.0f,0.0f,   0.0f},

  {  777.0f,  255.0f,  612.0f,   -0.612372f,  -0.5f,       -0.612372f,     0.0f,1.0f,0.0f,   0.0f},//
  {  766.0f,  285.0f,  149.0f,   -0.81915204f,-0.57357644f, 0.0f,          0.0f,1.0f,0.0f,   0.0f},
  {  695.0f,  227.0f, -164.0f,   -0.717968f,  -0.5f,        0.484275f,     0.0f,1.0f,0.0f,   0.0f},

  {  612.0f,  255.0f, -291.0f,   -0.612372f,  -0.5f,        0.612372f,     0.0f,1.0f,0.0f,   0.0f},//
  {  149.0f,  285.0f, -280.0f,    0.0f,       -0.57357644f, 0.81915204f,   0.0f,1.0f,0.0f,   0.0f},
  { -164.0f,  227.0f, -209.0f,    0.484275f,  -0.5f,        0.717968f,     0.0f,1.0f,0.0f,   0.0f},

};

/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.

 * No code other than the show module may start nor stop sound
 * and graphic items.  Fields indicating desired actions can be filled in
 * in the display state which the show routine will pick up.  In this way,
 * the show and routine can take responsibility for tracking all data
 * required to pause or clear the screen at any time.

 * There will be no locking the computer as a wait for all sequences to finish
 * might do.  Stack processing can be halted until sequences are finished,
 * but you will have to use a timer to poll your sequence status.  This is to
 * allow processing of pause buttons or user camera controls at all times, as
 * well as to make programmers lives more difficult :)
 */

void DISPLAY_UDBOARD_Initialize( void )
{
  // Initialize any permanent display objects.  Avoid temporary ones when possible.
  register int t;

  DISPLAY_state.game3Don = pc3D::Using3DHardware(); // FIXME - a game parameter - an option or config thingy.  Means use 3D board & backdrop in main view, affects lighting.

#ifdef _DEBUG
  DISPLAY_state.manualCamLock = FALSE;// TOY WITH THIS ONE.
#else
  DISPLAY_state.manualCamLock = FALSE;// Just making sure I never release with a camera lock.
#endif
  DISPLAY_state.manualCamRequested = FALSE;
  DISPLAY_state.manualCamDesired = 0;
  DISPLAY_state.manualMouseCamLock = FALSE;

  DISPLAY_state.viewportInUse = VIEWPORT_OFF;
  DISPLAY_state.viewportBackgroundFillOn = FALSE;
  LE_REND3D_SetViewportRect( 1, viewRects[DISPLAY_state.viewportInUse] );
  LE_REND3D_ClearBeforeRender( 1, DISPLAY_state.viewportBackgroundFillOn );

  // Projection stuff
  LE_REND3D_SetClippingPlanes( 1, CLIPPING_PLANE_NEAR, CLIPPING_PLANE_FAR );

  // Lighting
  LE_REND3D_SetAmbient(1, &mAmbientHalf );
  DISPLAY_state.lightSpotlightOn = FALSE;
  DISPLAY_state.lightSun = FALSE;
  DISPLAY_state.lightBoardGlow = FALSE;

  // Set the screen
  DISPLAY_state.city = 0; // Classic board
  DISPLAY_state.board3Don = FALSE;
  DISPLAY_state.current2DView = -1;
  DISPLAY_state.current2DCamera = -1; // Initially, nothing showing.
  DISPLAY_state.desired2DCamera = VIEW2D02_TOPDOWN_SOCCER;
  DISPLAY_state.desiredCameraInvalidatedLock = FALSE;
  DISPLAY_state.desiredCameraClearToValidate = FALSE;
  DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands = FALSE;
  DISPLAY_state.desiredCameraCustomMoveStartTime = 0;

  // Set the camera
  CameraMoveWaiting = CameraIsFloatingIdle = FALSE;
  CameraInterpolationData.End__Location = D3DVECTOR( CameraAngles2D[0][0], CameraAngles2D[0][1], CameraAngles2D[0][2] );
  CameraInterpolationData.End__ForwardV = D3DVECTOR( CameraAngles2D[0][3], CameraAngles2D[0][4], CameraAngles2D[0][5] );
  CameraInterpolationData.End__UpwardsV = D3DVECTOR( CameraAngles2D[0][6], CameraAngles2D[0][7], CameraAngles2D[0][8] );
  CameraInterpolationData.StartLocation = CameraInterpolationData.End__Location;
  CameraInterpolationData.StartForwardV = CameraInterpolationData.End__ForwardV;
  CameraInterpolationData.StartUpwardsV = CameraInterpolationData.End__UpwardsV;

  // set the FX type & time
  CameraInterpolationData.ElapsedTimeRatio = -1.0f; // Flags the elapsed time is to be calculated.
  CameraInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
  CameraInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
  CameraInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;
  CameraInterpolationData.StartTime = CameraInterpolationData.CurrentTime = CameraInterpolationData.EndTime = LE_TIME_TickCount;
  InterpolationFX_3DPositionAndOrientation( &CameraInterpolationData );// Sets up interpolated values
    
  DISPLAY_state.currentBackdropID = LED_EI;
  //CurrentBoard = LED_IFT( DAT_3D, HMD_board ); // This has been moved to udpeices where it is preloaded to start.
  DISPLAY_state.currentBoardOverlayID = LED_EI;
  UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect );
  DISPLAY_state.DemoModeOn = FALSE;
  DISPLAY_state.DemoModeDesired = FALSE;

  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    DISPLAY_state.HighlightDataIDs[t] = NULL;
  }

  // Custom 2D board stuff
  ZeroMemory(g_aid2DBoards, sizeof(LE_DATA_DataId) * NUM_2DBOARDS);

  // Create the permanent 2D main screen buffers
  UDBoard_CurrentBufferDisplayed = 0;
  // FIXME - if we initialized hardware, no need for these.  Number of buffers could be memory dependant.
  for( t = 0; t < UDBoard_2D_constructedBoardBufferMax; t++ )
  {
    UDBoard_2DLoadedViews[t].ViewLoaded = -1;
    UDBoard_2DLoadedViews[t].TimeLoaded = 0;
    UDBoard_2DLoadedViews[t].BoardBuffer = LE_GRAFIX_ObjectCreate( (short)(viewRects[VIEWPORT_MAIN].right - viewRects[VIEWPORT_MAIN].left), (short)(viewRects[VIEWPORT_MAIN].bottom - viewRects[VIEWPORT_MAIN].top), LE_GRAFIX_ObjectNotTransparent );
  }
  Board2DTrade  = LE_GRAFIX_ObjectCreate( (short)(viewRects[VIEWPORT_TRADE].right - viewRects[VIEWPORT_TRADE].left), (short)(viewRects[VIEWPORT_TRADE].bottom - viewRects[VIEWPORT_TRADE].top), LE_GRAFIX_ObjectNotTransparent );

  // Create lights
  for( t = 0; t < CE_ARTLIB_NumLights; t++ )
  {
      VERIFY(LE_REND3D_MakeLight(1, t));
  }

}


void DISPLAY_UDBOARD_Destroy( void )
{ // Deallocate stuff from Initialize.
  register int t;

  // Destroy lights
  for( t = 0; t < CE_ARTLIB_NumLights; t++ )
  {
      VERIFY(LE_REND3D_DeleteLight(1, t));
  }

}

/*****************************************************************************
 * TickActions - handles demo mode, actual camera move & lighting.
 */
void DISPLAY_UDBOARD_TickActions( int numberOfTicks )
{// 60ths of a second, this is called before show to allow programmatic movement control
  static Object_3D_Interpolation_data mySpotLightPosInterpolationData;

  static int startcamera, cameras, waitTimeThingie = 0;
  static int allcamerascycled = 0;

  static Rend3DCamera *lpCamera, camera; // New camera coords
  static Rend3DCamera startCameraCoord, endCameraCoord; // During smooth moves, start & end frame (not used as cameras)
//  static TYPE_Tick    lastManualMove; // Records the time of last user camera movement (interrupts demo sequence)
//  static int deltax, deltay;
  int    tickspermovebase = 85, ticksIdleBase = 0;
  static int ticksPerMove;
  unsigned int secondsBeforeDemoMode = 90;
  float fXCoordinate, fYCoordinate, fZCoordinate, fYAngle; // Temporary variables

  static BOOL canFloat = FALSE;

  int t;
  BOOL lightOn;


  if( numberOfTicks == 0 ) return; // No time elapsed.


  // DEMO MODE CODE
#if USE_HOTKEYS2
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_D) )
  {
    LE_KEYBRD_ClearLatchedKey(LE_KEYBRD_D);
    DISPLAY_state.LE_TickCount_Last_Button_Click = 0; // Will force demo mode
    if( LE_TIME_TickCount < secondsBeforeDemoMode * 60 ) LE_TIME_TickCount = secondsBeforeDemoMode * 60;
  }
#endif

  if( ((DISPLAY_state.LE_TickCount_Last_Button_Click + 60 * secondsBeforeDemoMode) <= LE_TIME_TickCount) && (!DISPLAY_state.board3Don || !DISPLAY_state.game3Don) )
    DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount; // if 3D-board is not visible, restart timer.
  if( DISPLAY_state.manualCamLock )
    DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount; // if manual camera, don't demo.
  if( ((DISPLAY_state.LE_TickCount_Last_Button_Click + 60 * secondsBeforeDemoMode) < LE_TIME_TickCount) && DISPLAY_state.board3Don)
  {// Demo mode, figure out what to do about it.
    if( !DISPLAY_state.DemoModeDesired )
    {// Go into demo mode.
      startcamera = cameras = DISPLAY_state.desired2DCamera;
      allcamerascycled = 0;
      waitTimeThingie = ticksPerMove + ticksIdleBase; // Move camera right off the bat
      //lastManualMove = 0;
      ticksPerMove = (int)(tickspermovebase *((float)(rand()%800 + 600)/1000));
      //if( rand()%10 == 0 ) // Perhaps count a number of demos - if excessive go for a slow move.
      //  ticksPerMove = tickspermovebase * 10;
      //deltax = deltay = 0;

      CameraInterpolationData.ElapsedTimeRatio = -1.0f;
      CameraInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
      CameraInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
      CameraInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;
    }
    
    //if( lastManualMove + 60 * 4 < LE_TIME_TickCount )
    //{// Keep the demo alive, escalate as required.
      waitTimeThingie += numberOfTicks;
      if( waitTimeThingie >= ticksPerMove + ticksIdleBase )
      {
        if( cameras == startcamera ) // demomodedesired is not yet set first time in, otherwise we are on the first camera and would immediately quit.
        {
          //Been around the loop
          allcamerascycled++;
          if( allcamerascycled == 2 ) // goes to 1 immediately when waittimethingie is not > tikspermove on setup.
          DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
        }
        waitTimeThingie = 0;
        // Set start to former end location - actually the interpolated just in case it isn't the same.
        CameraInterpolationData.StartLocation = CameraInterpolationData.InterLocation;
        CameraInterpolationData.StartForwardV = CameraInterpolationData.InterForwardV;
        CameraInterpolationData.StartUpwardsV = CameraInterpolationData.InterUpwardsV;
        cameras++;
        if( cameras > 38 ) cameras = 0;
        CameraInterpolationData.End__Location = D3DVECTOR( CameraAngles2D[cameras][0], CameraAngles2D[cameras][1], CameraAngles2D[cameras][2] );
        CameraInterpolationData.End__ForwardV = D3DVECTOR( CameraAngles2D[cameras][3], CameraAngles2D[cameras][4], CameraAngles2D[cameras][5] );
        CameraInterpolationData.End__UpwardsV = D3DVECTOR( CameraAngles2D[cameras][6], CameraAngles2D[cameras][7], CameraAngles2D[cameras][8] );

        CameraInterpolationData.StartTime = CameraInterpolationData.CurrentTime = LE_TIME_TickCount;
        CameraInterpolationData.EndTime = LE_TIME_TickCount + ticksPerMove;

      }
    //  deltax = deltay = 0;
    //}
    DISPLAY_state.DemoModeDesired = TRUE;
  } else
  { // We are moving out of demo mode
    if( DISPLAY_state.DemoModeDesired)// we know it is about to be false, this is the trailing edge of demo mode.
    {// Hack hack - this is how userifce.cpp - extended init does it at startup.
      DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount; // reset timer.
      DISPLAY_state.DemoModeDesired = FALSE;
      // Restore original camera
      CameraInterpolationData.StartLocation = CameraInterpolationData.InterLocation;
      CameraInterpolationData.StartForwardV = CameraInterpolationData.InterForwardV;
      CameraInterpolationData.StartUpwardsV = CameraInterpolationData.InterUpwardsV;
      CameraInterpolationData.End__Location = D3DVECTOR( CameraAngles2D[startcamera][0], CameraAngles2D[startcamera][1], CameraAngles2D[startcamera][2] );
      CameraInterpolationData.End__ForwardV = D3DVECTOR( CameraAngles2D[startcamera][3], CameraAngles2D[startcamera][4], CameraAngles2D[startcamera][5] );
      CameraInterpolationData.End__UpwardsV = D3DVECTOR( CameraAngles2D[startcamera][6], CameraAngles2D[startcamera][7], CameraAngles2D[startcamera][8] );

      CameraInterpolationData.StartTime = LE_TIME_TickCount;
      CameraInterpolationData.EndTime = LE_TIME_TickCount + ticksPerMove;
    }
  }


  // REGULAR CAMERA MOVEMENT
  // Here we may have a camera moving or not.  Typically, you request a new path by resetting the
  // CameraInterpolationData record yourself (put the interpolated stuff into the start to stay smooth),
  // this section will keep interpolation happening for you and start 'floating camera idles'
  // when there is no other movement to do.
  if( CameraInterpolationData.EndTime != CameraInterpolationData.CurrentTime )
  {// Camera is moving, update time index
    CameraInterpolationData.CurrentTime = LE_TIME_TickCount;
    if( CameraInterpolationData.CurrentTime > CameraInterpolationData.EndTime )
      CameraInterpolationData.CurrentTime = CameraInterpolationData.EndTime;
  }
  // Check that the position & orientation is correct.  Even with no time change, an 'instant' change may have been requested.
  InterpolationFX_3DPositionAndOrientation( &CameraInterpolationData );
  if( !(LE_REND3D_GetLocation( 1 )    == CameraInterpolationData.InterLocation) ||
    !(LE_REND3D_GetForwardVector( 1 ) == CameraInterpolationData.InterForwardV) ||
    !(LE_REND3D_GetUpwardVector( 1 )  == CameraInterpolationData.InterUpwardsV) )
  {
    // Use sequencer command for synchronisation with scene changes.
    LE_SEQNCR_SetCamera3D( &CameraInterpolationData.InterLocation,
      &CameraInterpolationData.InterForwardV,
      &CameraInterpolationData.InterUpwardsV,
      (float) (45.0 / 180.0 * 3.1415926) /* 45 degree standard camera field of view setting used by artists in Max, so it matches 2D board */,
      CLIPPING_PLANE_NEAR, CLIPPING_PLANE_FAR, 1 );
  }

  // If there is a stacked camera move, see if we can start it.
  if( CameraMoveWaiting )
  {// NOTE: For this to work, we need moves which use the time values (not preset interpolation values).  Watch that if bugs lead you here - endtime counts.
    if( CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime || CameraIsFloatingIdle )
    { // Done, start the chained one.
      if( (!DISPLAY_state.manualCamLock && !DISPLAY_state.manualMouseCamLock ) || DISPLAY_state.manualCamRequested )
      {
        UNS32 deltaTime = LE_TIME_TickCount - CameraWaitingData.StartTime;// We will increment the time by by this much in the new move.
        
        DISPLAY_state.manualCamRequested = FALSE; // Clear it - is a one-at-a-time request.
        DISPLAY_state.manualMouseCamLock = FALSE; // test - this should be enough to clear the mouse cam lock.

        CameraWaitingData.StartLocation = CameraInterpolationData.InterLocation;
        CameraWaitingData.StartForwardV = CameraInterpolationData.InterForwardV;
        CameraWaitingData.StartUpwardsV = CameraInterpolationData.InterUpwardsV;
        
        CameraInterpolationData = CameraWaitingData;
        
        CameraInterpolationData.StartTime = LE_TIME_TickCount;
        CameraInterpolationData.EndTime  += deltaTime;

        canFloat = TRUE; // After the move, we can do one float.
      }
      
      CameraMoveWaiting = CameraIsFloatingIdle = FALSE;
      if( DISPLAY_state.viewBSSMCamState == 2 )
      {
        DISPLAY_state.viewBSSMCamState = 0;// Something took control back.
        //TRACE("BSSM return discarded - new cam took over.");
      }
      // This line can be missed when the new cam is the same as the old - timer catches it, not used.
      if( DISPLAY_state.viewBSSMCamState == 1 ) DISPLAY_state.viewBSSMCamState = 2;// Flag BSSM camera move has been processed
    }
  }
  
  {
    // Floating camera - if the last move is done, we can start a floating camera.  A stacked camera would already have
    // started, we don't need to check again.
    if( CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime && !DISPLAY_state.manualCamLock &&
      !DISPLAY_state.DemoModeDesired && DISPLAY_state.board3Don && DISPLAY_state.game3Don && DISPLAY_state.desired2DCamera != VIEW2D01_TOPDOWN_SQUARE &&
      !DISPLAY_state.manualMouseCamLock && canFloat )
    {// Nothing grabbed control, lets do the floaty thing.
      static float LastFloatingVarX, LastFloatingVarY, LastFloatingVarZ;
      static TYPE_Tick lastFloatEndTime = 0;
      float FloatingVarX = 0, FloatingVarY = 0, FloatingVarZ = 0;
      float FloatingCamRandVariance, FloatingCamMinVariance;
      D3DVECTOR tempVect;
      
      if( !CameraIsFloatingIdle )//|| lastFloatEndTime + 60 * 3 < LE_TIME_TickCount )
      {// //Leave a few seconds between floats
        // Prior to a floaty cam starting, see if we should unroll a BSSM cam
        {
          canFloat = FALSE;          
          
          if( DISPLAY_state.TokenAnimStackIndex == -1 )
          {// Make the move more subtle if this is 'between' animations.
            FloatingCamMinVariance  =  CameraAngles2D[DISPLAY_state.desired2DCamera][1] * 0.08f; // Camera Y value based - larger movements when farther from baord.
            FloatingCamRandVariance =  CameraAngles2D[DISPLAY_state.desired2DCamera][1] * 0.08f; // Note the min is additive, can be 3 times this value (tops)
          } else
          {// Anims running
            FloatingCamMinVariance  =  CameraAngles2D[DISPLAY_state.desired2DCamera][1] * 0.07f; // Camera Y value based - larger movements when farther from baord.
            FloatingCamRandVariance =  CameraAngles2D[DISPLAY_state.desired2DCamera][1] * 0.05f; // Note the min is additive, can be 3 times this value (tops)
          }
          
          if( !CameraIsFloatingIdle ) // We are just starting to float
          {// Start in the last direction of motion
            if( CameraInterpolationData.End__Location.x == CameraInterpolationData.StartLocation.x &&
              CameraInterpolationData.End__Location.y == CameraInterpolationData.StartLocation.y &&
              CameraInterpolationData.End__Location.z == CameraInterpolationData.StartLocation.z )
            {// No direction, start slow
              LastFloatingVarX = LastFloatingVarY = LastFloatingVarZ = 0.0f;
            } else
            {// Use the 'rough' direction of the last move.  RandVar is 0 to max, so trim it down a bit.
              tempVect = FloatingCamRandVariance * 0.33f * Normalize( CameraInterpolationData.StartLocation - CameraInterpolationData.End__Location ); // note:  this is backward because it is inverted later to make a smooth besier seam.
              LastFloatingVarX = tempVect.x;
              LastFloatingVarY = tempVect.y;
              LastFloatingVarZ = tempVect.z;
            } 
          }
          
          // Start & endpoints are the same, the Bezier modifiers will float the camera.
          CameraInterpolationData.StartLocation = CameraInterpolationData.End__Location;
          CameraInterpolationData.StartForwardV = CameraInterpolationData.End__ForwardV;
          CameraInterpolationData.StartUpwardsV = CameraInterpolationData.End__UpwardsV;
          
          while( (FloatingVarX + FloatingVarY + FloatingVarZ) < FloatingCamMinVariance )
          {// Find a good arc.
            FloatingVarX = FloatingCamRandVariance * ((float)(rand()%100)) / 100;
            FloatingVarY = FloatingCamRandVariance * ((float)(rand()%100)) / 100;
            FloatingVarZ = FloatingCamRandVariance * ((float)(rand()%100)) / 100;
          }
          FloatingVarX -= FloatingCamRandVariance / 2;
          FloatingVarY -= FloatingCamRandVariance / 2;
          FloatingVarZ -= FloatingCamRandVariance / 2;
          
          // Start the move with the first offset an inverse of the last one (for a smooth seam), random new ending.
          CameraInterpolationData.BezierSegmentsUsed = 0; // Reset the number of segments
          InterpolationAddBezierSegment( &CameraInterpolationData, 0, &CameraInterpolationData.StartLocation, 
            &D3DVECTOR( 
            CameraInterpolationData.StartLocation.x - LastFloatingVarX, 
            CameraInterpolationData.StartLocation.y - LastFloatingVarY, 
            CameraInterpolationData.StartLocation.z - LastFloatingVarZ ),
            &D3DVECTOR( 
            CameraInterpolationData.StartLocation.x + FloatingVarX, 
            CameraInterpolationData.StartLocation.y + FloatingVarY, 
            CameraInterpolationData.StartLocation.z + FloatingVarZ ),
            &CameraInterpolationData.End__Location,
            BEZIER_TANGENT_DELTA );
          LastFloatingVarX = FloatingVarX;
          LastFloatingVarY = FloatingVarY;
          LastFloatingVarZ = FloatingVarZ;
          
          // Finally, set the time & type
          CameraInterpolationData.StartTime = LE_TIME_TickCount;
          CameraInterpolationData.EndTime   = LE_TIME_TickCount + 100;
          CameraInterpolationData.ElapsedTimeRatio = -1; // Let interpolation calculate ratio by time
          //CameraInterpolationData.InterpolationFXDesired = INTERPOLATION_BEZIER_POSITION_ACCELERATE_IN_AND_OUT;
          CameraInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
          CameraInterpolationData.InterpolationFXCurveType = INTERPOLATION_BEZIER;
          CameraInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;//INTERPOLATION_DIRECTION_IGNORE;
          
          CameraIsFloatingIdle = TRUE; // Flags that this motion can be interrupted as though the camera is idle.
          lastFloatEndTime = CameraInterpolationData.EndTime;
        }

      }// If waiting before float

      // If a BSSM cam is in effect, as soon as first swoop is done go back.  This wont trigger immediately as the float does.
      //if( DISPLAY_state.viewBSSMCamState == 2 ) // NOTE: In 2D this is missed.
      //{// We have a BSSM cam which we may want to leave
      //  if( CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime )
      //  {
      //    DISPLAY_state.viewBSSMCamState = 0;
      //    DISPLAY_state.desired2DCamera = DISPLAY_state.viewPriorToBSSMCam;
      //    TRACE("BSSM return - float");
      //  }
      //}
    }
  }
  // Put in a BSSM cam return check (for 2D or any reason float camera might be off)
  if( DISPLAY_state.viewBSSMCamState > 0 && (DISPLAY_state.viewBSSMCamRequestTime + 60 * 5 < LE_TIME_TickCount) )
  {// We have a BSSM cam which we may want to leave
    //if( CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime )
    //{
    if( DISPLAY_state.viewLastRequested == DISPLAY_state.desired2DCamera )
      DISPLAY_state.desired2DCamera = DISPLAY_state.viewPriorToBSSMCam;
    
    DISPLAY_state.viewBSSMCamState = 0;
    TRACE("BSSM return - time");
    //}
  }
  

  /*static TYPE_Point3D test;
  if( !(CameraInterpolationData.StartLocation == test ))
  {
    test = CameraInterpolationData.StartLocation;
    TRACE("New Camera Start, Y %f, time %d\n", CameraInterpolationData.StartLocation.y, LE_TIME_TickCount );
  }*/


  // LIGHTING
  // Directional Light (board color 'reflection')
  lightOn = DISPLAY_state.game3Don && DISPLAY_state.IsOptionLightingOn; // Any logic here - probably only a lights-off config option.
  if( lightOn )
  {// Affect change
    if( !DISPLAY_state.lightBoardGlow )
    {// Board color.  Should be a bit dimmer than sunlight, but practically we need the light.
      VERIFY(LE_REND3D_SetLightType     ( 1, UDBOARD_LIGHT_BOARDREFLECTION, D3DLIGHT_DIRECTIONAL ));
      VERIFY(LE_REND3D_SetLightColor    ( 1, UDBOARD_LIGHT_BOARDREFLECTION, CLRVALUE( 0.75f*0.35f, 0.86f*0.35f, 0.77f*0.35f, 0.0f)));
      VERIFY(LE_REND3D_SetLightDirection( 1, UDBOARD_LIGHT_BOARDREFLECTION, &D3DVECTOR( 0.0f, 1.0f, 0.0f )));
      VERIFY(LE_REND3D_SetLightFlags    ( 1, UDBOARD_LIGHT_BOARDREFLECTION, D3DLIGHT_ACTIVE ));
      DISPLAY_state.lightBoardGlow = TRUE;
    }
  } else
  {
    if( DISPLAY_state.lightBoardGlow )
    {
      VERIFY(LE_REND3D_SetLightFlags(1, UDBOARD_LIGHT_BOARDREFLECTION, 0));
      DISPLAY_state.lightBoardGlow = FALSE;
    }
  }

  // Directional Light - Sun or overhead.  Ambient affected by above (same logic as here)
  lightOn = DISPLAY_state.game3Don && DISPLAY_state.IsOptionLightingOn; // Any logic here - probably only a lights-off config option.  Ambient should go up if this goes off.
  if( lightOn )
  {// Affect change
    if( !DISPLAY_state.lightSun )
    {
      VERIFY(LE_REND3D_SetLightType     ( 1, UDBOARD_LIGHT_SUN, D3DLIGHT_DIRECTIONAL ));
      //VERIFY(LE_REND3D_SetLightColor    ( 1, UDBOARD_LIGHT_SUN, CLRVALUE( 0.30f, 0.30f, 0.30f, 0.0f )));
      VERIFY(LE_REND3D_SetLightColor    ( 1, UDBOARD_LIGHT_SUN, CLRVALUE( 0.28f, 0.28f, 0.28f, 0.0f )));
      VERIFY(LE_REND3D_SetLightDirection( 1, UDBOARD_LIGHT_SUN, &D3DVECTOR( 0.0f, -1.0f, 0.0f )));
      VERIFY(LE_REND3D_SetLightFlags    ( 1, UDBOARD_LIGHT_SUN, D3DLIGHT_ACTIVE ));
      DISPLAY_state.lightSun = TRUE;
    }
  } else
  {
    if( DISPLAY_state.lightSun )
    {
      VERIFY(LE_REND3D_SetLightFlags(1, UDBOARD_LIGHT_SUN, 0));
      DISPLAY_state.lightSun = FALSE;
    }
  }

  // Spotlight

  static  BOOL      ascendingX = TRUE, ascendingZ = TRUE;
  static  float     directionX = 0, directionY = 0, directionZ = -1;
  float moveIncrement = 1.5f;
  float idlemoveIncrement = 3.0f;

  lightOn = DISPLAY_state.game3Don && DISPLAY_state.board3Don && DISPLAY_state.IsOptionLightingOn; // 3D acceleration && board showing
  if( lightOn )
  {// Affect change
    if( !DISPLAY_state.lightSpotlightOn )
    {
      VERIFY(LE_REND3D_SetLightType       ( 1, UDBOARD_LIGHT_SPOTLIGHT, D3DLIGHT_SPOT ));
      //VERIFY(LE_REND3D_SetLightType       ( 1, UDBOARD_LIGHT_SPOTLIGHT, D3DLIGHT_POINT ));
      VERIFY(LE_REND3D_SetLightColor      ( 1, UDBOARD_LIGHT_SPOTLIGHT, CLRVALUE(1.0f, 1.0f, 1.0f, 0.0f) ));
      VERIFY(LE_REND3D_SetLightPosition   ( 1, UDBOARD_LIGHT_SPOTLIGHT, &D3DVECTOR(240.0f, 300.0f, 240.0f) ));
      VERIFY(LE_REND3D_SetLightDirection  ( 1, UDBOARD_LIGHT_SPOTLIGHT, &D3DVECTOR(0.0f, -1.0f, 0.0f) ));
      VERIFY(LE_REND3D_SetLightAttenuation( 1, UDBOARD_LIGHT_SPOTLIGHT, &D3DVECTOR(0.0f, 1.0f, 0.0f) ));
      //VERIFY(LE_REND3D_SetLightRange      ( 1, UDBOARD_LIGHT_SPOTLIGHT, 800.0f ));
      VERIFY(LE_REND3D_SetLightRange      ( 1, UDBOARD_LIGHT_SPOTLIGHT, 300.0f ));
      VERIFY(LE_REND3D_SetLightFalloff    ( 1, UDBOARD_LIGHT_SPOTLIGHT, 4.0f ));//1 is a line, higher drops faster
      VERIFY(LE_REND3D_SetLightTheta      ( 1, UDBOARD_LIGHT_SPOTLIGHT, float(PIDOUBLE/7) ));// Theta is inner.  6 & 2 work good
      VERIFY(LE_REND3D_SetLightPhi        ( 1, UDBOARD_LIGHT_SPOTLIGHT, float(PIDOUBLE/2) ));
      VERIFY(LE_REND3D_SetLightFlags      ( 1, UDBOARD_LIGHT_SPOTLIGHT, D3DLIGHT_ACTIVE) );
      DISPLAY_state.lightSpotlightOn = TRUE;

      // Now, initialize our data structure - all motion will be done through it.  NOTE OUR TARGET POSITION is interpolated on.
      //mySpotLightPosInterpolationData
      mySpotLightPosInterpolationData.End__Location = TYPE_Point3D( 240.0f, 300.0f, 240.0f );
      mySpotLightPosInterpolationData.End__ForwardV = TYPE_Point3D(   0.0f,   0.0f,   0.0f );// Color - angle not used.
      mySpotLightPosInterpolationData.End__UpwardsV = TYPE_Point3D(   0.0f,   1.0f,   0.0f );// Up is irrelevant for lights

      mySpotLightPosInterpolationData.StartLocation = mySpotLightPosInterpolationData.End__Location;
      mySpotLightPosInterpolationData.StartForwardV = mySpotLightPosInterpolationData.End__ForwardV;
      mySpotLightPosInterpolationData.StartUpwardsV = mySpotLightPosInterpolationData.End__UpwardsV;

      mySpotLightPosInterpolationData.InterLocation = mySpotLightPosInterpolationData.End__Location;
      mySpotLightPosInterpolationData.InterForwardV = mySpotLightPosInterpolationData.End__ForwardV;
      mySpotLightPosInterpolationData.InterUpwardsV = mySpotLightPosInterpolationData.End__UpwardsV;

      mySpotLightPosInterpolationData.ElapsedTimeRatio = -1.0f; // Flags that ratios are calculated from times, not here.
      //mySpotLightPosInterpolationData.InterpolationFXDesired = INTERPOLATION_ACCELERATE_IN_AND_OUT_SIN;
      mySpotLightPosInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
      mySpotLightPosInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
      mySpotLightPosInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;

      mySpotLightPosInterpolationData.StartTime = mySpotLightPosInterpolationData.CurrentTime = mySpotLightPosInterpolationData.EndTime = 
        LE_TIME_TickCount;
    }

    // Some motion
    for( t = 0; t < numberOfTicks; t++ )
    {
      if( ((DISPLAY_state.LE_TickCount_Last_Button_Click + 60 * 25) < LE_TIME_TickCount) && DISPLAY_state.board3Don)
      {// Light is tired of idle token, move around.
        mySpotLightPosInterpolationData.StartTime = mySpotLightPosInterpolationData.CurrentTime = mySpotLightPosInterpolationData.EndTime = 
          LE_TIME_TickCount;// Flags immediate move - go to destination.
        //mySpotLightPosInterpolationData.InterpolationFXDesired = INTERPOLATION_ACCELERATE_IN_AND_OUT_SIN;
        mySpotLightPosInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
        mySpotLightPosInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
        mySpotLightPosInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;

        if( ascendingX )
        {
          if( mySpotLightPosInterpolationData.End__Location.x < 420 )
            mySpotLightPosInterpolationData.End__Location.x += idlemoveIncrement;
          else
            ascendingX = !ascendingX;
        } else
        {
          if( mySpotLightPosInterpolationData.End__Location.x > 0 )
            mySpotLightPosInterpolationData.End__Location.x -= idlemoveIncrement;
          else
            ascendingX = !ascendingX;
        }
        if( ascendingZ )
        {
          if( mySpotLightPosInterpolationData.End__Location.z < 420 )
            mySpotLightPosInterpolationData.End__Location.z += idlemoveIncrement;
          else
            ascendingZ = !ascendingZ;
        } else
        {
          if( mySpotLightPosInterpolationData.End__Location.z > 0 )
            mySpotLightPosInterpolationData.End__Location.z -= idlemoveIncrement;
          else
            ascendingZ = !ascendingZ;
        }
      } else
      {// Focus on current player
        if( DISPLAY_state.TokenAnimStackIndex == -1 ) // Good indication no move animation is on.
        {// This should be used unless the token is moving.
          UDPIECES_GetTokenOrientation( UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare, 
            &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
          
          if( mySpotLightPosInterpolationData.End__Location.x != fXCoordinate ||
            mySpotLightPosInterpolationData.End__Location.y != fYCoordinate ||
            mySpotLightPosInterpolationData.End__Location.z != fZCoordinate )
          {// Set the start pos to the current pos, reset other params for our move.
            // FIXME: Momentum should be preserved not discarded - we need another transition effect.
            //mySpotLightPosInterpolationData.InterpolationFXDesired = INTERPOLATION_ACCELERATE_IN_AND_OUT_SIN;
            mySpotLightPosInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
            mySpotLightPosInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
            mySpotLightPosInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;

           
            mySpotLightPosInterpolationData.StartLocation = mySpotLightPosInterpolationData.InterLocation;
            mySpotLightPosInterpolationData.StartForwardV = mySpotLightPosInterpolationData.InterForwardV;// Color
            mySpotLightPosInterpolationData.StartUpwardsV = mySpotLightPosInterpolationData.InterUpwardsV;// not used
            
            mySpotLightPosInterpolationData.End__Location = TYPE_Point3D( fXCoordinate, fYCoordinate, fZCoordinate );
            
            mySpotLightPosInterpolationData.ElapsedTimeRatio = -1.0f;
            mySpotLightPosInterpolationData.StartTime = LE_TIME_TickCount;
            mySpotLightPosInterpolationData.EndTime = LE_TIME_TickCount + 210;
          }
        } else
          
        {// Locate the tokens actual position in its running sequence - move the light to it in parts to smooth start offsets.
          UDPIECES_GetTokenActualOrientation( UICurrentGameState.CurrentPlayer, 
            &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
          
          //if( mySpotLightPosInterpolationData.End__Location.x != fXCoordinate ||
          //  mySpotLightPosInterpolationData.End__Location.y != fYCoordinate ||
          //  mySpotLightPosInterpolationData.End__Location.z != fZCoordinate )
          //{// Set the start pos to the current pos, reset other params for our move.
          //mySpotLightPosInterpolationData.InterpolationFXDesired = INTERPOLATION_LINEAR_MOVEMENT;
          mySpotLightPosInterpolationData.InterpolationFXSpeedType = INTERPOLATION_LINEAR_MOVEMENT;
          mySpotLightPosInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
          mySpotLightPosInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;
          
          mySpotLightPosInterpolationData.StartLocation = mySpotLightPosInterpolationData.InterLocation;
          mySpotLightPosInterpolationData.StartForwardV = mySpotLightPosInterpolationData.InterForwardV;// Color
          mySpotLightPosInterpolationData.StartUpwardsV = mySpotLightPosInterpolationData.InterUpwardsV;// not used
          
          mySpotLightPosInterpolationData.End__Location = TYPE_Point3D( fXCoordinate, fYCoordinate, fZCoordinate );
          
          mySpotLightPosInterpolationData.ElapsedTimeRatio = 0.3f;// Not time dependant in this use.
          //}
        }
      }
    }
    mySpotLightPosInterpolationData.CurrentTime = LE_TIME_TickCount;
    InterpolationFX_3DPositionAndOrientation( &mySpotLightPosInterpolationData ); // Note: we could have just set the InterLocation ourselves, but the algorythm will be improved soon to be an actual interpolation
    //VERIFY(LE_REND3D_SetLightDirection  ( 1, UDBOARD_LIGHT_SPOTLIGHT, 
    VERIFY(LE_REND3D_SetLightPosition  ( 1, UDBOARD_LIGHT_SPOTLIGHT, 
      &D3DVECTOR(mySpotLightPosInterpolationData.InterLocation.x,// - 240.0f,
      mySpotLightPosInterpolationData.InterLocation.y + 200.0f, 
      mySpotLightPosInterpolationData.InterLocation.z) )); // Offset light source above board.
      //&mySpotLightPosInterpolationData.InterLocation ));

    // Some color
    int ca, cb, cc;
    float crf, cgf, cbf;
    float colorIncrement = 0.003f;
    float colorMagnifier = 0.75f;
    static float currentSpotlightR = 1.0f, currentSpotlightG = 1.0f, currentSpotlightB = 1.0;
    
    ca = cb = cc = RULE_PlayerColourToRGB[UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].colour];
    ca = ca >> 16;
    cb = (cb >> 8 ) & 255;
    cc = cc & 255;
    cbf = (colorMagnifier * ca)/255;
    cgf = (colorMagnifier * cb)/255;
    crf = (colorMagnifier * cc)/255;
    
    for( t = 0; t < numberOfTicks; t++ )
    {
      if( currentSpotlightR != crf )
      {
        if( fabs( currentSpotlightR - crf ) <= colorIncrement )
          currentSpotlightR = crf;
        else
        {
          if( currentSpotlightR > crf )
            currentSpotlightR -= colorIncrement;
          else
            currentSpotlightR += colorIncrement;
        }
      }
      if( currentSpotlightG != cgf )
      {
        if( fabs( currentSpotlightG - cgf ) <= colorIncrement )
          currentSpotlightG = cgf;
        else
        {
          if( currentSpotlightG > cgf )
            currentSpotlightG -= colorIncrement;
          else
            currentSpotlightG += colorIncrement;
        }
      }
      if( currentSpotlightB != cbf )
      {
        if( fabs( currentSpotlightB - cbf ) <= colorIncrement )
          currentSpotlightB = cbf;
        else
        {
          if( currentSpotlightB > cbf )
            currentSpotlightB -= colorIncrement;
          else
            currentSpotlightB += colorIncrement;
        }
      }
    }

    VERIFY(LE_REND3D_SetLightColor(1, UDBOARD_LIGHT_SPOTLIGHT,
                                   CLRVALUE(currentSpotlightR,
                                            currentSpotlightG,
                                            currentSpotlightB, 0.0f)));
    
  } else
  {
    if( DISPLAY_state.lightSpotlightOn )
    {
      VERIFY(LE_REND3D_SetLightFlags(1, UDBOARD_LIGHT_SPOTLIGHT, 0));
      DISPLAY_state.lightSpotlightOn = FALSE;
    }
  }

  // Finally set the render state.
  static BOOL isLowState = FALSE; // Even if we start in software, the state starts high (see display init)
//  if( DISPLAY_state.lightSpotlightOn ) // One frame too slow...
  if( DISPLAY_state.game3Don && DISPLAY_state.IsOptionLightingOn )//DISPLAY_state.board3Don && 
  {// Make sure settings are high
    if( isLowState )
    {// These may be options, turn em on for a start.
      LE_REND3D_SetAmbient(1, &mAmbientHalf );
      isLowState = FALSE;
    }
  } else
  {// Software, always use the cheapass model no frills.
    if( !isLowState )
    {
      LE_REND3D_SetAmbient(1, &mAmbientFull );
      isLowState = TRUE;
    }
  }
                                 
}



/*****************************************************************************
 * Show - Updates display based on state - board, ownership indicators.
 */
void DISPLAY_UDBOARD_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything should be in the user interface and display states.
  register int t;
  int player;
  LE_DATA_DataId highlightID;


  // Update the 2D backdrop
  if( DISPLAY_state.manualCamLock && DISPLAY_state.current2DCamera!= -1 ) 
    DISPLAY_state.desired2DCamera = DISPLAY_state.manualCamDesired; // Manual camera, reverse the request so cards fly off from the correct view.

  if( (DISPLAY_state.desired2DView != DISPLAY_state.current2DView) ||   // screen changed
    (DISPLAY_state.desired2DCamera != DISPLAY_state.current2DCamera) || // camera changed
    (DISPLAY_state.desiredCameraClearToValidate && DISPLAY_state.desiredCameraInvalidatedLock ) || // Custom movement discontinued (reset the camera)
    ( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA ) && (DISPLAY_state.game3Don != DISPLAY_state.board3Don) )) // 3D mode changed (screen change will meet the logic as well, this catches on-the-fly changes mostly to support a hotkey.
  { // shut off current view
    // First some custom special stuff
    if( DISPLAY_state.desired2DView != DISPLAY_state.current2DView) // screen changed - kill demo mode, reset idle timer.
      DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
    if( DISPLAY_state.desiredCameraClearToValidate && DISPLAY_state.desiredCameraInvalidatedLock )
    {// Clear the lock indicators - the camera will be reset below.
      DISPLAY_state.desiredCameraInvalidatedLock = FALSE;
      DISPLAY_state.desiredCameraClearToValidate = FALSE;
    }

    if( DISPLAY_state.currentBackdropID != LED_EI )
    {
      LE_SEQNCR_Stop( DISPLAY_state.currentBackdropID, DISPLAY_BackgroundPriority );
      DISPLAY_state.currentBackdropID = LED_EI;
    }
    if( DISPLAY_state.desired2DView != -1 )
    { // turn on new view
      BOOL formerFillMode = DISPLAY_state.viewportBackgroundFillOn;
      int  formerView     = DISPLAY_state.viewportInUse;

      if( DISPLAY_state.game3Don ) // Full 3D view
        DISPLAY_state.viewportBackgroundFillOn = TRUE;
      else
        DISPLAY_state.viewportBackgroundFillOn = FALSE;
      switch( DISPLAY_state.desired2DView )
      {
      case DISPLAY_SCREEN_PortfolioA:
        DISPLAY_state.viewportInUse = VIEWPORT_STATUS;
        g_bOptionsButtonsOn = FALSE;
        break;
      case DISPLAY_SCREEN_MainA:
        DISPLAY_state.viewportInUse = VIEWPORT_MAIN;
        g_bOptionsButtonsOn = FALSE;
        break;
      case DISPLAY_SCREEN_TradeA:
        DISPLAY_state.viewportInUse = VIEWPORT_TRADE;
        g_bOptionsButtonsOn = FALSE;
        break;
        
      case DISPLAY_SCREEN_Pselect:
        DISPLAY_state.viewportInUse = VIEWPORT_OFF;
        g_bOptionsButtonsOn = TRUE;
        DISPLAY_state.currentBackdropID = LED_IFT( DAT_LANG2, BMP_sybkgrnd );
        break;
      case DISPLAY_SCREEN_PselectRules:
        DISPLAY_state.viewportInUse = VIEWPORT_OFF;
        DISPLAY_state.currentBackdropID = LED_IFT( DAT_PAT, BMP_rnbacknd );
        break;

      case DISPLAY_SCREEN_AuctionA:
        DISPLAY_state.currentBackdropID = LED_IFT(DAT_PAT, BMP_auctiona); 
        DISPLAY_state.viewportInUse = VIEWPORT_OFF;
        g_bOptionsButtonsOn = FALSE;
        break;

      case DISPLAY_SCREEN_Options:
        DISPLAY_state.currentBackdropID = LED_IFT(DAT_PAT, BMP_rnbacknd );
        DISPLAY_state.viewportInUse = VIEWPORT_OFF;
        g_bOptionsButtonsOn = TRUE;
        break;

      case DISPLAY_SCREEN_Black:
      default:
        DISPLAY_state.currentBackdropID = LED_EI; 
        DISPLAY_state.viewportInUse = VIEWPORT_OFF;
        g_bOptionsButtonsOn = FALSE;
        break;
      }
      // Adjust modes if required
      if( DISPLAY_state.viewportInUse != formerView )
        //LE_REND3D_SetViewportRect( 1, viewRects[DISPLAY_state.viewportInUse] );
        LE_SEQNCR_SetViewport( &viewRects[DISPLAY_state.viewportInUse], 1 );
      if( DISPLAY_state.viewportBackgroundFillOn != formerFillMode )
        LE_REND3D_ClearBeforeRender( 1, DISPLAY_state.viewportBackgroundFillOn );

      UDBOARD_ActivateDesiredCamera( DISPLAY_state.desired2DCamera ); // Accounts for 3D board in main view.
      if( DISPLAY_state.currentBackdropID != LED_EI )
      { // Fire it up
        //LE_SEQNCR_Start( DISPLAY_state.currentBackdropID, DISPLAY_BackgroundPriority );
        LE_SEQNCR_StartCXYSlot(DISPLAY_state.currentBackdropID, DISPLAY_BackgroundPriority,
          NULL, 0, FALSE, 
          viewRects[DISPLAY_state.viewportInUse].left, viewRects[DISPLAY_state.viewportInUse].top, (1 << 0) /*slot*/);
      }
    }
  }

  // Update the 3D board - note this follows the 2D so the 2D can detect a change in the game3Don and kill the 2D board immediately (rather than next screen/camera change)
  //if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA || 
  //  DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA ||
  //  ( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA) && DISPLAY_state.game3Don ) )
  if( DISPLAY_state.game3Don && DISPLAY_IsBoardVisible )
  {// Board should be on
    if( !DISPLAY_state.board3Don )
    {
      TYPE_Matrix3D mxScale;
      DISPLAY_state.board3Don = TRUE;
      LE_MATRIX_3D_SetScale(0.10f,&mxScale);
      // Kinda a quickie, but if city specify city mesh else standard.
#if USA_VERSION
      if( DISPLAY_state.city != 0 ) //DISPLAY_state.cityTextureSet && DISPLAY_state.game3Don )
        UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_citymed ), FALSE );
      else
        UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardmed ), FALSE );
#else
        UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardmed ), FALSE );
#endif
      LE_SEQNCR_Start( CurrentBoard, DISPLAY_Board3dPriority );
      LE_SEQNCR_MoveTheWorks(CurrentBoard, DISPLAY_Board3dPriority,
        LED_EI,0,FALSE,TRUE,TRUE,3,&mxScale);
    }
  } else
  {// Board should be off
    if( DISPLAY_state.board3Don )
    {
      DISPLAY_state.board3Don = FALSE;
      LE_SEQNCR_Stop( CurrentBoard, DISPLAY_Board3dPriority );
    }
  }

  // Property ownership & mortgage bars
  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    static TYPE_Point3D ptPosition;
    static TYPE_Angle3D fRotate;

    player = UICurrentGameState.Squares[t].owner;
    if( (player < RULE_MAX_PLAYERS) && DISPLAY_IsBoardVisible )
    { // Property highlights, 2D & 3D together - just an ID to me
      highlightID = NULL;
      if( DISPLAY_state.board3Don || DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA || DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA )
      {// 3D ownership & mortgage indicators
        // We have a 3D ownership indicator
        if( UICurrentGameState.Squares[t].mortgaged )
          highlightID = LED_IFT( DAT_3D, HMD_mnmi1 ) + UICurrentGameState.Players[UICurrentGameState.Squares[t].owner].colour;
        else
          highlightID = LED_IFT( DAT_3D, HMD_mnoi1 ) + UICurrentGameState.Players[UICurrentGameState.Squares[t].owner].colour;
        UDBOARD_GetHighlightPosition( t, &ptPosition.x, &ptPosition.z, &fRotate );
      } else
      {// camera dependant 2D graphics
        if( UICurrentGameState.Squares[t].mortgaged )
          highlightID = LED_IFT( DAT_BOARD2, TAB_mnmip01pc0000) + DISPLAY_state.desired2DCamera * 168 +
          DISPLAY_propertyToOwnablePropertyConversion( t ) * RULE_MAX_PLAYERS +
          UICurrentGameState.Players[player].colour;
        else
          highlightID = LED_IFT( DAT_BOARD, TAB_mnoip01pc0000) + DISPLAY_state.desired2DCamera * 168 +
          DISPLAY_propertyToOwnablePropertyConversion( t ) * RULE_MAX_PLAYERS +
          UICurrentGameState.Players[player].colour;
      }
      
      if( DISPLAY_state.HighlightDataIDs[t] != highlightID )
      { // not the right highlight displayed
        if( DISPLAY_state.HighlightDataIDs[t] != NULL )
          LE_SEQNCR_Stop( DISPLAY_state.HighlightDataIDs[t], DISPLAY_state.HighlightPriority[t] );
        DISPLAY_state.HighlightDataIDs[t] = highlightID;
        
        if( DISPLAY_state.HighlightDataIDs[t] != NULL )
        {
          DISPLAY_state.HighlightPriority[t] = DISPLAY_BoardHighlightPriority + t;
          if( DISPLAY_state.board3Don || DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA || DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA )
          {
            //LE_SEQNCR_StartXY( DISPLAY_state.HighlightDataIDs[t], DISPLAY_state.HighlightPriority[t],
            //(DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) ? BoardLocations[t][0] : BoardLocations[t][0] + DISPLAY_View2XOff,
            //(DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) ? BoardLocations[t][1] : BoardLocations[t][1] + DISPLAY_View2YOff );
            LE_SEQNCR_StartRySTxz( DISPLAY_state.HighlightDataIDs[t], DISPLAY_state.HighlightPriority[t],
              fRotate,0.10f,ptPosition.x,ptPosition.z);
          } else
          {
            LPNEWBITMAPHEADER	BitmapUCPHeaderPntr;
            
            BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER) LE_DATA_UseRef( DISPLAY_state.HighlightDataIDs[t] );
            //LE_SEQNCR_StartCXYSlot(DISPLAY_state.currentBackdropID, DISPLAY_BackgroundPriority,
            //  NULL, 0, FALSE, 
            //  0, 0, (1 << 0) /*slot*/);
            LE_SEQNCR_StartCXYSlot( DISPLAY_state.HighlightDataIDs[t], DISPLAY_state.HighlightPriority[t],
              NULL, 0, FALSE, // Chain to crap
              BitmapUCPHeaderPntr->nXOriginOffset,
              BitmapUCPHeaderPntr->nYOriginOffset, (1 << 0) );
            LE_DATA_RemoveRef( DISPLAY_state.HighlightDataIDs[t] );
          }
        }
      }
    } else
    { // Be sure no ownership highlight is on this square
      if( DISPLAY_state.HighlightDataIDs[t] != NULL )
      {    
        LE_SEQNCR_Stop( DISPLAY_state.HighlightDataIDs[t], DISPLAY_state.HighlightPriority[t] );
        DISPLAY_state.HighlightDataIDs[t] = NULL;
      }
    }

  } // end for t loop each property


  // This flags the camera and view as being affected - not a good place since there is no assurance we did anything, but hey.
  DISPLAY_state.current2DView = DISPLAY_state.desired2DView;
  DISPLAY_state.current2DCamera = DISPLAY_state.desired2DCamera;
}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDBOARD_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{
  D3DVECTOR centerPoint = D3DVECTOR( 243.0f, 10.0f, 243.0f );

  // Mouse controlled camera controls.
  if( (UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED && LE_MOUSE_MousePressed[LEFT_BUTTON] &&
    DISPLAY_state.board3Don && DISPLAY_state.viewportInUse != VIEWPORT_OFF )
    || DISPLAY_state.manualMouseCamLock )
  {// See if this is in the 3D viewport - there are no controls ever in the viewport, so we know a camera move is requested.
    // Some duplicate logic - we want the inner loop to process the mouse move, the outer to always reprocess direction.
    if( UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED && LE_MOUSE_MousePressed[LEFT_BUTTON] &&
      viewRects[DISPLAY_state.viewportInUse].left   <= UIMessagePntr->numberA &&
      viewRects[DISPLAY_state.viewportInUse].right  >= UIMessagePntr->numberA &&
      viewRects[DISPLAY_state.viewportInUse].top    <= UIMessagePntr->numberB &&
      viewRects[DISPLAY_state.viewportInUse].bottom >= UIMessagePntr->numberB )
    {// Now we know the user wants to control the camera - let any current camera move complete.
      if( (CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime) || CameraIsFloatingIdle || DISPLAY_state.manualMouseCamLock ) 
      { // Start / continue manual control.
        DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
        CameraIsFloatingIdle = FALSE; // Logic below handles moving the 'current' data to end anyway, no special code to come out of float mode.
        DISPLAY_state.manualMouseCamTime = LE_TIME_TickCount;
        if( !DISPLAY_state.manualMouseCamLock )
        {// initialize manual mouse cam - primarily set up a smooth move to point at the center of orbit.
          DISPLAY_state.manualMouseCamLock = TRUE;

          CameraInterpolationData.StartLocation = CameraInterpolationData.InterLocation;
          CameraInterpolationData.StartForwardV = Normalize(CameraInterpolationData.InterForwardV);
          CameraInterpolationData.StartUpwardsV = CameraInterpolationData.InterUpwardsV;

          CameraInterpolationData.PointingAt = centerPoint;

          CameraInterpolationData.End__Location = CameraInterpolationData.StartLocation;
          CameraInterpolationData.End__ForwardV = Normalize( centerPoint - CameraInterpolationData.InterLocation );
          CameraInterpolationData.End__UpwardsV = CameraInterpolationData.StartUpwardsV;

          CameraInterpolationData.StartTime = CameraInterpolationData.CurrentTime = LE_TIME_TickCount;
          CameraInterpolationData.EndTime   = LE_TIME_TickCount + CAMERA_BASE_MOVE_TIME;
          CameraInterpolationData.ElapsedTimeRatio = -1; // Let interpolation calculate ratio by time
          CameraInterpolationData.InterpolationFXSpeedType = INTERPOLATION_LINEAR_MOVEMENT;
          CameraInterpolationData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
          CameraInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;//INTERPOLATION_DIRECTION_POINT_AT;
        }

        // Until the first time segment is done, this motion is somewhat muted.  May have to change start location as well if the effect is undesireable.
        // Orbit horizontally by numberC
        D3DVECTOR tempvect = CameraInterpolationData.End__Location - centerPoint; // Move to 'center'
        tempvect.y = 0; // ignore height - just orbit x/z
        //tempvect = Normalize( tempvect );// Just the direction is now left.
        float tempangle, tempMagnitude = Magnitude( tempvect );

        if( tempMagnitude > 0.0000001f )
        {
          tempangle = (float)atan2(tempvect.x, tempvect.z);
          if( UIMessagePntr->numberC > 40 )  UIMessagePntr->numberC =  50;
          if( UIMessagePntr->numberC < -40 ) UIMessagePntr->numberC = -50;
          tempangle += (float)(((float)UIMessagePntr->numberC / 300.0f) * pi); // Pretty much made up - decent speed.
          CameraInterpolationData.End__Location.x = (float)(tempMagnitude * sin( tempangle) + centerPoint.x); // Move back out from center
          CameraInterpolationData.End__Location.z = (float)(tempMagnitude * cos( tempangle) + centerPoint.z);
        }

        // Zoom or Orbit vertically depending on right mouse button state
        if( LE_MOUSE_MousePressed[ RIGHT_BUTTON ] || LE_KEYBRD_CheckPressed( LE_KEYBRD_CONTROL ) )
        {// Up/down - Rotation takes too much brain power - change Y then scale to right magnitude...FIXME?
          if( UIMessagePntr->numberD > 50 )  UIMessagePntr->numberD =  50;
          if( UIMessagePntr->numberD < -50 ) UIMessagePntr->numberD = -50;
          
          if( tempMagnitude > 1.0f )
          {
            tempvect = CameraInterpolationData.End__Location - centerPoint; // Move to 'center'
            tempMagnitude = Magnitude( tempvect );
            
            tempvect.y += UIMessagePntr->numberD * (float)tempMagnitude / 300.0f;
            if( tempvect.y < -50 ) tempvect.y = -50;
            if( tempvect.y > 0.98f * tempMagnitude ) tempvect.y = 0.98f * tempMagnitude;
           
            tempvect = tempMagnitude * Normalize(tempvect); // Scale it down - tends to pull it around an arc but not in a linear fashion.

            CameraInterpolationData.End__Location = tempvect + centerPoint; // Back out where it came from.
          }
        } else
        {//move in/out by numberD.
          if( UIMessagePntr->numberD > 42 )  UIMessagePntr->numberD =  42;
          if( UIMessagePntr->numberD < -42 ) UIMessagePntr->numberD = -42;
          
          tempvect = CameraInterpolationData.End__Location - centerPoint;
          tempMagnitude = Magnitude(tempvect);
          tempMagnitude += (float)UIMessagePntr->numberD / 0.47f;
          if( tempMagnitude < 100 ) tempMagnitude = 100;
          if( tempMagnitude > 1200 ) tempMagnitude = 1200;
          CameraInterpolationData.End__Location = tempMagnitude * Normalize(tempvect) + centerPoint;
        }
      }
    }//endif in viewport

    // Update the camera direction IF we have a mouse lock (other moves not in the viewport will still get here)
    if( DISPLAY_state.manualMouseCamLock )
    {
      if( CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime )
      {// We should be pointing at the center - change to point at as it needs to be recalculated whenever the camera moves.
        CameraInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_POINT_AT;
      } else
      {// Keep the end direction up to date - the camera is moving
        CameraInterpolationData.CurrentTime = LE_TIME_TickCount;
        InterpolationFX_3DPositionAndOrientation( &CameraInterpolationData );// This will be a little closer to the actual location when rendered.
        CameraInterpolationData.End__ForwardV = Normalize( centerPoint - CameraInterpolationData.InterLocation );
      }
    }
  }
  // Check if it is time to release control.
  if( DISPLAY_state.manualMouseCamLock && !DISPLAY_state.manualCamLock )
  {// Don't ever reset this when a manual cam lock is in use (always on manual).
    if( DISPLAY_state.manualMouseCamTime < LE_TIME_TickCount - 60 * DISPLAY_MOUSE_CAM_RELEASE_SECONDS )
    {// Release lock - force the camera to reset.
      DISPLAY_state.manualMouseCamLock = FALSE;
      DISPLAY_state.desiredCameraInvalidatedLock = TRUE;
      // The next lines force the camera to correct itself.  I don't like the effect right now, so its out.
      // NOTE: even without it the camera was resetting - extended the time delay(RK)
      DISPLAY_state.desiredCameraClearToValidate = TRUE;
    }
  }


    


#if USE_HOTKEYS
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_V) )
  {// Cheat to test video mode switch
    LE_KEYBRD_ClearLatchedKey( LE_KEYBRD_V );
    DISLAY_NewScreenSizeNDepth( 1024, 768, 16 );
  }
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_W) )
  {// Cheat to test video mode switch
    static BOOL isWire = FALSE;
    LE_KEYBRD_ClearLatchedKey( LE_KEYBRD_W );
    if( isWire )
      SET_FILLMODE( D3DFILL_SOLID );
    else
      SET_FILLMODE( D3DFILL_WIREFRAME );
    isWire = !isWire;
  }
	if( LE_KEYBRD_CheckLatched(LE_KEYBRD_J) )
  {// Cheat to test jail card display for status screen
    LE_KEYBRD_ClearLatchedKey( LE_KEYBRD_J );
    UICurrentGameState.Cards[0].jailOwner = 0;
		UICurrentGameState.Cards[1].jailOwner = 0;
  }
#endif
}


/****************************************************************************/
/*****************************************************************************
 * Set the current standard view (and corresponding camera angle)
 */

void UDBOARD_SetBackdrop( int backdrop )
{
  DISPLAY_state.desired2DView = backdrop;
}


/*****************************************************************************
 * UDBOARD_CompileBackdrop - Construct a 2D backdrop with overlays into a buffer
 */
void UDBOARD_CompileBackdrop( int cameraview, int bufferindex )
{
  switch( DISPLAY_state.desired2DView )
  {
  case DISPLAY_SCREEN_PortfolioA:
  case DISPLAY_SCREEN_TradeA:     //Board2DTrade will be updated
    LE_GRAFIX_ShowObject( Board2DTrade,  0, 0,
#if USA_VERSION
      LED_IFT( DAT_BOARD, BMP_mybss0001 + cameraview) + DISPLAY_2D_BOARDS_PER_CITY * max(DISPLAY_state.city,0) );
#else
      LED_IFT( DAT_LANGDIALOG, BMP_mybss01 + cameraview) );
#endif
    break;
  case DISPLAY_SCREEN_MainA:      // Buffered
    //Overlay=LED_IFT( DAT_BOARD, BMP_mybs0001 + cameraview) + DISPLAY_2D_BOARDS_PER_CITY * DISPLAY_state.city;
    if (0 > DISPLAY_state.city)
    {
      LE_GRAFIX_ShowObject(UDBoard_2DLoadedViews[bufferindex].BoardBuffer, 0,
                           0, g_aid2DBoards[cameraview]);
    }
    else
    {
      LE_GRAFIX_ShowObject(UDBoard_2DLoadedViews[bufferindex].BoardBuffer, 0,
#if USA_VERSION
                           0, LED_IFT(DAT_BOARD, BMP_mybs0001 + cameraview) + 
                           DISPLAY_2D_BOARDS_PER_CITY * max(DISPLAY_state.city, 0)
#else
                           0, LED_IFT(DAT_LANGDIALOG, BMP_mybs01 + cameraview)
#endif
                           );
    }
    break;
  }

#if USA_VERSION
  return; // No overlays ever.
#else

  LE_DATA_DataId    Overlay;
  LPNEWBITMAPHEADER	BitmapUCPHeaderPntr;


  // Overlays - text (properties) & currency
  switch( DISPLAY_state.desired2DView )
  {
  case DISPLAY_SCREEN_PortfolioA:
  case DISPLAY_SCREEN_TradeA:     //Board2DTrade will be updated
    if (DISPLAY_state.city >= 0)
    {// Dont put text on a custom board
      Overlay = LED_IFT( DAT_BOARD, TAB_mncps020001 + cameraview ) + DISPLAY_state.city * UDBOARD_CAMERA_VIEWS;
      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER) LE_DATA_UseRef( Overlay );
      LE_GRAFIX_ShowTCB_Alpha( Board2DTrade, BitmapUCPHeaderPntr->nXOriginOffset, BitmapUCPHeaderPntr->nYOriginOffset, 
        Overlay );
      LE_DATA_RemoveRef( Overlay );
    

      Overlay = LED_IFT( DAT_BOARD, TAB_mncus020001 + cameraview ) + DISPLAY_state.system * UDBOARD_CAMERA_VIEWS;
      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER) LE_DATA_UseRef( Overlay );
      LE_GRAFIX_ShowTCB_Alpha( Board2DTrade, BitmapUCPHeaderPntr->nXOriginOffset, BitmapUCPHeaderPntr->nYOriginOffset, 
        Overlay );
      LE_DATA_RemoveRef( Overlay );
    }
    break;

  case DISPLAY_SCREEN_MainA:
    if (DISPLAY_state.city >= 0)
    {// Dont put text on a custom board
      Overlay = LED_IFT( DAT_BOARD, TAB_mncp020001 + cameraview ) + DISPLAY_state.city * UDBOARD_CAMERA_VIEWS;
      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER) LE_DATA_UseRef( Overlay );
      LE_GRAFIX_ShowTCB_Alpha( UDBoard_2DLoadedViews[bufferindex].BoardBuffer, BitmapUCPHeaderPntr->nXOriginOffset, BitmapUCPHeaderPntr->nYOriginOffset, 
        Overlay );
      LE_DATA_RemoveRef( Overlay );
    

      Overlay = LED_IFT( DAT_BOARD, TAB_mncu020001 + cameraview ) + DISPLAY_state.system * UDBOARD_CAMERA_VIEWS;
      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER) LE_DATA_UseRef( Overlay );
      LE_GRAFIX_ShowTCB_Alpha( UDBoard_2DLoadedViews[bufferindex].BoardBuffer, BitmapUCPHeaderPntr->nXOriginOffset, BitmapUCPHeaderPntr->nYOriginOffset, 
        Overlay );
      LE_DATA_RemoveRef( Overlay );
    }
    UDBoard_2DLoadedViews[bufferindex].ViewLoaded = cameraview;
    UDBoard_2DLoadedViews[bufferindex].TimeLoaded = LE_TIME_TickCount;
    break;
  }

#endif // Not USA
}

/*****************************************************************************
 * UDBOARD_SwitchToBackdrop - Will switch to the requested cameraview, loading
 * it if required.  NOTE: the caller must start the object, this routine
 * maintains UDBoard_CurrentBufferDisplayed for the requested camera view.
 */
void UDBOARD_SwitchToBackdrop( int cameraview )
{
  int t, indexfind, indexoldest;
  TYPE_Tick oldest;

  switch( DISPLAY_state.desired2DView )
  {
  case DISPLAY_SCREEN_PortfolioA: //Board2DTrade will be updated
  case DISPLAY_SCREEN_TradeA:
    UDBOARD_CompileBackdrop( cameraview, 0 );
    break;
  case DISPLAY_SCREEN_MainA:      // Buffered
    oldest = UDBoard_2DLoadedViews[0].TimeLoaded;
    indexoldest = 0;
    indexfind = -1;
    
    for( t = 0; t < UDBoard_2D_constructedBoardBufferMax; t++ )
    {// Search for everything we might need - the backdrop if already loaded, the oldest if we need to load.
      if( oldest > UDBoard_2DLoadedViews[t].TimeLoaded )
      {
        oldest = UDBoard_2DLoadedViews[t].TimeLoaded;
        indexoldest = t;
      }
      if( cameraview == UDBoard_2DLoadedViews[t].ViewLoaded )
        indexfind = t;
    }
    
    if( indexfind == -1 )
    {
      UDBOARD_CompileBackdrop( cameraview, indexoldest );
      UDBoard_CurrentBufferDisplayed = indexoldest;
    } else
      UDBoard_CurrentBufferDisplayed = indexfind;
    break;
  }
}

/*****************************************************************************
 * UDBOARD_PreLoadBackdrop - If we know we will need some fast camera 
 * switches, this will prebuffer them.  Which are dropped is based on time -
 * prebuffer too many and you lose the earlier ones.
 */
void UDBOARD_PreLoadBackdrop( int cameraview )
{
  int t, indexfind, indexoldest;
  TYPE_Tick oldest;

  oldest = UDBoard_2DLoadedViews[0].TimeLoaded;
  indexoldest = 0;
  indexfind = -1;

  for( t = 0; t < UDBoard_2D_constructedBoardBufferMax; t++ )
  {// Search for everything we might need - the backdrop if already loaded, the oldest if we need to load.
    if( oldest > UDBoard_2DLoadedViews[t].TimeLoaded )
    {
      oldest = UDBoard_2DLoadedViews[t].TimeLoaded;
      indexoldest = t;
    }
    if( cameraview == UDBoard_2DLoadedViews[t].ViewLoaded )
      indexfind = t;
  }

  if( indexfind == -1 )
  {
    UDBOARD_CompileBackdrop( cameraview, indexoldest );
  }
}

/*****************************************************************************
 * UDBOARD_SetCameraToPresetPostion - 3D camera to indicated board view
 */
void UDBOARD_SetCameraToPresetPostion( int cameraIndex )
{
  BOOL forceCameraInterrupt = TRUE;
  Object_3D_Interpolation_data *cameraToSet;
  
  // 2D - Camera override MUST REGISTER NOW
  if( DISPLAY_state.board3Don && DISPLAY_state.game3Don )
  //if( !DISPLAY_state.game3Don )
    forceCameraInterrupt = FALSE;

  if( forceCameraInterrupt )
  {// Forget anything waiting, do the camera move
    cameraToSet = &CameraInterpolationData;
    CameraMoveWaiting = FALSE;
  }
  else
  {// Register a waiting move.  It will start immediately if no other move is active.
    cameraToSet = &CameraWaitingData;
    CameraMoveWaiting = TRUE;
  }
  
  // Set the start position to the current position.  These will be reset if we stacked the move to the correct position at that time.
  cameraToSet->StartLocation = cameraToSet->InterLocation;
  cameraToSet->StartForwardV = cameraToSet->InterForwardV;
  cameraToSet->StartUpwardsV = cameraToSet->InterUpwardsV;
  
  // Record the end position
  cameraToSet->End__Location = D3DVECTOR( CameraAngles2D[cameraIndex][0], CameraAngles2D[cameraIndex][1], CameraAngles2D[cameraIndex][2] );
  cameraToSet->End__ForwardV = D3DVECTOR( CameraAngles2D[cameraIndex][3], CameraAngles2D[cameraIndex][4], CameraAngles2D[cameraIndex][5] );
  cameraToSet->End__UpwardsV = D3DVECTOR( CameraAngles2D[cameraIndex][6], CameraAngles2D[cameraIndex][7], CameraAngles2D[cameraIndex][8] );
  
  // Specify the transition type & time
  //cameraToSet->InterpolationFXDesired = INTERPOLATION_ACCELERATE_IN_AND_OUT_SIN;
  cameraToSet->InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
  cameraToSet->InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
  cameraToSet->InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;
  cameraToSet->StartTime = cameraToSet->CurrentTime = LE_TIME_TickCount;
  cameraToSet->ElapsedTimeRatio = -1.0f; // Flags the elapsed time is to be calculated.
  
  // The timer will be moved forward to the start time if the camera move waits for an existing one to finish.
  if( forceCameraInterrupt )
  {// Process this puppy now, we need the camera move in this collect commands.
    cameraToSet->EndTime = LE_TIME_TickCount; // Instant move.
    InterpolationFX_3DPositionAndOrientation( &CameraInterpolationData );
    // Use sequencer command for synchronisation with scene changes.
    LE_SEQNCR_SetCamera3D( &CameraInterpolationData.InterLocation,
      &CameraInterpolationData.InterForwardV,
      &CameraInterpolationData.InterUpwardsV,
      (float) (45.0 / 180.0 * 3.1415926) /* 45 degree standard camera field of view setting used by artists in Max, so it matches 2D board */,
      CLIPPING_PLANE_NEAR, CLIPPING_PLANE_FAR, 1 );
  }
  else
    cameraToSet->EndTime = LE_TIME_TickCount + CAMERA_BASE_MOVE_TIME; // Transition time in 60ths of a second.
};


/*****************************************************************************
 * UDBOARD_ActivateDesiredCamera - Does what it takes to activate a camera change.
 */
void UDBOARD_ActivateDesiredCamera( int cameraIndex )
{
  if( DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands )
  {// Check for a timeout
    if( DISPLAY_state.desiredCameraCustomMoveStartTime + DISPLAY_CUSTOM_MOVE_MAX_SECONDS * 60 < LE_TIME_TickCount )
      DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands = FALSE; // Clear the lock.
    else
      return; // Ignore the request - someone 'smarter' is having fun.
  }
  //if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA && !DISPLAY_state.game3Don )
  if( DISPLAY_IsBoardVisible && !DISPLAY_state.game3Don )
  { // 2D stuff
    UDBOARD_SwitchToBackdrop( DISPLAY_state.desired2DCamera ); // 2d construction
    switch( DISPLAY_state.desired2DView )
    {
    case DISPLAY_SCREEN_PortfolioA:
    case DISPLAY_SCREEN_TradeA:
      DISPLAY_state.currentBackdropID = Board2DTrade;
      break;
    case DISPLAY_SCREEN_MainA: // Buffered
      DISPLAY_state.currentBackdropID = UDBoard_2DLoadedViews[UDBoard_CurrentBufferDisplayed].BoardBuffer;
      break;
    }
  }
  UDBOARD_SetCameraToPresetPostion( cameraIndex );

  // Bright lights from far away, dim them if camera lights are close to board.
  //D3DDevice* p3DDevice = pc3D::Get3DDevice();
  //if( cameraIndex <= VIEW2D03_TOPDOWN_STARWARS )
  //  p3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE( 225, 225, 225, 220));
  //else
  //  p3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE( 150, 150, 150, 220));

}

/*****************************************************************************
 * UDBOARD_SelectAppropriateView - It is better to have camera selection logic in one place.
 * Use this function rather than coding selection logic in bits everywhere.
 * Variable can have different functions for different viewtypes - see below for notes.
 */
int UDBOARD_SelectAppropriateView( VIEW_SELECTION_TYPES viewtype, int variable )
{
  int returnvalue = DISPLAY_state.desired2DCamera, t;

  if(variable <= 0 ) variable = 1;

  switch (viewtype)
  {
  case VIEW_ROLLDICE: // Variable not used
    // Change to a full board view.  Rarely (variable) if not already in one.
    //if( !(returnvalue >= VIEW2D01_TOPDOWN_SQUARE && returnvalue <= VIEW2D03_TOPDOWN_STARWARS ) || (rand()%variable) == 0 )
    //  returnvalue = VIEW2D01_TOPDOWN_SQUARE + rand()%3;

    // Look ahead 15 view from the current players starting square.
    t = UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare; // Square to look forward from.
    if( t > 39 ) t = 10;  // In jail or off board (which shouldn't roll anyway).
    returnvalue = 3 * ( (int)(t/10) ); // start at correct side
    t = t % 10; t--; if (t < 0 ) t = 0; t = (int)(t / 3); // 0 - 3 = 0, 4-6 = 1, 7-9 = 2.
    returnvalue += t;
    returnvalue += VIEW2D28_15TILES01;// Offset to 15 forward views
    break;
  case VIEW_JAILCHOICE: // Variable not used.
    returnvalue = VIEW2D17_CORNER_JAIL; break;
    /*if( !(returnvalue == VIEW2D02_TOPDOWN_SOCCER && returnvalue != VIEW2D17_CORNER_JAIL && returnvalue != VIEW2D31_15TILES04 ) || (rand()%variable) == 0 )
    {
      switch( rand()%3 )
      {
      case 0: returnvalue = VIEW2D02_TOPDOWN_SOCCER; break;
      case 1: case 2: 
      //case 2: returnvalue = VIEW2D31_15TILES04; break;
      }
    }*/
    break;
      
  }

  return returnvalue;
};


/*****************************************************************************
* Calculate rotation and position for highight/mortgage hightlight of the tile
*/
void UDBOARD_GetHighlightPosition(
  int nBoardTile,
  float *lpfXCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle)
{
  //RECT adjustRects[4][2] = { { -20, -10 } , { 5, 5 }, { 276, 140 }, { 276, 140 } };
  int aHor = -19, aVert = -8; // From the first board side.  This may need to be adjusted for some boards.

  if (nBoardTile == SQ_IN_JAIL)
    nBoardTile = SQ_JUST_VISITING;
  if (nBoardTile < SQ_JUST_VISITING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + aVert;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + aHor;
    *lpfYAngle = 0.0f;
  }
  else if (nBoardTile < SQ_FREE_PARKING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + aHor;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - aVert;
    *lpfYAngle = (float)(pi / 2.0);
  }
  else if (nBoardTile < SQ_GO_TO_JAIL)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - aVert;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - aHor;
    *lpfYAngle = (float)pi;
  }
  else
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - aHor;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + aVert;
    *lpfYAngle = -(float)(pi / 2.0);
  }
}


