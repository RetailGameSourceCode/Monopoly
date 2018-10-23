// ===========================================================================
// Module:      TexInfo.cpp
//
// Description: Contains array definitions of texture info for the various
//              kinds of boards available in monopoly
//
// Created:     August 27, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "GameInc.h"
#include "TexInfo.h"

// Common texture components =================================================

LPCTSTR g_aszCommonMed128Textures[NUM_MEDCOMMON_TEXTURES] =
{
  TEXT("CT11_128.bmp"),
  TEXT("CT12_128.bmp"),
  TEXT("CT13_128.bmp"),
  TEXT("CT14_128.bmp")
};

LPCTSTR g_aszCommonMed256Textures[NUM_MEDCOMMON_TEXTURES] =
{
  TEXT("CT11_256.bmp"),
  TEXT("CT12_256.bmp"),
  TEXT("CT13_256.bmp"),
  TEXT("CT14_256.bmp")
};

LPCTSTR g_aszCommonHigh128Textures[NUM_HIGHCOMMON_TEXTURES] =
{
  TEXT("CT10_128.bmp"),
  TEXT("CT11_128.bmp"),
  TEXT("CT12_128.bmp"),
  TEXT("CT13_128.bmp"),
  TEXT("CT14_128.bmp"),
  TEXT("CT15_128.bmp"),
  TEXT("CT16_128.bmp"),
  TEXT("CT17_128.bmp"),
  TEXT("CT18_128.bmp"),
  TEXT("CT19_128.bmp"),
  TEXT("CT20_128.bmp"),
  TEXT("CT21_128.bmp"),
  TEXT("CT22_128.bmp")
};

LPCTSTR g_aszCommonHigh256Textures[NUM_HIGHCOMMON_TEXTURES] =
{
  TEXT("CT10_256.bmp"),
  TEXT("CT11_256.bmp"),
  TEXT("CT12_256.bmp"),
  TEXT("CT13_256.bmp"),
  TEXT("CT14_256.bmp"),
  TEXT("CT15_256.bmp"),
  TEXT("CT16_256.bmp"),
  TEXT("CT17_256.bmp"),
  TEXT("CT18_256.bmp"),
  TEXT("CT19_256.bmp"),
  TEXT("CT20_256.bmp"),
  TEXT("CT21_256.bmp"),
  TEXT("CT22_256.bmp")
};

LPCTSTR g_aszCity128Photos[NUM_CITY_PHOTOS] =
{
  TEXT("CT01_128.BMP"),
  TEXT("CT02_128.BMP"),
  TEXT("CT03_128.BMP"),
  TEXT("CT04_128.BMP"),
  TEXT("CT05_128.BMP"),
  TEXT("CT06_128.BMP"),
};

LPCTSTR g_aszCity256Photos[NUM_CITY_PHOTOS] =
{
  TEXT("CT01_256.BMP"),
  TEXT("CT02_256.BMP"),
  TEXT("CT03_256.BMP"),
  TEXT("CT04_256.BMP"),
  TEXT("CT05_256.BMP"),
  TEXT("CT06_256.BMP")
};

// Medium classic board stuff ================================================

const WORDPAIR g_aBoardMedTxtrPositions[NUM_MEDBOARD_TEXTURES] =
{
  { 448,   0 }, // 1
  { 320,   0 }, // 2
  { 256,   0 }, // 3
  { 192,   0 }, // 4
  {  64,   0 }, // Borders, Center text
  {   0,   0 }, // 4 Corners
  { 128,   0 }, // Logos
  { 384,   0 }  // Prices
};

LPCTSTR g_aszBoardMed256Textures[NUM_MEDCITY_CITIES] =
{
  TEXT("BRD01_256.bmp"),
  TEXT("BRD02_256.bmp"),
  TEXT("BRD03_256.bmp"),
  TEXT("BRD04_256.bmp")
};

// High classic board stuff ==================================================

const WORDPAIR g_aBoardHighTxtrPositions[NUM_HIBOARD_TEXTURES] =
{
  { 192,   0 }, // 01
  { 128,   0 }, // 02
  {  64,   0 }, // 03
  { 448,   0 }, // 04
  { 384,   0 }, // 05
  { 320,   0 }, // 06
  { 256,   0 }, // 07
  { 384, 128 }, // 08
  { 320, 128 }, // 09
  { 256, 128 }, // 10
  { 192, 128 }, // 11
  { 128, 128 }, // 12
  {  64, 128 }, // 13
  {   0, 128 }, // 14
  {   0,   0 }, // 15
  { 448, 128 }  // 16
};

LPCTSTR g_aszBoardHigh256Textures[NUM_HICITY_CITIES] =
{
  TEXT("BRD01_256.bmp"),
  TEXT("BRD02_256.bmp"),
  TEXT("BRD03_256.bmp")
};


// Medium city board stuff ===================================================

const WORDPAIR g_aCityMedTxtrPositions[NUM_MEDCITY_TEXTURES] =
{
  { 320,   0 },
  { 256,   0 },
  { 192,   0 },
  { 128,   0 },
  {  64,   0 },
  {   0,   0 },
  { 320, 128 },
  { 384, 128 },
  { 256, 128 },
  { 192, 128 },
  {  64, 128 },
  {   0, 128 },
  { 128, 128 },
  { 384,   0 }
};

LPCTSTR g_aszCityMed128Textures[NUM_MEDCITY_CITIES] =
{
  TEXT("CT07_128.BMP"),
  TEXT("CT08_128.BMP"),
  TEXT("CT09_128.BMP"),
  TEXT("CT10_128.BMP")
};

LPCTSTR g_aszCityMed256Textures[NUM_MEDCITY_TEXTURES] =
{
  TEXT("CT07_256.BMP"),
  TEXT("CT08_256.BMP"),
  TEXT("CT09_256.BMP"),
  TEXT("CT10_256.BMP")
};

// High city board stuff =====================================================

const WORDPAIR g_aCityHighTxtrPositions[NUM_HICITY_TEXTURES] =
{
  { 512,   0 }, // 01
  { 448,   0 }, // 02
  { 128, 128 }, // 03
  {  64, 128 }, // 04
  {   0, 128 }, // 05
  {   0,   0 }, // 06
  { 192,   0 }, // 07
  { 128,   0 }, // 08
  {  64,   0 }, // 09
  { 640,   0 }, // 10
  { 384,   0 }, // 11
  { 320,   0 }, // 12
  { 256,   0 }, // 13
  { 448, 128 }, // 14
  { 384, 128 }, // 15
  { 320, 128 }, // 16
  { 256, 128 }, // 17
  { 192, 128 }, // 18
  { 576,   0 }, // 19
  { 576, 128 }, // 20
  { 512, 128 }, // 21
  { 640, 128 }  // 22
};

LPCTSTR g_aszCityHigh128Textures[NUM_HICITY_CITIES] =
{
  TEXT("CT07_256.BMP"),
  TEXT("CT08_256.BMP"),
  TEXT("CT09_256.BMP")
};

LPCTSTR g_aszCityHigh256Textures[NUM_HICITY_TEXTURES] =
{
  TEXT("CT07_256.BMP"),
  TEXT("CT08_256.BMP"),
  TEXT("CT09_256.BMP")
};

// 2D board stuff ============================================================

LPCTSTR g_asz2DBoards[NUM_2DBOARDS] =
{
  TEXT("2DVIEW01.BMP"),
  TEXT("2DVIEW02.BMP"),
  TEXT("2DVIEW03.BMP"),
  TEXT("2DVIEW04.BMP"),
  TEXT("2DVIEW05.BMP"),
  TEXT("2DVIEW06.BMP"),
  TEXT("2DVIEW07.BMP"),
  TEXT("2DVIEW08.BMP"),
  TEXT("2DVIEW09.BMP"),
  TEXT("2DVIEW10.BMP"),
  TEXT("2DVIEW11.BMP"),
  TEXT("2DVIEW12.BMP"),
  TEXT("2DVIEW13.BMP"),
  TEXT("2DVIEW14.BMP"),
  TEXT("2DVIEW15.BMP"),
  TEXT("2DVIEW16.BMP"),
  TEXT("2DVIEW17.BMP"),
  TEXT("2DVIEW18.BMP"),
  TEXT("2DVIEW19.BMP"),
  TEXT("2DVIEW20.BMP"),
  TEXT("2DVIEW21.BMP"),
  TEXT("2DVIEW22.BMP"),
  TEXT("2DVIEW23.BMP"),
  TEXT("2DVIEW24.BMP"),
  TEXT("2DVIEW25.BMP"),
  TEXT("2DVIEW26.BMP"),
  TEXT("2DVIEW27.BMP"),
  TEXT("2DVIEW28.BMP"),
  TEXT("2DVIEW29.BMP"),
  TEXT("2DVIEW30.BMP"),
  TEXT("2DVIEW31.BMP"),
  TEXT("2DVIEW32.BMP"),
  TEXT("2DVIEW33.BMP"),
  TEXT("2DVIEW34.BMP"),
  TEXT("2DVIEW35.BMP"),
  TEXT("2DVIEW36.BMP"),
  TEXT("2DVIEW37.BMP"),
  TEXT("2DVIEW38.BMP"),
  TEXT("2DVIEW39.BMP")
};

// ===========================================================================
// European stuff ============================================================
// ===========================================================================

LPCTSTR g_aszEUROMed128Boards[NUM_EURO_MEDIUMBOARDS] =
{
  TEXT("BRD01_128.BMP"),
  TEXT("BRD02_128.BMP"),
  TEXT("BRD03_128.BMP"),
  TEXT("BRD04_128.BMP")
};

LPCTSTR g_aszEUROMed256Boards[NUM_EURO_MEDIUMBOARDS] =
{
  TEXT("BRD01_256.BMP"),
  TEXT("BRD02_256.BMP"),
  TEXT("BRD03_256.BMP"),
  TEXT("BRD04_256.BMP")
};

LPCTSTR g_aszEUROHigh128Boards[NUM_EURO_HIGHBOARDS] =
{
  TEXT("BRD01_128.BMP"),
  TEXT("BRD02_128.BMP"),
  TEXT("BRD03_128.BMP")
};

LPCTSTR g_aszEUROHigh256Boards[NUM_EURO_HIGHBOARDS] =
{
  TEXT("BRD01_256.BMP"),
  TEXT("BRD02_256.BMP"),
  TEXT("BRD03_256.BMP")
};


LPCTSTR g_aszEUROMed128Currencies[NUM_EURO_MEDIUMCURRENCIES] =
{
  TEXT("CUR01_128.BMP")
};

LPCTSTR g_aszEUROMed256Currencies[NUM_EURO_MEDIUMCURRENCIES] =
{
  TEXT("CUR01_256.BMP")
};

LPCTSTR g_aszEUROHigh128Currencies[NUM_EURO_HIGHCURRENCIES] =
{
  TEXT("CUR01_128.BMP"),
  TEXT("CUR02_128.BMP"),
  TEXT("CUR03_128.BMP"),
  TEXT("CUR04_128.BMP"),
  TEXT("CUR05_128.BMP")
};

LPCTSTR g_aszEUROHigh256Currencies[NUM_EURO_HIGHCURRENCIES] =
{
  TEXT("CUR01_256.BMP"),
  TEXT("CUR02_256.BMP"),
  TEXT("CUR03_256.BMP"),
  TEXT("CUR04_256.BMP"),
  TEXT("CUR05_256.BMP")
};


LPCTSTR g_aszEUROMed128Lang[NUM_EURO_MEDIUMLANGUAGES] =
{
  TEXT("LANG01_128.BMP"),
  TEXT("LANG02_128.BMP"),
  TEXT("LANG03_128.BMP"),
  TEXT("LANG04_128.BMP"),
  TEXT("LANG05_128.BMP"),
  TEXT("LANG06_128.BMP"),
  TEXT("LANG07_128.BMP")
};

LPCTSTR g_aszEUROMed256Lang[NUM_EURO_MEDIUMLANGUAGES] =
{
  TEXT("LANG01_256.BMP"),
  TEXT("LANG02_256.BMP"),
  TEXT("LANG03_256.BMP"),
  TEXT("LANG04_256.BMP"),
  TEXT("LANG05_256.BMP"),
  TEXT("LANG06_256.BMP"),
  TEXT("LANG07_256.BMP")
};

LPCTSTR g_aszEUROHigh128Lang[NUM_EURO_HIGHLANGUAGES] =
{
  TEXT("LANG01_128.BMP"),
  TEXT("LANG02_128.BMP"),
  TEXT("LANG03_128.BMP"),
  TEXT("LANG04_128.BMP"),
  TEXT("LANG05_128.BMP"),
  TEXT("LANG06_128.BMP"),
  TEXT("LANG07_128.BMP"),
  TEXT("LANG08_128.BMP"),
  TEXT("LANG09_128.BMP"),
  TEXT("LANG10_128.BMP"),
  TEXT("LANG11_128.BMP")
};

LPCTSTR g_aszEUROHigh256Lang[NUM_EURO_HIGHLANGUAGES] =
{
  TEXT("LANG01_256.BMP"),
  TEXT("LANG02_256.BMP"),
  TEXT("LANG03_256.BMP"),
  TEXT("LANG04_256.BMP"),
  TEXT("LANG05_256.BMP"),
  TEXT("LANG06_256.BMP"),
  TEXT("LANG07_256.BMP"),
  TEXT("LANG08_256.BMP"),
  TEXT("LANG09_256.BMP"),
  TEXT("LANG10_256.BMP"),
  TEXT("LANG11_256.BMP")
};

LPCTSTR g_szEURO_GO_MedOverlay128   = TEXT("GO_OVERLAY128.bmp");
LPCTSTR g_szEURO_GO_MedOverlay256   = TEXT("GO_OVERLAY256.bmp");
LPCTSTR g_szEURO_GO_HighOverlay128  = TEXT("GO_OVERLAY128.bmp");
LPCTSTR g_szEURO_GO_HighOverlay256  = TEXT("GO_OVERLAY256.bmp");

const WORDPAIR g_aEUROMedBoardPositions[NUM_MEDBOARD_TEXTURES] =
{
  { 448,   0 }, // Marvin Gardens to Boardwalk, Electric Company, 
                //  Water Works, Income Tax, Luxury Tax
  { 320,   0 }, // St. James to Ventnor
  { 256,   0 }, // Mediterranean to Virginia, Chance, Community Chest
  { 192,   0 }, // Railroads, Center Logo, card instructions
  {  64,   0 }, // Borders, Center text
  { 128,   0 }, // Logos
  {   0,   0 }, // 4 Corners
  { 384,   0 }  // Prices
};

const WORDPAIR g_aEUROHighBoardPositions[NUM_HIBOARD_TEXTURES] =
{
  { 192,   0 }, // New York, Marvin Gardens, railroads
  { 128,   0 }, // St. Charles to Tennessee, Kentucky to Ventnor
  {  64,   0 }, // Mediterranean to Connecticut, Pacific to Boardwalk
  { 384,   0 }, // Chance, Community Chest, taxes, card instructions,
                //  utilities 
  { 320,   0 }, // Logos
  { 256,   0 }, // Borders, Center text
  { 448, 128 }, // Center logo
  {   0, 128 }, // Jail
  {  64, 128 }, // Free Parking
  { 448,   0 }, // Goto Jail
  {   0,   0 }, // GO
  { 384, 128 }, // $400, Income Tax
  { 320, 128 }, // $280 to $350
  { 256, 128 }, // $200 to $260
  { 192, 128 }, // $140 to $180
  { 128, 128 }  // $60, Luxury Tax, $100, $120
};

const WORDPAIR g_aEUROMedCityBoardPositions[NUM_MEDCITY_TEXTURES] =
{
  { 320,   0 }, // City textures
  { 256,   0 },
  { 192,   0 },
  { 128,   0 },
  {  64,   0 },
  {   0,   0 },
  { 320, 128 }, // Marvin Gardens to Boardwalk, Electric Company, 
                //  Water Works, Income Tax, Luxury Tax
  { 384, 128 }, // St. James to Ventnor
  { 256, 128 }, // Mediterranean to Virginia, Chance, Community Chest
  { 192, 128 }, // Railroads, Center Logo, card instructions
  {  64, 128 }, // Borders, Center text
  { 128, 128 }, // Logos
  {   0, 128 }, // 4 Corners
  { 384,   0 }  // Prices
};

const WORDPAIR g_aEUROHighCityBoardPositions[NUM_HICITY_TEXTURES] =
{
  { 512,   0 }, // City textures
  { 448,   0 }, 
  { 128, 128 }, 
  {  64, 128 }, 
  {   0, 128 }, 
  {   0,   0 }, 
  { 192,   0 }, // New York, Marvin Gardens, railroads
  { 128,   0 }, // St. Charles to Tennessee, Kentucky to Ventnor
  {  64,   0 }, // Mediterranean to Connecticut, Pacific to Boardwalk
  { 384,   0 }, // Chance, Community Chest, taxes, card instructions,
                //  utilities 
  { 320,   0 }, // Logos
  { 256,   0 }, // Borders, Center text
  { 640, 128 }, // Center Logo
  { 576, 128 }, // Jail
  { 576,   0 }, // Free Parking
  { 640,   0 }, // Goto Jail
  { 512, 128 }, // GO
  { 448, 128 }, // $400, Income Tax
  { 384, 128 }, // $280 to $350
  { 320, 128 }, // $200 to $260
  { 256, 128 }, // $140 to $180
  { 192, 128 }  // %60, Luxury Tax($75), $100, $120
};
