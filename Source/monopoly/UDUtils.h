#ifndef INC_UDUTILS
#define INC_UDUTILS

// ===========================================================================
// Module:      UDUtils.h
//
// Description: Utilities for use with the user display/interface stuff &
//              curve interpolation.
//
// Created:     March 19, 1999
//
// Author:      Dave Wilson / Russell Kleinsteuber
//
// Copywrite:		Artech, 1999
// ===========================================================================

#include "TexInfo.h"

// ===========================================================================
// DEFINES ===================================================================
// ===========================================================================
#define UI_DATAFILE         DAT_PAT
#define BEZIER_MAX_POINTS   10
#define BEZIER_TANGENT_DELTA 0.0005f

// INI file stuff
#define CLASSIC_BOARD           TEXT("Classic Board")
#define CITY_BOARD              TEXT("City Board")
#define GENERAL_SECTION         TEXT("General")
#define SOUND_SECTION           TEXT("Sound")

#define USE_HIRES_MESH          TEXT("UseHighResMesh")
#define USE_HIRES_TEXTURES      TEXT("UseHighResTextures")
#define USE_DITHERING           TEXT("UseDithering")
#define USE_BILINEARFILTERING   TEXT("UseBilinearFiltering")
#define MUSIC_ON                TEXT("PlayMusic")
#define MUSIC_SCORE             TEXT("TuneNumber")

#define USE_3DBOARD             TEXT("Use3DBoard")
#define USE_BACKGROUND          TEXT("UseBackground")


// ===========================================================================
// Types =====================================================================
// ===========================================================================

struct BtnStruct
{
  LE_DATA_DataId    idImage;
  LE_DATA_DataId    idPressedImage;
  LE_DATA_DataId    idDisabledImage;
};

#define EMPTYBUTTON { LE_DATA_EmptyItem, LE_DATA_EmptyItem, LE_DATA_EmptyItem }

enum InterpolationFX
{
  // Accelerate/Decelerate features
  INTERPOLATION_LINEAR_MOVEMENT = 1,        // basic move, no acceleration
  INTERPOLATION_ACCELERATE_IN_AND_OUT = 2,  // SIN functions used to accelerate/decelerate move increments - numeric values will be upgraded to curve modifiers eventually
  INTERPOLATION_FAST_ACCEL_IO_FLAT_MID = 3, // sharper acceleration for a 'flatter' middle - good for multi-segment beziers with cusps or slow parts in the middle (a quick fix - use several beziers for better deceleration at a mid point)

  // Curve types
  INTERPOLATION_STRAIGHT_LINE,
  INTERPOLATION_BEZIER,

  // Direction options
  INTERPOLATION_DIRECTION_IGNORE,
  INTERPOLATION_DIRECTION_INTERPOLATED,
  INTERPOLATION_DIRECTION_TANGENT,
  INTERPOLATION_DIRECTION_POINT_AT,       // point at .PointingAt

  // Statement of fact
  INTERPOLATION_PHILL_IS_A_SMELLY_GUY_BUT_RUSSELL_IS_NOT      // Reality bytes
};
struct Object_3D_Interpolation_data
{
  TYPE_Point3D StartLocation;
  TYPE_Point3D StartForwardV;
  TYPE_Point3D StartUpwardsV;
  TYPE_Point3D End__Location;
  TYPE_Point3D End__ForwardV;
  TYPE_Point3D End__UpwardsV;
  TYPE_Point3D InterLocation;
  TYPE_Point3D InterForwardV;
  TYPE_Point3D InterUpwardsV;

  TYPE_Point3D PointingAt;    // Set to the point to aim the camera at (when feature used)

  // Bezier points/segments are intended to be added using the add segment function (this will be enforced later when this is a class)
  int           BezierSegmentsUsed;                   //The add Segment function will maintain this.
  TYPE_Point3D  BezierSegments[BEZIER_MAX_POINTS][4];   //The start location and endlocation are not used but are maintained (when you use the add function, they will be updated)
  float         BezierLengths[BEZIER_MAX_POINTS];     //calculated lengths of the 3 lines - if you use the add function, these are calcated automatically.  These are used to split the total time ration between the various points.
  float         TangentDeltas[BEZIER_MAX_POINTS];     // The delta for the second point used in tangent calculations (negative = backward)
  //TYPE_Point3D PointBBezierV;
  //TYPE_Point3D PointCBezierV;
  //TYPE_Point3D PointDBezierV;// First curve endpoint when doing a double curve, unused for single.
  //TYPE_Point3D PointEBezierV;// Start point of second curve
  //TYPE_Point3D PointFBezierV;
  //TYPE_Point3D PointGBezierV;

  float ElapsedTimeRatio; // 0<= x <= 1.  Values above 1 trim to one, values below zero mean calculate the ratio from the time.
  int InterpolationFXSpeedType;
  int InterpolationFXCurveType;
  int InterpolationFXDirection;

  TYPE_Tick StartTime, CurrentTime, EndTime;// Not all functions use all fields, this is always a handy spot to store stuff though.

};

struct WORDPAIR;
class Mesh;

struct BoardTextureSet
{
  LPCTSTR*        aszCityNames;
  LPCTSTR*        aszCityPhotos;
  LPCTSTR*        aszCommon;
  int             nNumCityNames;
  int             nNumCityPhotos;
  int             nNumCommon;
  const WORDPAIR* aPositions;
  LPCTSTR         szBoardType;
  TCHAR           szTexturePath[MAX_PATH];
};


// ===========================================================================
// Global externs ============================================================
// ===========================================================================
extern LE_DATA_DataId g_aid2DBoards[NUM_2DBOARDS];


// ===========================================================================
// Function Prototypes =======================================================
// ===========================================================================

// Loads an image/sequence/sound from a datafile
extern BOOL LoadDataFileImage(int nFile, int idRes, LE_DATA_DataId& idImage);

extern BOOL ChangeObjectTransparency(LE_DATA_DataId idImage, BOOL bTransparent);

extern void InterpolationFX_3DPositionAndOrientation( Object_3D_Interpolation_data * InterpolationData );
extern void InterpolationAddBezierSegment( Object_3D_Interpolation_data * IData, int index, D3DVECTOR * v1, D3DVECTOR * v2, D3DVECTOR * v3, D3DVECTOR * v4, float tangentDelta );
extern void InterpolationAddBezierSegmentSmooth( Object_3D_Interpolation_data * IData, int index, D3DVECTOR * v3, D3DVECTOR * v4, float tangentDelta );

extern int  UDUTILS_CashPlayerCouldRaiseIfNeeded( RULE_PlayerNumber PlayerNo );
extern int  UDUTILS_Get_Net_Worth( RULE_PlayerNumber player );

extern void UDUTILS_GetModulePath(LPTSTR szPath, int nSize);

extern BOOL UDUTILS_LoadBoardTextureSet(Mesh* pMesh,
                                        const BoardTextureSet& txtrSet);

extern BOOL UDUTILS_Load2DBoardSet();

extern BOOL UDUTILS_FileExists(LPCSTR szFile);

extern void UDUTILS_AppPauseCallback(BOOL bPaused, LPVOID pData);

extern BOOL UDUTILS_ConvertToShadow(Surface* pSurface);
extern void UDUTILS_RestoreShadows();

extern BOOL UDUTILS_GenerateINIFile();

extern BOOL UDUTILS_ReadINIDisplaySettings();
extern BOOL UDUTILS_WriteINIDisplaySettings();
extern BOOL UDUTILS_WriteINIDisplaySettingsSpecial(); // hack of above

extern void UDUTILS_SwitchToBoardUSA();
extern void UDUTILS_SwitchToBoardEURO();

extern BOOL UDUTILS_RenderSpeedTest();

#if USA_VERSION
#define UDBOARD_SwitchToBoard(board, flag)   UDUTILS_SwitchToBoardUSA()
#else
#define UDBOARD_SwitchToBoard(board, flag)   UDUTILS_SwitchToBoardEURO()
#endif // USA_VERSION


#endif // INC_UDUTILS
