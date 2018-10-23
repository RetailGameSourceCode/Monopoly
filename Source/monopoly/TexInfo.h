// ===========================================================================
// Module:      TexInfo.h
//
// Description: Contains array definitions of texture info for the various
//              kinds of boards available in monopoly
//
// Created:     August 13, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_TEXINFO
#define INC_TEXINFO

struct WORDPAIR { WORD w1, w2; };

#define NUM_MEDCITY_CITIES      4  
#define NUM_HICITY_CITIES       3
#define NUM_CITY_PHOTOS         6

#define NUM_MEDBOARD_TEXTURES   8
#define NUM_HIBOARD_TEXTURES    16
#define NUM_MEDCITY_TEXTURES    14
#define NUM_HICITY_TEXTURES     22

#define NUM_MEDCOMMON_TEXTURES  4
#define NUM_HIGHCOMMON_TEXTURES 13

#define NUM_2DBOARDS            39

// Common texture components =================================================

extern LPCTSTR g_aszCommonMed128Textures[NUM_MEDCOMMON_TEXTURES];
extern LPCTSTR g_aszCommonMed256Textures[NUM_MEDCOMMON_TEXTURES];
extern LPCTSTR g_aszCommonHigh128Textures[NUM_HIGHCOMMON_TEXTURES];
extern LPCTSTR g_aszCommonHigh256Textures[NUM_HIGHCOMMON_TEXTURES];

extern LPCTSTR g_aszCity128Photos[NUM_CITY_PHOTOS];
extern LPCTSTR g_aszCity256Photos[NUM_CITY_PHOTOS];

// Medium classic board stuff ================================================

extern const WORDPAIR g_aBoardMedTxtrPositions[NUM_MEDBOARD_TEXTURES];
extern LPCTSTR g_aszBoardMed256Textures[NUM_MEDCITY_CITIES];

// High classic board stuff ==================================================

extern const WORDPAIR g_aBoardHighTxtrPositions[NUM_HIBOARD_TEXTURES];

extern LPCTSTR g_aszBoardHigh256Textures[NUM_HICITY_CITIES];


// Medium city board stuff ===================================================

extern const WORDPAIR g_aCityMedTxtrPositions[NUM_MEDCITY_TEXTURES];

extern LPCTSTR g_aszCityMed128Textures[NUM_MEDCITY_CITIES];
extern LPCTSTR g_aszCityMed256Textures[NUM_MEDCITY_TEXTURES];

// High city board stuff =====================================================

extern const WORDPAIR g_aCityHighTxtrPositions[NUM_HICITY_TEXTURES];

extern LPCTSTR g_aszCityHigh128Textures[NUM_HICITY_CITIES];
extern LPCTSTR g_aszCityHigh256Textures[NUM_HICITY_TEXTURES];

// 2D board stuff ============================================================

extern LPCTSTR g_asz2DBoards[NUM_2DBOARDS];

// ===========================================================================
// European board stuff ======================================================
// ===========================================================================

#define NUM_EURO_MEDIUMBOARDS               4
#define NUM_EURO_HIGHBOARDS                 3

#define NUM_EURO_MEDIUMCURRENCIES           1
#define NUM_EURO_HIGHCURRENCIES             5

#define NUM_EURO_MEDIUMLANGUAGES            7
#define NUM_EURO_HIGHLANGUAGES              11

extern const WORDPAIR g_aEUROMedBoardPositions[NUM_MEDBOARD_TEXTURES];
extern const WORDPAIR g_aEUROHighBoardPositions[NUM_HIBOARD_TEXTURES];
extern const WORDPAIR g_aEUROMedCityBoardPositions[NUM_MEDCITY_TEXTURES];
extern const WORDPAIR g_aEUROHighCityBoardPositions[NUM_HICITY_TEXTURES];

extern LPCTSTR g_aszEUROMed128Boards[NUM_EURO_MEDIUMBOARDS];
extern LPCTSTR g_aszEUROMed256Boards[NUM_EURO_MEDIUMBOARDS];
extern LPCTSTR g_aszEUROHigh128Boards[NUM_EURO_HIGHBOARDS];
extern LPCTSTR g_aszEUROHigh256Boards[NUM_EURO_HIGHBOARDS];

extern LPCTSTR g_aszEUROMed128Currencies[NUM_EURO_MEDIUMCURRENCIES];
extern LPCTSTR g_aszEUROMed256Currencies[NUM_EURO_MEDIUMCURRENCIES];
extern LPCTSTR g_aszEUROHigh128Currencies[NUM_EURO_HIGHCURRENCIES];
extern LPCTSTR g_aszEUROHigh256Currencies[NUM_EURO_HIGHCURRENCIES];

extern LPCTSTR g_aszEUROMed128Lang[NUM_EURO_MEDIUMLANGUAGES];
extern LPCTSTR g_aszEUROMed256Lang[NUM_EURO_MEDIUMLANGUAGES];
extern LPCTSTR g_aszEUROHigh128Lang[NUM_EURO_HIGHLANGUAGES];
extern LPCTSTR g_aszEUROHigh256Lang[NUM_EURO_HIGHLANGUAGES];

extern LPCTSTR g_szEURO_GO_MedOverlay128;
extern LPCTSTR g_szEURO_GO_MedOverlay256;
extern LPCTSTR g_szEURO_GO_HighOverlay128;
extern LPCTSTR g_szEURO_GO_HighOverlay256;

#endif // INC_TEXINFO
// ===========================================================================
