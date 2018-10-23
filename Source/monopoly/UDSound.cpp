/*****************************************************************************
 *
 * FILE:        UDSOUND.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles starting, stopping and pausing sounds.  Structured
 *    like the other UD subfiles.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/

#include "gameinc.h"


/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/
int TokenVolumes[MAX_TOKENS] = { 100, 100, 100, 100, 100, 
  100, // Horse (raise?)
  100, 100, 100, 100, 100 };

BOOL  unprocessedSoundIssued; // Flags that sounds have been issued which would not have been processed
                              // yet (so we will have to wait even though no sounds found running).

BOOL  IsBackgroundMusicOn = FALSE;

LE_DATA_DataId UDSOUND_Token_Sound_Details[TKS_MAX_LINES][MAX_TOKENS][2] =
{// For each sound, each token, record start ID - the number of alternates is found by subtracting the ID from the next in the array.
 //                                                Cannon               Race car             Dog                  Top hat              Iron                 Horse                 Battleship           Shoe                 Thimble              Wheelbarrow          Money bag
   {{ WAV_kyaint01, 7 }, { WAV_kybint01, 7 }, { WAV_kycint01, 10}, { WAV_kydint01, 8 }, { WAV_kyeint01, 8 }, { WAV_kyfint01, 8 },  { WAV_kygint01, 8 }, { WAV_kyhint01, 8 }, { WAV_kyiint01, 8 }, { WAV_kyjint01, 8 }, { WAV_kykint01, 8 }}, // TKS_FirstTurn
   {{ WAV_kyaact01, 2 }, { WAV_kybact01, 2 }, { WAV_kycact01, 2 }, { WAV_kydact01, 2 }, { WAV_kyeact01, 2 }, { WAV_kyfact01, 2 },  { WAV_kygact01, 2 }, { WAV_kyhact01, 2 }, { WAV_kyiact01, 2 }, { WAV_kyjact01, 2 }, { WAV_kykact01, 2 }}, // TKS_Build
   {{ WAV_kyaact03, 2 }, { WAV_kybact03, 2 }, { WAV_kycact03, 2 }, { WAV_kydact03, 2 }, { WAV_kyeact03, 2 }, { WAV_kyfact03, 2 },  { WAV_kygact03, 2 }, { WAV_kyhact03, 2 }, { WAV_kyiact03, 2 }, { WAV_kyjact03, 2 }, { WAV_kykact03, 2 }}, // TKS_Sell
   {{ WAV_kyaact05, 2 }, { WAV_kybact05, 2 }, { WAV_kycact05, 2 }, { WAV_kydact05, 2 }, { WAV_kyeact05, 2 }, { WAV_kyfact05, 2 },  { WAV_kygact05, 2 }, { WAV_kyhact05, 2 }, { WAV_kyiact05, 2 }, { WAV_kyjact05, 2 }, { WAV_kykact05, 2 }}, // TKS_Trade
   {{ WAV_kyaact07, 2 }, { WAV_kybact07, 3 }, { WAV_kycact07, 2 }, { WAV_kydact07, 2 }, { WAV_kyeact07, 2 }, { WAV_kyfact07, 2 },  { WAV_kygact07, 2 }, { WAV_kyhact07, 2 }, { WAV_kyiact07, 2 }, { WAV_kyjact07, 2 }, { WAV_kykact07, 2 }}, // TKS_Mortgage
   {{ WAV_kyaact09, 2 }, { WAV_kybact10, 2 }, { WAV_kycact09, 2 }, { WAV_kydact09, 2 }, { WAV_kyeact09, 2 }, { WAV_kyfact09, 2 },  { WAV_kygact09, 2 }, { WAV_kyhact09, 2 }, { WAV_kyiact09, 2 }, { WAV_kyjact09, 2 }, { WAV_kykact09, 2 }}, // TKS_UnMortgage
   {{ WAV_kyastrt1, 5 }, { WAV_kybstrt1, 5 }, { WAV_kycstrt1, 3 }, { WAV_kydstrt1, 3 }, { WAV_kyestrt1, 3 }, { WAV_kyfstrt1, 5 },  { WAV_kygstrt1, 5 }, { WAV_kyhstrt1, 5 }, { WAV_kyistrt1, 3 }, { WAV_kyjstrt1, 3 }, { WAV_kykstrt1, 4 }}, // TKS_Intro
   {{ WAV_kyarich1, 2 }, { WAV_kybrich1, 3 }, { WAV_kycrich1, 3 }, { WAV_kydrich1, 2 }, { WAV_kyerich1, 2 }, { WAV_kyfrich1, 3 },  { WAV_kygrich1, 2 }, { WAV_kyhrich1, 2 }, { WAV_kyirich1, 2 }, { WAV_kyjrich1, 2 }, { WAV_kykrich1, 3 }}, // TKS_StartSide4Only
   {{ WAV_kyaidle1, 5 }, { WAV_kybidle1, 5 }, { WAV_kycidle1, 5 }, { WAV_kydidle1, 5 }, { WAV_kyeidle1, 5 }, { WAV_kyfidle1, 5 },  { WAV_kygidle1, 5 }, { WAV_kyhidle1, 5 }, { WAV_kyiidle1, 5 }, { WAV_kyjidle1, 5 }, { WAV_kykidle1, 5 }}, // TKS_StartTurnGeneric
   {{ WAV_kyawnt01, 1 }, { WAV_kybwnt01, 1 }, { WAV_kycwnt01, 1 }, { WAV_kydwnt00, 1 }, { WAV_kyewnt01, 1 }, { WAV_kyfwnt01, 1 },  { WAV_kygwnt01, 1 }, { WAV_kyhwnt01, 1 }, { WAV_kyiwnt01, 1 }, { WAV_kyjwnt01, 1 }, { WAV_kykwnt01, 1 }}, // TKS_PositiveRolls
   {{ WAV_kyadnt01, 1 }, { WAV_kybdnt01, 1 }, { WAV_kycdnt01, 1 }, { WAV_kyddnt01, 1 }, { WAV_kyednt01, 1 }, { WAV_kyfdnt01, 1 },  { WAV_kygdnt01, 1 }, { WAV_kyhdnt01, 1 }, { WAV_kyidnt01, 1 }, { WAV_kyjdnt01, 1 }, { WAV_kykdnt01, 1 }}, // TKS_NegativeRolls
   {{ WAV_kyacolr1, 1 }, { WAV_kybcolr1, 1 }, { WAV_kyccolr1, 1 }, { WAV_kydcolr1, 1 }, { WAV_kyecolr1, 1 }, { WAV_kyfcolr1, 1 },  { WAV_kygcolr1, 1 }, { WAV_kyhcolr1, 1 }, { WAV_kyicolr1, 1 }, { WAV_kyjcolr1, 1 }, { WAV_kykcolr1, 1 }}, // TKS_NegativeMonopolies
   {{ WAV_kyabdlk1, 2 }, { WAV_kybbdlk1, 1 }, { WAV_kycbdlk1, 1 }, { WAV_kydbdlk1, 1 }, { WAV_kyebdlk1, 1 }, { WAV_kyfbdlk1, 1 },  { WAV_kygbdlk1, 1 }, { WAV_kyhbdlk1, 1 }, { WAV_kyibdlk1, 1 }, { WAV_kyjbdlk1, 1 }, { WAV_kykbdlk1, 2 }}, // TKS_ThirdDoubles
   {{ WAV_kyamove1, 5 }, { WAV_kybmove1, 6 }, { WAV_kycmove1, 5 }, { WAV_kydmove1, 5 }, { WAV_kyemove1, 5 }, { WAV_kyfmove1, 5 },  { WAV_kygmove1, 5 }, { WAV_kyhmove1, 5 }, { WAV_kyimove1, 5 }, { WAV_kyjmove1, 5 }, { WAV_kykmove1, 5 }}, // TKS_NobodyAround
   {{ LED_EI      , 0 }, { WAV_kybpas13, 2 }, { WAV_kycpas01, 2 }, { WAV_kydpas01, 1 }, { WAV_kyepas01, 2 }, { WAV_kyfpas13, 2 },  { WAV_kygpas01, 2 }, { WAV_kyhpas13, 2 }, { WAV_kyipas01, 2 }, { WAV_kyjpas01, 2 }, { WAV_kykpas13, 2 }}, // TKS_PassedByA
   {{ WAV_kyapas11, 2 }, { LED_EI      , 0 }, { WAV_kycpas11, 2 }, { WAV_kydpas11, 2 }, { WAV_kyepas11, 2 }, { WAV_kyfpas11, 2 },  { WAV_kygpas11, 2 }, { WAV_kyhpas11, 2 }, { WAV_kyipas11, 2 }, { WAV_kyjpas11, 2 }, { WAV_kykpas11, 2 }}, // TKS_PassedByB
   {{ WAV_kyapas01, 2 }, { WAV_kybpas01, 2 }, { LED_EI      , 0 }, { WAV_kydpas09, 2 }, { WAV_kyepas03, 2 }, { WAV_kyfpas01, 2 },  { WAV_kygpas09, 2 }, { WAV_kyhpas01, 2 }, { WAV_kyipas17, 2 }, { WAV_kyjpas05, 2 }, { WAV_kykpas01, 2 }}, // TKS_PassedByC
   {{ WAV_kyapas07, 2 }, { WAV_kybpas07, 2 }, { WAV_kycpas07, 2 }, { WAV_kydpas02, 1 }, { WAV_kyepas07, 2 }, { WAV_kyfpas07, 2 },  { WAV_kygpas07, 2 }, { WAV_kyhpas07, 2 }, { WAV_kyipas07, 2 }, { WAV_kyjpas07, 2 }, { WAV_kykpas07, 2 }}, // TKS_PassedByD
   {{ WAV_kyapas03, 2 }, { WAV_kybpas03, 2 }, { WAV_kycpas03, 2 }, { WAV_kydpas03, 2 }, { LED_EI      , 0 }, { WAV_kyfpas03, 2 },  { WAV_kygpas03, 2 }, { WAV_kyhpas03, 2 }, { WAV_kyipas03, 2 }, { WAV_kyjpas03, 2 }, { WAV_kykpas03, 2 }}, // TKS_PassedByE
   {{ WAV_kyapas13, 2 }, { WAV_kybpas19, 2 }, { WAV_kycpas13, 2 }, { WAV_kydpas13, 2 }, { WAV_kyepas13, 2 }, { LED_EI      , 0 },  { WAV_kygpas13, 2 }, { WAV_kyhpas19, 2 }, { WAV_kyipas13, 2 }, { WAV_kyjpas13, 2 }, { WAV_kykpas19, 2 }}, // TKS_PassedByF
   {{ WAV_kyapas09, 2 }, { WAV_kybpas09, 2 }, { WAV_kycpas09, 2 }, { WAV_kydpas07, 2 }, { WAV_kyepas09, 2 }, { WAV_kyfpas09, 2 },  { LED_EI      , 0 }, { WAV_kyhpas09, 2 }, { WAV_kyipas09, 2 }, { WAV_kyjpas09, 2 }, { WAV_kykpas09, 2 }}, // TKS_PassedByG
   {{ WAV_kyapas15, 2 }, { WAV_kybpas15, 2 }, { WAV_kycpas15, 2 }, { WAV_kydpas15, 2 }, { WAV_kyepas15, 2 }, { WAV_kyfpas15, 2 },  { WAV_kygpas15, 2 }, { LED_EI      , 0 }, { WAV_kyipas15, 2 }, { WAV_kyjpas15, 2 }, { WAV_kykpas15, 2 }}, // TKS_PassedByH
   {{ WAV_kyapas17, 2 }, { WAV_kybpas17, 2 }, { WAV_kycpas17, 2 }, { WAV_kydpas17, 2 }, { WAV_kyepas17, 2 }, { WAV_kyfpas17, 2 },  { WAV_kygpas17, 2 }, { WAV_kyhpas17, 2 }, { LED_EI      , 0 }, { WAV_kyjpas17, 2 }, { WAV_kykpas17, 2 }}, // TKS_PassedByI
   {{ WAV_kyapas05, 2 }, { WAV_kybpas05, 2 }, { WAV_kycpas05, 2 }, { WAV_kydpas05, 2 }, { WAV_kyepas05, 2 }, { WAV_kyfpas05, 2 },  { WAV_kygpas05, 2 }, { WAV_kyhpas05, 2 }, { WAV_kyipas05, 2 }, { LED_EI      , 0 }, { WAV_kykpas05, 2 }}, // TKS_PassedByJ
   {{ WAV_kyapas19, 2 }, { WAV_kybpas11, 2 }, { WAV_kycpas19, 2 }, { WAV_kydpas19, 2 }, { WAV_kyepas19, 2 }, { WAV_kyfpas19, 2 },  { WAV_kygpas19, 2 }, { WAV_kyhpas15, 2 }, { WAV_kyipas19, 2 }, { WAV_kyjpas19, 2 }, { LED_EI      , 0 }}, // TKS_PassedByK
   {{ WAV_kyapas21, 3 }, { WAV_kybpas21, 3 }, { WAV_kycpas21, 3 }, { WAV_kydpas21, 3 }, { WAV_kyepas21, 3 }, { WAV_kyfpas21, 3 },  { WAV_kygpas21, 4 }, { WAV_kyhpas21, 3 }, { WAV_kyipas21, 3 }, { WAV_kyjpas21, 3 }, { WAV_kykpas21, 3 }}, // TKS_PassedByGeneric
   {{ WAV_kyabkrt1, 3 }, { WAV_kybbkrt1, 1 }, { WAV_kycbkrt1, 1 }, { WAV_kydbkrt1, 3 }, { WAV_kyebkrt1, 1 }, { WAV_kyfbkrt1, 1 },  { WAV_kygbkrt1, 4 }, { WAV_kyhbkrt1, 1 }, { WAV_kyibkrt1, 1 }, { WAV_kyjbkrt1, 1 }, { WAV_kykbkrt1, 2 }}, // TKS_AboutToBankrupt
   {{ WAV_kyaneg01, 1 }, { WAV_kybneg01, 1 }, { WAV_kycneg01, 1 }, { WAV_kydneg01, 1 }, { WAV_kyeneg01, 1 }, { WAV_kyfneg01, 1 },  { WAV_kygneg01, 1 }, { WAV_kyhneg01, 1 }, { WAV_kyineg01, 1 }, { WAV_kyjneg01, 1 }, { WAV_kykneg01, 1 }}, // TKS_LandOnOtherHotel
   {{ WAV_kyapos01, 1 }, { WAV_kybpos01, 1 }, { WAV_kycpos01, 1 }, { WAV_kydpos01, 1 }, { WAV_kyepos01, 1 }, { WAV_kyfpos01, 1 },  { WAV_kygpos01, 1 }, { WAV_kyhpos01, 1 }, { WAV_kyipos01, 1 }, { WAV_kyjpos01, 1 }, { WAV_kykpos01, 1 }}, // TKS_LandOnUnownedPropertyFillingMonopoly
   {{ WAV_kyaneg02, 1 }, { WAV_kybneg02, 1 }, { WAV_kycneg02, 1 }, { WAV_kydneg02, 1 }, { WAV_kyeneg02, 1 }, { WAV_kyfneg02, 1 },  { WAV_kygneg02, 1 }, { WAV_kyhneg02, 1 }, { WAV_kyineg02, 1 }, { WAV_kyjneg02, 1 }, { WAV_kykneg02, 1 }}, // TKS_LandOnBiggerHit
   {{ WAV_kyaneg03, 1 }, { WAV_kybneg03, 1 }, { WAV_kycneg03, 1 }, { WAV_kydneg03, 1 }, { WAV_kyeneg03, 1 }, { WAV_kyfneg03, 1 },  { WAV_kygneg03, 1 }, { WAV_kyhneg03, 1 }, { WAV_kyineg00, 1 }, { WAV_kyjneg03, 1 }, { WAV_kykneg03, 1 }}, // TKS_LandOn4Houses
   {{ WAV_kyapos02, 1 }, { WAV_kybpos02, 1 }, { WAV_kycpos02, 1 }, { WAV_kydpos02, 1 }, { WAV_kyepos02, 1 }, { WAV_kyfpos02, 1 },  { WAV_kygpos02, 1 }, { WAV_kyhpos02, 1 }, { WAV_kyipos02, 1 }, { WAV_kyjpos02, 1 }, { WAV_kykpos02, 1 }}, // TKS_HitGOWithDoubleCashRule
   {{ WAV_kyaneg04, 1 }, { WAV_kybneg04, 1 }, { WAV_kycneg04, 1 }, { WAV_kydneg04, 1 }, { WAV_kyeneg04, 1 }, { WAV_kyfneg04, 1 },  { WAV_kygneg04, 1 }, { WAV_kyhneg04, 1 }, { WAV_kyineg03, 1 }, { WAV_kyjneg04, 1 }, { WAV_kykneg04, 1 }}, // TKS_LandOn1To3Houses
   {{ WAV_kyapos03, 1 }, { WAV_kybpos03, 1 }, { WAV_kycpos03, 1 }, { WAV_kydpos03, 1 }, { WAV_kyepos03, 1 }, { WAV_kyfpos03, 1 },  { WAV_kygpos03, 1 }, { WAV_kyhpos03, 1 }, { WAV_kyipos03, 1 }, { WAV_kyjpos03, 1 }, { WAV_kykpos03, 1 }}, // TKS_LandOnMorgagedProperty
   {{ WAV_kyaneg05, 1 }, { WAV_kybneg05, 1 }, { WAV_kycneg05, 1 }, { WAV_kydneg05, 1 }, { WAV_kyeneg05, 1 }, { WAV_kyfneg05, 1 },  { WAV_kygneg05, 1 }, { WAV_kyhneg05, 1 }, { WAV_kyineg04, 1 }, { WAV_kyjneg05, 1 }, { WAV_kykneg05, 1 }}, // TKS_LandOnBigHit
   {{ WAV_kyaneg06, 1 }, { WAV_kybneg06, 1 }, { WAV_kycneg06, 1 }, { WAV_kydneg06, 1 }, { WAV_kyeneg06, 1 }, { WAV_kyfneg06, 1 },  { WAV_kygneg06, 1 }, { WAV_kyhneg06, 1 }, { WAV_kyineg05, 1 }, { WAV_kyjneg06, 1 }, { WAV_kykneg06, 1 }}, // TKS_LandOn0Houses
   {{ WAV_kyapos24, 1 }, { WAV_kybpos04, 1 }, { WAV_kycpos04, 1 }, { WAV_kydpos04, 1 }, { WAV_kyepos04, 1 }, { WAV_kyfpos04, 1 },  { WAV_kygpos04, 1 }, { WAV_kyhpos04, 1 }, { WAV_kyipos04, 1 }, { WAV_kyjpos04, 1 }, { WAV_kykpos04, 1 }}, // NEW TKS_LandOnOwnProperty
   {{ WAV_kyapos04, 1 }, { WAV_kybpos05, 1 }, { WAV_kycpos05, 1 }, { WAV_kydpos05, 1 }, { WAV_kyepos05, 1 }, { WAV_kyfpos05, 1 },  { WAV_kygpos05, 1 }, { WAV_kyhpos05, 1 }, { WAV_kyipos05, 1 }, { WAV_kyjpos05, 1 }, { WAV_kykpos05, 1 }}, // TKS_LandOnUnownedPPOrBoardwalk
   {{ WAV_kyaneg07, 1 }, { WAV_kybneg07, 1 }, { WAV_kycneg07, 1 }, { WAV_kydneg07, 1 }, { WAV_kyeneg07, 1 }, { WAV_kyfneg07, 1 },  { WAV_kygneg07, 1 }, { WAV_kyhneg07, 1 }, { WAV_kyineg06, 1 }, { WAV_kyjneg07, 1 }, { WAV_kykneg07, 1 }}, // TKS_LandOnGoToJailEarly
   {{ WAV_kyapos05, 1 }, { WAV_kybpos06, 1 }, { WAV_kycpos06, 1 }, { WAV_kydpos06, 1 }, { WAV_kyepos06, 1 }, { WAV_kyfpos06, 1 },  { WAV_kygpos06, 1 }, { WAV_kyhpos06, 1 }, { WAV_kyipos06, 1 }, { WAV_kyjpos06, 1 }, { WAV_kykpos06, 1 }}, // TKS_LandOnGoToJailLate
   {{ WAV_kyaneg08, 1 }, { WAV_kybneg08, 1 }, { WAV_kycneg08, 1 }, { WAV_kydneg08, 1 }, { WAV_kyeneg08, 1 }, { WAV_kyfneg08, 1 },  { WAV_kygneg08, 1 }, { WAV_kyhneg08, 1 }, { WAV_kyineg07, 1 }, { WAV_kyjneg08, 1 }, { WAV_kykneg08, 1 }}, // TKS_LandOnLowHit
   {{ WAV_kyapos06, 1 }, { WAV_kybpos07, 1 }, { WAV_kycpos07, 1 }, { WAV_kydpos07, 1 }, { WAV_kyepos07, 1 }, { WAV_kyfpos07, 1 },  { WAV_kygpos07, 1 }, { WAV_kyhpos07, 1 }, { WAV_kyipos07, 1 }, { WAV_kyjpos07, 1 }, { WAV_kykpos07, 1 }}, // TKS_LandOnFreeParkingPaying
   {{ WAV_kyapos07, 1 }, { WAV_kybpos08, 1 }, { WAV_kycpos08, 1 }, { WAV_kydpos08, 1 }, { WAV_kyepos08, 1 }, { WAV_kyfpos08, 1 },  { WAV_kygpos08, 1 }, { WAV_kyhpos08, 1 }, { WAV_kyipos08, 1 }, { WAV_kyjpos08, 1 }, { WAV_kykpos08, 1 }}, // TKS_LandOnTileWith2PlusOthers
   {{ WAV_kyaneg09, 1 }, { WAV_kybneg09, 1 }, { WAV_kycneg09, 1 }, { WAV_kydneg09, 1 }, { WAV_kyeneg09, 1 }, { WAV_kyfneg09, 1 },  { WAV_kygneg09, 1 }, { WAV_kyhneg09, 1 }, { WAV_kyineg08, 1 }, { WAV_kyjneg09, 1 }, { WAV_kykneg09, 1 }}, // TKS_LandOnIncomeTaxPoor
   {{ WAV_kyaneg10, 1 }, { WAV_kybneg10, 1 }, { WAV_kycneg10, 1 }, { WAV_kydneg10, 1 }, { WAV_kyeneg10, 1 }, { WAV_kyfneg10, 1 },  { WAV_kygneg10, 1 }, { WAV_kyhneg10, 1 }, { WAV_kyineg09, 1 }, { WAV_kyjneg10, 1 }, { WAV_kykneg10, 1 }}, // TKS_LandOnIncomeTaxRich
   {{ WAV_kyaneg11, 1 }, { WAV_kybneg11, 1 }, { WAV_kycpos09, 1 }, { WAV_kydneg11, 1 }, { WAV_kyeneg11, 1 }, { WAV_kyfneg11, 1 },  { WAV_kygneg11, 1 }, { WAV_kyhneg11, 1 }, { WAV_kyineg10, 1 }, { WAV_kyjneg11, 1 }, { WAV_kykneg11, 1 }}, // TKS_LandOnLuxuryTax
   {{ WAV_kyapos08, 1 }, { WAV_kybpos09, 1 }, { WAV_kycpos10, 1 }, { WAV_kydpos09, 1 }, { WAV_kyepos09, 1 }, { WAV_kyfpos09, 1 },  { WAV_kygpos09, 1 }, { WAV_kyhpos09, 1 }, { WAV_kyipos09, 1 }, { WAV_kyjpos09, 1 }, { WAV_kykpos09, 1 }}, // TKS_LandOnUnownedGreen
   {{ WAV_kyaneg12, 1 }, { WAV_kybneg12, 1 }, { WAV_kycneg11, 1 }, { WAV_kydneg12, 1 }, { WAV_kyeneg12, 1 }, { WAV_kyfneg12, 1 },  { WAV_kygneg12, 1 }, { WAV_kyhneg12, 1 }, { WAV_kyineg11, 1 }, { WAV_kyjneg12, 1 }, { WAV_kykneg12, 1 }}, // TKS_LandOnLowerHit
   {{ WAV_kyapos10, 1 }, { WAV_kybpos11, 1 }, { WAV_kycpos12, 1 }, { WAV_kydpos11, 1 }, { WAV_kyepos10, 1 }, { WAV_kyfpos11, 1 },  { WAV_kygpos11, 1 }, { WAV_kyhpos11, 1 }, { WAV_kyipos11, 1 }, { WAV_kyjpos11, 1 }, { WAV_kykpos11, 1 }}, // TKS_LandOnJailNotEmpty
   {{ WAV_kyapos11, 1 }, { WAV_kybpos12, 1 }, { WAV_kycpos13, 1 }, { WAV_kydpos12, 1 }, { WAV_kyepos11, 1 }, { WAV_kyfpos12, 1 },  { WAV_kygpos12, 1 }, { WAV_kyhpos12, 1 }, { WAV_kyipos12, 1 }, { WAV_kyjpos12, 1 }, { WAV_kykpos12, 1 }}, // TKS_LandOnJailJustVisiting
   {{ WAV_kyaneg13, 1 }, { WAV_kybneg13, 1 }, { WAV_kycneg12, 1 }, { WAV_kydneg13, 1 }, { WAV_kyeneg13, 1 }, { WAV_kyfneg13, 1 },  { WAV_kygneg13, 1 }, { WAV_kyhneg13, 1 }, { WAV_kyineg12, 1 }, { WAV_kyjneg13, 1 }, { WAV_kykneg13, 1 }}, // TKS_LandOnLowLowHit
   {{ WAV_kyapos12, 1 }, { WAV_kybpos13, 1 }, { WAV_kycpos14, 1 }, { WAV_kydpos13, 1 }, { WAV_kyepos12, 1 }, { WAV_kyfpos13, 1 },  { WAV_kygpos13, 1 }, { WAV_kyhpos14, 1 }, { WAV_kyipos13, 1 }, { WAV_kyjpos13, 1 }, { WAV_kykpos13, 1 }}, // TKS_LandFreeParkingNotPaying
   {{ WAV_kyapos13, 1 }, { WAV_kybpos14, 1 }, { WAV_kycpos15, 1 }, { WAV_kydpos14, 1 }, { WAV_kyepos13, 1 }, { WAV_kyfpos14, 1 },  { WAV_kygpos14, 1 }, { WAV_kyhpos15, 1 }, { WAV_kyipos14, 1 }, { WAV_kyjpos14, 1 }, { WAV_kykpos14, 1 }}, // TKS_LandOnUnownedRailroadHoldingOther3
   {{ WAV_kyapos14, 1 }, { WAV_kybpos15, 1 }, { WAV_kycpos16, 1 }, { WAV_kydpos15, 1 }, { WAV_kyepos14, 1 }, { WAV_kyfpos15, 1 },  { WAV_kygpos15, 1 }, { WAV_kyhpos16, 1 }, { WAV_kyipos15, 1 }, { WAV_kyjpos15, 1 }, { WAV_kykpos15, 1 }}, // TKS_LandOnUnownedRailroad
   {{ WAV_kyaneg14, 1 }, { WAV_kybneg14, 1 }, { WAV_kycpos17, 1 }, { WAV_kydneg14, 1 }, { WAV_kyeneg14, 1 }, { WAV_kyfneg14, 1 },  { WAV_kygneg14, 1 }, { WAV_kyhneg14, 1 }, { WAV_kyineg13, 1 }, { WAV_kyjneg14, 1 }, { WAV_kykneg14, 1 }}, // TKS_LandOnLowestHit
   {{ WAV_kyapos15, 1 }, { WAV_kybpos16, 1 }, { WAV_kycpos18, 1 }, { WAV_kydpos16, 1 }, { WAV_kyepos15, 1 }, { WAV_kyfpos16, 1 },  { WAV_kygpos16, 1 }, { WAV_kyhpos17, 1 }, { WAV_kyipos16, 1 }, { WAV_kyjpos16, 1 }, { WAV_kykpos16, 1 }}, // TKS_LandOnUnownedUtilityHoldingOther
   {{ WAV_kyapos16, 1 }, { WAV_kybpos17, 1 }, { WAV_kycpos19, 1 }, { WAV_kydpos17, 1 }, { WAV_kyepos16, 1 }, { WAV_kyfpos17, 1 },  { WAV_kygpos17, 1 }, { WAV_kyhpos18, 1 }, { WAV_kyipos17, 1 }, { WAV_kyjpos17, 1 }, { WAV_kykpos17, 1 }}, // TKS_LandOnUnownedUtility
   {{ WAV_kyabuy01, 2 }, { WAV_kybbuy01, 2 }, { WAV_kycbuy01, 2 }, { WAV_kydbuy01, 2 }, { WAV_kyebuy01, 2 }, { WAV_kyfbuy01, 2 },  { WAV_kygbuy01, 2 }, { WAV_kyhbuy01, 2 }, { WAV_kyibuy01, 2 }, { WAV_kyjbuy01, 2 }, { WAV_kykbuy01, 2 }}, // TKS_BuySide4
   {{ WAV_kyabuy03, 2 }, { WAV_kybbuy03, 2 }, { WAV_kycbuy03, 2 }, { WAV_kydbuy03, 2 }, { WAV_kyebuy03, 2 }, { WAV_kyfbuy03, 2 },  { WAV_kygbuy03, 2 }, { WAV_kyhbuy03, 2 }, { WAV_kyibuy03, 2 }, { WAV_kyjbuy03, 2 }, { WAV_kykbuy03, 2 }}, // TKS_BuySide3
   {{ WAV_kyabuy05, 2 }, { WAV_kybbuy05, 2 }, { WAV_kycbuy05, 2 }, { WAV_kydbuy05, 2 }, { WAV_kyebuy05, 2 }, { WAV_kyfbuy05, 2 },  { WAV_kygbuy05, 2 }, { WAV_kyhbuy05, 2 }, { WAV_kyibuy05, 2 }, { WAV_kyjbuy05, 2 }, { WAV_kykbuy05, 2 }}, // TKS_BuySide2
   {{ WAV_kyabuy07, 2 }, { WAV_kybbuy07, 2 }, { WAV_kycbuy07, 2 }, { WAV_kydbuy07, 2 }, { WAV_kyebuy07, 2 }, { WAV_kyfbuy07, 2 },  { WAV_kygbuy07, 2 }, { WAV_kyhbuy07, 2 }, { WAV_kyibuy07, 2 }, { WAV_kyjbuy07, 2 }, { WAV_kykbuy07, 2 }}, // TKS_BuySide1
   {{ WAV_kyaauct1, 2 }, { WAV_kybauct1, 2 }, { WAV_kycauct1, 2 }, { WAV_kydauct1, 2 }, { WAV_kyeauct1, 2 }, { WAV_kyfauct1, 2 },  { WAV_kygauct1, 1 }, { WAV_kyhauct1, 2 }, { WAV_kyiauct1, 2 }, { WAV_kyjauct1, 2 }, { WAV_kykauct1, 2 }}, // TKS_ChoseAuction
   {{ WAV_kyabld01, 2 }, { WAV_kybbld02, 2 }, { WAV_kycbld01, 2 }, { WAV_kydbld01, 2 }, { WAV_kyebld01, 2 }, { WAV_kyfbld01, 2 },  { WAV_kygbld01, 2 }, { WAV_kyhbld01, 2 }, { WAV_kyibld01, 2 }, { WAV_kyjbld01, 2 }, { WAV_kykbld01, 2 }}, // TKS_BuildingHouseAfterthought
   {{ WAV_kyasel01, 2 }, { WAV_kybsel01, 2 }, { WAV_kycsel01, 2 }, { WAV_kydsel01, 2 }, { WAV_kyesel01, 2 }, { WAV_kyfsel01, 2 },  { WAV_kygsel00, 2 }, { WAV_kyhsel01, 2 }, { WAV_kyisel01, 2 }, { WAV_kyjsel01, 2 }, { WAV_kyksel01, 2 }}, // TKS_SellHouseAfterthought
   {{ WAV_kyasel03, 2 }, { WAV_kybsel03, 2 }, { WAV_kycsel03, 2 }, { WAV_kydsel03, 2 }, { WAV_kyesel03, 2 }, { WAV_kyfsel03, 2 },  { WAV_kygsel02, 2 }, { WAV_kyhsel03, 2 }, { WAV_kyisel03, 2 }, { WAV_kyjsel03, 2 }, { WAV_kyksel03, 2 }}, // TKS_ProposingTrade
   {{ WAV_kyasel05, 2 }, { WAV_kybsel05, 2 }, { WAV_kycsel05, 2 }, { WAV_kydsel05, 2 }, { WAV_kyesel05, 2 }, { WAV_kyfsel05, 2 },  { WAV_kygsel04, 2 }, { WAV_kyhsel05, 2 }, { WAV_kyisel05, 2 }, { WAV_kyjsel05, 2 }, { WAV_kyksel05, 2 }}, // TKS_MortgageAfterthought
   {{ WAV_kyasel07, 2 }, { WAV_kybsel07, 2 }, { WAV_kycsel07, 2 }, { WAV_kydsel07, 2 }, { WAV_kyesel07, 2 }, { WAV_kyfsel07, 2 },  { WAV_kygsel06, 2 }, { WAV_kyhsel07, 2 }, { WAV_kyisel07, 2 }, { WAV_kyjsel07, 2 }, { WAV_kyksel07, 2 }}, // TKS_UnmortgageAfterthought
   {{ WAV_kyacncd1, 3 }, { WAV_kybcncd1, 3 }, { WAV_kycncd1 , 2 }, { WAV_kydcncd1, 2 }, { WAV_kyecncd1, 2 }, { WAV_kyfcncd1, 2 },  { WAV_kygcncd1, 3 }, { WAV_kyhcncd1, 2 }, { WAV_kyicncd1, 2 }, { WAV_kyjcncd1, 2 }, { WAV_kykcncd1, 2 }}, // TKS_GiveUp
   {{ WAV_kyabkrt4, 2 }, { WAV_kybbkrt2, 2 }, { WAV_kycbkrt2, 2 }, { WAV_kydbkrt4, 2 }, { WAV_kyebkrt2, 3 }, { WAV_kyfbkrt2, 2 },  { WAV_kygbkrt5, 2 }, { WAV_kyhbkrt2, 2 }, { WAV_kyibkrt2, 3 }, { WAV_kyjbkrt2, 2 }, { WAV_kykbkrt3, 2 }}, // TKS_Bankrupt
   {{ WAV_kyaendt1, 1 }, { WAV_kybendt1, 2 }, { WAV_kycendt1, 1 }, { WAV_kydendt1, 1 }, { WAV_kyeendt1, 1 }, { WAV_kyfendt1, 1 },  { WAV_kygendt1, 2 }, { WAV_kyhendt1, 1 }, { WAV_kyiendt1, 1 }, { WAV_kyjendt1, 1 }, { WAV_kykendt1, 1 }}, // TKS_DoneTurn
   {{ WAV_kyaendg1, 6 }, { WAV_kybendg1, 7 }, { WAV_kycendg1, 7 }, { WAV_kydendg1, 6 }, { WAV_kyeendg1, 5 }, { WAV_kyfendg1, 4 },  { WAV_kygendg1, 5 }, { WAV_kyhendg1, 7 }, { WAV_kyiendg1, 6 }, { WAV_kyjendg1, 6 }, { WAV_kykendg1, 6 }}, // TKS_WonGame
   {{ WAV_kyapos17, 7 }, { WAV_kybpos18, 6 }, { WAV_kycpos20, 6 }, { WAV_kydpos18, 7 }, { WAV_kyepos17, 7 }, { WAV_kyfpos18, 6 },  { WAV_kygpos18, 7 }, { WAV_kyhpos19, 9 }, { WAV_kyipos18, 8 }, { WAV_kyjpos18, 7 }, { WAV_kykpos18, 7 }}, // TKS_GenericGood
   {{ WAV_kyaneg15, 7 }, { WAV_kybneg15, 8 }, { WAV_kycneg13, 5 }, { WAV_kydneg15, 7 }, { WAV_kyeneg15, 7 }, { WAV_kyfneg15, 6 },  { WAV_kygneg15, 7 }, { WAV_kyhneg15, 8 }, { WAV_kyineg14, 6 }, { WAV_kyjneg15, 7 }, { WAV_kykneg15, 8 }}, // TKS_GenericBad
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

void DISPLAY_UDSOUND_Initialize( void )
{ // Initialize any permanent display objects.  Avoid temporary ones when possible.
  int t;

  DISPLAY_state.CurrentBackgroundMusic = LED_EI;

  DISPLAY_state.PennybagsLastSoundStarted = LED_EI;
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
    DISPLAY_state.TokenLastSoundStarted[t] = LED_EI;
  for( t = 0; t < DISPLAY_MAX_SOUND_LOCKS; t++ )
    DISPLAY_state.SoundLocks[t] = LED_EI;
  DISPLAY_state.SoundLockIndex = 0;
  DISPLAY_state.SoundLockGameQueueLockActive = FALSE;
  DISPLAY_state.SoundToPlayPostLock = DISPLAY_state.SoundPlayedPostLock = LED_EI;

  DISPLAY_state.SoundSystemClearToTest = FALSE;

  // BSSM sound/camera
  DISPLAY_state.viewPriorToBSSMCam = 0;
  DISPLAY_state.viewBSSMCamState = 0;
}

void DISPLAY_UDSOUND_Destroy( void )
{ // Deallocate stuff from Initialize.  It is safe to assume Hide has been called.
}


/*****************************************************************************
 * Tick Actions - 60 ticks per second are processed, though we get more than
 * one tick at a time if the framerate is suffering.
 */
void DISPLAY_UDSOUND_tickActions( int numberOfTicks )
{
  int t;
  BOOL test = FALSE;
  LE_SEQNCR_RuntimeInfoRecord      InfoGeneric;
  //static TYPE_Tick lastCheck = 0;

  if( !DISPLAY_state.SoundSystemClearToTest )
    return; // don't check for sound escalation until they have had a chance to start.

  // We process the sound stack.  This can be intensive, so we will only do it every 3rd tick at most.
  //if( lastCheck + 3 <= LE_TIME_TickCount )
  {// Lets check for sound
    //lastCheck = LE_TIME_TickCount;
    for( t = 0; t < DISPLAY_state.SoundLockIndex; t++ )
    {
      if( DISPLAY_state.SoundLocks[t] != LED_EI )
      { // We need to check this sound & clear the entry if it is complete or flag that lock persists if not.
        if(LE_SEQNCR_GetInfo( DISPLAY_state.SoundLocks[t], 0, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {// Sound is running
          if( InfoGeneric.endTime > InfoGeneric.sequenceClock ) // Sound is not done
            test = TRUE; // The sound is not finished
          else
            DISPLAY_state.SoundLocks[t] = LED_EI;
        } else
        {// Sound is not running - either not started or shut itself down, we assume the latter.
          DISPLAY_state.SoundLocks[t] = LED_EI;
        }
      }
    }
  }

  // Errors of this nature are sounds issued without locks which dont start, then a 'wait for all to finish' is issued before they start
  // Set a breakpoint to find them, then put a manual lock on em to avoid this.
  if( DISPLAY_state.SoundToPlayPostLock != LED_EI && !DISPLAY_state.SoundLockGameQueueLockActive )
    DISPLAY_state.SoundToPlayPostLock = LED_EI;

  // Finally, check if we are done and need to release any locks.
  if( DISPLAY_state.SoundLockGameQueueLockActive && !test ) // No sounds but locked - kill the lock.
  //if( DISPLAY_state.SoundPlayedPostLock != LED_EI && !test ) // No sounds but locked - kill the lock.
  {// First one stacked issues the lock.  Unlock is in tickactions.
    DISPLAY_state.SoundLockGameQueueLockActive = FALSE;
    gameQueueUnLock();
    DISPLAY_state.SoundLockIndex = 0; // Will be full of LED_EI's at this point.

    // Now, check for a sound waiting for the lock to clear to start.  It is not itself locked.
    if( DISPLAY_state.SoundToPlayPostLock != LED_EI )
    {
      DISPLAY_state.SoundPlayedPostLock = DISPLAY_state.SoundToPlayPostLock;
      if( DISPLAY_state.SoundToPlayPostLockToken == RULE_MAX_PLAYERS )
        DISPLAY_state.PennybagsLastSoundStarted = DISPLAY_state.SoundPlayedPostLock;
      else
        DISPLAY_state.TokenLastSoundStarted[DISPLAY_state.SoundToPlayPostLockToken] = DISPLAY_state.SoundPlayedPostLock;

      DISPLAY_state.SoundToPlayPostLock = LED_EI;
      LE_SEQNCR_Start( DISPLAY_state.SoundPlayedPostLock, 0 );
      LE_SEQNCR_SetVolume( DISPLAY_state.SoundPlayedPostLock, 0, DISPLAY_state.SoundPlayedPostVolume );
      unprocessedSoundIssued = TRUE;
    }
  }

}


void DISPLAY_UDSOUND_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything is in the user interface and display states.

  // NOTE:  Haven't really thought about how this applies to sound.  Using a more linear approach.
  UDSOUND_BackgroundMusicUpdate();
}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDSOUND_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{
}


/****************************************************************************/
/*****************************************************************************
 * Various 'start sound' routines
 */
void UDSOUND_Warning( void )
{
  LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_beep), 0 ); // Warning sound
}

void UDSOUND_Click( void )
{
  LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_btn1), 0 ); // Generic cheapo button click
  LE_SEQNCR_SetVolume( LED_IFT(DAT_MAIN, WAV_btn1), 0, 25 );
}

void UDSOUND_CashUpFx( void )
{
  //LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_moneyclicka1), 0 );
  LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_cash_in), 0 );
}

void UDSOUND_CashDownFx( void )
{
  //LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_moneyclickb4), 0 );
  LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_cash_out), 0 );
}





/*
 * Return the status - if we skipped.  Return value is just true/false at first implementation.
 */
int UDSOUND_TokenGenericPlaySound( int token, int soundType, int clipaction )
{// Temporary code, fix it up.
  //LE_SEQNCR_RuntimeInfoRecord      InfoGeneric;
  BOOL lastIsRunning = FALSE;
  BOOL skip = FALSE;
  BOOL LockNewSound = FALSE;
  BOOL waitCheck = FALSE;

  LE_DATA_DataId  tempId;
  int numAlternates;


  // not sure if this will work yet
  // if the user sets the token voices flag to off, simply return out of the function
  if(!DISPLAY_state.IsOptionTokenVoicesOn)
	  return skip;

  //TRACE("Token sound, action %d time %d\n", clipaction, LE_TIME_TickCount);
  // This section determines the status of any existing sounds playing, including stopping
  // sounds to make the status 'all quiet' if clipping is requested.
  switch(clipaction)
  {
  case Sound_ClipOldSoundIfPlaying:
    UD_SOUND_KillAllTalkingSounds();
    break;
  case Sound_ClipOldSoundIfPlayingWithLock:
    UD_SOUND_KillAllTalkingSounds();
    LockNewSound = TRUE;
    break;
  case Sound_SkipIfOldSoundPlaying:
    skip = UD_SOUND_CheckForTalkingSounds();
    break;
  case Sound_WaitForAnyOldSoundThenPlay:
    waitCheck = UD_SOUND_CheckForTalkingSoundsWithLock(); // Note the new sound wont be locked - add another type if you need that feature.
    break;
  case Sound_WaitForAnyOldSoundThenPlayWithLock:
    waitCheck = UD_SOUND_CheckForTalkingSoundsWithLock();
    LockNewSound = TRUE;
    break;
  default:
    break;
  }

  if( !skip )
  {
    tempId = UDSOUND_Token_Sound_Details[ soundType ][ token ][0];
    numAlternates = UDSOUND_Token_Sound_Details[ soundType ][ token ][1];
    if(numAlternates <= 0)
      numAlternates = 0;// Means no sound is available.
    else
      tempId += rand()%numAlternates;
    if( tempId == 0 || numAlternates == 0 )
      skip = TRUE;
  }
  if( !skip )
  {
    if( waitCheck )
    { // Queue the sound - it will start when the lock clears.
      DISPLAY_state.SoundToPlayPostLock = LED_IFT( DAT_LANGDIALOG, tempId );
      DISPLAY_state.SoundToPlayPostLockToken = token;
      DISPLAY_state.SoundPlayedPostVolume = TokenVolumes[token];
    } else
    {
      DISPLAY_state.TokenLastSoundStarted[token] = LED_IFT( DAT_LANGDIALOG, tempId );
      LE_SEQNCR_Start( DISPLAY_state.TokenLastSoundStarted[token], 0 );
      LE_SEQNCR_SetVolume( DISPLAY_state.TokenLastSoundStarted[token], 0, TokenVolumes[token] );
      if( LockNewSound )
        UDSOUND_WatchSoundWithQueueLock( DISPLAY_state.TokenLastSoundStarted[token] );
    }
    unprocessedSoundIssued = TRUE;
  }

  DISPLAY_state.SoundSystemClearToTest = FALSE;// Make sure sequences begin before escalation code runs.

  return( skip );// enumerate something if any caller ever cares...
}


//Update background music score - should be called each display_show.
void UDSOUND_BackgroundMusicUpdate( void )
{
  LE_DATA_DataId desiredMusic = LED_EI;
  static int creditsStatus = 0;

  // Figure out what should be playing
  if(DISPLAY_state.IsOptionGameStarted)
  {
    if( DISPLAY_state.IsOptionMusicOn )
    {// User wants to listen to a tune
      desiredMusic = LE_DATA_IdFromTag(DAT_MAIN, WAV_m1) + DISPLAY_state.OptionMusicTuneIndex;
    }
  }
  // Override if looking at credits
  if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_Options && current_options_result == CREDITS_SCREEN )
  {
    switch( creditsStatus )
    { 
    case 0:// Fire it up
      desiredMusic = LE_DATA_IdFromTag(DAT_MAIN, WAV_credit_a);
      // This might buffer the next looping tune
      LE_DATA_UseRef( LE_DATA_IdFromTag(DAT_MAIN, WAV_m1) );
      LE_DATA_UnloadRef( LE_DATA_IdFromTag(DAT_MAIN, WAV_m1) );
      creditsStatus = 1;
      break;

    case 1:// Monitor first sound
      desiredMusic = LE_DATA_IdFromTag(DAT_MAIN, WAV_credit_a);
      if( !LE_SEQNCR_GetInfo( LE_DATA_IdFromTag(DAT_MAIN, WAV_credit_a), 0, FALSE,
        NULL, NULL, NULL, NULL, NULL ) )
        creditsStatus = 2;// Must be done.
      else
        break;

    case 2:// Final looping music
      desiredMusic = LE_DATA_IdFromTag(DAT_MAIN, WAV_m1);
      break;
    }
  } else
    creditsStatus = 0;

  // Adjust what is playing if required.
  if( desiredMusic != DISPLAY_state.CurrentBackgroundMusic )
  {// Update music
    if( DISPLAY_state.CurrentBackgroundMusic != LED_EI )
		  LE_SEQNCR_Stop( DISPLAY_state.CurrentBackgroundMusic, 0 );

    DISPLAY_state.CurrentBackgroundMusic = desiredMusic;

    if( DISPLAY_state.CurrentBackgroundMusic != LED_EI )
    {
      LE_SEQNCR_Start( DISPLAY_state.CurrentBackgroundMusic, 0 );
      // Credits music does not loop, let it die and regular tune starts.
      if( desiredMusic != LE_DATA_IdFromTag(DAT_MAIN, WAV_credit_a) )
        LE_SEQNCR_SetEndingAction( DISPLAY_state.CurrentBackgroundMusic, 0,
          LE_SEQNCR_EndingActionLoopToBeginning );
      IsBackgroundMusicOn = TRUE;
      LE_SEQNCR_SetVolume( DISPLAY_state.CurrentBackgroundMusic, 0, 20 );
    }
  }
  
/*  if(DISPLAY_state.IsOptionGameStarted)
  {
    if( !IsBackgroundMusicOn )
    {
      LE_SEQNCR_Start( LE_DATA_IdFromTag(DAT_MAIN, WAV_m2kthm1), 0 );
      LE_SEQNCR_SetEndingAction( LE_DATA_IdFromTag(DAT_MAIN, WAV_m2kthm1), 0,
        LE_SEQNCR_EndingActionLoopToBeginning );
      IsBackgroundMusicOn = TRUE;
    }
    LE_SEQNCR_SetVolume( LE_DATA_IdFromTag(DAT_MAIN, WAV_m2kthm1), 0, 40 );
  }*/
}

// Music Off is called from music update or possibly? shutdown.  Not anywhere at time of writing.
void UDSOUND_BackgroundMusicOff( void )
{
	if(DISPLAY_state.IsOptionGameStarted)
	{
	  if( IsBackgroundMusicOn )
	  {
		LE_SEQNCR_Stop( DISPLAY_state.CurrentBackgroundMusic, 0 );
    DISPLAY_state.CurrentBackgroundMusic = LED_EI;
		IsBackgroundMusicOn = FALSE;
	  }
	}
}


// This function adds a sound to a watch list - it will lock the queue until all sounds are finished
// Note we don't do sound checks here - look to tickactions - as the sound may not have started yet.
void UDSOUND_WatchSoundWithQueueLock( LE_DATA_DataId sound )
{
  int t;
  BOOL test;

  for( t = 0, test = FALSE; t < DISPLAY_state.SoundLockIndex; t++ )
  {
    if( DISPLAY_state.SoundLocks[t] == sound ) test = TRUE; // The sound is already on the list
  }
  if( DISPLAY_state.SoundLockIndex < DISPLAY_MAX_SOUND_LOCKS - 1 && !test )
  {// We have room for the new watch
    DISPLAY_state.SoundLocks[DISPLAY_state.SoundLockIndex++] = sound;
  }

  if( !DISPLAY_state.SoundLockGameQueueLockActive )
  {// First one stacked issues the lock.  Unlock is in tickactions.
    DISPLAY_state.SoundLockGameQueueLockActive = TRUE;
    gameQueueLock();
  }
}


void UD_SOUND_KillAllTalkingSounds( void )
{
  int t;

  if( DISPLAY_state.PennybagsLastSoundStarted != LED_EI )
  {// Check out Pennybags sound
    LE_SEQNCR_Stop( DISPLAY_state.PennybagsLastSoundStarted, 0 );
    DISPLAY_state.PennybagsLastSoundStarted = LED_EI; // Might save some checks another time.
  }

  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
  {
    if( DISPLAY_state.TokenLastSoundStarted[t] != LED_EI )
    {
      LE_SEQNCR_Stop( DISPLAY_state.TokenLastSoundStarted[t], 0 );
      DISPLAY_state.TokenLastSoundStarted[t] = LED_EI;
    }
  }
  // Kill posted sound
  DISPLAY_state.SoundToPlayPostLock = LED_EI;
}


// Will check for a talking token or Pennybags, stop checking (& updating status)
// as soon as any sound is found.
BOOL UD_SOUND_CheckForTalkingSounds( void )
{
  LE_SEQNCR_RuntimeInfoRecord     InfoGeneric;
  int                             t;
  BOOL foundARunningSound = FALSE;

  if( unprocessedSoundIssued ) return TRUE;
  if( DISPLAY_state.SoundToPlayPostLock != LED_EI ) return TRUE; // we have a posted sound

  if( DISPLAY_state.PennybagsLastSoundStarted != LED_EI )
  {// Check out Pennybags sound, find a reason to call it running or forget it.
    if( LE_SEQNCR_GetInfo( DISPLAY_state.PennybagsLastSoundStarted, 0, FALSE,
      &InfoGeneric, NULL, NULL, NULL, NULL ) )
    {
      if( InfoGeneric.sequenceClock < InfoGeneric.endTime )
        foundARunningSound = TRUE;
    } else// NOTE: It may be possible the sound has not started yet - be aware of this in case bugs lead you here.
      DISPLAY_state.PennybagsLastSoundStarted = LED_EI;
  }

  t = 0;
  while( !foundARunningSound && (t < RULE_MAX_PLAYERS) )
  {
    if( DISPLAY_state.TokenLastSoundStarted[t] != LED_EI )
    {
      if( LE_SEQNCR_GetInfo( DISPLAY_state.TokenLastSoundStarted[t], 0, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {// We know it exists
        if( InfoGeneric.sequenceClock < InfoGeneric.endTime )
          foundARunningSound = TRUE; // It is not finished
      } else// NOTE: It may be possible the sound has not started yet - be aware of this in case bugs lead you here.
        DISPLAY_state.TokenLastSoundStarted[t] = LED_EI;
    }
    t++;
  }// endwhile

  return( foundARunningSound );

}


// Will check for a talking token or Pennybags, adding any active sounds to the
// lock watch.
BOOL UD_SOUND_CheckForTalkingSoundsWithLock( void )
{// FIXME - what about unprocessed sounds?  Be careful, where multiple sounds are issued in a collect commands, manually add the first to the watch list.
  LE_SEQNCR_RuntimeInfoRecord     InfoGeneric;
  int                             t;
  BOOL foundARunningSound = FALSE;

  if( DISPLAY_state.SoundToPlayPostLock != LED_EI ) foundARunningSound = TRUE; // we have a posted sound

  if( DISPLAY_state.PennybagsLastSoundStarted != LED_EI )
  {// Check out Pennybags sound, find a reason to call it running or forget it.
    if( LE_SEQNCR_GetInfo( DISPLAY_state.PennybagsLastSoundStarted, 0, FALSE,
      &InfoGeneric, NULL, NULL, NULL, NULL ) )
    {
      if( InfoGeneric.sequenceClock < InfoGeneric.endTime )
      {
        foundARunningSound = TRUE;
        UDSOUND_WatchSoundWithQueueLock( DISPLAY_state.PennybagsLastSoundStarted );
      }
    } else// NOTE: It may be possible the sound has not started yet - be aware of this in case bugs lead you here.
    {// The sound may be in POST FIXME this is the problem.
      //if( DISPLAY_state.PennybagsLastSoundStarted ==
      if( !unprocessedSoundIssued )
        DISPLAY_state.PennybagsLastSoundStarted = LED_EI;
    }
  }

  t = 0;
  while( t < RULE_MAX_PLAYERS )
  {
    if( DISPLAY_state.TokenLastSoundStarted[t] != LED_EI )
    {
      if( LE_SEQNCR_GetInfo( DISPLAY_state.TokenLastSoundStarted[t], 0, FALSE,
        &InfoGeneric, NULL, NULL, NULL, NULL ) )
      {// We know it exists
        if( InfoGeneric.sequenceClock < InfoGeneric.endTime )
        {
          foundARunningSound = TRUE; // It is not finished
          UDSOUND_WatchSoundWithQueueLock( DISPLAY_state.TokenLastSoundStarted[t] );
        }
      } else// NOTE: It may be possible the sound has not started yet - be aware of this in case bugs lead you here.
      {
        if( !unprocessedSoundIssued )
          DISPLAY_state.TokenLastSoundStarted[t] = LED_EI;
      }
    }
    t++;
  }// endwhile

  return( foundARunningSound || unprocessedSoundIssued); // NOTE: The unprocessed sound will not be on the lock list automatically - calling code must check for it.
}


// Return to UDPENNY.cpp if anyone ever really cares.
void UDPENNY_PB_ReadCard( int lastCardIn )
{
	int wavTag = 0;
#if USA_VERSION
  UDPENNY_PennybagsSpecific( WAV_pb186 + lastCardIn, Sound_WaitForAnyOldSoundThenPlay );
#else
  // Everyone has forgotten why the euro audio files were taken out,
  // so we are putting them back in
  //if(DISPLAY_state.system == NOTW_MONA_EURO)
  //{
  //    wavTag = LED_EI;
  //}
  //else
  //{
    wavTag = UDPENNY_GetCorrectCardWav(lastCardIn);
  //}
  UDPENNY_PennybagsSpecific( wavTag, Sound_WaitForAnyOldSoundThenPlay );
#endif
};


// function to properly play card wav file in euro versions
int UDPENNY_GetCorrectCardWav(int lastCardIn)
{
	int wavTag = LED_EI;

    // set up a temp variable to point to the correct city when
    // playing a custom board
    int tempCity = DISPLAY_state.city;
    if(tempCity == -1)
        tempCity = iLangId -2;

#if !USA_VERSION
	switch(lastCardIn)
	{
	// check chance cards
	case 0:		// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0201; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0301; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0401; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0501; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0601; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0701; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0801; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0901; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1001; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1101; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1201; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1301; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0001; break;
				}				
				break;
				
	case 1:		// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_UK:			wavTag = WAV_cych0202; break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych022b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych022c; break;
											}
											break;
				case NOTW_BORD_FRENCH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											case NOTW_MONA_FRENCH:		wavTag = WAV_cych0302; break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych032b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych032c; break;
											}
											break;
				case NOTW_BORD_GERMAN:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											case NOTW_MONA_GERMAN:		wavTag = WAV_cych0402; break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych042b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych042c; break;
											}
											break;
				case NOTW_BORD_SPANISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											case NOTW_MONA_SPANISH:		wavTag = WAV_cych0502; break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych052b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych052c; break;
											}
											break;
				case NOTW_BORD_DUTCH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											case NOTW_MONA_DUTCH:		wavTag = WAV_cych0602; break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych062b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych062c; break;
											}
											break;
				case NOTW_BORD_SWEDISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0702; break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych072b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych072c; break;
											}
											break;
				case NOTW_BORD_FINNISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											case NOTW_MONA_FINNISH:		wavTag = WAV_cych0802; break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych082b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych082c; break;
											}
											break;
				case NOTW_BORD_DANISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											case NOTW_MONA_DANISH:		wavTag = WAV_cych092a; break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych092b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych092c; break;
											}
											break;
				case NOTW_BORD_NORWEGIAN:	// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1002; break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych102b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych102c; break;
											}
											break;
				case NOTW_BORD_AUSTRALIAN:	// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1102; break;
											//case NOTW_MONA_BELGIUM:		break;
											//case NOTW_MONA_SINGAPORE:	break;
											//case NOTW_MONA_EURO:		break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych112a; break;
											}
											break;
				case NOTW_BORD_BELGIUM:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1202; break;
											//case NOTW_MONA_SINGAPORE:	break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych122b; break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych122c; break;
											}
											break;
				case NOTW_BORD_SINGAPORE:	// switch on monatary system
											switch(DISPLAY_state.system)
											{
											//case NOTW_MONA_UK:		break;
											//case NOTW_MONA_FRENCH:	break;
											//case NOTW_MONA_GERMAN:	break;
											//case NOTW_MONA_SPANISH:	break;
											//case NOTW_MONA_DUTCH:	break;
											//case NOTW_MONA_SWEDISH:	break;
											//case NOTW_MONA_FINNISH:	break;
											//case NOTW_MONA_DANISH:	break;
											//case NOTW_MONA_NORWEGIAN:	break;
											//case NOTW_MONA_AUSTRALIAN:	break;
											//case NOTW_MONA_BELGIUM:		break;
											case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1302; break;
											//case NOTW_MONA_EURO:		break;
											//case NOTW_MONA_US:			break;
											default:					wavTag = WAV_cych132b; break;
											}
											break;
				default:	break;
				}					
				break;

	case 2:		// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			
				case NOTW_BORD_FRENCH:		
				case NOTW_BORD_GERMAN:		
				case NOTW_BORD_SPANISH:		
				case NOTW_BORD_DUTCH:		
				case NOTW_BORD_SWEDISH:		
				case NOTW_BORD_FINNISH:		
				case NOTW_BORD_DANISH:		
				case NOTW_BORD_NORWEGIAN:	
				case NOTW_BORD_AUSTRALIAN:	
				case NOTW_BORD_BELGIUM:		
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cychal03; break;
				default:	break;
				}					
				break;		
		
	case 3:		// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0204; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0304; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0404; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0504; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0604; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0704; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0804; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0904; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1004; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1104; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1204; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1304; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0004; break;
				}				
				break;
		
		
	case 4:		// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0205; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0305; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0405; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0505; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0605; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0705; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0805; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0905; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1005; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1105; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1205; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1305; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0005; break;
				}				
				break;		
		
		
	case 5:		// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0206; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0306; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0406; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0506; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0606; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0706; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0806; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0906; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1006; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1106; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1206; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1306; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0006; break;
				}				
				break;
		
		

	case 6:		// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			wavTag = WAV_cych0207; break;
				case NOTW_BORD_FRENCH:		wavTag = WAV_cych0307; break;
				case NOTW_BORD_GERMAN:		wavTag = WAV_cych0407; break;
				case NOTW_BORD_SPANISH:		wavTag = WAV_cych0507; break;
				case NOTW_BORD_DUTCH:		wavTag = WAV_cych0607; break;
				case NOTW_BORD_SWEDISH:		wavTag = WAV_cych0707; break;
				case NOTW_BORD_FINNISH:		wavTag = WAV_cych0807; break;
				case NOTW_BORD_DANISH:		wavTag = WAV_cych0907; break;
				case NOTW_BORD_NORWEGIAN:	wavTag = WAV_cych1007; break;
				case NOTW_BORD_AUSTRALIAN:	wavTag = WAV_cych1107; break;
				case NOTW_BORD_BELGIUM:		wavTag = WAV_cych1207; break;
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cych1307; break;
				default:	break;
				}					
				break;

	case 7:		// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_UK:		wavTag = WAV_cych0208; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych028b; break;
											default:				wavTag = WAV_cych028c; break;
											}
											break;
				case NOTW_BORD_FRENCH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_FRENCH:	wavTag = WAV_cych0308; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych038b; break;
											default:				wavTag = WAV_cych038c; break;
											}
											break;
				case NOTW_BORD_GERMAN:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_GERMAN:	wavTag = WAV_cych0408; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych048b; break;
											default:				wavTag = WAV_cych048c; break;
											}
											break;
				case NOTW_BORD_SPANISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_SPANISH:	wavTag = WAV_cych0508; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych058b; break;
											default:				wavTag = WAV_cych058c; break;
											}
											break;
				case NOTW_BORD_DUTCH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_DUTCH:	wavTag = WAV_cych0608; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych068b; break;
											default:				wavTag = WAV_cych068c; break;
											}
											break;
				case NOTW_BORD_SWEDISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_SWEDISH:	wavTag = WAV_cych0708; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych078b; break;
											default:				wavTag = WAV_cych078c; break;
											}
											break;
				case NOTW_BORD_FINNISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_FINNISH:	wavTag = WAV_cych0808; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych088b; break;
											default:				wavTag = WAV_cych088c; break;
											}
											break;
				case NOTW_BORD_DANISH:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_DANISH:	wavTag = WAV_cych0908; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych098b; break;
											default:				wavTag = WAV_cych098c; break;
											}
											break;
				case NOTW_BORD_NORWEGIAN:	// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1008; break;
											case NOTW_MONA_EURO:		wavTag = WAV_cych108a; break;
											default:					wavTag = WAV_cych108b; break;
											}
											break;
				case NOTW_BORD_AUSTRALIAN:	// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1108; break;
											default:					wavTag = WAV_cych118a; break;
											}
											break;
				case NOTW_BORD_BELGIUM:		// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_BELGIUM:	wavTag = WAV_cych1208; break;
											case NOTW_MONA_EURO:	wavTag = WAV_cych128b; break;
											default:				wavTag = WAV_cych128c; break;
											}
											break;
				case NOTW_BORD_SINGAPORE:	// switch on monatary system
											switch(DISPLAY_state.system)
											{
											case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1308; break;
											default:					wavTag = WAV_cych138a; break;
											}
											break;
				default:	break;
				}					
				break;
	case 8:		// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			
				case NOTW_BORD_FRENCH:		
				case NOTW_BORD_GERMAN:		
				case NOTW_BORD_SPANISH:		
				case NOTW_BORD_DUTCH:		
				case NOTW_BORD_SWEDISH:		
				case NOTW_BORD_FINNISH:		
				case NOTW_BORD_DANISH:		
				case NOTW_BORD_NORWEGIAN:	
				case NOTW_BORD_AUSTRALIAN:	
				case NOTW_BORD_BELGIUM:		
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cychal09; break;
				default:	break;
				}					
				break;

	case 9:		// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			
				case NOTW_BORD_FRENCH:		
				case NOTW_BORD_GERMAN:		
				case NOTW_BORD_SPANISH:		
				case NOTW_BORD_DUTCH:		
				case NOTW_BORD_SWEDISH:		
				case NOTW_BORD_FINNISH:		
				case NOTW_BORD_DANISH:		
				case NOTW_BORD_NORWEGIAN:	
				case NOTW_BORD_AUSTRALIAN:	
				case NOTW_BORD_BELGIUM:		
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cychal10; break;
				default:	break;
				}					
				break;

	case 10:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0211; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0311; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0411; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0511; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0611; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0711; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0811; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0911; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1011; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1111; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1211; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1311; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0011; break;
				}				
				break;
		
		

	case 11:	// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			
				case NOTW_BORD_FRENCH:		
				case NOTW_BORD_GERMAN:		
				case NOTW_BORD_SPANISH:		
				case NOTW_BORD_DUTCH:		
				case NOTW_BORD_SWEDISH:		
				case NOTW_BORD_FINNISH:		
				case NOTW_BORD_DANISH:		
				case NOTW_BORD_NORWEGIAN:	
				case NOTW_BORD_AUSTRALIAN:	
				case NOTW_BORD_BELGIUM:		
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cychal12; break;
				default:	break;
				}					
				break;

	case 12:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0213; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0313; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0413; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0513; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0613; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0713; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0813; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0913; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1013; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1113; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1213; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1313; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0013; break;
				}				
				break;
		
		
	case 13:	// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			wavTag = WAV_cych0214; break;
				case NOTW_BORD_FRENCH:		wavTag = WAV_cych0314; break;
				case NOTW_BORD_GERMAN:		wavTag = WAV_cych0414; break;
				case NOTW_BORD_SPANISH:		wavTag = WAV_cych0514; break;
				case NOTW_BORD_DUTCH:		wavTag = WAV_cych0614; break;
				case NOTW_BORD_SWEDISH:		wavTag = WAV_cych0714; break;
				case NOTW_BORD_FINNISH:		wavTag = WAV_cych0814; break;
				case NOTW_BORD_DANISH:		wavTag = WAV_cych0914; break;
				case NOTW_BORD_NORWEGIAN:	wavTag = WAV_cych1014; break;
				case NOTW_BORD_AUSTRALIAN:	wavTag = WAV_cych1114; break;
				case NOTW_BORD_BELGIUM:		wavTag = WAV_cych1214; break;
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cych1314; break;
				default:	break;
				}					
				break;

	case 14:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cych0215; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cych0315; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cych0415; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cych0515; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cych0615; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cych0715; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cych0815; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cych0915; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cych1015; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cych1115; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cych1215; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cych1315; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cych0015; break;
				}				
				break;
		
	case 15:	// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			
				case NOTW_BORD_FRENCH:		
				case NOTW_BORD_GERMAN:		
				case NOTW_BORD_SPANISH:		
				case NOTW_BORD_DUTCH:		
				case NOTW_BORD_SWEDISH:		
				case NOTW_BORD_FINNISH:		
				case NOTW_BORD_DANISH:		
				case NOTW_BORD_NORWEGIAN:	
				case NOTW_BORD_AUSTRALIAN:	
				case NOTW_BORD_BELGIUM:		
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cychal15; break;
				default:	break;
				}					
				break;

	// check community chest cards
	case 16:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0201; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0301; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0401; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0501; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0601; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0701; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0801; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0901; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1001; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1101; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1201; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1301; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0001; break;
				}				
				break;
		
	case 17:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0202; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0302; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0402; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0502; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0602; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0702; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0802; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0902; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1002; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1102; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1202; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1302; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0002; break;
				}				
				break;
		
		

	case 18:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0203; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0303; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0403; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0503; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0603; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0703; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0803; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0903; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1003; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1103; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1203; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1303; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0003; break;
				}				
				break;
		
		

	case 19:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0204; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0304; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0404; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0504; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0604; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0704; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0804; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0904; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1004; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1104; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1204; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1304; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0004; break;
				}				
				break;
		
		
	case 20:	// switch on country
				switch(tempCity)
				{
				case NOTW_BORD_UK:			
				case NOTW_BORD_FRENCH:		
				case NOTW_BORD_GERMAN:		
				case NOTW_BORD_SPANISH:		
				case NOTW_BORD_DUTCH:		
				case NOTW_BORD_SWEDISH:		
				case NOTW_BORD_FINNISH:		
				case NOTW_BORD_DANISH:		
				case NOTW_BORD_NORWEGIAN:	
				case NOTW_BORD_AUSTRALIAN:	
				case NOTW_BORD_BELGIUM:		
				case NOTW_BORD_SINGAPORE:	wavTag = WAV_cycoal05; break;
				default:	break;
				}					
				break;

	case 21:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0206; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0306; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0406; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0506; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0606; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0706; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0806; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0906; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1006; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1106; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1206; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1306; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0006; break;
				}				
				break;
				
		
	case 22:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0207; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0307; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0407; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0507; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0607; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0707; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0807; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0907; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1007; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1107; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1207; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1307; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0007; break;
				}				
				break;
		
		

	case 23:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0208; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0308; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0408; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0508; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0608; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0708; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0808; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0908; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1008; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1108; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1208; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1308; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0008; break;
				}				
				break;

		
	case 24:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0209; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0309; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0409; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0509; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0609; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0709; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0809; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0909; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1009; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1109; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1209; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1309; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0009; break;
				}				
				break;
		
		

	case 25:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0210; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0310; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0410; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0510; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0610; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0710; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0810; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0910; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1010; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1110; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1210; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1310; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0010; break;
				}				
				break;

		

	case 26:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0211; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0311; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0411; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0511; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0611; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0711; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0811; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0911; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1011; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1111; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1211; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1311; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0011; break;
				}				
				break;
		
		

	case 27:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0212; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0312; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0412; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0512; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0612; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0712; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0812; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0912; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1012; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1112; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1212; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1312; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0012; break;
				}				
				break;
		
		

	case 28:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0213; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0313; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0413; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0513; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0613; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0713; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0813; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0913; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1013; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1113; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1213; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1313; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0013; break;
				}				
				break;
		
		
		
	case 29:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0214; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0314; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0414; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0514; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0614; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0714; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0814; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0914; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1014; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1114; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1214; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1314; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0014; break;
				}				
				break;
				

	case 30:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0215; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0315; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0415; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0515; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0615; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0715; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0815; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0915; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1015; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1115; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1215; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1315; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0015; break;
				}				
				break;
				

	case 31:	// switch on monatary system
				switch(DISPLAY_state.system)
				{
				case NOTW_MONA_UK:			wavTag = WAV_cyco0216; break;
				case NOTW_MONA_FRENCH:		wavTag = WAV_cyco0316; break;
				case NOTW_MONA_GERMAN:		wavTag = WAV_cyco0416; break;
				case NOTW_MONA_SPANISH:		wavTag = WAV_cyco0516; break;
				case NOTW_MONA_DUTCH:		wavTag = WAV_cyco0616; break;
				case NOTW_MONA_SWEDISH:		wavTag = WAV_cyco0716; break;
				case NOTW_MONA_FINNISH:		wavTag = WAV_cyco0816; break;
				case NOTW_MONA_DANISH:		wavTag = WAV_cyco0916; break;
				case NOTW_MONA_NORWEGIAN:	wavTag = WAV_cyco1016; break;
				case NOTW_MONA_AUSTRALIAN:	wavTag = WAV_cyco1116; break;
				case NOTW_MONA_BELGIUM:		wavTag = WAV_cyco1216; break;
				case NOTW_MONA_SINGAPORE:	wavTag = WAV_cyco1316; break;
				case NOTW_MONA_EURO:		wavTag = WAV_cyco0016; break;
				}				
				break;

	}

#endif	//!USA_VERSION
	return wavTag;
}



/*****************************************************************************
 * BSSM - Play sound effect, conditionally PB sound, conditionally Camera move
 */
void UDSOUND_HandleBSSMSoundNCam(int player, int square, int action)
{
  static initialized = FALSE; // Since we are using locals we need to initialize em here.

  static TYPE_Tick LastSoundPlayedAt[RULE_MAX_PLAYERS][4]; // BSSM to track
  const int secondsBetweenSounds = 30;


  if( !initialized)
  {
    int t;
    for( t = 0; t < RULE_MAX_PLAYERS; t++ )
    {
      LastSoundPlayedAt[t][0] = 0;
      LastSoundPlayedAt[t][1] = 0;
      LastSoundPlayedAt[t][2] = 0;
      LastSoundPlayedAt[t][3] = 0;
    }
    initialized = TRUE;
  }

  if( player < 0 || player >= RULE_MAX_PLAYERS || action < 0 || action >= 4 )
    return;

  // Sound effects - always play
  switch( action )
  {
  case 0:// Buy
    LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_build), 0 );
    break;
  case 1:// Sell
    LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_unbuild), 0 );
    break;
  case 2:// Mortgage
    UDSOUND_Click();
    break;
  case 3:// Unmortgage
    UDSOUND_Click();
    break;
  }
 
  // Camera requests
  if( action == 0 || action == 1 || DISPLAY_state.viewBSSMCamState > 0 )// If we are already moving the camera, show mort/unmort
  {// Camera move desired
    if( ((CameraInterpolationData.CurrentTime == CameraInterpolationData.EndTime || CameraIsFloatingIdle) && 
      !DISPLAY_state.manualCamLock &&
      !DISPLAY_state.DemoModeDesired && //DISPLAY_state.board3Don && DISPLAY_state.game3Don && // Skip in 2D 'cause we can't easily come back.
      !DISPLAY_state.manualMouseCamLock ) || DISPLAY_state.viewBSSMCamState > 0)
    {// Camera is idle (or following one of our moves, we can go ahead.
      if( DISPLAY_state.viewBSSMCamState == 0 ) 
      {// Only record the first view - we can have a chain of BSSM shifts
        DISPLAY_state.viewPriorToBSSMCam = DISPLAY_state.desired2DCamera;
      }
      DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( square );
      DISPLAY_state.viewLastRequested = DISPLAY_state.desired2DCamera;
      DISPLAY_state.viewBSSMCamState = 1;// Flag we have issued a BSSM cam (becomes 2 next camera change).
      DISPLAY_state.viewBSSMCamRequestTime = LE_TIME_TickCount;
    }
  }

  {// Issue a camera move too
    //DISPLAY_state.viewPriorToBSSMCam = DISPLAY_state.desired2DCamera;
    //DISPLAY_state.viewBSSMCamState = 1;
  }

  // What to have PB say? Not much.
  if( LE_TIME_TickCount > LastSoundPlayedAt[player][action] + secondsBetweenSounds * 60 )
  {// Play it
    switch( action )
    {
    case 0:// Buy
      //PB_PlayerBuiltFirstHotel could be coded
      if( LastSoundPlayedAt[player][action] == 0 )
      {// First time
        UDPENNY_PennybagsGeneral( PB_PlayerBuiltFirstHouse, Sound_SkipIfOldSoundPlaying );
      } else
      {// Another house (no lines)
      }
      break;
    case 1:// Sell - no lines
      break;
    case 2:// Mortgage - no lines
      break;
    case 3:// Unmortgage - no lines
      break;
    }
    LastSoundPlayedAt[player][action] = LE_TIME_TickCount;
    
  }
}


void UDSOUND_PlaySiren( void )
{
  LE_DATA_DataId siren;

  if(!DISPLAY_state.IsOptionTokenVoicesOn)
	  return;

  siren = LED_IFT( DAT_3D, WAV_siren1 + rand()%2 );

  LE_SEQNCR_Start( siren, 0 );
  LE_SEQNCR_SetVolume( siren, 0, 74 );
}
