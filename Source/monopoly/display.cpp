/* Display.cpp:  Handles display issues for Monopoly
 * By Artech Digital Entertainments, copyright (c) 1999.
 */

#include "gameinc.h"
#include "..\PC3D\PC3DHdr.h"

/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

/* Map colours defined in RULE_PlayerColour enum to RGB colours */
int RULE_PlayerColourToRGB[MAX_PLAYER_COLOURS] =
{
  LEG_MCR(255,000,000), // Red
  LEG_MCR(000,000,255), // Blue
  LEG_MCR(060,150,060), // Green
  LEG_MCR(255,255,000), // Yellow
  LEG_MCR(255,000,255), // Purple
  LEG_MCR(255,128,000)  // Orange

  //LEG_MCR(240,143,149), // Red - colors from cashbox tabs
  //LEG_MCR(130,119,179), // Blue
  //LEG_MCR(175,239,144), // Green
  //LEG_MCR(255,253, 80), // Yellow
  //LEG_MCR(207, 96,255), // Purple
  //LEG_MCR(255,160, 98)  // Orange
};

DISPLAY_stateType DISPLAY_state;
DISPLAY_RenderSettingsStruct DISPLAY_RenderSettings;


int BoardLocations[42][2] =
{ // Upper left corners, 0 - 39 property locations.  40 jail, 41 off board.
  { 344, 344 }, // go
  { 312, 344 },
  { 279, 344 },
  { 248, 344 },
  { 216, 344 },
  { 183, 344 },
  { 151, 344 },
  { 119, 344 },
  {  87, 344 },
  {  55, 344 },

  {   3, 344 }, // jail
  {   3, 312 },
  {   3, 279 },
  {   3, 248 },
  {   3, 216 },
  {   3, 183 },
  {   3, 151 },
  {   3, 119 },
  {   3,  87 },
  {   3,  55 },

  {   3, 3 }, // Free parking
  {  55, 3 },
  {  87, 3 },
  { 119, 3 },
  { 151, 3 },
  { 183, 3 },
  { 215, 3 },
  { 248, 3 },
  { 279, 3 },
  { 312, 3 },

  { 344,   3 },
  { 344,  55 },
  { 344,  87 },
  { 344, 119 },
  { 344, 151 },
  { 344, 183 },
  { 344, 216 },
  { 344, 248 },
  { 344, 279 },
  { 344, 313 },

  {  17, 345 },
  { 314, 312 }
};


/*****************************************************************************
 * Initialize display
 */
BOOL __cdecl DISPLAY_initialize( void )
{
  int t;

  DISPLAY_state.system = NOTW_BORD_UK; // Initialize to 0 - only used in European boards (deeds/currency overlays)

  // Message box item.  This is also a debug crutch - too much text for final version
  if( !LE_FONTS_SetFont("Arial.ttf", "Arial") )
    LE_FONTS_SetFont(ANSI_FIXED_FONT_FILE, NULL);//FIXME Tim - substitute our alternate when ready.
  LE_FONTS_SetSize  ( 12 ); // pixels tall.
  LE_FONTS_SetWeight(700);
  LE_FONTS_SetItalic(FALSE);

  // FIXME - why waste memory, remove before release
  DISPLAY_state.MessageBoxID = LE_GRAFIX_ObjectCreate( DISPLAY_MessageBoxW, DISPLAY_MessageBoxH, LE_GRAFIX_ObjectNotTransparent);
  DISPLAY_state.isMessageBoxDisplayed = FALSE;
  DISPLAY_state.MessageBoxSuppressed = TRUE; // Debug box, only hotkeys will enable it.
  strcpy( DISPLAY_state.CurrentMessageText, "[None]" );
  DISPLAY_setMessageBox( "Welcome to Monopoly!" );

  // General variables
  //gameQueueUnLock();// In userifce.cpp
  DISPLAY_state.TokenAnimStackIndex = -1; // Flags no anim running.
  DISPLAY_state.TokenCurrent3DSequence = LED_EI;

  // Start up the mouse
  LE_SEQNCR_AddMouseSubSequenceTheWorks( LED_IFT( DAT_MAIN, TAB_pointer ), 100, 0, 0, TRUE);

  // Frame rate tracker
  DISPLAY_state.frameRateTrackerIndex = 0;
  DISPLAY_state.frameCount = 0; // frames this track
  DISPLAY_state.frameRateTrackerStartTime = 0; // For this track
  for( t = 0; t < 12; t++ )
    DISPLAY_state.frameRateTracker[t] = FRAME_RATE_MINIMUM * (FRAME_RATE_SAMPLE_SECONDS + 2); // So early stats dont get excited too easily.  plus 2 so a couple low rates wont make us too low.

  // set the options screen flags
  DISPLAY_state.IsOptionTokenVoicesOn = TRUE;
  DISPLAY_state.IsOptionHostCommentsOn = TRUE;
  DISPLAY_state.IsOptionTokenAnimationsOn = TRUE;
  DISPLAY_state.IsOption3DCameraMovementOn = TRUE;
  DISPLAY_state.IsOptionLightingOn = TRUE;
  DISPLAY_state.OptionScreenResIndex = 0;	// used to determine which resolution the user chose
  DISPLAY_state.IsOptionGameStarted = FALSE;
  // Read (already) in UDUTILS_ReadINIDisplaySettings:
  //DISPLAY_state.IsOptionMusicOn = TRUE;
  //DISPLAY_state.OptionMusicTuneIndex = TUNE_1;
  //DISPLAY_state.IsOption3DBoardOn = TRUE;
  //DISPLAY_state.IsOptionFilteringOn = TRUE;
  //DISPLAY_state.IsOptionDitheringOn = TRUE;

  // Call subfunction initializations
  DISPLAY_UDAUCT_Initialize();
  DISPLAY_UDBOARD_Initialize();
  DISPLAY_UDIBAR_Initialize();
  DISPLAY_UDOPTS_Initialize();
  DISPLAY_UDPIECES_Initialize();
  DISPLAY_UDPSEL_Initialize();
  DISPLAY_UDSOUND_Initialize();
  DISPLAY_UDSTATS_Initialize();
  DISPLAY_UDTRADE_Initialize();

  FORCE_HMD_USEDEFAULTSHADING(TRUE);

  // Get the rendering/display settings
  if (!UDUTILS_ReadINIDisplaySettings())
  {
    // Run the speed test
    if (UDUTILS_RenderSpeedTest())
    {
      // Now write out the new settings
      if (!UDUTILS_WriteINIDisplaySettings())
        TRACE("Failed to write the monopoly.INI render settings\n");
    }
    else  
      // If the render speed returns FALSE, something went wrong or the
      //  test was aborted.  As a result we do not want to continue with
      //  the initialization.
      return(FALSE);
  }

  // Set the display state stuff
  DISPLAY_state.IsOption3DBoardOn   = DISPLAY_RenderSettings.bUse3DBoard;
  DISPLAY_state.IsOptionDitheringOn = DISPLAY_RenderSettings.bClassic_UseDithering;
  DISPLAY_state.IsOptionFilteringOn = DISPLAY_RenderSettings.bClassic_UseBilinearFiltering;

  // Are the 3D boards turned on???
  if (DISPLAY_RenderSettings.bUse3DBoard)
  {
#if USA_VERSION
    WORD wBmpID = IDB_BACKGROUND;
#else
    WORD wBmpID = IDB_BACKGROUNDEURO;
#endif
    // Set the background for the 3D stuff
    LE_REND3D_SetBackgroundSurface(1, new Surface(MAKEINTRESOURCE(wBmpID)),
                                   DISPLAY_RenderSettings.bUseBackground);
  }
  else
  {
    // No 3D board
    DISPLAY_state.game3Don          = FALSE;
    DISPLAY_state.IsOption3DBoardOn = FALSE;
  }

  // Set the monopoly callback for when the application is paused
  ASSERT(NULL != pc3D::m_pMainWnd);
  pc3D::m_pMainWnd->SetPauseCallback(UDUTILS_AppPauseCallback, NULL);

  DISPLAY_showAll;

  return(TRUE);
}


/*****************************************************************************
 * Destroy display - stop all graphics and deallocate display memory objects
 */
void DISPLAY_destroy( void )
{
  UDBOARD_SetBackdrop( DISPLAY_SCREEN_Black );
#if CE_ARTLIB_EnableMultitasking
    LE_SEQNCR_WaitForFullUpdateCycle();
#else
    LE_SEQNCR_ProcessUserCommands();
#endif
  DISPLAY_tickActions( 1 ); // Calls showAll2.
  //DISPLAY_showAll2(); // No collect commands in effect
  LE_SEQNCR_DoUpdateCycle(1);

  UDUTILS_WriteINIDisplaySettingsSpecial();


  // kill the objects
  LE_DATA_FreeRuntimeDataID( DISPLAY_state.MessageBoxID );

  DISPLAY_UDAUCT_Destroy();
  DISPLAY_UDBOARD_Destroy();
//  DISPLAY_UDCGE_Destroy();
  DISPLAY_UDIBAR_Destroy();
  DISPLAY_UDOPTS_Destroy();
  DISPLAY_UDPIECES_Destroy();
  DISPLAY_UDPSEL_Destroy();
  DISPLAY_UDSOUND_Destroy();
  DISPLAY_UDSTATS_Destroy();
  DISPLAY_UDTRADE_Destroy();
};


/*****************************************************************************
 * Tick Actions - 60 ticks per second are processed, though we get more than
 * one tick at a time if the framerate is suffering.
 */
void DISPLAY_tickActions( int numberOfTicks )
{// Feed the tick down.  Add other subs if used - no need to call empty ones.
  //static TYPE_Tick  lastFullDisplay = 0;

  LE_SEQNCR_CollectCommands();


  DISPLAY_UDBOARD_TickActions( numberOfTicks );
  DISPLAY_UDIBAR_TickActions( numberOfTicks );
  DISPLAY_UDPIECES_TickActions( numberOfTicks );
  DISPLAY_UDSOUND_tickActions( numberOfTicks );

  DISPLAY_showAll2();// Sounds are issued here, system updated above, sequencer processing after.  Dont move this ahead of tickactions.

  DISPLAY_state.SoundSystemClearToTest = TRUE;


  // Do not recalculate each 60th of a second - skip some.  Dropped - let the sub modules skip stuff if they wish.
  //if( LE_TIME_TickCount > lastFullDisplay + 4 ||
  //  ( DISPLAY_state.desired2DView != DISPLAY_state.current2DView) || // Dont wait if the camera changed
  //  ( DISPLAY_state.desired2DCamera != DISPLAY_state.current2DCamera) )
  //{
  //  lastFullDisplay = LE_TIME_TickCount;
  //}
  LE_SEQNCR_ExecuteCommands();

};


/*****************************************************************************
 * Show all - by calling this routine, all display components are checked.
 */
int MASTER_displayWidthAdjustedTo = 800;
void DISPLAY_showAll2( void )
{// This is called from tickActions - don't call it directly or some timed stuff wont increment and collect commands will not be in effect.
  BOOL slowForDebug = FALSE;
  int delay = 120;

  // Update the frame rate counter - FIXME - adjust to only track when the 3D board is on.
  DISPLAY_state.frameCount++; // frames this track
  if( DISPLAY_state.frameRateTrackerStartTime + FRAME_RATE_SAMPLE_SECONDS * 60 < LE_TIME_TickCount )
  {// Escalate
    DISPLAY_state.frameRateTracker[DISPLAY_state.frameRateTrackerIndex] = DISPLAY_state.frameCount; // Stored without dividing by the sample seconds.
    DISPLAY_state.frameRateTrackerStartTime = LE_TIME_TickCount;
    DISPLAY_state.frameRateTrackerIndex = (DISPLAY_state.frameRateTrackerIndex + 1) % 12;
    DISPLAY_state.frameCount = 0;
  }


  // Check if we need to adjust the 2D camera and mouse for non 800x600 displays.
  if( MASTER_displayWidthAdjustedTo != LE_GRAFIX_VirtualScreenWidthInPixels )
  {
    // Recalculate 3D view stuff
    for( int t = 0; t < 3; t++ )
    { // Calculate these from previous scale in case it is called more than once.
      viewRects[t].left   = viewRects[t].left   * LE_GRAFIX_VirtualScreenWidthInPixels / MASTER_displayWidthAdjustedTo;
      viewRects[t].top    = viewRects[t].top    * LE_GRAFIX_VirtualScreenWidthInPixels / MASTER_displayWidthAdjustedTo;
      viewRects[t].right  = viewRects[t].right  * LE_GRAFIX_VirtualScreenWidthInPixels / MASTER_displayWidthAdjustedTo;
      viewRects[t].bottom = viewRects[t].bottom * LE_GRAFIX_VirtualScreenWidthInPixels / MASTER_displayWidthAdjustedTo;
    }

    // Recalculate 2D camera scales
    MASTER_displayWidthAdjustedTo = LE_GRAFIX_VirtualScreenWidthInPixels;
    LE_REND2D_MoveCamera(0, 400, 300,
      (float)MASTER_displayWidthAdjustedTo/800 ); // Back 2D
    LE_REND2D_MoveCamera(2, 400, 300,
      (float)MASTER_displayWidthAdjustedTo/800 ); // Front 2D
    //LE_REND2D_MoveCamera(4, 400, 300,
    //  (float)MASTER_displayWidthAdjustedTo/800 ); // Mouse
  }

#if USE_HOTKEYS
  if( LE_KEYBRD_CheckPressed(LE_KEYBRD_S) ) return; // Don't process, speed test.
#endif
  //if( !slowForDebug ) // comment out this in tickactions (it wraps this and sub-tick fn's)
  //  LE_SEQNCR_CollectCommands();
  unprocessedSoundIssued = FALSE; // The sound system sets this TRUE when issuing sounds (inside Collect commands)
  DISPLAY_UDAUCT_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDBOARD_Show();
//  if( slowForDebug ) LE_TIMER_Delay( delay );
//  DISPLAY_UDCGE_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDIBAR_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDOPTS_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDPIECES_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDPSEL_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDSOUND_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDSTATS_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );
  DISPLAY_UDTRADE_Show();
  if( slowForDebug ) LE_TIMER_Delay( delay );

  //DISPLAY_showMessageBox();

  //if( !slowForDebug )
  //  LE_SEQNCR_ExecuteCommands();

};


/*****************************************************************************
 * Property number to sale dialog number conversion.  Only buyable properties have dialogs.
 */
short int propconv[SQ_MAX_SQUARE_TYPES] =
{
    -1, 0,-1, 1,-1, // Go - tax
     2, 3,-1, 4, 5, // RR - conn
    -1, 6, 7, 8, 9, // jail - virg
    10,11,-1,12,13, // penn - ny
    -1,14,-1,15,16, // fp - ill
    17,18,19,20,21, // bo - marv
    -1,22,23,-1,24, // gj - penn
    25,-1,26,-1,27, // sl - board
    -1, -1
};
/*int DISPLAY_propertyToOwnablePropertyConversion( int property )
{// with conversion graphics - removable later.
  int returnvalue;
//  returnvalue = DISPLAY_Dialog_Properties00;
  returnvalue = -1;
  switch( property )
  {
  //case 0: // all 'invalid' sale squares fall through to 0 (Mediteranean).
  //case 2: 4: 7: 10: 17: 20: 22: 30: 33: 36: 38:
  case 1: returnvalue = 0; break;
  case 3: returnvalue = 1; break;
  case 5: returnvalue = 2; break;
  case 6: returnvalue = 3; break;
  case 8: returnvalue = 4; break;
  case 9: returnvalue = 5; break;
  case 11: returnvalue = 6; break;
  case 12: returnvalue = 7; break;
  case 13: returnvalue = 8; break;
  case 14: returnvalue = 9; break;
  case 15: returnvalue = 10; break;
  case 16: returnvalue = 11; break;
  case 18: returnvalue = 12; break;
  case 19: returnvalue = 13; break;
  case 21: returnvalue = 14; break;
  case 23: returnvalue = 15; break;
  case 24: returnvalue = 16; break;
  case 25: returnvalue = 17; break;
  case 26: returnvalue = 18; break;
  case 27: returnvalue = 19; break;
  case 28: returnvalue = 20; break;
  case 29: returnvalue = 21; break;
  case 31: returnvalue = 22; break;
  case 32: returnvalue = 23; break;
  case 34: returnvalue = 24; break;
  case 35: returnvalue = 25; break;
  case 37: returnvalue = 26; break;
  case 39: returnvalue = 27; break;
//  default: // in case of bad values
//    returnvalue = -1; break;
  };
  return( returnvalue );
}*/






/*****************************************************************************
 * Show (update) Message Box
 */
void DISPLAY_showMessageBox( void )
{
  unsigned int index = 0, t;
  int line = 0, wordindex = 0, lastLineMarker = 0, lastMarker = 0, lineHeight;
  ACHAR tempString[DISPLAY_MessageMaxLen];
  BOOL messageContentUpdated = FALSE;

  // Determine screen locations (view dependant)
  if( !DISPLAY_state.MessageBoxSuppressed )
  {
    switch( DISPLAY_state.desired2DView )
    {
    case DISPLAY_SCREEN_AuctionA:
      DISPLAY_state.MessageDesiredX = 2;
      DISPLAY_state.MessageDesiredY = DISPLAY_MessageBoxY;
      break;
    case DISPLAY_SCREEN_TradeA:
      DISPLAY_state.MessageDesiredX = DISPLAY_MessageBoxX;
      DISPLAY_state.MessageDesiredY = 445 - DISPLAY_MessageBoxH;
      break;
    case DISPLAY_SCREEN_PortfolioA:
      DISPLAY_state.MessageDesiredX = DISPLAY_MessageBoxX;
      DISPLAY_state.MessageDesiredY = 445 - DISPLAY_MessageBoxH;
      break;
    case DISPLAY_SCREEN_Black:
      DISPLAY_state.MessageDesiredX = -1; // Signal remove from screen.
      break;
    case DISPLAY_SCREEN_MainA:
    default:
      DISPLAY_state.MessageDesiredX = 2;
      DISPLAY_state.MessageDesiredY = 2;
      break;
    }
  } else
    DISPLAY_state.MessageDesiredX = -1; // Signal remove from screen.

  if( DISPLAY_state.MessageDesiredX >= 0 ) // it will appear on the screen
  {
    if( strcmp( DISPLAY_state.CurrentMessageText, DISPLAY_state.DesiredMessageText ) != 0)
    {// Reset the chat contents.  This might be better scrolled than reset & redrawn...
      strcpy( DISPLAY_state.CurrentMessageText, DISPLAY_state.DesiredMessageText );
      messageContentUpdated = TRUE;
      LE_GRAFIX_ColorArea( DISPLAY_state.MessageBoxID,
        0, 0,
        DISPLAY_MessageBoxW, DISPLAY_MessageBoxH,
        LE_GRAFIX_MakeColorRef( 255, 255, 255 ) );
      LE_GRAFIX_ColorArea( DISPLAY_state.MessageBoxID,
        2, 2,
        DISPLAY_MessageBoxW - 4, DISPLAY_MessageBoxH - 4,
        LE_GRAFIX_MakeColorRef( 233, 158, 35 ) );

      // Write the text DISPLAY_ChatMaxLen CurLocX ChatText
      lineHeight = LE_FONTS_GetStringHeight( A_T("Xy") );
      mbstowcs( tempString, DISPLAY_state.CurrentMessageText, DISPLAY_MessageMaxLen);
      tempString[DISPLAY_MessageMaxLen] = 0;

      // Word wrap the text in the box.
      // Algorythm:  Loop through the string.  At each space, see if the line fits.
      // If so, mark the index and keep going.  If not, print from the last marker.
      // Update and keep going if there is more vertical space available.
      while( index <= strlen( DISPLAY_state.CurrentMessageText ) )
      {
        if( tempString[index] == ' ' )
        {// Marker - is the line done?
          tempString[index] = 0;
          t = LE_FONTS_GetStringWidth( tempString + lastLineMarker );
          if( LE_FONTS_GetStringWidth( tempString + lastLineMarker ) >= DISPLAY_MessageBoxW - (DISPLAY_MessageBoxTextBorder*2) )
          {// Must output the line less this last word which went over the border.
            tempString[lastMarker] = 0;
            LE_FONTS_Print( DISPLAY_state.MessageBoxID,
              DISPLAY_MessageBoxTextBorder, DISPLAY_MessageBoxTextBorder + line * lineHeight,
              LE_GRAFIX_MakeColorRef( 255, 255, 255 ),
              &tempString[lastLineMarker] );

            line++;
            lastLineMarker = lastMarker + 1;
          } else
          {// Keep on scanning
            lastMarker = index;
          }
          tempString[index] = ' ';
        }
        index++;
      } // end while

      if( index == strlen( DISPLAY_state.CurrentMessageText ) + 1 )
      { // The last line will not have printed and it was a normal exit.
        t = LE_FONTS_GetStringWidth( tempString + lastLineMarker );
        if( LE_FONTS_GetStringWidth( tempString + lastLineMarker ) >= DISPLAY_MessageBoxW - (DISPLAY_MessageBoxTextBorder*2) )
        {// Must output the line less this last word which went over the border.
          tempString[lastMarker] = 0;
          LE_FONTS_Print( DISPLAY_state.MessageBoxID,
            DISPLAY_MessageBoxTextBorder, DISPLAY_MessageBoxTextBorder + line * lineHeight,
            LE_GRAFIX_MakeColorRef( 255, 255, 255 ),
            &tempString[lastLineMarker] );

          line++;
          lastLineMarker = lastMarker + 1;
        }

        LE_FONTS_Print( DISPLAY_state.MessageBoxID,
          DISPLAY_MessageBoxTextBorder, DISPLAY_MessageBoxTextBorder + line * lineHeight,
          LE_GRAFIX_MakeColorRef( 255, 255, 255 ),
          &tempString[lastLineMarker] );
      }

      //LE_FONTS_Print( DISPLAY_state.MessageBoxID, 5, 5, LE_GRAFIX_MakeColorRef( 255, 255, 255 ),
      //  DISPLAY_state.MessageText );
    }


    // Update the box itself
    if( DISPLAY_state.isMessageBoxDisplayed )
    {
      if( DISPLAY_state.MessageCurLocX == DISPLAY_state.MessageDesiredX &&
        DISPLAY_state.MessageCurLocY == DISPLAY_state.MessageDesiredY )
      {
        // We only need to do this if the message has changed
        if( messageContentUpdated )
          LE_SEQNCR_ForceRedraw( DISPLAY_state.MessageBoxID, DISPLAY_MessageBoxPriority );
      }
      else// It has moved (view changed, most likely)
      {
        DISPLAY_state.MessageCurLocX = DISPLAY_state.MessageDesiredX;
        DISPLAY_state.MessageCurLocY = DISPLAY_state.MessageDesiredY;
        LE_SEQNCR_MoveXY( DISPLAY_state.MessageBoxID, DISPLAY_MessageBoxPriority,
          DISPLAY_state.MessageCurLocX, DISPLAY_state.MessageCurLocY );
      }
    }
    else
    { // Put it up
      DISPLAY_state.MessageCurLocX = DISPLAY_state.MessageDesiredX;
      DISPLAY_state.MessageCurLocY = DISPLAY_state.MessageDesiredY;

      LE_SEQNCR_StartXY( DISPLAY_state.MessageBoxID, DISPLAY_MessageBoxPriority,
        DISPLAY_state.MessageCurLocX, DISPLAY_state.MessageCurLocY );
      DISPLAY_state.isMessageBoxDisplayed = TRUE;
    }
  } else
  { // get it off screen.
    DISPLAY_hideMessageBox();
  }

}

/*****************************************************************************
 * Hide Message Box
 */
void DISPLAY_hideMessageBox( void )
{
  if( DISPLAY_state.isMessageBoxDisplayed )
  {
    LE_SEQNCR_Stop( DISPLAY_state.MessageBoxID, DISPLAY_MessageBoxPriority );
    DISPLAY_state.isMessageBoxDisplayed = FALSE;
    DISPLAY_state.MessageDesiredX = -1;
  }
}

/*****************************************************************************
 * Set the Message Box content
 */
void DISPLAY_setMessageBox( char *string )
{
  //if( strlen( string ) < DISPLAY_MessageMaxLen )
  //  strcpy( DISPLAY_state.DesiredMessageText, string );
  //else
  //  strcpy( DISPLAY_state.DesiredMessageText, "ERROR:  String too long to store" );
  //DISPLAY_showAll;
}


/*****************************************************************************
 * Test if it is possible to add or remove a house from the given square.
 * It looks at the even build rule, sees if the player has enough cash (unless
 * this is a sale or prepaid building from an auction), sees if the player
 * owns the square, sees if there is a monopoly, sees if the monopoly is
 * unmortgaged.  If Adding is TRUE then we're adding a house, FALSE for
 * removing a house.
 *
 * Also returns the number of free buildings in RemainingBuildingsPntr and
 * whether they are building a house or hotel in BuildingAHotelPntr (TRUE
 * for a hotel).  Both are only set if the overall result is TRUE.
 * Note that *RemainingBuildingsPntr can be negative if you are selling
 * phantom houses while in the hotel decomposition phase.
 *
 * The overall return value is TMN_NO_MESSAGE if successful, otherwise it
 * is an error message number and MessageArgumentPntr contains an optional
 * message argument (presumably you know the square and purchaser arguments).
 */

LANG_TextMessageNumber DISPLAY_TestBuildingPlacement (
  RULE_GameStatePointer GameStatePntr, RULE_PlayerNumber Purchaser,
  RULE_SquareType SquareNo, BOOL Adding, BOOL AddHouseOnly,
  BOOL AddHotelOnly, BOOL *BuildingAHotelPntr,
  int *RemainingBuildingsPntr, long *MessageArgumentPntr)
{
  BOOL              BuildingAHotel;
  BOOL              NoCharge;
  int               FreeHouses;
  int               FreeHotels;
  int               HighestHousesInMonopoly;
  int               HouseCount;
  int               HotelCount;
  int               LowestHousesInMonopoly;
  int               i;
  RULE_SquareGroups MonopolyGroup;
  int               SquareHouses;
  RULE_SquareInfoPointer SquarePntr;

  SquarePntr = GameStatePntr->Squares + SquareNo;
  MonopolyGroup = RULE_SquarePredefinedInfo[SquareNo].group;

  /* See if the player has enough money to buy the building.
     Don't care about money if selling, or placing a building
     already bought in an auction.  Or if just before or during
     an auction and trying to find out if they allowed to bid. */

  //if (GameStatePntr == &CurrentRulesState) // Rules version
  //{
  // NoCharge = (CurrentPhaseM == GF_PLACE_BUILDING ||
  //  CurrentPhaseM == GF_AUCTION ||
  //  CurrentPhaseM == GF_HOUSING_SHORTAGE_QUESTION);
  //} else {
      NoCharge = FALSE;
  //}

  if (Adding && !NoCharge)
  {
    if (RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost >
    GameStatePntr->Players[Purchaser].cash)
    {
      if (MessageArgumentPntr != NULL)
      {
        *MessageArgumentPntr =
        RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost;
      }
      return TMN_ERROR_BUILDING_NEEDS_CASH;
    }
  }

  /* See if the square is one where houses can be built. */

  if (MonopolyGroup > SG_PARK_PLACE)
    return TMN_ERROR_BUILDING_ON_NONPROPERTY;

  /* See if the buyer of houses owns the square. */

  if (SquarePntr->owner != Purchaser)
    return TMN_ERROR_BUILDING_ON_UNOWNED;

  /* Has to own the other squares in the monopoly too.  They also can't
     be mortgaged.  Also find the lowest and highest number of houses
     on the monopoly squares. */

  LowestHousesInMonopoly = GameStatePntr->GameOptions.housesPerHotel;
  HighestHousesInMonopoly = 0;
  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if (RULE_SquarePredefinedInfo[i].group == MonopolyGroup)
    {
      if (GameStatePntr->Squares[i].owner != Purchaser)
        return TMN_ERROR_BUILDING_NEEDS_MONOPOLY;

      if (GameStatePntr->Squares[i].mortgaged)
        return TMN_ERROR_BUILDING_MORTGAGED;

      if (GameStatePntr->Squares[i].houses < LowestHousesInMonopoly)
        LowestHousesInMonopoly = GameStatePntr->Squares[i].houses;
      if (GameStatePntr->Squares[i].houses > HighestHousesInMonopoly)
        HighestHousesInMonopoly = GameStatePntr->Squares[i].houses;
    }
  }

  /* See if there is already a hotel on that
     square.  Can't add any more in that case. */

  if (Adding &&
  SquarePntr->houses >= GameStatePntr->GameOptions.housesPerHotel)
    return TMN_ERROR_BUILDING_MAXED_OUT;

  /* Count up the houses and hotels currently on the board. */

  HouseCount = HotelCount = 0;
  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    SquareHouses = GameStatePntr->Squares[i].houses;
    if (SquareHouses < GameStatePntr->GameOptions.housesPerHotel)
      HouseCount += SquareHouses;
    else
      HotelCount++;
  }

  /* So how many are free?  Can have a negative FreeHouses if there are
     phantom houses during a hotel decomposition. */

  FreeHouses = GameStatePntr->GameOptions.maximumHouses - HouseCount;
  FreeHotels = GameStatePntr->GameOptions.maximumHotels - HotelCount;

  if (Adding)
  {
    /* Enforce the even build rule. */

    if (GameStatePntr->GameOptions.evenBuildRule &&
    SquarePntr->houses > LowestHousesInMonopoly)
    {
      if (MessageArgumentPntr != NULL)
        *MessageArgumentPntr = LowestHousesInMonopoly;
      return TMN_ERROR_BUILDING_NOT_EVEN;
    }

    if (SquarePntr->houses >= GameStatePntr->GameOptions.housesPerHotel - 1)
    {
      /* Trying to build a hotel. */

      BuildingAHotel = TRUE;
      if (FreeHotels < 1)
        return TMN_ERROR_BUILDING_NO_HOTELS;
    }
    else /* Building a house. */
    {
      BuildingAHotel = FALSE;
      if (FreeHouses < 1)
        return TMN_ERROR_BUILDING_NO_HOUSES;
    }
  }
  else /* Removing a hotel or house. */
  {
    if (SquarePntr->houses >= GameStatePntr->GameOptions.housesPerHotel)
    {
      /* Removing a hotel by breaking it down into houses. */

      BuildingAHotel = TRUE;

      /* See if there are enough houses to break down the hotel.
         If in the decomposition phase, ignore this test and temporarily
         allow more houses on the board than there should be. */

      //if ((GameStatePntr != &CurrentRulesState || CurrentPhaseM != GF_DECOMPOSE_HOTEL) &&
      //(FreeHouses < GameStatePntr->GameOptions.housesPerHotel - 1))
      //{
      //  if (MessageArgumentPntr != NULL)
      //    *MessageArgumentPntr = GameStatePntr->GameOptions.housesPerHotel - 1;
      //  return TMN_ERROR_BUILDING_NO_BREAKDOWN_HOUSES;
      //}
    }
    else /* Selling a house. */
    {
      BuildingAHotel = FALSE;

      /* Needs to be a house there before you can sell it. */

      if (SquarePntr->houses <= 0)
        return TMN_ERROR_SELL_NO_HOUSES;

      /* Enforce the even build rule while selling. */

      if (GameStatePntr->GameOptions.evenBuildRule &&
      SquarePntr->houses < HighestHousesInMonopoly)
      {
        if (MessageArgumentPntr != NULL)
          *MessageArgumentPntr = HighestHousesInMonopoly;
        return TMN_ERROR_BUILDING_NOT_EVEN;
      }
    }
  }

  if (BuildingAHotelPntr != NULL)
    *BuildingAHotelPntr = BuildingAHotel;

  if (RemainingBuildingsPntr != NULL)
    *RemainingBuildingsPntr = BuildingAHotel ? FreeHotels : FreeHouses;

  return TMN_NO_MESSAGE;
}

/*****************************************************************************
 * Makes a list of all the squares a player can put a building on.
 * Does not consider the type of building (ie, when it came from an auction)
 */
RULE_PropertySet DISPLAY_PlaceBuildingLegalSquares( int player )
{
  BOOL                    BuildingAHotel;
  LANG_TextMessageNumber  ErrorCode;
  RULE_PropertySet        LegalSet;
  RULE_SquareType         SquareNo;

  LegalSet = 0;
  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    ErrorCode = DISPLAY_TestBuildingPlacement (&UICurrentGameState,
      player,
      SquareNo,
      TRUE,     // Adding a building
      FALSE,    // Not forced to house only (add)
      FALSE,    // Not forced to hotel only (add)
      &BuildingAHotel, // Are we building a hotel (or house)
      NULL,     // Remaining buildings pointer
      NULL);    // house cost, lowest/highest houses in monopoly,


    if (ErrorCode == TMN_NO_MESSAGE) /* If ok to build there. */
    {
        LegalSet |= RULE_PropertyToBitSet (SquareNo);
    }
  }

  return LegalSet;
}


/*****************************************************************************
 * Makes a list of all the squares a player can sell a building.
 */
RULE_PropertySet DISPLAY_SellBuildingLegalSquares( int player )
{
  BOOL                    BuildingAHotel;
  LANG_TextMessageNumber  ErrorCode;
  RULE_PropertySet        LegalSet;
  RULE_SquareType         SquareNo;

  LegalSet = 0;
  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    ErrorCode = DISPLAY_TestBuildingPlacement (&UICurrentGameState,
      player,
      SquareNo,
      FALSE,     // Adding a building
      FALSE,    // Not forced to house only (add)
      FALSE,    // Not forced to hotel only (add)
      &BuildingAHotel, // Are we building a hotel (or house)
      NULL,     // Remaining buildings pointer
      NULL);    // house cost, lowest/highest houses in monopoly,


    if (ErrorCode == TMN_NO_MESSAGE) /* If ok to build there. */
    {
        LegalSet |= RULE_PropertyToBitSet (SquareNo);
    }
  }

  return LegalSet;
}


/*****************************************************************************
 * Attempts to resize the screen (setting values for 2D camera and mouse scaling)
 */
BOOL DISLAY_NewScreenSizeNDepth( int x, int y, int depth )
{
  return(DDRAW_SetINIMode((UINT)x, (UINT)y, (UINT)depth));
}

