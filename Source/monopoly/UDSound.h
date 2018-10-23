#ifndef __UDSOUND_H__
#define __UDSOUND_H__

/*************************************************************
*
*   FILE:             UDSOUND.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
//3define TOKEN_VOLUME  100

enum {
    TKS_FirstTurn= 0,                           // Hi!  I'm the cannon. With me you get more bang for your buck.
    TKS_Build,                                  // I'm going to build! Call the engineering corp.
    TKS_Sell,                                   // I think I'll dispose of  some of my assets.
    TKS_Trade,                                  // A cease-fire while we negotiate a trade.
    TKS_Mortgage,                               // Wonder if those people at the bank would help a soldier out!
    TKS_UnMortgage,                             // Feeling good! Time to unmortgage some property.
    TKS_Intro,                                  // Well, into the fray once again.
    TKS_StartSide4Only,                         // This neighborhood is five-star all the way!
    TKS_StartTurnGeneric,                       // Into the breech.
    TKS_PositiveRolls,                          // Gimme a deuce.
    TKS_NegativeRolls,                          // Anything but a two!
    TKS_NegativeMonopolies,                     // Those purple properties are like a minefield in waiting!
    TKS_ThirdDoubles,                           // Off to the brig! Drat!
    TKS_NobodyAround,                           // Seem to be in solitary. Nobody's around.
    TKS_PassedByA,                              // Howdy, pup.         cannon
    TKS_PassedByB,                              //                     car
    TKS_PassedByC,                              //                     dog
    TKS_PassedByD,                              //                     hat
    TKS_PassedByE,                              //                     iron
    TKS_PassedByF,                              //                     horse
    TKS_PassedByG,                              //                     ship
    TKS_PassedByH,                              //                     shoe
    TKS_PassedByI,                              //                     thimble
    TKS_PassedByJ,                              //                     barrow
    TKS_PassedByK,                              //                     moneybag
    TKS_PassedByGeneric,                        // Somebody's in a hurry.
    TKS_AboutToBankrupt,                        // I'm being hung out to dry here!
    TKS_LandOnOtherHotel,                       // This is as about as much fun as the frontline.
    TKS_LandOnUnownedPropertyFillingMonopoly,   // You are looking at a soldier of  fortune!
    TKS_LandOnBiggerHit,                        // You are looking at a soldier of misfortune!
    TKS_LandOn4Houses,                          // Hey! I'm cannon not cannon fodder!
    TKS_HitGOWithDoubleCashRule,                // Yes Siree! Bring it on!
    TKS_LandOn1To3Houses,                       // I've been ambushed!
    TKS_LandOnMorgagedProperty,                 // This is safe anyway!
    TKS_LandOnBigHit,                           // Great Scott! I'm taking a lickin'!
    TKS_LandOn0Houses,                          // Another broadside! Gadzooks man!
    TKS_LandOnOwnProperty,                      // I love being a property owner! Just let me bask for a second!
    TKS_LandOnUnownedPPOrBoardwalk,             // This is where the Generals must hang their hats! I'll buy!
    TKS_LandOnGoToJailEarly,                    // Off to the brig so soon?
    TKS_LandOnGoToJailLate,                     // A good place to lay low for awhile.
    TKS_LandOnLowHit,                           // I'm taking a beating!
    TKS_LandOnFreeParkingPaying,                // Make it happen, Captain!
    TKS_LandOnTileWith2PlusOthers,              // Looks like a party!
    TKS_LandOnIncomeTaxPoor,                    // Not the taxman again!
    TKS_LandOnIncomeTaxRich,                    // Uncle Sam gives and he takes away!
    TKS_LandOnLuxuryTax,                        // Luxury? I'm trying to take care of the necessities!
    TKS_LandOnUnownedGreen,                     // Hmmm. I prefer Khaki but green is cool.
    TKS_LandOnLowerHit,                         // Well, that's the way the cannonball bounces!
    TKS_LandOnJailNotEmpty,                     // Like to help you but you gotta do your time like the rest of us!
    TKS_LandOnJailJustVisiting,                 // Just visiting - don't lock the gates behind me!
    TKS_LandOnLowLowHit,                        // Holy Moses! I'm taking a proper thumping here!
    TKS_LandFreeParkingNotPaying,               // Think I'll kick back for some R. & R.
    TKS_LandOnUnownedRailroadHoldingOther3,     // Well, lookey here! I got me the whole set!
    TKS_LandOnUnownedRailroad,                  // Choo choo! I think I'm on the right track.
    TKS_LandOnLowestHit,                        // You'll have to hit me harder than that pal!.
    TKS_LandOnUnownedUtilityHoldingOther,       // Alright! I've got the pair!
    TKS_LandOnUnownedUtility,                   // These are strategic properties, sure I'll buy!
    TKS_BuySide4,                               // Roger! Will buy! Over and out!
    TKS_BuySide3,                               // Buy Buy Buy!
    TKS_BuySide2,                               // Sure, why not? Property is property!
    TKS_BuySide1,                               // I love a deal! Sure!
    TKS_ChoseAuction,                           // I feel like an auction!
    TKS_BuildingHouseAfterthought,              // That changes things.... I will build after all.
    TKS_SellHouseAfterthought,                  // This doughboy needs some dough- I'm going to sell some buildings back to the bank.
    TKS_ProposingTrade,                         // Hmmm... I think a trade is the order of the day!
    TKS_MortgageAfterthought,                   // The bank said to call'em if I needed them! I better mortgage.
    TKS_UnmortgageAfterthought,                 // The bank's been nice but I can unmortgage some property now.
    TKS_GiveUp,                                 // I surrender! It's game over for this soldier.
    TKS_Bankrupt,                               // I am tapped!
    TKS_DoneTurn,                               // Go ahead! The rest of you can play now.
    TKS_WonGame,                                // Yowsah! Yowsah!
    TKS_GenericGood,                            // Yes sir!
    TKS_GenericBad,                             // Oh no!
    TKS_MAX_LINES
};

/*enum {
  Sound_ClipOldIfPlaying = 0,
  Sound_SkipThisIfOldPlaying,
  Sound_AllowMultipleSounds
};*/
enum {
  Sound_ClipOldSoundIfPlaying = 0,
  Sound_ClipOldSoundIfPlayingWithLock,  // Will lock the game queue while playing - forces other effects to wait.
  Sound_SkipIfOldSoundPlaying,          // Use this for 'color' lines, important lines must be played.
  Sound_WaitForAnyOldSoundThenPlay,     // Will lock stack processing and wait for the previous sound.  Note only one can be stacked, use this only for sounds you really want played.
  // The last one doesn't work, dont use it.
  Sound_WaitForAnyOldSoundThenPlayWithLock // Will lock the stack for current sound and this sound * quicky, wont work fully.
};

/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/
extern BOOL  unprocessedSoundIssued;

/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDSOUND_Initialize( void );
void DISPLAY_UDSOUND_Destroy( void );
void DISPLAY_UDSOUND_tickActions( int numberOfTicks );
void DISPLAY_UDSOUND_Show( void );

void UDSOUND_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );


void UDSOUND_Warning( void );
void UDSOUND_Click( void );
void UDSOUND_CashUpFx( void );
void UDSOUND_CashDownFx( void );

int UDSOUND_TokenGenericPlaySound( int token, int soundType, int clipaction );

void UDSOUND_BackgroundMusicUpdate( void );
// Remove the following define when you can update the occurances (or never, RK).
#define UDSOUND_BackgroundMusicOn() UDSOUND_BackgroundMusicUpdate()
//void UDSOUND_BackgroundMusicOff( void );

void UDSOUND_WatchSoundWithQueueLock( LE_DATA_DataId sound );
void UD_SOUND_KillAllTalkingSounds( void );
BOOL UD_SOUND_CheckForTalkingSounds( void );
BOOL UD_SOUND_CheckForTalkingSoundsWithLock( void );
void UDPENNY_PB_ReadCard( int lastCardIn );
int UDPENNY_GetCorrectCardWav(int lastCardIn);

void UDSOUND_HandleBSSMSoundNCam(int player, int square, int action);

void UDSOUND_PlaySiren( void );


#endif // __UDSOUND_H__