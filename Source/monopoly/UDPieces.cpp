/*****************************************************************************
 *
 * FILE:        UDPIECES.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to
 *    displaying and animating Tokens, dice, houses & hotels.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/

#include "gameinc.h"

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
/*enum 
{
  OFFSETS_PROPERTY = 0,
  OFFSETS_RR_UT_CH,
  OFFSETS_GO_FP,
  OFFSETS_INJAIL,
  OFFSETS_JUSTVIS
};*/
// Track id's during jail anims
LE_DATA_DataId   PaddyAnimation = LED_EI, TokenAnimation = LED_EI;

enum 
{
  OFFSETS_GO_FP = 0,
  OFFSETS_INJAIL,
  OFFSETS_JUSTVIS,
  OFFSETS_PROPERTY,
  OFFSETS_RR_UT_CH
};
int UDPIECES_RESTING_IDLE_OFFSETS[5][6][3] = // Category, idle location #, offsets
{//  x     z   rotation(Car, shoe, barrow, boat, dog & cannon).
/*  {{  7,  22, 135 },//go/free parking
  { -34,  22, 135 },
  {   6, -23,  45 },
  { -14,  22, 135 },
  { -38,   0,  45 },
  { -40, -24,  45 }},

  {{  9, -25, -45 },//in jail
  {  -4, -24, -28 },
  {   9, -13, -60 },
  {  -6, -14,  50 },
  { -10, -25,   0 },
  {   9, -10, -90 }},

  {{  7,  22, 135 },//just visiting
  { -26,  22, 135 },
  { -38, -10,  45 },
  { -38,  11, 135 },
  {  -9,  22, 135 },
  { -38,  27,  45 }},*/

  {{-22,   7,  45 },//go/free parking
  { -22, -34,  45 },
  {  23,   6, -45 },
  { -22, -14,  45 },
  {   0, -38, -45 },
  {  24, -40, -45 }},

  {{  9, -25, -45 - 90 },//in jail
  {  -4, -24, -28 - 90 },
  {   9, -13, -60 - 90 },
  {  -6, -14,  50 - 90 },
  { -10, -25,   0 - 90 },
  {   9, -10, -90 - 90 }},

  {{  7,  22, 135 - 90 },//just visiting
  { -26,  22, 135 - 90 },
  { -38, -10,  45 - 90 },
  { -38,  11, 135 - 90 },
  {  -9,  22, 135 - 90 },
  { -38,  27,  45 - 90 }},

  {{-21,  10,  90 },//Property tile
  {  15, -10, -45 },
  {  15,  10, -45 },
  { -21, -10,  90 },
  { -25,   0,   0 },
  { -29, -10,   0 }},

  {{-20,  10,  90 },//Railroad tile
  {  20, -10, -90 },
  {  20,  10, -90 },
  { -20, -10,  90 },
  {  30,   0,   0 },
  { -15,   0,   0 }}

};
 

int UDPEICES_PROPERTY_TYPE_FOR_RESTING[41] =
{// Quick lookup table to identify the type of idle jump (to use the right one for the property type)
  OFFSETS_GO_FP,    OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_RR_UT_CH,
  OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_PROPERTY,
  OFFSETS_JUSTVIS,  OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_PROPERTY,
  OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_PROPERTY,
  OFFSETS_GO_FP,    OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_PROPERTY,
  OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY,
  OFFSETS_GO_FP,    OFFSETS_PROPERTY, OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY,
  OFFSETS_RR_UT_CH, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY, OFFSETS_RR_UT_CH, OFFSETS_PROPERTY,
  OFFSETS_INJAIL
};


/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.

 * No code other than the show module may start nor stop sound
 * and graphic items.  Fields indicating desired actions can be filled in
 * in the display state which the show routine will pick up.  In this way,
 * the show routine can take responsibility for tracking all data
 * required to pause or clear the screen at any time.

 * There will be no locking the computer as a wait for all sequences to finish
 * might do.  Stack processing can be halted until sequences are finished,
 * but you will have to use a timer to poll your sequence status.  This is to
 * allow processing of pause buttons or user camera controls at all times, as
 * well as to make programmers lives more difficult :)
 */


MESHType **tmpnt;//, myMesh;
void DISPLAY_UDPIECES_Initialize( void )
{ // Initialize any permanent display objects.  Avoid temporary ones when possible.
  int t, s;

  // Dice
  DISPLAY_state.QueueLockedForDiceAnim = FALSE;
  DISPLAY_state.DiceCameraSetTime = 0;
  DISPLAY_state.CurrentDiceID[0] = LED_EI;
  DISPLAY_state.CurrentDiceID[1] = LED_EI;
  DISPLAY_state.RollDiceAnimDesired = FALSE;
  DISPLAY_state.CurrentBobDice = LED_EI;
  DISPLAY_state.Current3DDiceAnim = LED_EI;

  // Houses
  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    DISPLAY_state.HighlightDataIDs[t] = NULL;
    DISPLAY_state.HotelFlag[t] = FALSE;
    for( s = 0; s < 4; s++ )
    {
      DISPLAY_state.HouseFlags[t][s] = FALSE;
      DISPLAY_state.HousingDataIDs[t][s] = NULL;
   }
  }

  // Tokens
  DISPLAY_state.FlashCurrentToken = FALSE; // Not used (is a hotseat feature)


  s = 0;
  SETTIMBANK( s );

  // Board & 3D object loading.

  //demo texture hack - different TIMS per token.  FIXME if and when req'd.

  DISPLAY_state.VirtualTimPageForAnythingNotPreloaded = 0;  // Initialize it before asking the Board to load, it is set correctly at the end of this routine.
  
  if( DISPLAY_state.game3Don )
    UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardmed ), FALSE );
  else
    CurrentBoard = LED_EI;

  s = 1;
  SETTIMBANK( s );


#if CE_ARTLIB_3DUsingOldFrame

  // Load all of the tokens
  for( t = HMD_bag01; t <= HMD_wbr04; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));
  
  // Load the mortgage indicators, house and hotel models
  SETTIMBANK( ++s );
  for( t = HMD_mnmi1; t <= HMD_mnoi6; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));
  for( t = HMD_hotel; t <= HMD_house; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));

  SETTIMBANK( ++s );
  for( t = HMD_dice01; t <= HMD_dice06; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));

#else // CE_ARTLIB_3DUsingOldFrame

  // Load all of the tokens
  Mesh_EnableTexturePurge(FALSE);
  for( t = HMD_bag01; t <= HMD_wbr04; t++ )
  {
    // Load the mesh
    tmpnt = (MESHType**)LE_DATA_UseRef( LED_IFT( DAT_3D, t ));
    if (HMD_ashadow <= t && HMD_kshadow >= t)
    {
      // Set the ZBias and blending factors for the mesh
      (*tmpnt)->SetZBias(2);
      (*tmpnt)->SetBlendingFactors(D3DBLEND_ZERO, D3DBLEND_SRCALPHA);

      // For now, only do the conversion if 8bit palettized textures are
      //  supported
      //if (DISPLAY_state.supports8BitPalTextures)
      {
        // Grab the mesh group from the mesh (should only be one)
        ASSERT(1 == (*tmpnt)->GetNumMeshGroups());
        MeshGroup* pGroup = (*tmpnt)->GetMeshGroup(0);

        // Get the texture from the mesh group
        TextureID* pTextureID = pGroup->GetTexture();
        ASSERT(NULL != pTextureID);

        // Convert the texture to a shadow
        VERIFY(UDUTILS_ConvertToShadow(pTextureID->pTexture));
      }
    }
    else
      (*tmpnt)->SetZBias(3);
  }
  Mesh_PurgeTextures();
  Mesh_EnableTexturePurge(TRUE);

  // Load the mortgage indicators, house and hotel models
  SETTIMBANK( ++s );
  Mesh_EnableTexturePurge(FALSE);
  for( t = HMD_mnmi1; t <= HMD_mnoi6; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));
  for( t = HMD_hotel; t <= HMD_house; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));
  Mesh_PurgeTextures();
  Mesh_EnableTexturePurge(TRUE);

  SETTIMBANK( ++s );

  // Load the dice
  Mesh_EnableTexturePurge(FALSE);
  for( t = HMD_dice01; t <= HMD_dice06; t++ )
    LE_DATA_UseRef( LED_IFT( DAT_3D, t ));
  Mesh_PurgeTextures();
  Mesh_EnableTexturePurge(TRUE);

#endif // CE_ARTLIB_3DUsingOldFrame

  SETTIMBANK( ++s ); // For anything we missed.
  DISPLAY_state.VirtualTimPageForAnythingNotPreloaded = s;
}

// Deallocate stuff from Initialize.
void DISPLAY_UDPIECES_Destroy( void )
{
  int t;

  return;  // unloads just whine.
  VERIFY(LE_DATA_UnloadRef( CurrentBoard ) );

/*  for (int t = HMD_car01; t <= HMD_dogk08; t++)
  {
    if (t == HMD_tk01_cannon)
      t = HMD_shu01;

    VERIFY(LE_DATA_UnloadRef(LED_IFT(DAT_3D, t)));
  }

  for (t = HMD_mnmi1; t <= HMD_mnoi6; t++)
    VERIFY(LE_DATA_UnloadRef(LED_IFT(DAT_3D, t)));*/
  for( t = HMD_bag01; t <= HMD_wbr04; t++ )//HMD_thimb05; t++ )
  {
    /*if( t == HMD_mnmi1  ) t = HMD_shu01; // Skip non-token hmd's
    if( t == HMD_dice01 ) t = HMD_dog01;
    if( t == HMD_boat01 )
    {// hack for barrow - include wheel stuff and return.
      for( int z = HMD_wheelb; z <= HMD_wbr04; z++ )
        LE_DATA_UseRef( LED_IFT( DAT_3D, z ));
    }

    if( t == HMD_bar01 || t == HMD_boat01 || t == HMD_can01 || t == HMD_car01 || t == HMD_dog01 ||
      t == HMD_hat01 || t == HMD_horse01 || t == HMD_iron01 || t == HMD_shu01 || t == HMD_thimb01 )
      HMD_SetCurrentTimBank( ++s );*/
    VERIFY(LE_DATA_UnloadRef( LED_IFT( DAT_3D, t )));
  }

  for( t = HMD_mnmi1; t <= HMD_mnoi6; t++ )
    VERIFY(LE_DATA_UnloadRef( LED_IFT( DAT_3D, t )));
  for( t = HMD_hotel; t <= HMD_house; t++ )
    VERIFY(LE_DATA_UnloadRef( LED_IFT( DAT_3D, t )));

  for( t = HMD_dice01; t <= HMD_dice06; t++ )
    VERIFY(LE_DATA_UnloadRef( LED_IFT( DAT_3D, t )));
}


void DISPLAY_UDPIECES_TickActions( int numberOfTicks )
{// 60ths of a second, this is called before show to allow programmatic movement control
}


void DISPLAY_UDPIECES_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything is in the user interface and display states.
  register int s,t;
  LE_DATA_DataId tempID;

  // Update the dice (2d, on IBAR)
  DISPLAY_state.RollDiceAnimDesired = (DISPLAY_state.IBarCurrentState == IBAR_STATE_StartTurn ) && ( DISPLAY_IsIBarVisible );
  for( t = 0; t <= 1; t++ )
  {
    tempID = LED_EI;
    if( (UICurrentGameState.Dice[t]) >= 1 &&
      (UICurrentGameState.Dice[t] <= 6) &&
      (!DISPLAY_state.RollDiceAnimDesired) && // We will want bobbing dice here.
      (DISPLAY_IsIBarVisible) )
    {
      if( t == 0 )
        //tempID = LED_IFT(DAT_MAIN, CNK_ds0s0v1) + UICurrentGameState.Dice[t] -1;
        tempID = LED_IFT(DAT_MAIN, CNK_tmpdc1) + UICurrentGameState.Dice[t] -1;
      else
        tempID = LED_IFT(DAT_MAIN, CNK_tmpdc1) + UICurrentGameState.Dice[t] -1;
    }
    if( DISPLAY_state.CurrentDiceID[t] != tempID || DISPLAY_state.DiceRollNotification )
    {// Corrective action
      if( DISPLAY_state.CurrentDiceID[t] != LED_EI )
      {
        LE_SEQNCR_Stop( DISPLAY_state.CurrentDiceID[t], DISPLAY_IBAR_GeneralPriority + t );
        DISPLAY_state.CurrentDiceID[t] = LED_EI;
      }
      DISPLAY_state.CurrentDiceID[t] = tempID;
      if( DISPLAY_state.CurrentDiceID[t] != LED_EI )
      {// Fire up the new one
        //LE_SEQNCR_StartXY( DISPLAY_state.CurrentDiceID[t], DISPLAY_IBAR_GeneralPriority, 362, 84 );
        LE_SEQNCR_StartXY( DISPLAY_state.CurrentDiceID[t], DISPLAY_IBAR_GeneralPriority + t, t * 24 - 35, 0 );
      }
    }
    DISPLAY_state.DiceRollNotification = FALSE;
  }
  if( DISPLAY_state.RollDiceAnimDesired )
  {// Regular dice will have been shut down, put up bobbing dice.
    tempID = LED_IFT(DAT_MAIN, CNK_tmpdcbob);
  } else
    tempID = LED_EI;
  if( DISPLAY_state.CurrentBobDice != tempID )
  {
    if( DISPLAY_state.CurrentBobDice != LED_EI )
    {
      LE_SEQNCR_Stop( DISPLAY_state.CurrentBobDice, DISPLAY_IBAR_GeneralPriority );
      LE_SEQNCR_Stop( DISPLAY_state.CurrentBobDice, DISPLAY_IBAR_GeneralPriority + 1 );
    }
    DISPLAY_state.CurrentBobDice = tempID;
    if( DISPLAY_state.CurrentBobDice != LED_EI )
    {
      LE_SEQNCR_StartXY( DISPLAY_state.CurrentBobDice, DISPLAY_IBAR_GeneralPriority, -35, 0 );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.CurrentBobDice, DISPLAY_IBAR_GeneralPriority,
        LE_SEQNCR_EndingActionLoopToBeginning );
      LE_SEQNCR_StartXYDrop( DISPLAY_state.CurrentBobDice, DISPLAY_IBAR_GeneralPriority + 1, 24 - 35, 0, LE_SEQNCR_DropDropFrames );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.CurrentBobDice, DISPLAY_IBAR_GeneralPriority + 1,
        LE_SEQNCR_EndingActionLoopToBeginning );
    }
  }

  // Update the dice (3d), a game queue lock is involved.
  tempID = LED_EI;

  //LE_SEQNCR_RuntimeInfoRecord     InfoGeneric;
  if( DISPLAY_state.QueueLockedForDiceAnim )// We are animating a dice roll during a system lock
  { // There is a game queue lock after the roll notification, we must clear it with or without showing an animation.
    static BOOL rollAnnounced;
    if( DISPLAY_state.DiceCameraSetTime != DISPLAY_state.QueueLockTime )
    { // Start the dice cam movement - we glide toward center board
      DISPLAY_state.DiceCameraSetTime = DISPLAY_state.QueueLockTime;
      if( DISPLAY_state.board3Don && DISPLAY_state.game3Don)
      {// on a 2D board, no go.
        float moveratioA, moveratioB;
        // Calculate from the end location as the camera move will 'chain' to it.
        CameraWaitingData.StartLocation = CameraInterpolationData.End__Location;
        CameraWaitingData.StartForwardV = Normalize(CameraInterpolationData.End__ForwardV);
        CameraWaitingData.StartUpwardsV = CameraInterpolationData.End__UpwardsV;
        moveratioA = 0.30f + (float)(rand()%14)/100; // Ratio moved toward final point - 1.0 would mean go to destination (extreme camera - go softer - ~0.5)
        moveratioB = 1.0f - moveratioA;
        CameraWaitingData.End__Location.x = CameraWaitingData.StartLocation.x*moveratioB + 240*moveratioA;
        CameraWaitingData.End__Location.y = CameraWaitingData.StartLocation.y*moveratioB + 300*moveratioA;// FIXME We should let the camera get to destination before doing this move to 200, for now 300 will lift us a bit when the camera is stopped in the middle of the board.
        CameraWaitingData.End__Location.z = CameraWaitingData.StartLocation.z*moveratioB + 240*moveratioA;
        // Pointing 'below' the board will have the effect of lowering the camera a bit, which should give a more central view.
        CameraWaitingData.End__ForwardV = Normalize(D3DVECTOR( 243, -10, 243 ) - CameraWaitingData.End__Location);
        CameraWaitingData.End__UpwardsV = D3DVECTOR( 0, 1, 0 );

        CameraWaitingData.ElapsedTimeRatio = -1; // Means use the clock rather than preset values.
        CameraWaitingData.StartTime = LE_TIME_TickCount;
        CameraWaitingData.EndTime = LE_TIME_TickCount + CAMERA_BASE_MOVE_TIME; // 60ths of a second to move.
        //CameraWaitingData.InterpolationFXDesired = INTERPOLATION_ACCELERATE_IN_AND_OUT_SIN;
        CameraWaitingData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
        CameraWaitingData.InterpolationFXCurveType = INTERPOLATION_STRAIGHT_LINE;
        CameraWaitingData.InterpolationFXDirection = INTERPOLATION_DIRECTION_INTERPOLATED;
        
        CameraMoveWaiting = TRUE;
      }
      DISPLAY_state.desiredCameraInvalidatedLock = TRUE; // Let camera control know we are taking over.
      DISPLAY_state.desiredCameraClearToValidate = FALSE;
      rollAnnounced = FALSE;
    }

    if( (UICurrentGameState.Dice[0] >= 1) &&
      (UICurrentGameState.Dice[0] <= 6) &&
      (UICurrentGameState.Dice[1] >= 1) &&
      (UICurrentGameState.Dice[1] <= 6) )
    {// There is a valid dice roll, an animation is required.
      tempID = LED_IFT( DAT_3D, CNK_mnd11a) + (UICurrentGameState.Dice[0]-1)*6*2 + (UICurrentGameState.Dice[1]-1)*2 + 0*(rand()%2);
    }
    // kill the old sequence
    if( tempID != DISPLAY_state.Current3DDiceAnim )
    {
      if( DISPLAY_state.Current3DDiceAnim )
        LE_SEQNCR_Stop( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority );
      DISPLAY_state.Current3DDiceAnim = LED_EI;
    }

    // Allow a short delay before starting the animation
    if( DISPLAY_state.QueueLockTime + 35 < LE_TIME_TickCount ) // 60ths if a second delay before starting animation.
    {
      if( tempID != DISPLAY_state.Current3DDiceAnim )
      { // We know current is null from above, we are starting the dice roll
        DISPLAY_state.Current3DDiceAnim = tempID;
        LE_SEQNCR_StartRySTxzDrop( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority,
          0.0f, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority,
          LE_SEQNCR_EndingActionStayAtEnd );
      }
    }

    // Now we look for any excuse to end this lock state - end of sequence or invalid screen for showing dice.
    //if(LE_SEQNCR_GetInfo( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority, FALSE,
    //&InfoGeneric, NULL, NULL, NULL, NULL ) )
    // // This logic terminates the lock after the sequence - we are aiming for a little longer generally.
    //if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
    // // This is the expected way to exit.  We get here after the anim is started and finished.
    //DISPLAY_state.QueueLockedForDiceAnim = FALSE; // FLAG release (before leaving code block)
    //
    if( (DISPLAY_state.QueueLockTime + 90 < LE_TIME_TickCount) && !DISPLAY_state.desiredCameraClearToValidate ) // 60ths if a second before freeing camera
    {
      DISPLAY_state.desiredCameraClearToValidate = TRUE; // Release control, camera functions will fix up the camera.
      if( !rollAnnounced )
      {// Somehow, this is causeing trouble hence it's own flag.
        UDPENNY_AnnounceRoll( UICurrentGameState.Dice[0] + UICurrentGameState.Dice[1] );// Good time to announce roll
        rollAnnounced = TRUE;
      }
    }
    if( DISPLAY_state.QueueLockTime + 120 < LE_TIME_TickCount || !DISPLAY_IsIBarVisible ) //  lock & EXIT CONDITION
    //if( !DISPLAY_state.QueueLockedForDiceAnim )
    { // We are done.
      if( DISPLAY_state.Current3DDiceAnim != NULL )
      {
        DISPLAY_state.QueueLockedForDiceAnim = FALSE; // release
        LE_SEQNCR_Stop( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority );
        DISPLAY_state.Current3DDiceAnim = NULL;
      }
      gameQueueUnLock();
    }
  } //else - alternate has its own test to avoid a blank frame switching from the anim to the idle.
  if( !DISPLAY_state.QueueLockedForDiceAnim ) // No anim, pop the dice up.
  { // We just want idle dice
    if( (UICurrentGameState.Dice[0] >= 1) &&
      (UICurrentGameState.Dice[0] <= 6) &&
      (UICurrentGameState.Dice[1] >= 1) &&
      (UICurrentGameState.Dice[1] <= 6) &&
      DISPLAY_IsBoardVisible)
    {// There is a valid dice roll
      tempID = LED_IFT( DAT_3D, CNK_mndi11a) + (UICurrentGameState.Dice[0]-1)*6*2 + (UICurrentGameState.Dice[1]-1)*2 + 0*(rand()%2);
    }
    // If there is a change, kill the old sequence
    if( tempID != DISPLAY_state.Current3DDiceAnim )
    {
      if( DISPLAY_state.Current3DDiceAnim )
        LE_SEQNCR_Stop( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority );
      DISPLAY_state.Current3DDiceAnim = tempID;
      if( DISPLAY_state.Current3DDiceAnim )
      {
        LE_SEQNCR_StartRySTxzDrop( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority,
          0.0f, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( DISPLAY_state.Current3DDiceAnim, DISPLAY_Generic3dPriority,
          LE_SEQNCR_EndingActionStayAtEnd );
      }
    }
  }// End if 3D dice


  // Update shadows PRIOR to issuing/discontinuing sequences (so they have time to start).
  // This causes shadows to be one tick behind - we would have to do this on the sequencer side of the game loop to fix it.
  UDPIECES_UpdateShadows();

  // Update the 3D token animation (if running)
  if( DISPLAY_state.TokenAnimStackIndex > -1 )
  { // A token animation is playing, presumably with the rules engine processing locked.
    BOOL AnimEscalate = FALSE;
    LE_SEQNCR_RuntimeInfoRecord      InfoGeneric;
    int index;
    float fXCoordinate, fYCoordinate, fZCoordinate, fYAngle;
    static int CustomCameraAnimStackIndex; // Tracks custom cams issued with token anims.
    static int camEffect;
    int   detectedRemainingTime; // -1 indicates not detected (seq not running)


    // Custom camera worthwhile?
    if(  DISPLAY_state.TokenAnimStackIndex == 0 && DISPLAY_state.game3Don )
    {// We are just starting the animation - decide if we want a custom cam.
      // NOTE: We will not use the 'dirty cam' stuff - we will leave the camera at or moving to the right location.
      BOOL  useCustomCam = FALSE;
      int   distance;

      distance = DISPLAY_state.TokenAnimStack[DISPLAY_state.TokenAnimStackIndex].SquareLandingNewSequence - DISPLAY_state.TokenAnimStack[DISPLAY_state.TokenAnimStackIndex].SquareStartingNewSequence;
      if( distance < 0 ) distance += 40; // Passed go.
      //if( rand()%1 == 0 )
      // This effect causes problems for longer animations - mostly that when the camera is released an old desired camera starts late, pretty animated.
      // See if we are on the last leg of animation, do a swoop.
      if( distance > 5 && (DISPLAY_state.TokenAnimStackIndex == DISPLAY_state.TokenAnimStackTop -1) && rand()%2 == 0 ) // Corners don't go that far, this is a long run along a side.
      {//Expand on this a bit - anything good, mark it so it can be processed (parameters in anim stack for this?)
        camEffect = TOKEN_CAM_EFFECT_FirstSeqSwoopPAT;
        useCustomCam = TRUE;
      }

      if( useCustomCam )
      {// Just turn it on, logic below does the rest.
        DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands = TRUE; 
        DISPLAY_state.desiredCameraCustomMoveStartTime = LE_TIME_TickCount;
        CustomCameraAnimStackIndex = -1; // 0th not issued.
      }
    }

    // Process the anim stack
    if( !DISPLAY_IsBoardVisible )
    {// Kill it fast - we'll do it with trickery - setting the status to done.
      AnimEscalate = FALSE; // Dont let that logic run
      DISPLAY_state.TokenAnimStackIndex = DISPLAY_state.TokenAnimStackTop + 1; // Sequence close flag.
    } else
    {
      // Move sequences/camera switches along
      if( DISPLAY_state.TokenAnimStackIndex == 0 )
      {// Just starting.
        AnimEscalate = TRUE;
      } else
      {// Anim is playing, see if it has finished - may27 - x26 big dog on screen, long endtime 499602d2.  Was car landing on pacific
        //july 7 - big cannon.
        AnimEscalate = TRUE;
        detectedRemainingTime = -1;
        if( DISPLAY_state.TokenCurrent3DSequence )
          if(LE_SEQNCR_GetInfo( DISPLAY_state.TokenCurrent3DSequence, DISPLAY_Generic3dPriority, FALSE,
            &InfoGeneric, NULL, NULL, NULL, NULL ) )
          {
            detectedRemainingTime = InfoGeneric.endTime - InfoGeneric.sequenceClock;
            if( InfoGeneric.endTime > InfoGeneric.sequenceClock )
            {
              if( InfoGeneric.endTime > 60 * 12 )
              {
                InfoGeneric.endTime = 60 * 12; // Debug catch - nothing should ever be this big.
#if CE_ARTLIB_EnableDebugMode
                wsprintf(LE_ERROR_DebugMessageBuffer, "UDPieces - animation too long");
                LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, 1);
#endif
              }
              AnimEscalate = FALSE;
            }
          }
      }
    }

    // Check for camera move only items.  Index over any of them if AnimEscalate is true - the anim has a camera move which is 'after' these.
    // Do not index over stacktop - if a final camera move is there, do it.
    while( AnimEscalate &&
      DISPLAY_state.TokenAnimStackIndex < DISPLAY_state.TokenAnimStackTop &&
      DISPLAY_state.TokenAnimStack[DISPLAY_state.TokenAnimStackIndex].isCameraMoveOnly )
        DISPLAY_state.TokenAnimStackIndex++; // this would indicate a bad time index - longer than the running sequence.

    // Finally, escalate if it is time to.
    if( AnimEscalate )
    { // Fire up the next sequence.  Any camera only items will have been popped, except possibly a final camera move.
      index = DISPLAY_state.TokenAnimStackIndex;
      if( !DISPLAY_state.TokenAnimStack[index].isCameraMoveOnly )
      { // is a sequence item
        if( DISPLAY_state.TokenCurrent3DSequence )
        {
          LE_SEQNCR_Stop( DISPLAY_state.TokenCurrent3DSequence, DISPLAY_Generic3dPriority );
          DISPLAY_state.TokenCurrent3DSequence = NULL;
        }
        DISPLAY_state.TokenCurrent3DSequence = DISPLAY_state.TokenAnimStack[index].NewSequenceID;
        if( DISPLAY_state.TokenCurrent3DSequence == NULL )
          DISPLAY_state.TokenCurrent3DSequence = 0;// debug catch - bad sequence obviously.
        else
        {
          // Check if we pass go - if so start the sound.
          if( DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence >= 30 &&  //Not too far ahead of go (like baltic)
            DISPLAY_state.TokenAnimStack[index].SquareLandingNewSequence <= 9 )       //We land on go or probably just corner, 2 would do.
          {// One more test - if we landed on go the line is handled elsewhere.
            if( UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare == 0 )
            {// We land on GO
              TRACE( "Landed on go\n" );
              
            } else
            {
              TRACE( "Passed go\n\n" );
              UDPENNY_PassedGo(); // Land on square handles this if we land on go.
            }
          }


          UDPIECES_GetTokenOrientation(DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence,
            &fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
          // Special case - a corner rotation (CX0) should be considered the previous side for rotation (as it comes around to the next side) - spin it back.
          if( DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence % 10 == 0 &&
            (DISPLAY_state.TokenAnimStack[index].NewSequenceID - LED_IFT( DAT_3D, CNK_knacx0 )) % ( CNK_knbcx0 - CNK_knacx0) == 0 )
          {// Back it up, angle plus offsets
            //TRACE("CX0 adjustments, corner %d\n", (int)(DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence / 10) );
            fYAngle -= (float)pi/2;
            switch( (int)(DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence / 10) )
            {
            /* 3: // go to Jail
              fXCoordinate += 15;
              fZCoordinate -= 16;
              break;
            case 2: // Free parking
              fXCoordinate -= 15;
              fZCoordinate += 16;
              break;
            case 1: // jail
              fXCoordinate -= 16;
              fZCoordinate -= 15;
              break;
            default: //go, 0 or 4.
              fXCoordinate -= 16;
              fZCoordinate -= 15;
              break;*/
            case 3: // go to Jail
              fXCoordinate += 15;
              fZCoordinate += 16;
              break;
            case 2: // Free parking
              fXCoordinate -= 15;
              fZCoordinate += 16;
              break;
            case 1: // jail
              fXCoordinate -= 16;
              fZCoordinate -= 15;
              break;
            default: //go, 0 or 4.
              fXCoordinate += 16;
              fZCoordinate -= 15;
              break;
            }
          }
          /*// Special case - moving 10, anim starts a square back
          if( (DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence % 10 == 0) &&
            ((DISPLAY_state.TokenAnimStack[index].NewSequenceID - LED_IFT( DAT_3D, CNK_knamxa )) % ( CNK_knbcx0 - CNK_knacx0) == 0 ) )
          {// Move it back one jump (not a square since we are on a corner, but the distance of a move forward one
            switch( (int)(DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence / 10) )
            {
            case 3: // green, blue
              fXCoordinate -= PROPERTY_XWIDTH;
              break;
            case 2: // red, yellow
              fZCoordinate -= PROPERTY_XWIDTH;
              break;
            case 1: // purple, orange
              fXCoordinate += PROPERTY_XWIDTH;
              break;
            default: //purple, light blue
              fZCoordinate += PROPERTY_XWIDTH;
              break;
            }
          }*/

          LE_SEQNCR_StartRySTxzDrop(DISPLAY_state.TokenCurrent3DSequence, DISPLAY_Generic3dPriority,
            fYAngle, 1.0f, fXCoordinate, fZCoordinate, LE_SEQNCR_DropDropFrames );
          LE_SEQNCR_SetEndingAction( DISPLAY_state.TokenCurrent3DSequence, DISPLAY_Generic3dPriority,
            LE_SEQNCR_EndingActionStayAtEnd );
        }
      }
      // Specify the new camera.
      DISPLAY_state.desired2DCamera = DISPLAY_state.TokenAnimStack[index].PresetCameraView;
      DISPLAY_state.TokenAnimStackIndex++;
      // Special for victory animation
      if( DISPLAY_state.IsVictoryAnim )
      {// See if the index is at the top of the anim
        if( DISPLAY_state.TokenLoopAnimEnd == DISPLAY_state.TokenAnimStackIndex )
        {// Reset - release game lock if appropriate also
          DISPLAY_state.TokenAnimStackIndex = DISPLAY_state.TokenLoopAnimTop;
          if( !DISPLAY_state.isVictoryGameLockReleased )
          {
            DISPLAY_state.isVictoryGameLockReleased = TRUE;
            gameQueueUnLock();
          }
          // Give another sound!
          UDSOUND_TokenGenericPlaySound( DISPLAY_state.VictoryToken, TKS_WonGame, Sound_SkipIfOldSoundPlaying );
          UDSOUND_WatchSoundWithQueueLock( DISPLAY_state.TokenLastSoundStarted[DISPLAY_state.VictoryToken] );
        }
      }
    }
    
    // Custom camera controls.  Note we let the regular camera logic (above) process as usual.  We are voyeurs along for the ride.
    if( DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands )
    {// Monitor anim, do cool camera stuff.
      if( CustomCameraAnimStackIndex != DISPLAY_state.TokenAnimStackIndex && !CameraMoveWaiting)
      { // Issue new camera move.  Set 'basics' first, then effect specifics in the following switch.
        int cameraIndex = DISPLAY_state.TokenAnimStack[index+1].PresetCameraView; // there is always one more until the final camera move is issued.
        float fXCoordinate, fYCoordinate, fZCoordinate, fYAngle; // Temporary variables
        
        // We will set some basics for the camera before specific code - note if the anim is over we just stop.
        if( DISPLAY_state.TokenAnimStackIndex > DISPLAY_state.TokenAnimStackTop )
        {
          DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands = FALSE;
        } else
        {
          CameraWaitingData.StartLocation = CameraInterpolationData.End__Location; // Standard starting position
          CameraWaitingData.StartForwardV = CameraInterpolationData.End__ForwardV;
          CameraWaitingData.StartUpwardsV = CameraInterpolationData.End__UpwardsV;
          
          CameraWaitingData.End__Location = D3DVECTOR( CameraAngles2D[cameraIndex][0], CameraAngles2D[cameraIndex][1], CameraAngles2D[cameraIndex][2] );
          CameraWaitingData.End__ForwardV = D3DVECTOR( CameraAngles2D[cameraIndex][3], CameraAngles2D[cameraIndex][4], CameraAngles2D[cameraIndex][5] );
          CameraWaitingData.End__UpwardsV = D3DVECTOR( CameraAngles2D[cameraIndex][6], CameraAngles2D[cameraIndex][7], CameraAngles2D[cameraIndex][8] );
          
          //CameraWaitingData.InterpolationFXDesired = INTERPOLATION_BEZIER_POSITION_DIRECTION_ACCELERATE_IN_AND_OUT;
          CameraWaitingData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
          CameraWaitingData.InterpolationFXCurveType = INTERPOLATION_BEZIER;
          CameraWaitingData.InterpolationFXDirection = INTERPOLATION_DIRECTION_POINT_AT;
          CameraWaitingData.StartTime = CameraWaitingData.CurrentTime = LE_TIME_TickCount;
          CameraWaitingData.EndTime = LE_TIME_TickCount + 120;  // Aproximation for a sequence - monitor will adjust this once the anim is running.
          CameraWaitingData.ElapsedTimeRatio = -1.0f; // Flags the elapsed time is to be calculated.
          
          //CameraWaitingData.PointBBezierV = CameraWaitingData.StartLocation; // Starters only, adjust if used.
          //CameraWaitingData.PointCBezierV = CameraWaitingData.End__Location;
          CameraWaitingData.BezierSegmentsUsed = 0; // Reset the number of segments
          InterpolationAddBezierSegment( &CameraWaitingData, 0, &CameraWaitingData.StartLocation, 
            &CameraWaitingData.StartLocation, &CameraWaitingData.End__Location, &CameraWaitingData.End__Location, BEZIER_TANGENT_DELTA );
          
          // The trick is to point at the intersection of the final forward vector & the board (center of the view, ground 0)
          CameraWaitingData.PointingAt = CameraWaitingData.StartLocation + -(CameraWaitingData.StartLocation.y/CameraInterpolationData.End__ForwardV.y) * CameraInterpolationData.End__ForwardV ;
          
          CameraMoveWaiting = TRUE;
          
          // Customize parameters for the specific effects.
          switch( camEffect)
          {
          case TOKEN_CAM_EFFECT_FirstSeqSwoopPAT: // Swoop past in the first move pointing at token then release lock (used for single side anims - one move or one plus CX0)
            if( DISPLAY_state.TokenAnimStackIndex == 1 )
            {// Set bezier points to token start & finish positions.
              UDPIECES_GetTokenOrientation( DISPLAY_state.TokenAnimStack[index].SquareStartingNewSequence, 
                &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
              CameraWaitingData.BezierSegments[0][1] = TYPE_Point3D( fXCoordinate, fYCoordinate + 240, fZCoordinate );
              UDPIECES_GetTokenOrientation( DISPLAY_state.TokenAnimStack[index].SquareLandingNewSequence, 
                &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
              CameraWaitingData.BezierSegments[0][2] = TYPE_Point3D( fXCoordinate, fYCoordinate + 240, fZCoordinate );
              // NOTE: this method of setting bezier points bypasses a better method of curve length calculation for smooth time splits on multi-segment curves.
            }
            else
            {// A new camera view will just have been ordered following the last animation - release the lock.  This may not even be reached if the stack is depleted (we will be short circuited with the same effect)
              DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands = FALSE;
              //UDBOARD_ActivateDesiredCamera( DISPLAY_state.desired2DCamera );  // udboard show will handle this.
            }
            break;
          }// End switch effect
        }

        CustomCameraAnimStackIndex = DISPLAY_state.TokenAnimStackIndex;
      }

      // Monitor function - primarily to update 'point at' field.
      if( DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands ) // Status may have changed
      {
        switch( camEffect)
        {
        case TOKEN_CAM_EFFECT_FirstSeqSwoopPAT: // Swoop past in the first move pointing at token then release lock (used for single side anims - one move or one plus CX0)
          if( DISPLAY_state.TokenAnimStackIndex == 1 )
          {// Monitor token, keep camera aimed at it.
            D3DVECTOR tempVector;
            // bigger menas
            float pointatRatio = 0.26f;// FIXME - this will result in a frame rate dependant ratio (lower means less increments)

            if( !CameraMoveWaiting && !DISPLAY_state.manualCamLock && !DISPLAY_state.manualMouseCamLock)
            {// Point camera toward token
              UDPIECES_GetTokenActualOrientation( UICurrentGameState.CurrentPlayer, 
                &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
              tempVector = TYPE_Point3D( fXCoordinate, fYCoordinate, fZCoordinate );
              // now, move to the token but not instantly.  THIS IS TRICKY - the magnitudes counts.  The camera points at this spot.
              // Alter the ratio to move faster when the difference is further - the camera looses fast tokens.
              pointatRatio = pointatRatio * (Magnitude( CameraInterpolationData.PointingAt - tempVector ) / 66 );
              if( pointatRatio < 0.16f ) pointatRatio = 0.16f;
              if( pointatRatio > 0.35f ) pointatRatio = 0.35f;
              CameraInterpolationData.PointingAt = 
                (1.0f - pointatRatio)*CameraInterpolationData.PointingAt + pointatRatio*tempVector;
              if( detectedRemainingTime != -1 )
              {// A camera move waiting bypasses this - so if the camera moves after the sequence, not updates occur.
                //TRACE( "Setting Custom Cam End time from %d to %d\n", CameraInterpolationData.EndTime, max( CameraInterpolationData.EndTime, LE_TIME_TickCount + detectedRemainingTime ) );
                //CameraInterpolationData.EndTime = max( CameraInterpolationData.EndTime, LE_TIME_TickCount + detectedRemainingTime +2);
                CameraInterpolationData.EndTime = LE_TIME_TickCount + detectedRemainingTime;
              }
            }
          }
          break;
        }// End switch effect
      }
    }
    
    // Termination operations
    if( DISPLAY_state.TokenAnimStackIndex > DISPLAY_state.TokenAnimStackTop )
    {// The last sequence is finished.
      if( DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands )
      {// Close up custom camera shop.
        DISPLAY_state.desiredCameraCustomMoveIgnoreStandardCommands = FALSE;
      }
      DISPLAY_state.TokenAnimStackIndex = -1; // Off
      TRACE( "Terminating token animation %d\n", LE_TIME_TickCount );
      UDPENNY_LandedOnSquare( UICurrentGameState.CurrentPlayer, UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare );
      gameQueueUnLock();
      if( DISPLAY_state.TokenCurrent3DSequence )
      {
        LE_SEQNCR_Stop( DISPLAY_state.TokenCurrent3DSequence, DISPLAY_Generic3dPriority );
        DISPLAY_state.TokenCurrent3DSequence = NULL;
      }
    }

  }// end Token animation


  // TOKEN CHANGE IDLE STATE ANIMATION
  if( DISPLAY_state.IdleToCenterMoveStatus > 0 )
  {// We have work to do FIXME: Fast escalation if the board disappears
    float fXCoordinate, fYCoordinate, fZCoordinate, fYAngle; // Temporary variables
    bool  tempTest;
    LE_SEQNCR_RuntimeInfoRecord     InfoGeneric;
    int   t, s;
    
    switch( DISPLAY_state.IdleToCenterMoveStatus )
    {
    case 1: // Start animations
      DISPLAY_state.PlayerMovingInID = DISPLAY_state.PlayerMovingOutID = LED_EI;

      DISPLAY_state.PlayerMovingOut = DISPLAY_state.CurrentPlayerInCenterIdle;
      if( DISPLAY_state.PlayerMovingOut >= 0 )
      {// Find a spot to hop to, start the anim
        for( t = 5, s = 0; t >= 0; t-- ) 
        {
          if( DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare][t] == -1 ) s = t; // Available
        }
        DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare][s] = DISPLAY_state.PlayerMovingOut;
        DISPLAY_state.PlayerMovingOutID = LED_IFT( DAT_3D, CNK_knarc1a ) + ANIMS_PER_TOKEN*UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].token +
           + 2*s + 12*UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare];
        //UDPIECES_GetTokenRestingIdleOrientation(UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare, 
        //  s, UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].token,
        UDPIECES_GetTokenOrientation(UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare, 
          &fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
        // SPECIAL - corner anims were set up from the jail perspective - back up 90 degrees
        if( OFFSETS_GO_FP == UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare] ||
          OFFSETS_JUSTVIS == UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare] ||
          OFFSETS_INJAIL  == UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare])
         fYAngle -= (float)pi/2;      

        LE_SEQNCR_StartRySTxzDrop( DISPLAY_state.PlayerMovingOutID, DISPLAY_TokenPriority, fYAngle, 1.0f, fXCoordinate, fZCoordinate, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( DISPLAY_state.PlayerMovingOutID, DISPLAY_TokenPriority, LE_SEQNCR_EndingActionStayAtEnd );
        //TRACE("TO IDLE  , square %d, goes to resting square %d, type %d, angle %f, data ID offset %d / %d.\n", 
        //  UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare,
        //  s, UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].currentSquare],
        //  fYAngle * 180/pi,
        //  DISPLAY_state.PlayerMovingOutID - LED_IFT( DAT_3D, CNK_knarc1a ),
        //  DISPLAY_state.PlayerMovingOutID - LED_IFT( DAT_3D, CNK_knarc1a ) - ANIMS_PER_TOKEN*UICurrentGameState.Players[DISPLAY_state.PlayerMovingOut].token );
      }
      
      DISPLAY_state.PlayerMovingIn  = UICurrentGameState.CurrentPlayer;
      if( DISPLAY_state.PlayerMovingIn >= 0 )
      {// Find which resting spot they're on, start the anim.
        for( t = 0, s = 0; t <= 5; t++ ) 
        {
          if( DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare][t] == DISPLAY_state.PlayerMovingIn ) s = t; // Found it.
        }
        DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare][s] = -1; // Clear the spot.
        DISPLAY_state.PlayerMovingInID = LED_IFT( DAT_3D, CNK_knarc1c ) + ANIMS_PER_TOKEN * UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].token +
        + 2*s + 12*UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare];
        UDPIECES_GetTokenOrientation(UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare, 
          &fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
        // SPECIAL - corner anims were set up from the jail perspective - back up 90 degrees
        if( OFFSETS_GO_FP == UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare] ||
          OFFSETS_JUSTVIS == UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare] ||
          OFFSETS_INJAIL  == UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare])
          fYAngle -= (float)pi/2;      

        LE_SEQNCR_StartRySTxzDrop( DISPLAY_state.PlayerMovingInID, DISPLAY_TokenPriority, fYAngle, 1.0f, fXCoordinate, fZCoordinate, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( DISPLAY_state.PlayerMovingInID, DISPLAY_TokenPriority, LE_SEQNCR_EndingActionStayAtEnd );
        //TRACE("FROM IDLE, square %d,    from resting square %d, type %d, angle %f, data ID offset %d / %d.\n", 
        //  UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare,
        //  s, UDPEICES_PROPERTY_TYPE_FOR_RESTING[UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].currentSquare],
        //  fYAngle * 180/pi,
        //  DISPLAY_state.PlayerMovingInID - LED_IFT( DAT_3D, CNK_knarc1c ),
        //  DISPLAY_state.PlayerMovingInID - LED_IFT( DAT_3D, CNK_knarc1c ) - ANIMS_PER_TOKEN*UICurrentGameState.Players[DISPLAY_state.PlayerMovingIn].token );
      }

      DISPLAY_state.IdleToCenterMoveStatus = 2;
      DISPLAY_state.CurrentPlayerInCenterIdle = DISPLAY_state.PlayerMovingIn;// Flag now so regular idles start in right spot.
      break;
      
    case 2:// Monitor animations, wait for all of em to finish.
      //if( DISPLAY_state.LockGameQueueLastAction + 120 < LE_TIME_TickCount ) // little delay
      tempTest = FALSE;
      if( DISPLAY_state.PlayerMovingOutID != LED_EI )
      {
        if(LE_SEQNCR_GetInfo( DISPLAY_state.PlayerMovingOutID, DISPLAY_TokenPriority, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {
          if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
            tempTest = TRUE;
        } else
          tempTest = TRUE;
        if( tempTest )
        {// Done moving in.
          LE_SEQNCR_Stop( DISPLAY_state.PlayerMovingOutID, DISPLAY_TokenPriority );
          DISPLAY_state.PlayerMovingOutID = LED_EI;
          DISPLAY_state.PlayerMovingOut = -1;
        }
      }
      
      tempTest = FALSE;
      if( DISPLAY_state.PlayerMovingInID != LED_EI )
      {
        if(LE_SEQNCR_GetInfo( DISPLAY_state.PlayerMovingInID, DISPLAY_TokenPriority, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {
          if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
            tempTest = TRUE;
        } else
          tempTest = TRUE;
        if( tempTest )
        {// Done moving in.
          LE_SEQNCR_Stop( DISPLAY_state.PlayerMovingInID, DISPLAY_TokenPriority );
          DISPLAY_state.PlayerMovingInID = LED_EI;
          //DISPLAY_state.CurrentPlayerInCenterIdle = DISPLAY_state.PlayerMovingIn;
          DISPLAY_state.PlayerMovingIn = -1;
        }
      }
      
      if( DISPLAY_state.PlayerMovingIn == -1 && DISPLAY_state.PlayerMovingOut == -1 )
      {
        DISPLAY_state.IdleToCenterMoveStatus= 3;
        DISPLAY_state.IdleToCenterMoveStatus = 0;
        gameQueueUnLock();
      }
      break;
      
    }
  }// end if there is a center to resting idle move.


  // Token/paddy wagon Go To Jail animation
  if( DISPLAY_state.GoingToJailStatus > 0 )
  {// set DISPLAY_state.PlayerInPaddywagon to the player to make the idle invisible, -1 to appear
    static int tokenSquare, loadSquare;
    static Object_3D_Interpolation_data PaddyWagonInterpolationData;
    static TYPE_Tick tempTimer;
    float fXCoordinate, fYCoordinate, fZCoordinate, fYAngle; // Temporary variables
    D3DVECTOR v1, v2, v3, v4, t1, t2, t3, t4; // Temporary variables.
    LE_SEQNCR_RuntimeInfoRecord     InfoGeneric;
    static Matrix mxResultOld;       // Can store a build translation/rotation matrix for later recycling.
    const skipAnimValue = 14; // state to jump to to short circuit animation.


    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 1))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 1 )
    { // Establish the TO TOKEN animation to use & build the bezier stack
      tokenSquare = DISPLAY_state.GoingToJailFromSquare;
      loadSquare = (tokenSquare + 1) % 40; // Load square is of little value - the corners would have bad locations & rotations.
      PaddyWagonInterpolationData.BezierSegmentsUsed = 0; // Reset the number of segments

      UDSOUND_PlaySiren();

      switch( (int)(tokenSquare/10) )
      {
      case 0://Code for side 0 - does a turn around.
        // First, out to property border of token, point down.
        UDPIECES_GetTokenOrientation( 9, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v1 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate + 39 + 20 ); // Go back a bit extra
        UDPIECES_GetTokenOrientation( tokenSquare, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        t4 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate + 39 );

        v4 = t4;
        v4.z += 19; // Border
        v3 = v4;
        v4.x -= 32; // Low
        v3.x += 3;
        v2 = 0.5f * (v1 + v3);
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &v4, BEZIER_TANGENT_DELTA );

        t1 = v4;
        t2 = v3;
        t3 = t4;
        t3.z += 8;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &t1, &t2, &t3, &t4, -BEZIER_TANGENT_DELTA );// Backward

        if( tokenSquare > 5 )
          DISPLAY_state.desired2DCamera = VIEW2D21_5TILES02;// 5 view near jail - other jumps too much.
        else
          DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor15Squares(tokenSquare);
        break;

      case 1:// Side one - hop or go around token to next property
        if( tokenSquare == 10 )
          DISPLAY_state.GoingToJailStatus = skipAnimValue;
        else
        {
          UDPIECES_GetTokenOrientation( 10, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
          v1 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate );//Jail square
          v1.x -= 10; // give it a little room to move if token is on st charlies.
          UDPIECES_GetTokenOrientation( tokenSquare, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
          t4 = D3DVECTOR( fXCoordinate + 39, fYCoordinate, fZCoordinate );//This is always positive X above v1. 
          v4 = t4;
          v4.x -= 78; // Square before the token
          v2 = 0.7f * v1 + 0.3f * v4;
          v3 = 0.3f * v1 + 0.7f * v4;
          InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &v4, BEZIER_TANGENT_DELTA );
          
          /*t2 = t3 = t4; // These are OK bases for 'hop' or FIXME go around
          t2.y += 36;// Altitude
          t2.x -= 39;
          t3.x -= 7;// Remember, t4 hence this is back
          InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &v4, &t2, &t3, &t4, BEZIER_TANGENT_DELTA );// Backward*/


          t1 = t2 = v4;
          t2.x += 8;
          v4 = t4;
          v4.x -= 39;
          // on top of token
            //v4.y += 25; // Easy to adjust this to a side - a .z change.
            v4.z += 22; // right side
          v3 = v4;
          v3.x -=8;
          InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &t1, &t2, &v3, &v4, BEZIER_TANGENT_DELTA );
          
          t3 = t4;
          t3.x -= 8;
          InterpolationAddBezierSegmentSmooth( &PaddyWagonInterpolationData, 2, &t3, &t4, BEZIER_TANGENT_DELTA );
        }

        if( tokenSquare < 15 )
          DISPLAY_state.desired2DCamera = VIEW2D22_5TILES03;// 5 view near jail
        else
          DISPLAY_state.desired2DCamera = VIEW2D30_15TILES03;
        break;

      case 2:// Side 2 (red, yellow) - Run to free parking, then as per 1 above (hop over or around).
        UDPIECES_GetTokenOrientation( 10, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v1 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate );//Jail square
        UDPIECES_GetTokenOrientation( 20, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v3 = v4 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate );
        v3.z += 26; // Move back to first isle (probably want a bit more than the 16 to help pull the arc out).
        v3.x += 5;
        v2 = 0.5f * (v3 + v1);
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &v4, BEZIER_TANGENT_DELTA );

        // Now go to square before token
        v1 = v4;
        UDPIECES_GetTokenOrientation( tokenSquare, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v4 = t4 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate - 39 ); // Final destination
        // FIXME - expand this logic to pick a clear spot high or low - high center for now.
        v4.z += 78; // Square before the token
        v2 = 0.7f * v1 + 0.3f * v4;
        v3 = 0.3f * v1 + 0.7f * v4;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &v1, &v2, &v3, &v4, BEZIER_TANGENT_DELTA );

        /*t2 = t3 = t4; // These are OK bases for 'hop' or FIXME go around
        t2.y += 36;// Altitude
        t2.z += 39;
        t3.z += 7;// Remember, t4 hence this is back
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 2, &v4, &t2, &t3, &t4, BEZIER_TANGENT_DELTA );// Backward*/
        
        t1 = t2 = v4;
        t2.z -= 8;
        v4 = t4;
        v4.z += 39;
        // on top of token
        v4.y += 25; // Easy to adjust this to a side - a .x change.
        v3 = v4;
        v3.z +=8;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 2, &t1, &t2, &v3, &v4, BEZIER_TANGENT_DELTA );
        
        t3 = t4;
        t3.z += 8;
        InterpolationAddBezierSegmentSmooth( &PaddyWagonInterpolationData, 3, &t3, &t4, BEZIER_TANGENT_DELTA );


        if( tokenSquare < 22 )
          DISPLAY_state.desired2DCamera = VIEW2D18_CORNER_FREEPARKING;
        else if( tokenSquare < 25 )
          DISPLAY_state.desired2DCamera = VIEW2D24_5TILES05;// 5 view near free parking
        else
          DISPLAY_state.desired2DCamera = VIEW2D33_15TILES06;// 15 view looking from left
        break;

      case 3:// blast to go corner, back out to token pick up site.
        UDPIECES_GetTokenOrientation( 9, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v1 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate + 39 );//Property width, jail square pointing at connecticut.
        UDPIECES_GetTokenOrientation( 0, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v3 = v4 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate );
        v2 = 0.5f * (v1 + v3);
        v2.x += 5; // Little arc.  Call me crazy.
        v4.x -= 16;//Dip a bit
        v4.z -= 16;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &v4, BEZIER_TANGENT_DELTA );

        // Next, back up to the final location.
        UDPIECES_GetTokenOrientation( tokenSquare, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        t4 = D3DVECTOR( fXCoordinate - 39, fYCoordinate, fZCoordinate );
        UDPIECES_GetTokenOrientation( 39, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        t2 = D3DVECTOR( fXCoordinate - 39, fYCoordinate, fZCoordinate ); // We cross a corner, not location 0.
        t3 = 0.5 * (t4 + v3); // Halfway from GO to dest - can be the same spot...
        if( tokenSquare == 39 )
        {// t2, t3 & t4 will be the same
          t2.x -= 10;
          t3.x -= 5;
        }
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &v4, &t2, &t3, &t4, -BEZIER_TANGENT_DELTA );

        if( tokenSquare == 30 )
        {
          if( rand()%2 == 0 )
            DISPLAY_state.desired2DCamera = VIEW2D19_CORNER_GOTOJAIL;
          else
            DISPLAY_state.desired2DCamera = VIEW2D36_15TILES09;
        }
        else if( tokenSquare < 35 )
          DISPLAY_state.desired2DCamera = VIEW2D26_5TILES07;// 5 view near free parking
        else
          DISPLAY_state.desired2DCamera = VIEW2D27_5TILES08;// 15 view looking from left
        break;
      }
      // set the FX type & time
      PaddyWagonInterpolationData.ElapsedTimeRatio = -1.0f; // Flags to use time.
      PaddyWagonInterpolationData.InterpolationFXSpeedType = INTERPOLATION_ACCELERATE_IN_AND_OUT;
      PaddyWagonInterpolationData.InterpolationFXCurveType = INTERPOLATION_BEZIER;
      PaddyWagonInterpolationData.InterpolationFXDirection = INTERPOLATION_DIRECTION_TANGENT;
      PaddyWagonInterpolationData.StartTime = PaddyWagonInterpolationData.CurrentTime = LE_TIME_TickCount;
      if( DISPLAY_state.GoingToJailStatus != skipAnimValue )
      {
        if( tokenSquare > 10 && tokenSquare < 30 )
          PaddyWagonInterpolationData.EndTime = LE_TIME_TickCount + 60 + 5*(tokenSquare - 10);
        else
        {
          if( tokenSquare >= 30 )
            PaddyWagonInterpolationData.EndTime = LE_TIME_TickCount + 60 + 5*(50 - tokenSquare);
          else
            PaddyWagonInterpolationData.EndTime = LE_TIME_TickCount + 60 + 5*(10 - tokenSquare);
        }
      }
      else // Overkill, but indicate no anim to run.
        PaddyWagonInterpolationData.EndTime = PaddyWagonInterpolationData.StartTime;
      
      DISPLAY_state.GoingToJailStatus++;
    }// endif startup animation


    // Paddy wagon in * We hold off on the animation set up above, but need the start coordinates.
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 2))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 2)
    {// Pop in the paddy wagon
      if( PaddyAnimation != LED_IFT(DAT_3D, CNK_mnwpi) )
      {
        Matrix mxResult(Matrix::IDENTITY);
        
        PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwpi);
        UDPIECES_GetTokenOrientation( tokenSquare, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, 0.0f, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );

        PaddyWagonInterpolationData.CurrentTime = PaddyWagonInterpolationData.StartTime;
        InterpolationFX_3DPositionAndOrientation( &PaddyWagonInterpolationData );
        mxResult.IDENTITY;
        mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
        mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
        mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
        LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );
     
        //DISPLAY_state.desired2DCamera = VIEW2D17_CORNER_JAIL;
      }

      if(LE_SEQNCR_GetInfo( PaddyAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The animation is done.
          LE_SEQNCR_Stop( PaddyAnimation, 77 );
          PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwrl);
          LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, 0.0f, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
          LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionLoopToBeginning );

          // Now, move the animation times forward since we killed a lot of time...
          PaddyWagonInterpolationData.CurrentTime = LE_TIME_TickCount - PaddyWagonInterpolationData.StartTime; // Delta - used like a temp variable here.
          PaddyWagonInterpolationData.EndTime   += PaddyWagonInterpolationData.CurrentTime;
          PaddyWagonInterpolationData.StartTime += PaddyWagonInterpolationData.CurrentTime;
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }


    // Monitor the paddywagon 'to' animation, updating it as we go
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 3))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 3 )
    {
      Matrix mxResult(Matrix::IDENTITY);

      PaddyWagonInterpolationData.CurrentTime = LE_TIME_TickCount;
      InterpolationFX_3DPositionAndOrientation( &PaddyWagonInterpolationData );
      mxResult.IDENTITY;
      mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
      mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
      mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
      
      LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );

      // Now, when the anim is done escalate
      if( PaddyWagonInterpolationData.CurrentTime >= PaddyWagonInterpolationData.EndTime - 55 )
        DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( (tokenSquare + 1) % 40 ); // give it a head start
      if( PaddyWagonInterpolationData.CurrentTime >= PaddyWagonInterpolationData.EndTime )
      {
        LE_SEQNCR_Stop( PaddyAnimation, 77 );
        DISPLAY_state.GoingToJailStatus++;
      }
    }


    // Stop the wagon and open the doors
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 4))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 4 )
    {
      if( PaddyAnimation != LED_IFT(DAT_3D, CNK_mnwdo) )
      {
        Matrix mxResult(Matrix::IDENTITY);
        
        PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwdo);
        LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, 0.0f, 1.0f, 70.0f, 380.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );

        PaddyWagonInterpolationData.CurrentTime = LE_TIME_TickCount;
        InterpolationFX_3DPositionAndOrientation( &PaddyWagonInterpolationData );
        mxResult.IDENTITY;
        mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
        mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
        mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
        LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );
      }

      if(LE_SEQNCR_GetInfo( PaddyAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The animation is done, start the idle (with doors open)
          Matrix mxResult(Matrix::IDENTITY);
          
          LE_SEQNCR_Stop( PaddyAnimation, 77 );
          PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwoi);
          LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, 0.0f, 1.0f, 70.0f, 380.0f, LE_SEQNCR_DropDropFrames );
          LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionLoopToBeginning );
          TokenAnimation = LED_EI;  // Just a precaution (to be sure it starts next code block)
          
          mxResult.IDENTITY;
          mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
          mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
          mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
          LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );
          
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }

    // Load the token
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 5))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 5 )
    {// First up, shut down the token & register its location in jail (registration was skipped during the normal response to notify jump to square)
      DISPLAY_state.PlayerInPaddywagon = UICurrentGameState.CurrentPlayer;
      UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare = 40;
      if( TokenAnimation != LED_IFT(DAT_3D, CNK_knawji) + ANIMS_PER_TOKEN*UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].token )
      {
        TokenAnimation = LED_IFT(DAT_3D, CNK_knawji) + ANIMS_PER_TOKEN*UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].token;
        UDPIECES_GetTokenOrientation( tokenSquare, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        LE_SEQNCR_StartRySTxzDrop( TokenAnimation, 77, fYAngle, 1.0f, fXCoordinate, fZCoordinate, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( TokenAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );
      }
      if(LE_SEQNCR_GetInfo( TokenAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The token animation is done.
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }
    
    // Close the wagon (then stop the token animation)
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 6))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 6 )
    {
      if( PaddyAnimation != LED_IFT(DAT_3D, CNK_mnwdc) )
      {
        Matrix mxResult(Matrix::IDENTITY);

        LE_SEQNCR_Stop( PaddyAnimation, 77 );

        PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwdc);
        LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, fYAngle, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );

        mxResult.IDENTITY;
        mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
        mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
        mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
        LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );
        mxResultOld = mxResult;
      }
      if(LE_SEQNCR_GetInfo( PaddyAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The token is hidden, kill it.
          LE_SEQNCR_Stop( TokenAnimation, 77 );
          TokenAnimation = LED_EI;
          
          // We'll switch to the idle anim now.
          LE_SEQNCR_Stop( PaddyAnimation, 77 );
          PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwrl);
          LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, fYAngle, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
          LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionLoopToBeginning );
          LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResultOld );

          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }

    
    // Back to jail paddywagon anim construction
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 7 )) //skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 7 )
    {// Back to jail time
      UDSOUND_PlaySiren();

      PaddyWagonInterpolationData.BezierSegmentsUsed = 0; // Reset the number of segments

      // Do the start/end calcs in advance
      v1 = PaddyWagonInterpolationData.End__Location; // Last position
      UDPIECES_GetTokenOrientation( 11, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
      t4 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate );

      PaddyWagonInterpolationData.BezierSegmentsUsed = 0; // Reset the number of segments
      switch( (int)(tokenSquare/10) )
      {
      case 0:// Around jail to the drop zone
        UDPIECES_GetTokenOrientation( 10, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v3 = D3DVECTOR( fXCoordinate - 20, fYCoordinate, fZCoordinate - 5 );//try to stay in the isle by lowering x - may need another segment.
        v2 = 0.5 * (v1 + v3);
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &t4, BEZIER_TANGENT_DELTA );

        DISPLAY_state.desired2DCamera = VIEW2D17_CORNER_JAIL;
        break;

      case 1:// Reverse to drop zone
        v2 = 0.7f * v1 + 0.3f * t4;
        v3 = 0.3f * v1 + 0.7f * t4;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &t4, -BEZIER_TANGENT_DELTA );

        if( tokenSquare < 14 )
          DISPLAY_state.desired2DCamera = VIEW2D07_3TILES04; // close up
        else
          DISPLAY_state.desired2DCamera = VIEW2D17_CORNER_JAIL; // It will usually already be here.
        break;

      case 2:// Reverse to corner, turn around side of drop, turn around to drop
        UDPIECES_GetTokenOrientation( 20, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v4 = D3DVECTOR( fXCoordinate + 10, fYCoordinate, fZCoordinate + 26 );
        v3 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate + 16 ); // Meeting of the isles
        v2 = 0.6f*v1 + 0.4f*v3;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &v4, -BEZIER_TANGENT_DELTA );

        // Now, forward to side of destination
        t1 = v4;
        t2 = v3;
        //v3 = 0.3f*t2 + 0.7f*t4;// This is always a 9 run (we come from the corner)
        v3 = t4;
        v3.x += 20;
        v3.z -= 5; // help hold it out
        v4 = t4;
        v4.z += 20;
        v4.x += 20;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &t1, &t2, &v3, &v4, BEZIER_TANGENT_DELTA );

        // Reverse to park (should be slow - shall we **'HACK' up its distance so it gets a bigger chunk of time?
        t3 = t4;
        t3.x += 14;
        t1 = v4;
        t2 = v4 + 14 * Normalize(v3 - v4);
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 2, &t1, &t2, &t3, &t4, -BEZIER_TANGENT_DELTA );
        PaddyWagonInterpolationData.BezierLengths[2] = PaddyWagonInterpolationData.BezierLengths[2] * 2; // Hack which slows final segment.

        if( tokenSquare < 24 )
          DISPLAY_state.desired2DCamera = VIEW2D30_15TILES03;
        else
          DISPLAY_state.desired2DCamera = VIEW2D30_15TILES03; // It will usually already be here.
        break;

      case 3:// Around go, jail to drop.
        UDPIECES_GetTokenOrientation( 0, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        v4 = D3DVECTOR( fXCoordinate, fYCoordinate, fZCoordinate );
        v3 = v4;
        v3.z -= 24; // the 0 idle is a little left of isle
        v3.x -= 5;
        v2 = 0.6f*v1 + 0.4f*v3;
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 0, &v1, &v2, &v3, &v4, BEZIER_TANGENT_DELTA );

        // Now around jail corner 
        UDPIECES_GetTokenOrientation( 10, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
        t3 = D3DVECTOR( fXCoordinate - 34, fYCoordinate, fZCoordinate + 6 );//try to stay in the isle by lowering x - may need another segment.
        t2 = 0.5 * (v4 + t3);
        InterpolationAddBezierSegment( &PaddyWagonInterpolationData, 1, &v4, &t2, &t3, &t4, BEZIER_TANGENT_DELTA );

        // A Bezier rather than 'over the board' would be great, here & previous side.
        if( tokenSquare < 35 )
          DISPLAY_state.desired2DCamera = VIEW2D39_15TILES12; // Try it - 15 ahead looking over boardwalk
        else
          DISPLAY_state.desired2DCamera = VIEW2D22_5TILES03;  // Over the board YUCK
        break;
      }
      PaddyWagonInterpolationData.StartTime = PaddyWagonInterpolationData.CurrentTime = LE_TIME_TickCount;
      // Time should be adjusted based on distance covered
      if( tokenSquare > 10 && tokenSquare < 30 )
        PaddyWagonInterpolationData.EndTime = LE_TIME_TickCount + 40 + 5*(tokenSquare - 10);// max120
      else
      {
        if( tokenSquare >= 30 )
          PaddyWagonInterpolationData.EndTime = LE_TIME_TickCount + 40 + 5*(50 - tokenSquare);
        else
          PaddyWagonInterpolationData.EndTime = LE_TIME_TickCount + 40 + 5*(10 - tokenSquare);
      }

      DISPLAY_state.GoingToJailStatus++;
    }


    // Monitor return to jail animation
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 8))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 8 )
    {
      Matrix mxResult(Matrix::IDENTITY);

      PaddyWagonInterpolationData.CurrentTime = LE_TIME_TickCount;
      InterpolationFX_3DPositionAndOrientation( &PaddyWagonInterpolationData );
      mxResult.IDENTITY;
      mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
      mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
      mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
      
      LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );

      // Now, when the anim is done escalate
      if( PaddyWagonInterpolationData.CurrentTime >= PaddyWagonInterpolationData.EndTime - 55 )
        DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( 40 );// Warning: this will issue each iteration.
      if( PaddyWagonInterpolationData.CurrentTime >= PaddyWagonInterpolationData.EndTime )
        DISPLAY_state.GoingToJailStatus++;
    }


    // At jail, open then idle
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 9))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 9 )
    {
      if( PaddyAnimation != LED_IFT(DAT_3D, CNK_mnwdo) )
      {// NOTE: The token is not visible - an idle pre-jump out? FIXME
        Matrix mxResult(Matrix::IDENTITY);

        LE_SEQNCR_Stop( PaddyAnimation, 77 );

        PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwdo);
        LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, fYAngle, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );

        mxResult.IDENTITY;
        mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
        mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
        mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
        LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );

        mxResultOld = mxResult;
      }

      if(LE_SEQNCR_GetInfo( PaddyAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The paddy wagon is open - start idle & token jump out.
          LE_SEQNCR_Stop( PaddyAnimation, 77 );
          
          PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwoi);
          LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, fYAngle, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
          LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionLoopToBeginning );
          LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResultOld );
          
          TokenAnimation = LED_IFT(DAT_3D, CNK_knawjo) + ANIMS_PER_TOKEN*UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].token;
          UDPIECES_GetTokenOrientation( 10, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
          LE_SEQNCR_StartRySTxzDrop( TokenAnimation, 77, fYAngle, 1.0f, fXCoordinate, fZCoordinate, LE_SEQNCR_DropDropFrames );
          LE_SEQNCR_SetEndingAction( TokenAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );
          
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }


    // Monitor the unload animation
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 10))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 10 )
    {
      if(LE_SEQNCR_GetInfo( TokenAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The token animation is done.
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }


    // Close the wagon - token can return to regular idle.
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 11))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 11 )
    {
      if( PaddyAnimation != LED_IFT(DAT_3D, CNK_mnwdc) )
      {
        Matrix mxResult(Matrix::IDENTITY);

        // Forget about token
        LE_SEQNCR_Stop( TokenAnimation, 77 );
        TokenAnimation = LED_EI;
        UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare = 40;
        DISPLAY_state.PlayerInPaddywagon = -1;
        
        // Paddywagon next
        LE_SEQNCR_Stop( PaddyAnimation, 77 );

        PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwdc);
        LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, fYAngle, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );

        mxResult.IDENTITY;
        mxResult.RotateY( (float)atan2(PaddyWagonInterpolationData.InterForwardV.x, PaddyWagonInterpolationData.InterForwardV.z) );
        mxResult.RotateX( -(float)asin(PaddyWagonInterpolationData.InterForwardV.y) );
        mxResult.Translate(PaddyWagonInterpolationData.InterLocation);
        LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );
        mxResultOld = mxResult;
      }
      if(LE_SEQNCR_GetInfo( PaddyAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }

    // Register the spot & free the idle to run (spot was registered earlier but what the heck.)
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 12))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 12 )
    {
      // And the paddywagon vaporizes
      LE_SEQNCR_Stop( PaddyAnimation, 77 );
      
      PaddyAnimation = LED_IFT(DAT_3D, CNK_mnwpo);
      LE_SEQNCR_StartRySTxzDrop( PaddyAnimation, 77, fYAngle, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
      LE_SEQNCR_SetEndingAction( PaddyAnimation, 77, LE_SEQNCR_EndingActionStayAtEnd );
      LE_SEQNCR_MoveTheWorks( PaddyAnimation, 77, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResultOld );

      DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( 40 );// Warning: this will issue each iteration.
      DISPLAY_state.GoingToJailStatus++;
    }


    // Monitor paddy out anim
    if( !DISPLAY_state.IsOptionTokenAnimationsOn && (DISPLAY_state.GoingToJailStatus == 13))//skips
      DISPLAY_state.GoingToJailStatus++;
    if( DISPLAY_state.GoingToJailStatus == 13 )
    {
      if(LE_SEQNCR_GetInfo( PaddyAnimation, 77, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {
        if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
        {// The token animation is done.
          DISPLAY_state.GoingToJailStatus++;
        }
      }
    }


    // Done - double check everything.
    if( DISPLAY_state.GoingToJailStatus >= skipAnimValue /*14*/ )
    {
      if( PaddyAnimation != LED_EI )
      {
        LE_SEQNCR_Stop( PaddyAnimation, 77 );
        PaddyAnimation = LED_EI;
      }
      if( TokenAnimation != LED_EI )
      {
        LE_SEQNCR_Stop( TokenAnimation, 77 );
        TokenAnimation = LED_EI;
      }
      UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare = 40;
      DISPLAY_state.GoingToJailStatus = 0;
      DISPLAY_state.PlayerInPaddywagon = -1;
      gameQueueUnLock();

      DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( 40 );
    }
  }// end if going to jail status



  // Update 3D token idles
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
  {
    float fXCoordinate;
    float fYCoordinate;
    float fZCoordinate;
    float fYAngle;

    // This needs the sequence to be started prior to working - another more efficient method?
    // At the least, an idle which remains idle should not be re-interrogated (performance).

    // TOKENS SITTING IDLE - resting & center - If the player is used, make sure the right token is displayed.
    if( ( t < UICurrentGameState.NumberOfPlayers ) &&
      ( DISPLAY_IsBoardVisible ) &&
      ( (DISPLAY_state.TokenAnimStackIndex == -1 ) || // Here we are determining if an anim is running for the token
      ( DISPLAY_state.TokenAnimStackIndex > -1 && t != UICurrentGameState.CurrentPlayer ) ) &&
      ( t != DISPLAY_state.PlayerInPaddywagon ) && // Token is in paddywagon
      ( t != DISPLAY_state.PlayerMovingIn  ) &&
      ( t != DISPLAY_state.PlayerMovingOut ) // Criteria to know when an idle center <=> idle resting is occuring so the idle turns off.
      && (UICurrentGameState.Players[t].currentSquare < 41) // not off board
      )
    {// We need to identify the sequence to play and where to play it - then do the animation.
      if( DISPLAY_state.CurrentPlayerInCenterIdle == t )
      {
        UDPIECES_GetTokenOrientation(UICurrentGameState.Players[t].currentSquare,
          &fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
      } else
      {
        for( int x = 0, s = 0; x <= 5; x++ ) 
        {
          if( DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[t].currentSquare][x] == t ) s = x; // Found it.
        }
        UDPIECES_GetTokenRestingIdleOrientation(UICurrentGameState.Players[t].currentSquare, s, UICurrentGameState.Players[t].token,
          &fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
      }
      
      tempID = LED_IFT( DAT_3D, CNK_knamx0 ) + UICurrentGameState.Players[t].token * ANIMS_PER_TOKEN;
    }
    else
    { // Token not showing
      DISPLAY_state.TokenCur3DLocX[t] = -1;
      tempID = LED_EI;
    }

    if( DISPLAY_state.Player3DTokenShown[t] != tempID )
    {// New token to show, kill old start new.
      if( DISPLAY_state.Player3DTokenShown[t] != LED_EI )
        LE_SEQNCR_Stop( DISPLAY_state.Player3DTokenShown[t], DISPLAY_TokenPriority + t );

      DISPLAY_state.Player3DTokenShown[t] = tempID;
      if( DISPLAY_state.Player3DTokenShown[t] != LED_EI )
      {
        DISPLAY_state.TokenCur3DLocX[t] = fXCoordinate;
        DISPLAY_state.TokenCur3DLocY[t] = fYCoordinate;
        DISPLAY_state.TokenCur3DLocZ[t] = fZCoordinate;
        DISPLAY_state.TokenCur3DAngleY[t] = fYAngle;

        LE_SEQNCR_StartRySTxzDrop( DISPLAY_state.Player3DTokenShown[t], DISPLAY_TokenPriority + t, fYAngle, 1.0f, fXCoordinate, fZCoordinate, LE_SEQNCR_DropDropFrames );
        if( DISPLAY_state.IsOptionTokenAnimationsOn )
          LE_SEQNCR_SetEndingAction( DISPLAY_state.Player3DTokenShown[t], DISPLAY_TokenPriority + t, LE_SEQNCR_EndingActionLoopToBeginning );
        else
          LE_SEQNCR_SetEndingAction( DISPLAY_state.Player3DTokenShown[t], DISPLAY_TokenPriority + t, LE_SEQNCR_EndingActionStayAtEnd );
      }
      //UDPIECES_UpdateShadow( t );
    }

    if( DISPLAY_state.Player3DTokenShown[t] != LED_EI)
    {// Check if token has moved.
      if ((DISPLAY_state.TokenCur3DLocX[t] != fXCoordinate) ||
        (DISPLAY_state.TokenCur3DLocY[t] != fYCoordinate) ||
        (DISPLAY_state.TokenCur3DLocZ[t] != fZCoordinate) ||
        (DISPLAY_state.TokenCur3DAngleY[t] != fYAngle))
      {
        //int nPreviousSquare; // FIXME - what was this previous getorientation shit here for?

        DISPLAY_state.TokenCur3DLocX[t] = fXCoordinate;
        DISPLAY_state.TokenCur3DLocY[t] = fYCoordinate;
        DISPLAY_state.TokenCur3DLocZ[t] = fZCoordinate;
        DISPLAY_state.TokenCur3DAngleY[t] = fYAngle;
        LE_SEQNCR_MoveRySTxz(tempID,DISPLAY_TokenPriority + t,fYAngle,1.0f,fXCoordinate,fZCoordinate);
        //UDPIECES_UpdateShadow( t );

        //nPreviousSquare = UICurrentGameState.Players[t].currentSquare - (UICurrentGameState.Dice[0] + UICurrentGameState.Dice[1]);
        //if (nPreviousSquare < 0)
        //  nPreviousSquare += 40;
        //UDPIECES_GetTokenOrientation(nPreviousSquare,&fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
      }
    }
  }

  // Show the hotels and houses
  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    // if hotel on this property
    if (UICurrentGameState.Squares[t].houses == UICurrentGameState.GameOptions.housesPerHotel &&
      (DISPLAY_IsBoardVisible) ) // NOTE: Screen logic occurs below (houses ) as well.
    {
      // if hotel not currently displayed
      if (!DISPLAY_state.HotelFlag[t])
      {
        TYPE_Point3D ptPosition;
        TYPE_Angle3D fRotate;

        // display hotel at correct position and rotation
        DISPLAY_state.HousingPriority[t][0] = DISPLAY_BoardHousingPriority + (t * 4);
        UDPIECES_GetHotelPosition(t,&ptPosition.x,&ptPosition.z,&fRotate);
        LE_SEQNCR_StartRySTxz(LED_IFT(DAT_3D,HMD_hotel),DISPLAY_state.HousingPriority[t][0],fRotate,0.10f,ptPosition.x,ptPosition.z);
        // set hotel flag
        DISPLAY_state.HotelFlag[t] = TRUE;

        // remove all displayed houses at this property
        for (s = 0; s < 4; s ++)
        {
          if (DISPLAY_state.HouseFlags[t][s])
          {
            LE_SEQNCR_Stop(LED_IFT(DAT_3D,HMD_house),DISPLAY_state.HousingPriority[t][s]);
            DISPLAY_state.HouseFlags[t][s] = FALSE;
          }
        }
      }
    }
    // no hotel, check for houses to display
    else
    {
      // remove hotel if displayed
      if (DISPLAY_state.HotelFlag[t])
      {
        LE_SEQNCR_Stop(LED_IFT(DAT_3D,HMD_hotel),DISPLAY_state.HousingPriority[t][0]);
        DISPLAY_state.HotelFlag[t] = FALSE;
      }

      // display or remove houses
      for (s = 0; s < 4; s ++)
      {
        // if house exists
        if (s < UICurrentGameState.Squares[t].houses &&
          (DISPLAY_IsBoardVisible) ) // Update as above - same screens for houses & hotels.
        {
          // if house not currently displayed
          if (!DISPLAY_state.HouseFlags[t][s])
          {
            TYPE_Point3D ptPosition;
            TYPE_Angle3D fRotate;

            // display house at correct position and rotation
            DISPLAY_state.HousingPriority[t][s] = DISPLAY_BoardHousingPriority + (t * 4) + s;
            UDPIECES_GetHousePosition(t,s,&ptPosition.x,&ptPosition.z,&fRotate);
            LE_SEQNCR_StartRySTxz(LED_IFT(DAT_3D,HMD_house),DISPLAY_state.HousingPriority[t][s],fRotate,0.10f,ptPosition.x,ptPosition.z);
            // set house flag
            DISPLAY_state.HouseFlags[t][s] = TRUE;
          }
        }
        // if house doesn't exist
        else
        {
          // if house displayed, remove house
          if (DISPLAY_state.HouseFlags[t][s])
          {
            LE_SEQNCR_Stop(LED_IFT(DAT_3D,HMD_house),DISPLAY_state.HousingPriority[t][s]);
            DISPLAY_state.HouseFlags[t][s] = FALSE;
          }
        }
      }
    }
  }

}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDPIECES_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{
}


/*****************************************************************************
 * Standard UI subfunction - Process game message to player.
 */

void UDPIECES_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
}


/****************************************************************************/
/*****************************************************************************
* The actual rotation and position for token.  It will return the theoretical
* position should the actual be unobtainable (no sequence data decipherable).
*/
void UDPIECES_GetTokenActualOrientation(
  int nPlayer,
  float *lpfXCoordinate,
  float *lpfYCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle)
{
  int LocateStatus = 0;
  //float x, y, z;
  LE_DATA_DataId currentSeq = LED_EI;
  static LE_SEQNCR_Runtime3DInfoRecord   Info3D;
  static TYPE_Matrix3D myMatrix3D;
  static float LastKnownData[6][4];

  if( nPlayer >= 0 && nPlayer < UICurrentGameState.NumberOfPlayers )
  { // Identify the current sequence

    if( DISPLAY_state.Player3DTokenShown[nPlayer] )
    { // Token is idle
      if(LE_SEQNCR_GetInfo( DISPLAY_state.Player3DTokenShown[nPlayer], DISPLAY_TokenPriority + nPlayer, FALSE, NULL, NULL, &Info3D, NULL, NULL ) )
      {
        *lpfXCoordinate = Info3D.sequenceToWorldTransformation.matrix3D._41;
        *lpfYCoordinate = Info3D.sequenceToWorldTransformation.matrix3D._42;
        *lpfZCoordinate = Info3D.sequenceToWorldTransformation.matrix3D._43;
        // opp/adj, sin/cos - if both rotx & rotz are 0, then
        *lpfYAngle = (float)atan2( Info3D.sequenceToWorldTransformation.matrix3D._31, Info3D.sequenceToWorldTransformation.matrix3D._11);
        LocateStatus = 1;
      }

    } else if( DISPLAY_state.TokenCurrent3DSequence )
    { // Token is moving
      if( LE_SEQNCR_GetChildMeshWorldMatrix( DISPLAY_state.TokenCurrent3DSequence, DISPLAY_Generic3dPriority, &myMatrix3D ) )
      {
        *lpfXCoordinate = myMatrix3D._41;
        *lpfYCoordinate = myMatrix3D._42;
        *lpfZCoordinate = myMatrix3D._43;
        *lpfYAngle = (float)atan2( myMatrix3D._31, myMatrix3D._11 );
        LocateStatus = 1;
      }

    } else if( nPlayer == DISPLAY_state.PlayerMovingOut)
    {// Token is moving to resting idle
      if( LE_SEQNCR_GetChildMeshWorldMatrix( DISPLAY_state.PlayerMovingOutID, DISPLAY_TokenPriority, &myMatrix3D ) )
      {
        *lpfXCoordinate = myMatrix3D._41;
        *lpfYCoordinate = myMatrix3D._42;
        *lpfZCoordinate = myMatrix3D._43;
        *lpfYAngle = (float)atan2( myMatrix3D._31, myMatrix3D._11 );
        LocateStatus = 1;
      }

    } else if( nPlayer == DISPLAY_state.PlayerMovingIn)
    {// Token is moving to resting idle
      if( LE_SEQNCR_GetChildMeshWorldMatrix( DISPLAY_state.PlayerMovingInID, DISPLAY_TokenPriority, &myMatrix3D ) )
      {
        *lpfXCoordinate = myMatrix3D._41;
        *lpfYCoordinate = myMatrix3D._42;
        *lpfZCoordinate = myMatrix3D._43;
        *lpfYAngle = (float)atan2( myMatrix3D._31, myMatrix3D._11 );
        LocateStatus = 1;
      }

    } else if( TokenAnimation != LED_EI )
    {// Go to jail animation
      if( LE_SEQNCR_GetChildMeshWorldMatrix( TokenAnimation, 77, &myMatrix3D ) )
      {
        *lpfXCoordinate = myMatrix3D._41;
        *lpfYCoordinate = myMatrix3D._42;
        *lpfZCoordinate = myMatrix3D._43;
        *lpfYAngle = (float)atan2( myMatrix3D._31, myMatrix3D._11 );
        LocateStatus = 1;
      }
    }


    if( !LocateStatus )
    {// Typically here means the move anim is over and the next or idle has not begun (likely will not happen after stayatend feature fixed)
      //UDPIECES_GetTokenOrientation( UICurrentGameState.Players[nPlayer].currentSquare,
      //  lpfXCoordinate, lpfYCoordinate, lpfZCoordinate, lpfYAngle );
      *lpfXCoordinate = LastKnownData[nPlayer][0];
      *lpfYCoordinate = LastKnownData[nPlayer][1];
      *lpfZCoordinate = LastKnownData[nPlayer][2];
      *lpfYAngle      = LastKnownData[nPlayer][3];
    } else
    {// Record known location
      LastKnownData[nPlayer][0] = *lpfXCoordinate;
      LastKnownData[nPlayer][1] = *lpfYCoordinate;
      LastKnownData[nPlayer][2] = *lpfZCoordinate;
      LastKnownData[nPlayer][3] = *lpfYAngle;
    }
  } else

  {// invalid player - Lets go for zeros.
    *lpfXCoordinate = 0;
    *lpfYCoordinate = 0;
    *lpfZCoordinate = 0;
    *lpfYAngle = 0;// Matrix does hold this if'n ya wanna figure it out.
  }
}

/*****************************************************************************
* the correct rotation and position for token on tile - does not correct
* for resting idle amin orientation problem (corners being jail oriented)
*/
void UDPIECES_GetTokenOrientation(
  int nBoardTile,
  float *lpfXCoordinate,
  float *lpfYCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle)
{
  *lpfYCoordinate = 0;
  if (nBoardTile == SQ_IN_JAIL)
    nBoardTile = SQ_JUST_VISITING;
  if (nBoardTile < SQ_JUST_VISITING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + TOKEN1_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - TOKEN1_XOFFSET;
    *lpfYAngle = 0.0f;
  }
  else if (nBoardTile < SQ_FREE_PARKING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - TOKEN1_XOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - TOKEN1_ZOFFSET;
    *lpfYAngle = (float)(pi / 2.0);
  }
  else if (nBoardTile < SQ_GO_TO_JAIL)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - TOKEN1_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + TOKEN1_XOFFSET;
    *lpfYAngle = (float)pi;
  }
  else
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + TOKEN1_XOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + TOKEN1_ZOFFSET;
    *lpfYAngle = -(float)(pi / 2.0);
  }
}

/*****************************************************************************
* the correct rotation and position for token on tile incorporating resting
* idle locations.
*/
void UDPIECES_GetTokenRestingIdleOrientation(
  int nBoardTile,
  int nRestingPosition,
  int nToken,
  float *lpfXCoordinate,
  float *lpfYCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle)
{
  int tileType;
  int nBoardTileOriginal;


  *lpfYCoordinate = 0;
  nBoardTileOriginal = nBoardTile;
  if (nBoardTile == SQ_IN_JAIL)
    nBoardTile = SQ_JUST_VISITING; // For centers, these are the same

  if (nBoardTile < SQ_JUST_VISITING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + TOKEN1_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - TOKEN1_XOFFSET;
    *lpfYAngle = 0.0f;
  }
  else if (nBoardTile < SQ_FREE_PARKING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - TOKEN1_XOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - TOKEN1_ZOFFSET;
    *lpfYAngle = (float)(pi / 2.0);
  }
  else if (nBoardTile < SQ_GO_TO_JAIL)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - TOKEN1_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + TOKEN1_XOFFSET;
    *lpfYAngle = (float)pi;
  }
  else
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + TOKEN1_XOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + TOKEN1_ZOFFSET;
    *lpfYAngle = -(float)(pi / 2.0);
  }


  // SPECIAL - corner anims were set up from the jail perspective - back up 10 squares.  Could just move the case entry to the correct spot.
  if( OFFSETS_GO_FP == UDPEICES_PROPERTY_TYPE_FOR_RESTING[nBoardTile] )//||
    //OFFSETS_JUSTVIS == UDPEICES_PROPERTY_TYPE_FOR_RESTING[nBoardTile] )
  {
    //nBoardTile = 20;//(nBoardTile + 30) % 40;      
    //*lpfYAngle -= (float)(pi / 2.0);// This looks good idling, but anims actually do spin to look back!
  }

  switch( nBoardTile )
  {// Use the appropriate offset table.  Tiletype is stored for conditional token rotations later.
  // side 1
  case SQ_GO:
    *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_GO_FP][nRestingPosition][0];
    *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_GO_FP][nRestingPosition][1];
    tileType = OFFSETS_GO_FP;
    break;

  case SQ_MEDITERRANEAN_AVENUE:
  case SQ_BALTIC_AVENUE:
  case SQ_ORIENTAL_AVENUE:
  case SQ_VERMONT_AVENUE:
  case SQ_CONNECTICUT_AVENUE:
    *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][0];
    *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][1];
    tileType = OFFSETS_PROPERTY;
    break;

  case SQ_COMMUNITY_CHEST_1:
  case SQ_INCOME_TAX:
  case SQ_READING_RR:
  case SQ_CHANCE_1:
    *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][0];
    *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][1];
    tileType = OFFSETS_RR_UT_CH;
    break;

  // Side 2
  case SQ_JUST_VISITING:
    // Note we add offsets like side 1, the anims were thustly done.
    if( nBoardTileOriginal == 40 )
    {// Special - in jail is considered just visiting right down to here - use the correct lookup.
      *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_INJAIL][nRestingPosition][0];
      *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_INJAIL][nRestingPosition][1];
      tileType = OFFSETS_INJAIL;
      //TRACE("IN JAIL OFFSETS");
    } else
    {
      *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_JUSTVIS][nRestingPosition][0];
      *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_JUSTVIS][nRestingPosition][1];
      tileType = OFFSETS_JUSTVIS;
    }
    break;

  case SQ_ST_CHARLES_PLACE:
  case SQ_STATES_AVENUE:
  case SQ_VIRGINIA_AVENUE:
  case SQ_ST_JAMES_PLACE:
  case SQ_TENNESSEE_AVENUE:
  case SQ_NEW_YORK_AVENUE:
    *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][1];
    *lpfZCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][0];
    tileType = OFFSETS_PROPERTY;
    break;

  case SQ_ELECTRIC_COMPANY:
  case SQ_PENNSYLVANIA_RR:
  case SQ_COMMUNITY_CHEST_2:
    *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][1];
    *lpfZCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][0];
    tileType = OFFSETS_RR_UT_CH;
    break;

  // side 3
  case SQ_FREE_PARKING:
  case SQ_GO_TO_JAIL:// Never idle there, but for completion(this would be wrong value)...
    *lpfXCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_GO_FP][nRestingPosition][0];
    *lpfZCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_GO_FP][nRestingPosition][1];
    tileType = OFFSETS_GO_FP;
    break;

  case SQ_KENTUCKY_AVENUE:
  case SQ_INDIANA_AVENUE:
  case SQ_ILLINOIS_AVENUE:
  case SQ_ATLANTIC_AVENUE:
  case SQ_VENTNOR_AVENUE:
  case SQ_MARVIN_GARDENS:
    *lpfXCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][0];
    *lpfZCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][1];
    tileType = OFFSETS_PROPERTY;
    break;

  case SQ_CHANCE_2:
  case SQ_BnO_RR:
  case SQ_WATER_WORKS:
    *lpfXCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][0];
    *lpfZCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][1];
    tileType = OFFSETS_RR_UT_CH;
    break;

  // side 4
  case SQ_PACIFIC_AVENUE:
  case SQ_NORTH_CAROLINA_AVENUE:
  case SQ_PENNSYLVANIA_AVENUE:
  case SQ_PARK_PLACE:
  case SQ_BOARDWALK:
    *lpfXCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][1];
    *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_PROPERTY][nRestingPosition][0];
    tileType = OFFSETS_PROPERTY;
    break;

  case SQ_COMMUNITY_CHEST_3:
  case SQ_SHORT_LINE_RR:
  case SQ_CHANCE_3:
  case SQ_LUXURY_TAX:
    *lpfXCoordinate -= UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][1];
    *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_RR_UT_CH][nRestingPosition][0];
    tileType = OFFSETS_RR_UT_CH;
    break;

  // side (5)
  case SQ_IN_JAIL:
    *lpfXCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_INJAIL][nRestingPosition][0];
    *lpfZCoordinate += UDPIECES_RESTING_IDLE_OFFSETS[OFFSETS_INJAIL][nRestingPosition][1];
    tileType = OFFSETS_INJAIL;
    break;

  case SQ_OFF_BOARD:
  default:
    tileType = 0;
    break;
  }


  if( nToken == TK_GUN || nToken == TK_CAR || nToken == TK_DOG || nToken == TK_SHIP || 
    nToken == TK_SHOE || nToken ==  TK_BARROW )// Tokens which have extra rotation
  {
    //*lpfYAngle += 2*pi*(UDPIECES_RESTING_IDLE_OFFSETS[tileType][nRestingPosition][2])/360; or:
    *lpfYAngle += (float)(pi*(UDPIECES_RESTING_IDLE_OFFSETS[tileType][nRestingPosition][2])/180);
  }

  //TRACE( "Tile %d, position %d, angle %f \n", tileType, nRestingPosition, *lpfYAngle );
  
}


/*****************************************************************************
* the correct rotation and position for hotels given the tile number
*/
void UDPIECES_GetHotelPosition(
  int nBoardTile,
  float *lpfXCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle)
{
  if (nBoardTile == SQ_IN_JAIL)
    nBoardTile = SQ_JUST_VISITING;
  if (nBoardTile < SQ_JUST_VISITING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + HOTEL_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - HOTEL_XOFFSET;
    *lpfYAngle = 0.0f;
  }
  else if (nBoardTile < SQ_FREE_PARKING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - HOTEL_XOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - HOTEL_ZOFFSET;
    *lpfYAngle = (float)(pi / 2.0);
  }
  else if (nBoardTile < SQ_GO_TO_JAIL)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - HOTEL_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + HOTEL_XOFFSET;
    *lpfYAngle = (float)pi;
  }
  else
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + HOTEL_XOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + HOTEL_ZOFFSET;
    *lpfYAngle = -(float)(pi / 2.0);
  }
}


/****************************************************************************/
/*****************************************************************************
* the correct rotation and position for hotels given the tile number
*/
void UDPIECES_GetHousePosition(
  int nBoardTile,
  int nHouse,
  float *lpfXCoordinate,
  float *lpfZCoordinate,
  float *lpfYAngle)
{
  float fXCoordinate;

  switch (nHouse)
  {
    case 0:
      fXCoordinate = HOUSE0_XOFFSET;
      break;
    case 1:
      fXCoordinate = HOUSE1_XOFFSET;
      break;
    case 2:
      fXCoordinate = HOUSE2_XOFFSET;
      break;
    case 3:
      fXCoordinate = HOUSE3_XOFFSET;
      break;
  }

  if (nBoardTile == SQ_IN_JAIL)
    nBoardTile = SQ_JUST_VISITING;
  if (nBoardTile < SQ_JUST_VISITING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + HOUSE_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - fXCoordinate;
    *lpfYAngle = 0.0f;
  }
  else if (nBoardTile < SQ_FREE_PARKING)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - fXCoordinate ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] - HOUSE_ZOFFSET;
    *lpfYAngle = (float)(pi / 2.0);
  }
  else if (nBoardTile < SQ_GO_TO_JAIL)
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] - HOUSE_ZOFFSET ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + fXCoordinate;
    *lpfYAngle = (float)pi;
  }
  else
  {
    *lpfXCoordinate = pt3DBoardLocations[nBoardTile][0] + fXCoordinate ;
    *lpfZCoordinate = pt3DBoardLocations[nBoardTile][2] + HOUSE_ZOFFSET;
    *lpfYAngle = -(float)(pi / 2.0);
  }
}


/*****************************************************************************
* 3 functions to pick appropriate camera views for 3, 5 & 15 ahead views
* the 3 ahead will return a corner view if passed a corner square,
* the 5 ahead may choose the corner view or the next 5 ahead.
*/
int UDPIECES_PickCameraFor3Squares( int square )
{
  int nViewLandingAt3;

  switch (square)
  {
  case SQ_GO:
    nViewLandingAt3 = VIEW2D16_CORNER_GO;
    break;
  case SQ_MEDITERRANEAN_AVENUE:
  case SQ_COMMUNITY_CHEST_1:
  case SQ_BALTIC_AVENUE:
    nViewLandingAt3 = VIEW2D04_3TILES01;
    break;
  case SQ_INCOME_TAX:
  case SQ_READING_RR:
  case SQ_ORIENTAL_AVENUE:
    nViewLandingAt3 = VIEW2D05_3TILES02;
    break;
  case SQ_CHANCE_1:
  case SQ_VERMONT_AVENUE:
  case SQ_CONNECTICUT_AVENUE:
    nViewLandingAt3 = VIEW2D06_3TILES03;
    break;
  case SQ_JUST_VISITING:
    nViewLandingAt3 = VIEW2D17_CORNER_JAIL;
    break;
  case SQ_ST_CHARLES_PLACE:
  case SQ_ELECTRIC_COMPANY:
  case SQ_STATES_AVENUE:
    nViewLandingAt3 = VIEW2D07_3TILES04;
    break;
  case SQ_VIRGINIA_AVENUE:
  case SQ_PENNSYLVANIA_RR:
  case SQ_ST_JAMES_PLACE:
    nViewLandingAt3 = VIEW2D08_3TILES05;
    break;
  case SQ_COMMUNITY_CHEST_2:
  case SQ_TENNESSEE_AVENUE:
  case SQ_NEW_YORK_AVENUE:
    nViewLandingAt3 = VIEW2D09_3TILES06;
    break;
  case SQ_FREE_PARKING:
    nViewLandingAt3 = VIEW2D18_CORNER_FREEPARKING;
    break;
  case SQ_KENTUCKY_AVENUE:
  case SQ_CHANCE_2:
  case SQ_INDIANA_AVENUE:
    nViewLandingAt3 = VIEW2D10_3TILES07;
    break;
  case SQ_ILLINOIS_AVENUE:
  case SQ_BnO_RR:
  case SQ_ATLANTIC_AVENUE:
    nViewLandingAt3 = VIEW2D11_3TILES08;
    break;
  case SQ_VENTNOR_AVENUE:
  case SQ_WATER_WORKS:
  case SQ_MARVIN_GARDENS:
    nViewLandingAt3 = VIEW2D12_3TILES09;
    break;
  case SQ_GO_TO_JAIL:
    nViewLandingAt3 = VIEW2D19_CORNER_GOTOJAIL;
    break;
  case SQ_PACIFIC_AVENUE:
  case SQ_NORTH_CAROLINA_AVENUE:
  case SQ_COMMUNITY_CHEST_3:
    nViewLandingAt3 = VIEW2D13_3TILES10;
    break;
  case SQ_PENNSYLVANIA_AVENUE:
  case SQ_SHORT_LINE_RR:
  case SQ_CHANCE_3:
    nViewLandingAt3 = VIEW2D14_3TILES11;
    break;
  case SQ_PARK_PLACE:
  case SQ_LUXURY_TAX:
  case SQ_BOARDWALK:
    nViewLandingAt3 = VIEW2D15_3TILES12;
    break;
  case SQ_IN_JAIL:
    nViewLandingAt3 = VIEW2D17_CORNER_JAIL;
    break;
  case SQ_OFF_BOARD:
  default:
    nViewLandingAt3 = VIEW2D16_CORNER_GO;
    break;
  }
  return( nViewLandingAt3 );
}

int UDPIECES_PickCameraFor5Squares( int square )
{// This function is normally called giving the destination square - return best view where the token
  // walks into the scene.  We wont do corners, rather let the token walk into or out of scene.
  int nViewLandingAt5;

  switch (square)
  {// NOTE: the camera views suck - the corners are not there, the railroads are doubled up.
  case SQ_GO:
  case SQ_MEDITERRANEAN_AVENUE:
  case SQ_COMMUNITY_CHEST_1:
  case SQ_BALTIC_AVENUE:
  case SQ_INCOME_TAX:
  case SQ_READING_RR:
    nViewLandingAt5 = VIEW2D20_5TILES01;
    break;
  case SQ_ORIENTAL_AVENUE:
  case SQ_CHANCE_1:
  case SQ_VERMONT_AVENUE:
  case SQ_CONNECTICUT_AVENUE:
    nViewLandingAt5 = VIEW2D21_5TILES02;
    break;
  case SQ_JUST_VISITING:
  case SQ_ST_CHARLES_PLACE:
  case SQ_ELECTRIC_COMPANY:
  case SQ_STATES_AVENUE:
  case SQ_VIRGINIA_AVENUE:
  case SQ_PENNSYLVANIA_RR:
    nViewLandingAt5 = VIEW2D22_5TILES03;
    break;
  case SQ_ST_JAMES_PLACE:
  case SQ_COMMUNITY_CHEST_2:
  case SQ_TENNESSEE_AVENUE:
  case SQ_NEW_YORK_AVENUE:
    nViewLandingAt5 = VIEW2D23_5TILES04;
    break;
  case SQ_FREE_PARKING:
  case SQ_KENTUCKY_AVENUE:
  case SQ_CHANCE_2:
  case SQ_INDIANA_AVENUE:
  case SQ_ILLINOIS_AVENUE:
  case SQ_BnO_RR:
    nViewLandingAt5 = VIEW2D24_5TILES05;
    break;
  case SQ_ATLANTIC_AVENUE:
  case SQ_VENTNOR_AVENUE:
  case SQ_WATER_WORKS:
  case SQ_MARVIN_GARDENS:
    nViewLandingAt5 = VIEW2D25_5TILES06;
    break;
  case SQ_GO_TO_JAIL:
  case SQ_PACIFIC_AVENUE:
  case SQ_NORTH_CAROLINA_AVENUE:
  case SQ_COMMUNITY_CHEST_3:
  case SQ_PENNSYLVANIA_AVENUE:
  case SQ_SHORT_LINE_RR:
    nViewLandingAt5 = VIEW2D26_5TILES07;
    break;
  case SQ_CHANCE_3:
  case SQ_PARK_PLACE:
  case SQ_LUXURY_TAX:
  case SQ_BOARDWALK:
    nViewLandingAt5 = VIEW2D27_5TILES08;
    break;
  case SQ_IN_JAIL:
    nViewLandingAt5 = VIEW2D17_CORNER_JAIL;// It is inappropriate for this call to be given this square, but just in case.
    break;
  case SQ_OFF_BOARD:
  default:
    nViewLandingAt5 = VIEW2D16_CORNER_GO;
    break;
  }
  return( nViewLandingAt5 );
}

int UDPIECES_PickCameraFor15Squares( int square )
{
  int nViewStartingAt15;

  switch (square)
  {// Note: This should already be the current camera (from roll dice) unless the dice roll changes it.
  case SQ_GO:
  case SQ_MEDITERRANEAN_AVENUE:
  case SQ_COMMUNITY_CHEST_1:
  case SQ_BALTIC_AVENUE:
    nViewStartingAt15 = VIEW2D28_15TILES01;
    break;
  case SQ_INCOME_TAX:
  case SQ_READING_RR:
  case SQ_ORIENTAL_AVENUE:
    nViewStartingAt15 = VIEW2D29_15TILES02;
    break;
  case SQ_CHANCE_1:
  case SQ_VERMONT_AVENUE:
  case SQ_CONNECTICUT_AVENUE:
    nViewStartingAt15 = VIEW2D30_15TILES03;
    break;
  case SQ_JUST_VISITING:
  case SQ_ST_CHARLES_PLACE:
  case SQ_ELECTRIC_COMPANY:
  case SQ_STATES_AVENUE:
    nViewStartingAt15 = VIEW2D31_15TILES04;
    break;
  case SQ_VIRGINIA_AVENUE:
  case SQ_PENNSYLVANIA_RR:
  case SQ_ST_JAMES_PLACE:
    nViewStartingAt15 = VIEW2D32_15TILES05;
    break;
  case SQ_COMMUNITY_CHEST_2:
  case SQ_TENNESSEE_AVENUE:
  case SQ_NEW_YORK_AVENUE:
    nViewStartingAt15 = VIEW2D33_15TILES06;
    break;
  case SQ_FREE_PARKING:
  case SQ_KENTUCKY_AVENUE:
  case SQ_CHANCE_2:
  case SQ_INDIANA_AVENUE:
    nViewStartingAt15 = VIEW2D34_15TILES07;
    break;
  case SQ_ILLINOIS_AVENUE:
  case SQ_BnO_RR:
  case SQ_ATLANTIC_AVENUE:
    nViewStartingAt15 = VIEW2D35_15TILES08;
    break;
  case SQ_VENTNOR_AVENUE:
  case SQ_WATER_WORKS:
  case SQ_MARVIN_GARDENS:
    nViewStartingAt15 = VIEW2D36_15TILES09;
    break;
  case SQ_GO_TO_JAIL:
  case SQ_PACIFIC_AVENUE:
  case SQ_NORTH_CAROLINA_AVENUE:
  case SQ_COMMUNITY_CHEST_3:
    nViewStartingAt15 = VIEW2D37_15TILES10;
    break;
  case SQ_PENNSYLVANIA_AVENUE:
  case SQ_SHORT_LINE_RR:
  case SQ_CHANCE_3:
    nViewStartingAt15 = VIEW2D38_15TILES11;
    break;
  case SQ_PARK_PLACE:
  case SQ_LUXURY_TAX:
  case SQ_BOARDWALK:
    nViewStartingAt15 = VIEW2D39_15TILES12;
    break;
  case SQ_IN_JAIL:
    nViewStartingAt15 = VIEW2D17_CORNER_JAIL;
    break;
  case SQ_OFF_BOARD:
  default:
    nViewStartingAt15 = VIEW2D16_CORNER_GO;
  }
  return( nViewStartingAt15 );
}

int UDPIECES_PickAGoodCamera( int square, int distance )
{// Note the first move will be 15 forward, the last is followed by a 3 square view.  This
  // routine will be called for everything else.  Note Custom cam can ignore these.
  int distanceToCorner = 10 - (square % 10);

  if( distanceToCorner == 10 && distance == 0 )
  {// Just a kitty corner, CX0, we stay on the start corner so distance to corner is 0 not 10 (when calculating corner cam)
    distanceToCorner = 0;
  }

  if( distance >= 9 )
  {// Long run.  The standard logic would only see this on a stright run ending on a corner.
    return( UDPIECES_PickCameraFor15Squares( square ) );
  }

  if( distanceToCorner < distance || distance == 0 )// Distance == 0 is the CX0 move - spin the corner on the corner square.
  {// Corner run - show the corner.
    switch( (int)((square + distanceToCorner) / 10) )
    {
    case 1:  return( VIEW2D17_CORNER_JAIL ); break;
    case 2:  return( VIEW2D18_CORNER_FREEPARKING ); break;
    case 3:  return( VIEW2D19_CORNER_GOTOJAIL ); break;
    default: return( VIEW2D16_CORNER_GO ); break; // 4.
    }
  } else
  {// Whatever is left is 'probably' going to land on the final spot, our camera would be followed
    // by the 3 square view.  Use a 5 square view of the destination.
    // NOTE: The 3 view looks bad when it moves 'backward' on the board.  Use the 5 view before it
    //  if possible, or just the 3 view itself or previous.
    if( (square + distance) % 10 <= 3 )
    {// this one looks funny following the 5 view - the 3 view moves the camera backward
      return( UDPIECES_PickCameraFor3Squares( (square + distance) % 40 ) ); // Should be the same as the final camera.
    }
    // The 5 view would look funny if you were on the first square in the view - most of the anim
    // would be offscreen.  I will compensate with a -1 on the view calculation, it may need more logic.
    return( UDPIECES_PickCameraFor5Squares( (square + distance - 1) % 40 ) );
  }
}


/*****************************************************************************
* UDPIECES_PlanMoveAnim - Plan animation sequence to move a token.
*/
void UDPIECES_PlanMoveAnim( int player, int rule_movecommand, int SQbefore, int SQafter )
{
  int StackIndex, SQIndex, idest, itemp;
  int nDiceRoll;
  int nSideBefore, nSideAfter, numCornersPassed;
  //int nFirstAnimLength;
  //int nLongMoveLength;
  int nViewLandingAt3;//, nViewLandingAt5;
  int nViewStartingAt15;

  LE_DATA_DataId TokenCornerAnimBaseID;
  LE_DATA_DataId TokenMoveAnimBaseID;

  // calculate dice roll
  nDiceRoll = SQafter - SQbefore;
  // check for past GO
  if (nDiceRoll < 0)
    nDiceRoll +=40;

  //FIXME - if the stack is not empty, what should we do? Is it possible to get a second move before the last is fully processed?  What happens?

  // get proper animation IDs for this token
  TokenCornerAnimBaseID = CNK_knacx0 + UICurrentGameState.Players[player].token * ANIMS_PER_TOKEN;
  TokenMoveAnimBaseID   = CNK_knamx0 + UICurrentGameState.Players[player].token * ANIMS_PER_TOKEN;

  nViewStartingAt15 = UDPIECES_PickCameraFor15Squares( SQbefore );
  nViewLandingAt3   = UDPIECES_PickCameraFor3Squares ( SQafter  );


  StackIndex = 0;
  switch( rule_movecommand )
  {
  case NOTIFY_MOVE_BACKWARDS: // Only happens with 'go back 3 spaces' - stack those anims when available.
    DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
    DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
    DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = nViewLandingAt3;
    DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQafter;
    // Run the proper 'back 3' anim for this square.
    itemp = 0;                    //income tax
    if( SQafter > 10 ) itemp = 1; //new york
    if( SQafter > 30 ) itemp = 2; //cc near park place 
    DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, CNK_knamxb3a + itemp + UICurrentGameState.Players[player].token * ANIMS_PER_TOKEN );
    StackIndex++;

    DISPLAY_state.TokenAnimStackTop = StackIndex - 1;
    DISPLAY_state.TokenAnimStackIndex = 0;
    gameQueueLock();
    break;

  case NOTIFY_JUMP_TO_SQUARE:
    if( SQafter == 40 )
    {// Special jail animation - DOES NOT USE THE SEQUENCE STACK BUT ITS OWN LOGIC.
      gameQueueLock();
      DISPLAY_state.GoingToJailStatus = 1;
      DISPLAY_state.GoingToJailFromSquare = SQbefore;
      // We have to store the 'from' location for the animation sequence
      break;
    }
    if( SQafter == 41 )
    {// Off board - happens when bankrupt & for last player on board
      BOOL test;
      int t, itemp;
      // Assume bankrupt for now
      // We must test for this being the last player ourself.
      // We jump to the square before being informed that the game is over.  Start the victory lap now, that message will bring up the game end buttons
      for( t = 0, itemp = 0; t < UICurrentGameState.NumberOfPlayers; t++ )
      {
        if( UICurrentGameState.Players[t].currentSquare == 41 ) itemp++;
      }
      if( itemp >= UICurrentGameState.NumberOfPlayers -1 )
        test = TRUE;// We are the last token standing.
      else 
        test = FALSE;

      if( !test )
      {// Bankrupt - zoom in, blast off (10 ahead anim) staying zoomed - token will disappear as idle ignores square 41.
        DISPLAY_state.CurrentPlayerInCenterIdle = -1; // Cause we vaporize.  May not need to set this, just a location indicator.

        //TRACE("Bankrupt animation, from %d to %d currentUIPlayer %d\n", SQbefore, SQafter, CurrentUIPlayer );
        // Stack a zoom with the token idle (camera only dosen't work except for last item)
        DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
        DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickCameraFor3Squares( SQbefore );
        DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID );
        DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
        StackIndex++;

        // A second idle to kill time
        DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
        DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickCameraFor3Squares( SQbefore );
        DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID );
        DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
        StackIndex++;

        // Now shoot the token away
        DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
        DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = DISPLAY_state.TokenAnimStack[StackIndex-1].PresetCameraView;
        DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID ) + 8;//8;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQbefore;// Lie to it, no camera move.
        DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE; // Sort of, distance 0 will identify the CX0 move.
        StackIndex++;

        // Push the final camera onto the stack.  Just seeing if it helps the rest of the animation...
        DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = TRUE;
        DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
        DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = 1;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQafter;
        DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_EI;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQafter;
        DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
        StackIndex ++;
        
        DISPLAY_state.TokenAnimStackTop = StackIndex - 1;
        DISPLAY_state.TokenAnimStackIndex = 0;
        gameQueueLock();

        if( SlotIsALocalHumanPlayer[player] )
          UDPENNY_PennybagsGeneral( PB_Bankrupt, Sound_WaitForAnyOldSoundThenPlay );
        else
        {
          if( rand()%2 == 0 )
            UDSOUND_TokenGenericPlaySound( UICurrentGameState.Players[player].token, TKS_GiveUp, Sound_WaitForAnyOldSoundThenPlay );
          else
            UDSOUND_TokenGenericPlaySound( UICurrentGameState.Players[player].token, TKS_Bankrupt, Sound_WaitForAnyOldSoundThenPlay );
        }
      } else
      {// End of game - victory animation
        // Go to corner & turn it
        //TRACE("Win animation, from %d to %d currentUIPlayer %d\n", SQbefore, SQafter, CurrentUIPlayer );
        // Stack a zoom with the token idle (camera only dosen't work except for last item)
        itemp = 10 - (SQbefore % 10); // Next corner
        if( itemp == 0 ) itemp = 10; // Go to next corner
        // Trim to landing square being careful that we may go around the board and land a couple squares behind the where we started.
        //if( itemp == 40 ) itemp = 0; // Special case GO

        DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
        DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickCameraFor15Squares( SQbefore );
        DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID ) + itemp;
        DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQbefore;
        DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
        StackIndex++;

        SQbefore += itemp;
        if( SQbefore >= 40) SQbefore = 0;

        if( itemp != 10 )
        {// spin around the corner
          DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
          DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
          DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQbefore;
          // Since we are spinning the corner, this might give us too much camera movement.  Adjust if required.
          DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickCameraFor15Squares( SQbefore );
          DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenCornerAnimBaseID );
          DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQbefore;
          DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = TRUE;
          StackIndex++;
        }

        // Now, set up the looping 'around the board' animation
        DISPLAY_state.TokenLoopAnimTop = StackIndex;
        if( SQbefore%10 != 0 )
          SQbefore = SQbefore;// Debug catch
        for( t = 0; t < 4; t++ )
        {
          DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
          DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
          DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQbefore;
          DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickCameraFor15Squares( SQbefore );
          DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID ) + 10;
          DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQbefore;
          DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
          StackIndex++;

          SQbefore += 10;
          if( SQbefore >= 40) SQbefore = 0;
        }
        DISPLAY_state.TokenLoopAnimEnd = StackIndex;
        
        // FIXME - initialize this crud each game
        DISPLAY_state.IsVictoryAnim = TRUE;
        DISPLAY_state.isVictoryGameLockReleased = FALSE;
        DISPLAY_state.VictoryToken = UICurrentGameState.Players[player].token;

        DISPLAY_state.TokenAnimStackTop = StackIndex - 1;
        DISPLAY_state.TokenAnimStackIndex = 0;
        gameQueueLock();

        // Sound 
        //if( SlotIsALocalHumanPlayer[player] )
        UDPENNY_PennybagsGeneral( PB_GameOver, Sound_WaitForAnyOldSoundThenPlay );
      };
      break;
      
    }


    if( SQafter == 10 && SQbefore == 40 )
    {// Got out of jail
      DISPLAY_state.GoingToJailStatus = 0;//debug catch - supress original moving idle hop, do it now.
      break;
    }
    // Let the move forwards code handle any other 'jump to' squares


  case NOTIFY_MOVE_FORWARDS:
    if( DISPLAY_state.IsOptionTokenAnimationsOn )
    {
      // calculate start side and end side.  A corner position is considered the next side at start, the previous at end.
      if( SQbefore < 40 )
        nSideBefore = SQbefore / 10;
      else // Jail or off board
        nSideBefore = 1;
      
      if( SQafter < 40 )
        nSideAfter = ( SQafter - 1 ) / 10; // Back up one - a corner landing should not be considered a new side, though a corner 0 turn will terminate the anim.
      else // Jail or off board
        nSideAfter = 0;// FIXME - we will have to see how 'getting out of jail' looks...
      if(SQafter - 1 < 0 ) nSideAfter = 3; // Special case - go itself.
      
      numCornersPassed = nSideAfter - nSideBefore;
      if( numCornersPassed < 0  )
        numCornersPassed += 4; // Passed go.
      if( numCornersPassed == 0 && SQbefore > SQafter )
        numCornersPassed = 4; // almost all around the board?
      {// Move forward - recursive logic in the while loop
        SQIndex = SQbefore;
        BOOL isFirst = TRUE;
        
        while( SQIndex != SQafter && StackIndex < (TOKEN_ANIM_ITEMS_MAX - 1) )
        {// Evaluate moves, stack em.
          // First up, calculate the distance to the next corner or landing square, whichever is closer.
          idest = SQIndex + ( 10 - (SQIndex % 10) ); // Next corner
          // Trim to landing square being careful that we may go around the board and land a couple squares behind the where we started.
          if( idest > SQafter && SQIndex < SQafter ) idest = SQafter; // The corner is farther than the landing destination which is ahead of us.
          if( idest == 40 ) idest = 0; // Special case GO
          
          // Next, if the destination crosses a corner we can modify the move to jump the corner.
          // We will use this technique if we only cross one corner or the kitty corner lands on
          //   the destination (1 or 2 squares up the next side).
          if( idest != SQafter )
          { // If the landing spot is not from 'here' to corner, we must cross the corner.
            if( (idest == SQafter - 1) || (idest == SQafter - 2) ||
              (idest == SQafter - 1 + 40) || (idest == SQafter - 2 + 40) ) // Accomodate crossing GO
            {// Kitty corner will land on destination after multi corner run.
              // If we can immedately jump the corner (are within 1 or 2 squares of corner) jump else move to jump position.
              if( 10 - (SQIndex % 10) > 2 )
                idest = idest - 1 - rand()%2; // **Move back 1 or 2 squares, next sequence will jump.
              else
                idest = SQafter; // ** We can reach the end now with one kitty corner jump.
            } else if( numCornersPassed == 1 )
            {// We will still do a kitty corner if this is the only corner travelled.  We know we don't land on the final square.
              if( 10 - (SQIndex % 10) > 2 )
                idest = idest - 1 - rand()%2; // **Move back 1 or 2 squares, next sequence will jump.
              else
                idest = idest + 1 + rand()%2; // **Hop over.  It might be nicer to only jump 1 if the dest square is only 3 or 4 away.
            }
            while( idest < 0 ) idest += 40;
            while( idest >=40) idest -= 40;
          }// End if crossing corner.
          
          
          // Now to stack it - we re-evaluate if a corner was crossed to determine the sequence & camera to use.
          DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
          DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
          DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQIndex;
          DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = idest;
          DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
          
          itemp = idest - SQIndex;
          if( itemp < 0 ) itemp += 40; // crossed go.
          if( (int)((idest+10-1)/10)%4 != (int)((SQIndex+10)/10)%4 ) // The dest corner is not considered the next side.  Add 10 to handle GO dest.
          {// Crossed corner
            DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = TRUE;
            if( isFirst )
              DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = nViewStartingAt15;
            else
              DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickAGoodCamera( SQIndex, itemp );
            if( itemp == 2 )
              DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenCornerAnimBaseID + 1);
            else if( itemp == 4 )
              DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenCornerAnimBaseID + 4 );
            else
            {// we have a 2/1 or 1/2 move
              if( idest % 10 == 1 )
                DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenCornerAnimBaseID + 3 );
              else if( idest % 10 == 2 )
                DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenCornerAnimBaseID + 2 );
              else
                itemp += 0;// debug catch
            }
          } else
          {// No corner crossed.
            if( isFirst )
              DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = nViewStartingAt15;
            else
              DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickAGoodCamera( SQIndex, itemp );
            DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID + itemp );
            // Check for landing on a corner - substitute a 10 with corner anim or stack the 0 square corner rotation anim.
            if( idest % 10 == 0 )
            {
              if( itemp == 10 )
                DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenMoveAnimBaseID + 10 ); // This one would work out anyway - temp == 10, but for clarity...
              else
              {
                StackIndex++;// OK to do here, stacked a move now stacking a 0 move corner.
                DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = FALSE;
                DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
                DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = idest;
                // Since we are spinning the corner, this might give us too much camera movement.  Adjust if required.
                DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = UDPIECES_PickAGoodCamera( idest, 0 );
                DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_IFT( DAT_3D, TokenCornerAnimBaseID );
                DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = idest;
                DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = TRUE; // Sort of, distance 0 will identify the CX0 move.
              }
            }
          }
          
          SQIndex += itemp;
          while( SQIndex >=40 ) SQIndex -= 40;
          StackIndex++;
          isFirst = FALSE;
        }
      }
    }// End if anims on
    
    // Push the final camera onto the stack.
    DISPLAY_state.TokenAnimStack[StackIndex].isCameraMoveOnly = TRUE;
    DISPLAY_state.TokenAnimStack[StackIndex].timeForCameraMoveOnly = 0;
    DISPLAY_state.TokenAnimStack[StackIndex].PresetCameraView = nViewLandingAt3;// Final destination.
    DISPLAY_state.TokenAnimStack[StackIndex].SquareStartingNewSequence = SQafter;
    DISPLAY_state.TokenAnimStack[StackIndex].NewSequenceID = LED_EI;
    DISPLAY_state.TokenAnimStack[StackIndex].SquareLandingNewSequence = SQafter;
    DISPLAY_state.TokenAnimStack[StackIndex].IsCorner = FALSE;
    StackIndex ++;
    
    DISPLAY_state.TokenAnimStackTop = StackIndex - 1;
    DISPLAY_state.TokenAnimStackIndex = 0;
    gameQueueLock();
    break;

    }

#if 0
  // Preload backdrops (2D only)
  int checkForUniqueBackdrops[UDBoard_2D_constructedBoardBufferMax];
  int checkIndex, check, s;

  // Removed - may be better to wait for each as it comes up rather than miss the whole first anim.
  checkForUniqueBackdrops[0] = DISPLAY_state.desired2DCamera;  // The current backdrop should be loaded already (it is displayed, after all...)
  checkIndex = 1;
  //for( t = 0; t <= min(UDBoard_2D_constructedBoardBufferMax - 2, DISPLAY_state.TokenAnimStackTop ); t++ )
  //  UDBOARD_PreLoadBackdrop( DISPLAY_state.TokenAnimStack[t].PresetCameraView );
  if( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA) && !DISPLAY_state.game3Don )
  {
    for( int t = 0; t <= DISPLAY_state.TokenAnimStackTop; t++ )
    {
      check = FALSE;
      for ( s = 0; s < checkIndex; s++ )
      {
        if( checkForUniqueBackdrops[s] == DISPLAY_state.TokenAnimStack[t].PresetCameraView )
          check = TRUE;
      }
      if( !check )
      {
        UDBOARD_PreLoadBackdrop( DISPLAY_state.TokenAnimStack[t].PresetCameraView );
        checkForUniqueBackdrops[checkIndex++] = DISPLAY_state.TokenAnimStack[t].PresetCameraView;
      }
      if( checkIndex >= UDBoard_2D_constructedBoardBufferMax )
      {
        // Full, cant buffer more.
        break;
      }
    }
  }
#endif
};


/*****************************************************************************
* UDPIECES_StartOfGameSetup
*/
void UDPIECES_StartOfGameSetup( void )
{
  int t, s;

  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  { 
    for( s = 0; s < RULE_MAX_PLAYERS; s++ )
    {
      DISPLAY_state.Player3DTokenIdlePos[t][s] = -1; // Flag that there are no tokens on the board
    }
  }
  DISPLAY_state.CurrentPlayerInCenterIdle = -1;
  DISPLAY_state.PlayerMovingIn  = -1;
  DISPLAY_state.PlayerMovingOut = -1;

  DISPLAY_state.GoingToJailStatus = 0;
  DISPLAY_state.PlayerInPaddywagon = -1;

  // Clear the buffers (in case the 2D board has changed)
  for( t = 0; t < UDBoard_2D_constructedBoardBufferMax; t++ )
  {
    UDBoard_2DLoadedViews[t].ViewLoaded = -1;
    UDBoard_2DLoadedViews[t].TimeLoaded = 0;
  }
}


/*****************************************************************************
* UDPIECES_UpdateShadow - Updates the location & rotation of the shadow
* Give the player not the token (in case we need player info),
* This is a fairly sensitive function, figuring out if the shadow is visible.
*/
void UDPIECES_UpdateShadows( void )
{
  static LE_DATA_DataId CurrentShadow[6] = { 0, 0, 0, 0, 0, 0 };
  float fXCoordinate, fYCoordinate, fZCoordinate, fYAngle;
  BOOL visible;
  int t;

  for( t = 0; t < 6; t++ )
  {
    // enhance this to figure out if the token is visible, then GetTokenActualOrientation uses similar logic to figure out which sequence to interrogate.
    visible = DISPLAY_IsBoardVisible && (t < UICurrentGameState.NumberOfPlayers);
    //Only special case - inside paddywagon
    
    if( visible && DISPLAY_state.IsOptionLightingOn && 
      (t != DISPLAY_state.PlayerInPaddywagon || DISPLAY_state.GoingToJailStatus != 8) &&
      // Kill em if we are in software (2D board is enough, doesn't look perfect) or no 8 bit palette support.
      DISPLAY_state.game3Don && //DISPLAY_state.supports8BitPalTextures &&
      // They could be bankrupt (drop em when the custom anim finishes)
      ( UICurrentGameState.Players[t].currentSquare < 41 || 
        (DISPLAY_state.TokenCurrent3DSequence != LED_EI && t == CurrentUIPlayer ))
      )
    {// Update
      Matrix mxResult(Matrix::IDENTITY);
      if( CurrentShadow[t] == LED_EI )
      {// Fire it up
        CurrentShadow[t] = LED_IFT( DAT_3D, /*CNK_knamx0*/CNK_shadowa ) + UICurrentGameState.Players[t].token;
        LE_SEQNCR_StartRySTxzDrop( CurrentShadow[t], DISPLAY_Generic3dPriority + t,
          0.0f, 1.0f, 0.0f, 0.0f, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( CurrentShadow[t], DISPLAY_Generic3dPriority + t,
          LE_SEQNCR_EndingActionStayAtEnd );
      }
      UDPIECES_GetTokenActualOrientation( t, &fXCoordinate, &fYCoordinate, &fZCoordinate, &fYAngle );
      //TRACE( "MOVE SHADOW %d to %f, %f, %f\n", UICurrentGameState.Players[t].token,
      //  fXCoordinate, fYCoordinate, fZCoordinate );
      //      LE_SEQNCR_MoveRySTxz( CurrentShadow[t], DISPLAY_Generic3dPriority + t, fYAngle, 1.0f, fXCoordinate, fZCoordinate );
      
      mxResult.IDENTITY;
      mxResult.RotateY( fYAngle );
      mxResult.Translate( D3DVECTOR(fXCoordinate,1,fZCoordinate) );
      LE_SEQNCR_MoveTheWorks( CurrentShadow[t], DISPLAY_Generic3dPriority + t, LED_EI,0,FALSE,TRUE,TRUE,3, &mxResult );
    } else
    {// Shutdown
      if( CurrentShadow[t] != LED_EI )
      {
        LE_SEQNCR_Stop( CurrentShadow[t], DISPLAY_Generic3dPriority + t );
        CurrentShadow[t] = LED_EI;
      }
    }
    
  }
  
}


