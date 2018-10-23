// ===========================================================================
// Module:      UDUtils.cpp
//
// Description: Utilities for use with the user display/interface stuff.
//
// Created:     March 19, 1999
//
// Author:      Dave Wilson, Russell Kleinsteuber
//
// Copywrite:		Artech, 1999
// ===========================================================================

#include "GameInc.h"
#include "UDUtils.h"

#if CE_ARTLIB_3DUsingOldFrame
#define USE_OLD_FRAME
#endif

#include "..\PC3D\PC3DHdr.h"
#include "..\pc3d\l_material.h"
#include "..\pc3d\DXINI.h"

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

// ===========================================================================
// Global variables ==========================================================
// ===========================================================================
LE_DATA_DataId g_aid2DBoards[NUM_2DBOARDS];


// ===========================================================================
// FUNCTIONS =================================================================
// ===========================================================================


// ===========================================================================
// Function:    LoadDataFileImage
//
// Description: Loads an image/sequence/sound from a datafile
//
// Params:      nFile:    The file number of the data file from which we want
//                        to load the image
//              idRes:    The ID of the resource we are loading
//              idImage:  The ID where the loaded resource's ID will be stored
//                        if loaded successfully
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the image was loaded successfully, and FALSE
//              otherwise.
// ===========================================================================
BOOL LoadDataFileImage(int nFile, int idRes, LE_DATA_DataId& idImage)
{
  // Load the image
  idImage = LE_DATA_IdFromTag(nFile, idRes);

  // Verify that the image was loaded correctly
  _ASSERT(LE_DATA_EmptyItem != idImage);
  return(LE_DATA_EmptyItem != idImage);
}


// ===========================================================================
// Function:    ChangeObjectTransparency
//
// Description: Changes an object's flags to indicate whether it is
//              transparent or not.
//
// Params:      idImage:      The id of the image we want to change the
//                            transparency of
//              bTransparent: Boolean indicating if we want transparency (TRUE)
//                            or wish to turn it off (FALSE)
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the transparency was successfully changed and
//              FALSE otherwise.
// ===========================================================================
BOOL ChangeObjectTransparency(LE_DATA_DataId idImage, BOOL bTransparent)
{
  LPNEWBITMAPHEADER pBmpHdr = NULL;

  // If we don't have a valid image we won't be able change its transparency
  if (LE_DATA_EmptyItem == idImage) return(FALSE);

  // Get the image properties from the header
  pBmpHdr = (LPNEWBITMAPHEADER)LE_DATA_Use(idImage);
  if (!pBmpHdr) return(FALSE);

  // Change the transparency flag
  if (bTransparent) pBmpHdr->dwFlags &= ~BITMAP_NOTRANSPARENCY;
  else              pBmpHdr->dwFlags |= BITMAP_NOTRANSPARENCY;

  return(TRUE);
}



// Adds a bezier segment updating the total used & approximating the curve length.
// v1 will be the previous v4, v2 will be the previous v3 flipped over v4 (to make a smooth joint)
void InterpolationAddBezierSegmentSmooth( Object_3D_Interpolation_data * IData, int index, D3DVECTOR * v3, D3DVECTOR * v4, float tangentDelta )
{// NOTE:  This should never be the first segment as it continues the previous segment.
  if( index == -1 )
  {// Auto add to end
    index = IData->BezierSegmentsUsed;
  }
  if( index >= BEZIER_MAX_POINTS )
    index = BEZIER_MAX_POINTS -1;

  if( index < 1 ) return; // error.

  IData->BezierSegments[index][0] = IData->BezierSegments[index-1][3];
  IData->BezierSegments[index][1] = IData->BezierSegments[index-1][3] + ( IData->BezierSegments[index-1][3] - IData->BezierSegments[index-1][2] );
  IData->BezierSegments[index][2] = *v3;
  IData->BezierSegments[index][3] = *v4;
  IData->TangentDeltas[index] = tangentDelta;

  // Housekeeping - update segment used count & approximate the distance of the curve as the distance of the 3 lines made by the four points.
  if( index == 0 )
    IData->StartLocation = IData->BezierSegments[0][0];
  if( index == IData->BezierSegmentsUsed )
  {
    IData->End__Location = IData->BezierSegments[index][3];
    IData->BezierSegmentsUsed++;
  }
  IData->BezierLengths[index] = LE_MATRIX_3D_VectorLength( &(IData->BezierSegments[index][3] - IData->BezierSegments[index][2]) )+
    LE_MATRIX_3D_VectorLength( &(IData->BezierSegments[index][2] - IData->BezierSegments[index][1]) ) + LE_MATRIX_3D_VectorLength( &(IData->BezierSegments[index][1] - IData->BezierSegments[index][0]) );


}


// Adds a bezier segment updating the total used & approximating the curve length.  TangentDelta specifies the magnitude (and by sign, direction) of the point offset used to calculate the slope of the curve.
void InterpolationAddBezierSegment( Object_3D_Interpolation_data * IData, int index, D3DVECTOR * v1, D3DVECTOR * v2, D3DVECTOR * v3, D3DVECTOR * v4, float tangentDelta = 0.0005f )
{
  if( index == -1 )
  {// Auto add to end
    index = IData->BezierSegmentsUsed;
  }
  if( index >= BEZIER_MAX_POINTS )
    index = BEZIER_MAX_POINTS -1;

  IData->BezierSegments[index][0] = *v1;
  IData->BezierSegments[index][1] = *v2;
  IData->BezierSegments[index][2] = *v3;
  IData->BezierSegments[index][3] = *v4;
  IData->TangentDeltas[index] = tangentDelta;

  // Housekeeping - update segment used count & approximate the distance of the curve as the distance of the 3 lines made by the four points.
  if( index == 0 )
    IData->StartLocation = IData->BezierSegments[0][0];
  if( index == IData->BezierSegmentsUsed )
  {
    IData->End__Location = IData->BezierSegments[index][3];
    IData->BezierSegmentsUsed++;
  }
  IData->BezierLengths[index] = LE_MATRIX_3D_VectorLength( &(IData->BezierSegments[index][3] - IData->BezierSegments[index][2]) )+
    LE_MATRIX_3D_VectorLength( &(IData->BezierSegments[index][2] - IData->BezierSegments[index][1]) ) + LE_MATRIX_3D_VectorLength( &(IData->BezierSegments[index][1] - IData->BezierSegments[index][0]) );
}


void InterpolationBezierPrimitiveCalc( float ratioMoved, D3DVECTOR * DestPoint, D3DVECTOR * PointA, D3DVECTOR * PointB, D3DVECTOR * PointC, D3DVECTOR * PointD )
{
  float B0, B1, B2, B3, U, u;
  //float , c1Mag, c2Mag, bezierDoubleRatio; // Bezier working variables
  
  U = ratioMoved;
  u = 1 - U;
  B0 =   u*u*u; // Derivatives - calculates the ratio each point affects the current location.
  B1 = 3*U*u*u;
  B2 = 3*U*U*u;
  B3 =   U*U*U;

  *DestPoint = B0 * *PointA + B1 * *PointB + B2 * *PointC + B3 * *PointD;
}


void InterpolationFX_3DPositionAndOrientation( Object_3D_Interpolation_data * IData )
{
  float ratioMoved, ratioMovedBezierSegment;
  float cMagTotal, segStart, segEnd;
  int   bezierIndex, t;

  // Calculate the linear ratio between start and finish times for interpolation
  if( IData->ElapsedTimeRatio < 0.0f )// Negative values mean calculate this ourself.
    if( IData->EndTime == IData->StartTime )
      ratioMoved = 1.0f;
    else
      ratioMoved = (float)(IData->CurrentTime - IData->StartTime)/(float)(IData->EndTime - IData->StartTime);
  else
    ratioMoved = IData->ElapsedTimeRatio;
  if( ratioMoved > 1.0f ) ratioMoved = 1.0f;
  if( ratioMoved < 0.0f ) ratioMoved = 0.0f;

  // Bend the ratio for speed change effects.
  switch( IData->InterpolationFXSpeedType )
  {// Eventually (after monopoly?), make this a formula with an acceleration magnifier, 1 = none or linear velocity.
  case INTERPOLATION_FAST_ACCEL_IO_FLAT_MID:
    /*{// Ellipse idea - y is max(1) at x = 0, min(0) at x = root(3) NOTE x^2 + y^2 = 1 is the same as the sin version below.
      // 1x^2 + 3y^2 = 3; // x is ratio(-1 to 0 & scale result), positive y is modified ratio (scale to 1)
      // 3y^2 = 3 - x^2;
      // y = root( (3-x^2)/3 )
      TRACE( "Ratio: %f", ratioMoved );
      double ratioScale;
      if( ratioMoved < 0.5f ) // Accelerate up
      {
        ratioScale = 2 * ratioMoved * sqrt(3); // double ratio(0 to 1) so squared is 3.
        ratioMoved = 0.5f - 0.5f*(float)sqrt((3 - ratioScale*ratioScale)/3);
      }
      else
      {
        ratioScale = 2 * (ratioMoved - 0.5f) * sqrt(3); // double ratio backed up(0 to 1) so squared is 3.
        ratioMoved = 0.5f + 0.5f*(float)sqrt(ratioScale*ratioScale/3);
      }
      TRACE( "Becomes: %f\n", ratioMoved );
    }    // Not working - rethink it.  I think the scale 
    break;*/
  case INTERPOLATION_ACCELERATE_IN_AND_OUT:
    if( ratioMoved < 0.5f ) // Accelerate up
      ratioMoved = 0.5f - 0.5f*(float) cos( ratioMoved * (PIFLOAT) );
    else
      ratioMoved = 0.5f + 0.5f*(float) sin( (ratioMoved - 0.5f) * (PIFLOAT) );
    break;

  case INTERPOLATION_LINEAR_MOVEMENT:
  default:
    break; // Leave the ratio linear.
  }

  // Calculate position, we have the adjusted time ratio.
  switch( IData->InterpolationFXCurveType )
  {
  case INTERPOLATION_STRAIGHT_LINE:
    //new position calculations (linear)
    IData->InterLocation = IData->StartLocation + (IData->End__Location - IData->StartLocation) * ratioMoved;
    break;

  case INTERPOLATION_BEZIER:
    if( IData->BezierSegmentsUsed >= BEZIER_MAX_POINTS ) IData->BezierSegmentsUsed = BEZIER_MAX_POINTS -1;
    
    // Find which Bezier segment we are in and scale the ratio to its time.
    for( t = 0, cMagTotal = 0; t < IData->BezierSegmentsUsed; t++ )
      cMagTotal += IData->BezierLengths[t];
    
    if( cMagTotal == 0 )
    { // The thing has no size at all?  Call the sides evenly spaced.  Maybe this will be a feature.
      for( t = 0; t < IData->BezierSegmentsUsed; t++ )
      {
        IData->BezierLengths[t] = 1;
        cMagTotal += 1;
      }
    }
    
    for( bezierIndex = 0, segStart = 0.0f, segEnd = 0.0f; bezierIndex < IData->BezierSegmentsUsed; bezierIndex++ )
    { // Now scale the ratio to the current segment
      segEnd += IData->BezierLengths[bezierIndex];
      if( (segEnd/cMagTotal + 0.000001f) >= ratioMoved ) break;
      segStart = segEnd;
    }
    
    if( IData->BezierSegmentsUsed > 1 )
    {// Save some math if only 1 segment is used (nothing to calculate)
      ratioMovedBezierSegment = ratioMoved - (segStart/cMagTotal);                // Drop the preceeding segment - ratio is 0 at segStart.
      //if( cMagTotal != segEnd )
      //ratioMovedBezierSegment = ratioMovedBezierSegment * (cMagTotal - segStart)/(cMagTotal-segEnd); // Extrapolate ratio so it hits 1 at end of current segment - so * remaining mag / current mag
      ratioMovedBezierSegment = ratioMovedBezierSegment / ((segEnd - segStart)/cMagTotal); // Extrapolate ratio so it hits 1 at end of current segment - so * remaining mag / current mag
      
      {static float oldratio = 45.0f; static int oldsegment = 89;
      if( oldratio != ratioMoved && IData->BezierSegmentsUsed > 1 )
      {
        //if( oldsegment != bezierIndex )
        //  TRACE("Segment Start %f, end %f\n", segStart, segEnd);
        oldsegment = bezierIndex;
        oldratio = ratioMovedBezierSegment;
        //TRACE( " Ratio %f: Bez seg %d, ratioMoved %f\n", ratioMoved, bezierIndex, ratioMovedBezierSegment );
      }}
    } else
      ratioMovedBezierSegment = ratioMoved;


    InterpolationBezierPrimitiveCalc( ratioMovedBezierSegment, &IData->InterLocation, 
      &IData->BezierSegments[bezierIndex][0], &IData->BezierSegments[bezierIndex][1], 
      &IData->BezierSegments[bezierIndex][2], &IData->BezierSegments[bezierIndex][3] );

    if( IData->InterpolationFXDirection == INTERPOLATION_DIRECTION_TANGENT )
    {// We estimate the slope by calculating a nearby point.
      TYPE_Point3D tempV;
      
      InterpolationBezierPrimitiveCalc( ratioMovedBezierSegment + IData->TangentDeltas[bezierIndex], &tempV, 
        &IData->BezierSegments[bezierIndex][0], &IData->BezierSegments[bezierIndex][1], 
        &IData->BezierSegments[bezierIndex][2], &IData->BezierSegments[bezierIndex][3] );
      tempV -= IData->InterLocation;
      D3DVALUE tempf = Magnitude( tempV );
      if( tempf > 0.00001 ) // Leave the forward vector alone if no change was significant.  * Could be uninitialized, otherwise will stay at last calculated direction.
        IData->InterForwardV = tempV / tempf;
      else
        tempf = 1.1f;// Crap - debug catch to see if it happens (how often, to tune the bailout.).
      
      IData->InterUpwardsV = D3DVECTOR( 0, 1, 0 );  // This one we cant calculate.
    }
    break;

  default:
    break;
  }
  

  // Calculate position, we have the adjusted time ratio.
  switch( IData->InterpolationFXDirection )
  {
  case INTERPOLATION_DIRECTION_INTERPOLATED:
    //New Orientation calculations (linear)
    IData->InterForwardV = IData->StartForwardV + (IData->End__ForwardV - IData->StartForwardV) * ratioMoved;
    IData->InterUpwardsV = IData->StartUpwardsV + (IData->End__UpwardsV - IData->StartUpwardsV) * ratioMoved;
    break;

  case INTERPOLATION_DIRECTION_TANGENT: // Beziers - handled above (due to complexity)
   if( IData->InterpolationFXCurveType != INTERPOLATION_BEZIER )
   {
     IData->InterForwardV = Normalize( D3DVECTOR(IData->End__Location - IData->StartLocation) );
     IData->InterUpwardsV = D3DVECTOR( 0, 1, 0 );  // This one we cant calculate.
   }
   break;

  case INTERPOLATION_DIRECTION_POINT_AT:
    IData->InterForwardV = Normalize( D3DVECTOR(IData->PointingAt - IData->InterLocation) );
    IData->InterUpwardsV = D3DVECTOR( 0, 1, 0 );  // This one we cant calculate.
    // WARNING: This will allow a smooth exit from point_at, but might cause problems.  Trying it.
    IData->End__ForwardV = IData->InterForwardV;
    IData->End__UpwardsV = IData->InterUpwardsV;
    break;

  case INTERPOLATION_DIRECTION_IGNORE:
  default:
    break;
  }

}


/*****************************************************************************
 * A utility function to find out how much money a player could raise if
 * he cashed in everything.  Doesn't include trading, or properties that
 * are held in escrow, just immediate cash-ins.  Taken from Rules.cpp
 */
int UDUTILS_CashPlayerCouldRaiseIfNeeded( RULE_PlayerNumber PlayerNo )
{
  long                        Cash;
  RULE_SquareType             SquareNo;
  RULE_SquareInfoPointer      SquarePntr;

  if (PlayerNo >= UICurrentGameState.NumberOfPlayers)
    return 0; /* This isn't a player. */

  if (UICurrentGameState.Players[PlayerNo].currentSquare >= SQ_OFF_BOARD)
    return 0; /* It is a bankrupt player. */

  Cash = UICurrentGameState.Players[PlayerNo].cash;

  /* Add on mortgagable squares and house sales. */

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = UICurrentGameState.Squares + SquareNo;
    if (SquarePntr->owner == PlayerNo)
    {
      if (!SquarePntr->mortgaged)
        Cash += RULE_SquarePredefinedInfo[SquareNo].mortgageCost;

      Cash += SquarePntr->houses *
        ((RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost + 1) / 2);
    }
  }
  return Cash;
}

/********************************************************************/
/*					Get_Net_Worth    							*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*			RULE_PlayerNumber	player		Check who owns it		*/
/*	 Output: long    total worth of player							*/		
/********************************************************************/
int UDUTILS_Get_Net_Worth( RULE_PlayerNumber player )
{
	long total = 0;
	//RULE_PropertySet	properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
	RULE_SquareType		SquareNo;
	

	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		//if(RULE_PropertyToBitSet(SquareNo) & properties_owned)	/* Do we own this property? */
    if( UICurrentGameState.Squares[SquareNo].owner == player )
		{// Half value for mortgaged properties
			total += RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost * UICurrentGameState.Squares[SquareNo].houses;
      if( UICurrentGameState.Squares[SquareNo].mortgaged )
			  total += (int)(RULE_SquarePredefinedInfo[SquareNo].purchaseCost + 1) / 2;
      else
			  total += RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
		}
	}
	
	// Now include cash on hand.
	total += UICurrentGameState.Players[player].cash;

  // Now include get out of jail cards
  if( UICurrentGameState.Cards[0].jailOwner == player ) total += 50;
  if( UICurrentGameState.Cards[1].jailOwner == player ) total += 50;
return total;
}


// ===========================================================================
// Function:    UDUTILS_GetModulePath
//
// Description: Gets the path where our executable lies
//
// Params:      szPath: Buffer where we want to store the path
//              nSize:  Size of the buffer
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDUTILS_GetModulePath(LPTSTR szPath, int nSize)
{
  // Get the filename for the executable
  GetModuleFileName(NULL, szPath, nSize);

  // Now strip off the file name from the path
  LPTSTR szTmp = _tcsrchr(szPath, '\\');
  szTmp[1] = '\0';
}


extern LE_DATA_DataId CurrentBoard;


// ===========================================================================
// Function:    UDUTILS_LoadBoardTextureSet
//
// Description: Loads a set of textures for a monopoly board.
//
// Params:      pMesh:    The mesh to load the texture set for
//              txtrSet:  Structure describing the texture set to load
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the texture set is successfully loaded, FALSE
//              if it is not.
// ===========================================================================
BOOL UDUTILS_LoadBoardTextureSet(Mesh* pMesh, const BoardTextureSet& txtrSet)
{
  ASSERT(NULL != pMesh);

  // If there is nothing to load, then that is easy to accomplish
  if (!txtrSet.aPositions) return(TRUE);

  SETTIMBANK( 0 );

  int   nNumTexturesLoaded  = 0;
  int   nOffset             = 0;
  TCHAR szFile[MAX_PATH];
  TCHAR szBitmap[MAX_PATH];

  TextureID* pTextureID     = NULL;

  // Load the photos
  for (int i = 0; i < txtrSet.nNumCityPhotos; i++)
  {
    _stprintf(szFile, TEXT("%sPhotos\\%s"), txtrSet.szTexturePath,
              txtrSet.aszCityPhotos[i]);

    if (!LE_DIRINI_ExpandFileName(szFile, szBitmap, LE_DIRINI_GlobalPathList))
      continue;

    pTextureID = LoadBMPTexture(txtrSet.aPositions[i].w1, 
                                txtrSet.aPositions[i].w2, szBitmap);

    if (pTextureID)
    {
      if (pMesh->SubstituteTexture(pTextureID))
        nNumTexturesLoaded++;
    }
  }

  nOffset = txtrSet.nNumCityPhotos;

  // Load the names
  for (i = 0; i < txtrSet.nNumCityNames; i++)
  {
    _stprintf(szFile, TEXT("%s%s\\%s"), txtrSet.szTexturePath,
              txtrSet.szBoardType, txtrSet.aszCityNames[i]);

    if (!LE_DIRINI_ExpandFileName(szFile, szBitmap, LE_DIRINI_GlobalPathList))
      continue;

    pTextureID = LoadBMPTexture(txtrSet.aPositions[nOffset + i].w1, 
                                txtrSet.aPositions[nOffset + i].w2, szBitmap);

    if (pTextureID)
    {
      if (pMesh->SubstituteTexture(pTextureID))
        nNumTexturesLoaded++;
    }
  }

  nOffset = txtrSet.nNumCityPhotos + txtrSet.nNumCityNames;

  // Load the common stuff
  for (i = 0; i < txtrSet.nNumCommon; i++)
  {
    _stprintf(szFile, TEXT("Cities\\Common\\%s\\%s"), txtrSet.szBoardType,
              txtrSet.aszCommon[i]);

    if (!LE_DIRINI_ExpandFileName(szFile, szBitmap, LE_DIRINI_GlobalPathList))
      continue;

    pTextureID = LoadBMPTexture(txtrSet.aPositions[nOffset + i].w1, 
                                txtrSet.aPositions[nOffset + i].w2, szBitmap);

    if (pTextureID)
    {
      if (pMesh->SubstituteTexture(pTextureID))
        nNumTexturesLoaded++;
    }
  }

  SETTIMBANK( DISPLAY_state.VirtualTimPageForAnythingNotPreloaded );

  // Return whether we managed to load all of the textures or not
  return(txtrSet.nNumCityPhotos + txtrSet.nNumCityNames + 
         txtrSet.nNumCommon == nNumTexturesLoaded);
}


// ===========================================================================
// Function:    UDUTILS_Load2DBoardSet
//
// Description: Loads a set of 2d views for a board
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the set of 2D board bitmaps are successfully
//              loaded, or FALSE if it is not.
// ===========================================================================
BOOL UDUTILS_Load2DBoardSet()
{
  ASSERT(-1 == DISPLAY_state.city);

  // Make sure we have a properly initialized data path
  if (0 == _tcslen(DISPLAY_state.customBoardPath)) return(FALSE);

  TCHAR szFile[MAX_PATH];

  // Load all of the 2D views for the board
  for (int i = 0; i < NUM_2DBOARDS; i++)
  {
    // Make sure we clean up our resources
    if (LED_EI != g_aid2DBoards[i])
      LE_DATA_FreeRuntimeDataID(g_aid2DBoards[i]);

    _stprintf(szFile, TEXT("%s2DBoards\\%s"), DISPLAY_state.customBoardPath,
              g_asz2DBoards[i]);

    // Load the next board
    g_aid2DBoards[i] = LE_DATA_AllocExternalFileDataID(szFile);

    // Make sure we loaded it successfully
    if (LED_EI == g_aid2DBoards[i]) return(FALSE);
  }

  return(TRUE);
}


// ===========================================================================
// Function:    UDUTILS_FileExists
//
// Description: Checks the given filename to see if it exists on the disk
//
// Params:      szFile: The file we want to check
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the file exists, FALSE if it does not.
// ===========================================================================
BOOL UDUTILS_FileExists(LPCSTR szFile)
{
  // Try opening the file
  HANDLE hFile = CreateFile(szFile, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, NULL);

  // Check to see if we obtained a valid handle
  if (INVALID_HANDLE_VALUE != hFile)
  {
    // We found it
    CloseHandle(hFile);
    return(TRUE);
  }

  // The file does not exist
  return(FALSE);
}


// ===========================================================================
// Function:    UDUTILS_AppPauseCallback
//
// Description: Callback function for when the application is paused or unpaused
//
// Params:      bPaused:  Is the application being paused/unpaused
//              pData:    Application specific data
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDUTILS_AppPauseCallback(BOOL bPaused, LPVOID pData)
{
  // Restore the shadows when the application is restored
  if (!bPaused) UDUTILS_RestoreShadows();
}


// ===========================================================================
// Function:    UDUTILS_ConvertToShadow
//
// Description: Takes a surface and converts it into a surface having an
//              alpha channel.  The new surface is then used as a shadow.
//
// Params:      pSurface: The surface we want to convert
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface is successfully converted, or
//              FALSE if it is not.  Assumes the surface is an 8bit or 16bit
//              surface.
// ===========================================================================
BOOL UDUTILS_ConvertToShadow(Surface* pSurface)
{
  BOOL  bUsing16Bit       = TRUE;

  DDPIXELFORMAT ddpf;
  ZeroMemory(&ddpf, sizeof(DDPIXELFORMAT));
  ddpf.dwSize   = sizeof(DDPIXELFORMAT);
  ddpf.dwFlags  = DDPF_ALPHAPIXELS | DDPF_RGB;

  // 16 bits per pixel, with 4 bit alpha channel
  ddpf.dwRGBBitCount      = 16;
  ddpf.dwRGBAlphaBitMask  = 0x0000F000;
  ddpf.dwRBitMask         = 0x00000F00;
  ddpf.dwGBitMask         = 0x000000F0;
  ddpf.dwBBitMask         = 0x0000000F;

  // Before we run off and do the conversion, we had better make sure that
  //  16bit with a 4bit alpha channel is a supported texture format of the
  //  device we are using
  if (!pc3D::Get3DDevice()->TextureFormatSupported(ddpf))
  {
    // The format is not supported
    TRACE("\n\n!!!\t\t16bit with 4bit alpha channel is not a supported\n");
    TRACE("!!!\t\ttexture format on the 3D device being used!!!\n");

    // Try 32 bit
    ddpf.dwRGBBitCount      = 32;
    ddpf.dwRGBAlphaBitMask  = 0xFF000000;
    ddpf.dwRBitMask         = 0x00FF0000;
    ddpf.dwGBitMask         = 0x0000FF00;
    ddpf.dwBBitMask         = 0x000000FF;

    if (!pc3D::Get3DDevice()->TextureFormatSupported(ddpf))
    {
      // 32 bit alpha is not supported either.  We cannot construct the shadows.
      TRACE("\n!!!\t\t32bit with 8bit alpha channel is not supported either\n");
      TRACE("!!!\t\tCannot construct shadows\n");
      return(FALSE);
    }

    bUsing16Bit = FALSE;
  }

  // Get the size of the surface we are converting
  SIZE size;
  pSurface->GetSize(size);

  // Create the new surface
  Surface* pShadow = new Surface;
  ASSERT(NULL != pShadow);
  pShadow->CreateTexture(size.cx, size.cy, ddpf);

  PDDPALETTE    pPalette = NULL;
  PALETTEENTRY  aPalEntries[256];

  // Grab the palette if there is one
  if (pSurface->IsPalettized())
  {
    pPalette = pSurface->GetPalette();
    ASSERT(NULL != pPalette);
    pPalette->GetEntries(0, 0, 256, aPalEntries);
  }

  // Setup our source and destination surface descriptions
  DDSURFDESC  ddsdSrc;
  DDSURFDESC  ddsdDest;
  ZeroMemory(&ddsdSrc, sizeof(DDSURFDESC));
  ZeroMemory(&ddsdDest, sizeof(DDSURFDESC));
  ddsdSrc.dwSize  = sizeof(DDSURFDESC);
  ddsdDest.dwSize = sizeof(DDSURFDESC);

  BYTE*   pSource     = NULL;
  WORD*   p16BitDest  = NULL;
  DWORD*  p32BitDest  = NULL;
  BOOL    bSuccess    = FALSE;
  WORD    wPixel      = 0;
  float   fPercent    = 0.0f;
  DWORD   dwBPPMode   = 0;
  int     i = 0, j    = 0;
  float   fMaxRed     = 0.0f;
  float   fMaxGreen   = 0.0f;
  float   fMaxBlue    = 0.0f;

  // Lock the surfaces
  if (!pSurface->Lock(ddsdSrc, DDLOCK_NOSYSLOCK | DDLOCK_READONLY | 
            DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT))
    return(FALSE);

  if (!pShadow->Lock(ddsdDest, DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY | 
                     DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT))
    goto ABORT_CONVERSION;

  // If the surface already has alpha info, we don't need to convert it
  if (0 != ddsdSrc.ddpfPixelFormat.dwRGBAlphaBitMask) goto ABORT_CONVERSION;

  // Grab the memory pointers
  pSource = (BYTE*)ddsdSrc.lpSurface;
  
  if (bUsing16Bit)
    p16BitDest = (WORD*)ddsdDest.lpSurface;
  else
    p32BitDest = (DWORD*)ddsdDest.lpSurface;

  dwBPPMode = ddsdSrc.ddpfPixelFormat.dwRGBBitCount / 8;

  if (!pPalette)
  {
    // This isn't a palettized surface, so we need to figure out the maximum
    //  values possible for each of the Red, Green and Blue components.
    fMaxRed   = (float)((1 << GetBitDepth(ddsdSrc.ddpfPixelFormat.dwRBitMask)) - 1);
    fMaxGreen = (float)((1 << GetBitDepth(ddsdSrc.ddpfPixelFormat.dwGBitMask)) - 1);
    fMaxBlue  = (float)((1 << GetBitDepth(ddsdSrc.ddpfPixelFormat.dwBBitMask)) - 1);
  }

  // Copy over the bits
  for (i = 0; i < size.cx; i++) // Rows
  {
    for (j = 0; j < size.cy; j++) // Coloumns
    {
      fPercent = 0.0f;

      // Get the next pixel
      if (pPalette)
      {
        // This is an 8bit palettized surface.  Grab the pixel colour from the
        //  palette
        ASSERT(1 == dwBPPMode);
        fPercent += (float)(aPalEntries[*(pSource + j + (i * ddsdSrc.lPitch))].peRed) / 765.0f;
        fPercent += (float)(aPalEntries[*(pSource + j + (i * ddsdSrc.lPitch))].peGreen) / 765.0f;
        fPercent += (float)(aPalEntries[*(pSource + j + (i * ddsdSrc.lPitch))].peBlue) / 765.0f;
      }
      else
      {
        // We need to convert the colour info using the bit masks
        ASSERT(2 == dwBPPMode);
        wPixel = *((WORD*)(pSource + (j * dwBPPMode) + (i * ddsdSrc.lPitch)));

        // Take each pixel, extract the red, green and blue components and
        //  then multiply each component by one third.  Add these together
        //  and we get the percentage of white we are looking for.
        //
        // To extract each component:
        //  1:  AND the pixel with the bit mask of the component we want
        //  2:  SHIFT it down so there are no trailing zeroes
        //  3:  DIVIDE by the max value for the component so we obtain
        //      its percentage.
        //  4:  MULTIPLY this percentage by one third (1/3).
        //  5:  ADD all of these values together to get the percentage of
        //      white in the pixel.

        fPercent += 0.33333f * ((float)((wPixel & ddsdSrc.ddpfPixelFormat.dwRBitMask) >> 
                    GetBitShift(ddsdSrc.ddpfPixelFormat.dwRBitMask)) / fMaxRed); // Red
        fPercent += 0.33333f * ((float)(wPixel & ddsdSrc.ddpfPixelFormat.dwGBitMask >> 
                    GetBitShift(ddsdSrc.ddpfPixelFormat.dwGBitMask)) / fMaxGreen); // Green
        fPercent += 0.33333f * ((float)(wPixel & ddsdSrc.ddpfPixelFormat.dwBBitMask >> 
                    GetBitShift(ddsdSrc.ddpfPixelFormat.dwBBitMask)) / fMaxBlue); // Blue
      }

      // If we're relatively close to 1, make it 1 so we get real transparency
      if (0.99f <= fPercent) 
        fPercent = 1.0f;
      else
      {
        // Otherwise add another 15% to the colour to lighten up the shadow a bit
        fPercent += 0.15f;
        // Adjust if we have gone over 100%
        if (1.0f < fPercent)
          fPercent = 1.0f;
      }

      // Set the new pixel. 0 is opaque.
      if (bUsing16Bit)
        *(p16BitDest + j + (i * size.cx)) = (WORD)(15.0f * fPercent) << 12;
      else
        *(p32BitDest + j + (i * size.cx)) = (DWORD)(255.0f * fPercent) << 24;
    }
  }

  // We were successful
  bSuccess = TRUE;

ABORT_CONVERSION:

  // Unlock the surfaces
  pSurface->Unlock();
  pShadow->Unlock();

  if (bSuccess)
  {
    // Destroy the old texture and replace it with the new one
    PDDSURFACE pDDSurface = pShadow->Detach();
    pSurface->Create(pDDSurface);
  }

  // Remember to release our resources
  DESTROYSURFACE(pShadow);

  return(TRUE);
}


// ===========================================================================
// Function:    UDUTILS_RestoreShadows
//
// Description: Restores the shadows meshes and textures
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDUTILS_RestoreShadows()
{
  Mesh**      ppMesh      = NULL;
  MeshGroup*  pGroup      = NULL;
  TextureID*  pTextureID  = NULL;

  SETTIMBANK(1);

  for (int i = HMD_ashadow; i <= HMD_kshadow; i++)
  {
    // Make sure the old shadow is removed
    while (LE_DATA_RemoveRef(LED_IFT(DAT_3D, i))) ;

    // Reload the shadow mesh
    ppMesh = (Mesh**)LE_DATA_UseRef(LED_IFT(DAT_3D, i));

    // Set the ZBias and blending factors for the mesh
    (*ppMesh)->SetZBias(2);
    (*ppMesh)->SetBlendingFactors(D3DBLEND_ZERO, D3DBLEND_SRCALPHA);

    // Grab the mesh group from the mesh (should only be one)
    ASSERT(1 == (*ppMesh)->GetNumMeshGroups());
    pGroup = (*ppMesh)->GetMeshGroup(0);

    // Get the texture from the mesh group
    pTextureID = pGroup->GetTexture();
    ASSERT(NULL != pTextureID);

    // Convert the texture to a shadow
    VERIFY(UDUTILS_ConvertToShadow(pTextureID->pTexture));
  }

  SETTIMBANK(DISPLAY_state.VirtualTimPageForAnythingNotPreloaded);
}


// ===========================================================================
// Function:    UDUTILS_GenerateINIFile
//
// Description: Generates the INI file for monopoly.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successful, FALSE otherwise.
// ===========================================================================
BOOL UDUTILS_GenerateINIFile()
{
  char szNewPath[MAX_PATH];
  char szModulePath[MAX_PATH];

  UDUTILS_GetModulePath(szModulePath, MAX_PATH);

  // Main folder
  if (!LE_DIRINI_AddPath(szModulePath, &LE_DIRINI_GlobalPathList)) return(FALSE);

  strcpy(szNewPath, "CD:\\");
  if (!LE_DIRINI_AddPath(szNewPath, &LE_DIRINI_GlobalPathList)) return(FALSE);

  // Data files
  sprintf(szNewPath, "%sDat_Mon\\", szModulePath);
  if (!LE_DIRINI_AddPath(szNewPath, &LE_DIRINI_GlobalPathList)) return(FALSE);

  strcpy(szNewPath, "CD:\\Dat_Mon\\");
  if (!LE_DIRINI_AddPath(szNewPath, &LE_DIRINI_GlobalPathList)) return(FALSE);

  // AVI files
  sprintf(szNewPath, "%sAVI\\", szModulePath);
  if (!LE_DIRINI_AddPath(szNewPath, &LE_DIRINI_GlobalPathList)) return(FALSE);

  strcpy(szNewPath, "CD:\\AVI\\");
  if (!LE_DIRINI_AddPath(szNewPath, &LE_DIRINI_GlobalPathList)) return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    UDUTILS_ReadINIDisplaySettings
//
// Description: Reads the display settings from the monopoly.ini file
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the display settings are successfully read
//              from the INI file, or FALSE if something goes wrong.
// ===========================================================================
BOOL UDUTILS_ReadINIDisplaySettings()
{
  // Define some local macros to make our life easier ========================
  #define READ_INI_STRING(section, key, str, size, defStr)                  \
    if (0 == GetPrivateProfileString(section, key, defStr, str, size,       \
                                     szINIFile))                            \
      return(FALSE)
  
  #define READ_INI_DWORD(section, key, dw)                                  \
    READ_INI_STRING(section, key, szValue, 20, TEXT("4294967295"));         \
    dw = _tcstoul(szValue, NULL, 10); if (0xFFFFFFFF == dw) return(FALSE)
  // End macro definitions ===================================================

  TCHAR szValue[20];
  TCHAR szINIFile[MAX_PATH];
  DWORD dwValue;

  // Format the INI file name
  UDUTILS_GetModulePath(szINIFile, MAX_PATH);
  _tcscat(szINIFile, TEXT(CE_ARTLIB_DirIniINIFileName));

  // Reset the rendering settings
  ZeroMemory(&DISPLAY_RenderSettings, sizeof(DISPLAY_RenderSettingsStruct));

  // Read the classic board settings
  READ_INI_DWORD(CLASSIC_BOARD, USE_HIRES_MESH, dwValue);
  DISPLAY_RenderSettings.bClassic_UseHiResMesh = (BYTE)dwValue;
  READ_INI_DWORD(CLASSIC_BOARD, USE_HIRES_TEXTURES, dwValue);
  DISPLAY_RenderSettings.bClassic_UseHiResTextures = (BYTE)dwValue;
  READ_INI_DWORD(CLASSIC_BOARD, USE_DITHERING, dwValue);
  DISPLAY_RenderSettings.bClassic_UseDithering = (BYTE)dwValue;
  READ_INI_DWORD(CLASSIC_BOARD, USE_BILINEARFILTERING, dwValue);
  DISPLAY_RenderSettings.bClassic_UseBilinearFiltering = (BYTE)dwValue;
  DISPLAY_state.IsOptionFilteringOn = DISPLAY_RenderSettings.bClassic_UseBilinearFiltering;
  DISPLAY_state.IsOptionDitheringOn = DISPLAY_RenderSettings.bClassic_UseDithering;

  // Read the city board settings
  READ_INI_DWORD(CITY_BOARD, USE_HIRES_MESH, dwValue);
  DISPLAY_RenderSettings.bCity_UseHiResMesh = (BYTE)dwValue;
  READ_INI_DWORD(CITY_BOARD, USE_HIRES_TEXTURES, dwValue);
  DISPLAY_RenderSettings.bCity_UseHiResTextures = (BYTE)dwValue;
  READ_INI_DWORD(CITY_BOARD, USE_DITHERING, dwValue);
  DISPLAY_RenderSettings.bCity_UseDithering = (BYTE)dwValue;
  READ_INI_DWORD(CITY_BOARD, USE_BILINEARFILTERING, dwValue);
  DISPLAY_RenderSettings.bCity_UseBilinearFiltering = (BYTE)dwValue;

  // Are we using the 3D board
  READ_INI_DWORD(GENERAL_SECTION, USE_3DBOARD, dwValue);
  DISPLAY_RenderSettings.bUse3DBoard = (BYTE)dwValue;
  DISPLAY_state.IsOption3DBoardOn = DISPLAY_RenderSettings.bUse3DBoard;

  // Will we be using the background bitmap with the 3D???
  READ_INI_DWORD(GENERAL_SECTION, USE_BACKGROUND, dwValue);
  DISPLAY_RenderSettings.bUseBackground = (BYTE)dwValue;

  // Whats our sound setting?
  READ_INI_DWORD(SOUND_SECTION, MUSIC_ON, dwValue);
  DISPLAY_state.IsOptionMusicOn = dwValue;
  READ_INI_DWORD(SOUND_SECTION, MUSIC_SCORE, dwValue);
  DISPLAY_state.OptionMusicTuneIndex = dwValue;


  // Remove our local macros
  #undef READ_INI_DWORD
  #undef READ_INI_STRING

  return(TRUE);
}


// ===========================================================================
// Function:    UDUTILS_WriteINIDisplaySettings
//
// Description: Writes the display settings to the monopoly.ini file
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the display settings are successfully written
//              to the INI file, or FALSE if something goes wrong.
// ===========================================================================
BOOL UDUTILS_WriteINIDisplaySettings()
{
  // Define some local macros to make our life easier ========================
  #define WRITE_INI_STRING(section, key, str)                               \
    if (!WritePrivateProfileString(section, key, str, szINIFile))           \
      return(FALSE)
  
  #define WRITE_INI_DWORD(section, key, dw)                                 \
    _ultot(dw, szValue, 10);                                                \
    WRITE_INI_STRING(section, key, szValue)

  #define RS  DISPLAY_RenderSettings
  // End macro definitions ===================================================

  TCHAR szValue[20];
  TCHAR szINIFile[MAX_PATH];

  // Format the INI file name
  UDUTILS_GetModulePath(szINIFile, MAX_PATH);
  _tcscat(szINIFile, TEXT(CE_ARTLIB_DirIniINIFileName));

  // Write the classic board settings
  WRITE_INI_DWORD(CLASSIC_BOARD, USE_HIRES_MESH,
                  (DWORD)RS.bClassic_UseHiResMesh);
  WRITE_INI_DWORD(CLASSIC_BOARD, USE_HIRES_TEXTURES,
                  (DWORD)RS.bClassic_UseHiResTextures);
  WRITE_INI_DWORD(CLASSIC_BOARD, USE_DITHERING,
                  (DWORD)RS.bClassic_UseDithering);
  WRITE_INI_DWORD(CLASSIC_BOARD, USE_BILINEARFILTERING,
                  (DWORD)RS.bClassic_UseBilinearFiltering);

  // Write the city board settings
  WRITE_INI_DWORD(CITY_BOARD, USE_HIRES_MESH, (DWORD)RS.bCity_UseHiResMesh);
  WRITE_INI_DWORD(CITY_BOARD, USE_HIRES_TEXTURES, 
                  (DWORD)RS.bCity_UseHiResTextures);
  WRITE_INI_DWORD(CITY_BOARD, USE_DITHERING, (DWORD)RS.bCity_UseDithering);
  WRITE_INI_DWORD(CITY_BOARD, USE_BILINEARFILTERING, 
                  (DWORD)RS.bCity_UseBilinearFiltering);

  // Are we going to use the 3D board?
  WRITE_INI_DWORD(GENERAL_SECTION, USE_3DBOARD, (DWORD)RS.bUse3DBoard);

  // Are we using the bitmap background with the 3D stuff??
  WRITE_INI_DWORD(GENERAL_SECTION, USE_BACKGROUND, (DWORD)RS.bUseBackground);

  // Whats our sound setting?
  // NOTE This is only the first time setup - give defaults.
  WRITE_INI_DWORD(SOUND_SECTION, MUSIC_ON, TRUE/*(DWORD)DISPLAY_state.IsOptionMusicOn*/);
  WRITE_INI_DWORD(SOUND_SECTION, MUSIC_SCORE, 0/*(DWORD)DISPLAY_state.OptionMusicTuneIndex*/);
  // Initialize em, looks like read is skipped first time.
  DISPLAY_state.IsOptionMusicOn = TRUE;
  DISPLAY_state.OptionMusicTuneIndex = 0;

  // Remove our local macros
  #undef RS
  #undef WRITE_INI_DWORD
  #undef WRITE_INI_STRING

  return(TRUE);
}
BOOL UDUTILS_WriteINIDisplaySettingsSpecial()
{
  //Copy of above which doesn't write back 3d board on flag (hack, Sept 30 - RK)
  // Define some local macros to make our life easier ========================
  #define WRITE_INI_STRING(section, key, str)                               \
    if (!WritePrivateProfileString(section, key, str, szINIFile))           \
      return(FALSE)
  
  #define WRITE_INI_DWORD(section, key, dw)                                 \
    _ultot(dw, szValue, 10);                                                \
    WRITE_INI_STRING(section, key, szValue)

  #define RS  DISPLAY_RenderSettings
  // End macro definitions ===================================================

  TCHAR szValue[20];
  TCHAR szINIFile[MAX_PATH];

  // Format the INI file name
  UDUTILS_GetModulePath(szINIFile, MAX_PATH);
  _tcscat(szINIFile, TEXT(CE_ARTLIB_DirIniINIFileName));

  // Write the classic board settings
  //WRITE_INI_DWORD(CLASSIC_BOARD, USE_HIRES_MESH,
  //                (DWORD)RS.bClassic_UseHiResMesh);
  //WRITE_INI_DWORD(CLASSIC_BOARD, USE_HIRES_TEXTURES,
  //                (DWORD)RS.bClassic_UseHiResTextures);
  //WRITE_INI_DWORD(CLASSIC_BOARD, USE_DITHERING,
  //                (DWORD)RS.bClassic_UseDithering);
  //WRITE_INI_DWORD(CLASSIC_BOARD, USE_BILINEARFILTERING,
  //                (DWORD)RS.bClassic_UseBilinearFiltering);

  // Write the city board settings
  //WRITE_INI_DWORD(CITY_BOARD, USE_HIRES_MESH, (DWORD)RS.bCity_UseHiResMesh);
  //WRITE_INI_DWORD(CITY_BOARD, USE_HIRES_TEXTURES, 
  //                (DWORD)RS.bCity_UseHiResTextures);
  //WRITE_INI_DWORD(CITY_BOARD, USE_DITHERING, (DWORD)RS.bCity_UseDithering);
  //WRITE_INI_DWORD(CITY_BOARD, USE_BILINEARFILTERING, 
  //                (DWORD)RS.bCity_UseBilinearFiltering);

  // Are we going to use the 3D board?
  //WRITE_INI_DWORD(GENERAL_SECTION, USE_3DBOARD, (DWORD)RS.bUse3DBoard);

  // Are we using the bitmap background with the 3D stuff??
  //WRITE_INI_DWORD(GENERAL_SECTION, USE_BACKGROUND, (DWORD)RS.bUseBackground);

  // Whats our sound setting?
  WRITE_INI_DWORD(SOUND_SECTION, MUSIC_ON, (DWORD)DISPLAY_state.IsOptionMusicOn);
  WRITE_INI_DWORD(SOUND_SECTION, MUSIC_SCORE, (DWORD)DISPLAY_state.OptionMusicTuneIndex);

  // Remove our local macros
  #undef RS
  #undef WRITE_INI_DWORD
  #undef WRITE_INI_STRING

  return(TRUE);
}


/*****************************************************************************
* Board loader - dumps old board & loads new resetting TIM page - the
* board is TIM page 0.  Cut out from UDBoard.cpp
*/
// ===========================================================================
void UDUTILS_SwitchToBoardUSA()
{
  // Remove any previously loaded board
  if (LED_EI != CurrentBoard)
    LE_DATA_RemoveRef(CurrentBoard);

  // If we aren't using a 3D board, don't bother loading anything
  if (!DISPLAY_RenderSettings.bUse3DBoard) return;

  // We now load the board according to the rendering settings

  // Are we loading a classic board mesh
  if (0 == DISPLAY_state.city)
  {
    CurrentBoard = DISPLAY_RenderSettings.bClassic_UseHiResMesh ?
                   LED_IFT(DAT_3D, HMD_boardhigh) :
                   LED_IFT(DAT_3D, HMD_boardmed);
    ENABLE_DITHERING((BOOL)DISPLAY_RenderSettings.bClassic_UseDithering);
    ENABLE_BILINEARFILTERING(0, (BOOL)DISPLAY_RenderSettings.
                             bClassic_UseBilinearFiltering);
  }
  else
  {
    // Load a city board mesh
    CurrentBoard = DISPLAY_RenderSettings.bCity_UseHiResMesh ?
                   LED_IFT(DAT_3D, HMD_board_cityhigh) :
                   LED_IFT(DAT_3D, HMD_board_citymed);
    ENABLE_DITHERING((BOOL)DISPLAY_RenderSettings.bCity_UseDithering);
    ENABLE_BILINEARFILTERING(0, (BOOL)DISPLAY_RenderSettings.
                             bCity_UseBilinearFiltering);
  }

  DISPLAY_state.cityTextureSet = DISPLAY_state.city;

  // Load the board
  SETTIMBANK(0);

#if CE_ARTLIB_3DUsingOldFrame
  HMD_ReleaseCurrentBank();
#endif

  MESHType** ppMesh = (MESHType**)LE_DATA_UseRef(CurrentBoard);
  ASSERT(NULL != ppMesh && NULL != *ppMesh);

  SETTIMBANK(DISPLAY_state.VirtualTimPageForAnythingNotPreloaded);

  // Now determine which texture set we need to load

  BoardTextureSet textureSet;
  ZeroMemory(&textureSet, sizeof(BoardTextureSet));

  switch (LOWORD(CurrentBoard))
  {
    case HMD_boardmed:
      TRACE("Loading texture set for medium mesh classic board\n");
      ASSERT(0 == DISPLAY_state.city);

      // If we aren't using high-res textures, we don't need to load a
      //  separate texture set.  The medium res textures are already
      //  included with the HMD.
      if (!DISPLAY_RenderSettings.bClassic_UseHiResTextures) break;

      textureSet.aszCityNames   = g_aszBoardMed256Textures;
      textureSet.aPositions     = g_aBoardMedTxtrPositions;
      textureSet.aszCommon      = g_aszCommonMed256Textures;
      textureSet.nNumCityNames  = NUM_MEDCITY_CITIES;
      textureSet.nNumCommon     = NUM_MEDCOMMON_TEXTURES;
      textureSet.szBoardType    = TEXT("Medium");
      _tcscpy(textureSet.szTexturePath, "Cities\\City00\\");
      break;

    case HMD_boardhigh:
      TRACE("Loading texture set for high mesh classic board\n");
      ASSERT(0 == DISPLAY_state.city);

      // If we aren't using high-res textures, we don't need to load a
      //  separate texture set.  The medium res textures are already
      //  included with the HMD.
      if (!DISPLAY_RenderSettings.bClassic_UseHiResTextures) break;

      textureSet.aszCityNames   = g_aszBoardHigh256Textures;
      textureSet.aPositions     = g_aBoardHighTxtrPositions;
      textureSet.aszCommon      = g_aszCommonHigh256Textures;
      textureSet.nNumCityNames  = NUM_HICITY_CITIES;
      textureSet.nNumCommon     = NUM_HIGHCOMMON_TEXTURES;
      textureSet.szBoardType    = TEXT("High");
      _tcscpy(textureSet.szTexturePath, "Cities\\City00\\");
      break;
    
    case HMD_board_citymed:
      TRACE("Loading texture set for medium mesh city board\n");
      if (DISPLAY_RenderSettings.bCity_UseHiResTextures)
      {
        textureSet.aszCityNames   = g_aszCityMed256Textures;
        textureSet.aszCityPhotos  = g_aszCity256Photos;
        textureSet.aPositions     = g_aCityMedTxtrPositions;
        textureSet.aszCommon      = g_aszCommonMed256Textures;
      }
      else
      {
        textureSet.aszCityNames   = g_aszCityMed128Textures;
        textureSet.aszCityPhotos  = g_aszCity128Photos;
        textureSet.aPositions     = g_aCityMedTxtrPositions;
        textureSet.aszCommon      = g_aszCommonMed128Textures;
      }
      textureSet.nNumCityNames  = NUM_MEDCITY_CITIES;
      textureSet.nNumCityPhotos = NUM_CITY_PHOTOS;
      textureSet.nNumCommon     = NUM_MEDCOMMON_TEXTURES;
      textureSet.szBoardType    = TEXT("Medium");

      if (0 < DISPLAY_state.cityTextureSet)
      {
        _stprintf(textureSet.szTexturePath, TEXT("Cities\\City%02d\\"),
                  DISPLAY_state.cityTextureSet);
      }
      else if (0 > DISPLAY_state.cityTextureSet)
        _tcscpy(textureSet.szTexturePath, DISPLAY_state.customBoardPath);
      
      break;

    case HMD_board_cityhigh:
      TRACE("Loading texture set for high mesh city board\n");
      if (DISPLAY_RenderSettings.bCity_UseHiResTextures)
      {
        textureSet.aszCityNames   = g_aszCityHigh256Textures;
        textureSet.aszCityPhotos  = g_aszCity256Photos;
        textureSet.aPositions     = g_aCityHighTxtrPositions;
        textureSet.aszCommon      = g_aszCommonHigh256Textures;
      }
      else
      {
        textureSet.aszCityNames   = g_aszCityHigh128Textures;
        textureSet.aszCityPhotos  = g_aszCity128Photos;
        textureSet.aPositions     = g_aCityHighTxtrPositions;
        textureSet.aszCommon      = g_aszCommonHigh128Textures;
      }
      textureSet.nNumCityNames  = NUM_HICITY_CITIES;
      textureSet.nNumCityPhotos = NUM_CITY_PHOTOS;
      textureSet.nNumCommon     = NUM_HIGHCOMMON_TEXTURES;
      textureSet.szBoardType    = TEXT("High");
      
      if (0 < DISPLAY_state.cityTextureSet)
      {
        _stprintf(textureSet.szTexturePath, TEXT("Cities\\City%02d\\"),
                  DISPLAY_state.cityTextureSet);
      }
      else if (0 > DISPLAY_state.cityTextureSet)
        _tcscpy(textureSet.szTexturePath, DISPLAY_state.customBoardPath);
      
      break;
  }

  // Load the texture set and check to make sure we actually loaded all of
  //  the textures
  if (!UDUTILS_LoadBoardTextureSet(*ppMesh, textureSet))
  {
#ifdef _DEBUG
    ::MessageBox(NULL, "Failed to load all of the textures",
                 "WARNING!", MB_OK);
#endif // _DEBUG
  }
}


// ===========================================================================
// Function:    UDUTILS_SwitchToBoardEURO
//
// Description: Loads a new board and its texture set for one of the European/
//              foreign boards
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDUTILS_SwitchToBoardEURO()
{
  // If we aren't using a 3D board, don't bother loading anything
  if (!DISPLAY_RenderSettings.bUse3DBoard) return;

  Surface     overlay;
  Surface     tmpSurf;
  TextureID*  pTextureID  = NULL;
  HBITMAP     hBmp        = NULL;
  DDCOLORKEY  ddClrKey    = { 0, 0 };
  RECT        rct         = { 0, 0, 128, 128 };
  BOOL        bUseHighResTextures = FALSE;

  // iLangId represents the installed language.
  // It looks like this
  // USEnglish  = 1
  // UKEnglish  = 2
  // French     = 3
  // German     = 4
  //  ... and so on (see lang.h).  This is why we offset by two here because
  //  our language folders start with UKEnglish at 0.

  int         nLanguage   = iLangId - 2;

  const WORDPAIR* aPositions    = NULL;
  LPCTSTR*  aszCityPhotos       = NULL;
  LPCTSTR*  aszCityNames        = NULL;   
  LPCTSTR*  aszLangTextures     = NULL;
  LPCTSTR*  aszBoardOverlays    = NULL;
  LPCTSTR*  aszCurrencyTextures = NULL;
  int       nNumTextures        = 0;
  int       nNumPhotos          = 0;
  int       nNumNames           = 0;
  int       nNumLangTxtrs       = 0;
  int       nNumOverlays        = 0;
  int       nNumCurrencyTxtrs   = 0;
  int       nNumLoaded          = 0;

  LPCTSTR   szType = NULL;
  TCHAR     szFile[MAX_PATH];
  TCHAR     szBitmap[MAX_PATH];
  TCHAR     szOverlay[MAX_PATH];

  DDBLTFX   ddBltFx;
  ZeroMemory(&ddBltFx, sizeof(DDBLTFX));
  ddBltFx.dwSize = sizeof(DDBLTFX);

  // Make sure we have a valid language
  ASSERT(0 <= nLanguage);

  // Remove any previously loaded board
  if (LED_EI != CurrentBoard)
    LE_DATA_RemoveRef(CurrentBoard);

  // We now load the board according to the rendering settings

  // We only get to load the city mesh boards if we are using a custom board.
  //  Otherwise we always load either the medium or high classic board.
  if (-1 != DISPLAY_state.city)
  {
    CurrentBoard = DISPLAY_RenderSettings.bClassic_UseHiResMesh ?
                   LED_IFT(DAT_3D, HMD_boardhigh) :
                   LED_IFT(DAT_3D, HMD_boardmed);
    ENABLE_DITHERING((BOOL)DISPLAY_RenderSettings.bClassic_UseDithering);
    ENABLE_BILINEARFILTERING(0, (BOOL)DISPLAY_RenderSettings.
                             bClassic_UseBilinearFiltering);
  }
  else
  {
    // Load a city board mesh
    CurrentBoard = DISPLAY_RenderSettings.bCity_UseHiResMesh ?
                   LED_IFT(DAT_3D, HMD_board_cityhigh) :
                   LED_IFT(DAT_3D, HMD_board_citymed);
    ENABLE_DITHERING((BOOL)DISPLAY_RenderSettings.bCity_UseDithering);
    ENABLE_BILINEARFILTERING(0, (BOOL)DISPLAY_RenderSettings.
                             bCity_UseBilinearFiltering);
  }

  DISPLAY_state.cityTextureSet = DISPLAY_state.city;

  // Load the board
  SETTIMBANK(0);

#if CE_ARTLIB_3DUsingOldFrame
  HMD_ReleaseCurrentBank();
#endif

  MESHType** ppMesh = (MESHType**)LE_DATA_UseRef(CurrentBoard);
  ASSERT(NULL != ppMesh && NULL != *ppMesh);

  SETTIMBANK(DISPLAY_state.VirtualTimPageForAnythingNotPreloaded);

  // Now determine which texture set we need to load

  BoardTextureSet textureSet;
  ZeroMemory(&textureSet, sizeof(BoardTextureSet));

  switch (LOWORD(CurrentBoard))
  {
    case HMD_boardmed:
      TRACE("Loading texture set for medium mesh foreign board\n");
      if (DISPLAY_RenderSettings.bClassic_UseHiResTextures)
      {
        aszLangTextures     = g_aszEUROMed256Lang;
        aszBoardOverlays    = g_aszEUROMed256Boards;
        aszCurrencyTextures = g_aszEUROMed256Currencies;
        bUseHighResTextures = TRUE;
      }
      else
      {
        aszLangTextures     = g_aszEUROMed128Lang;
        aszBoardOverlays    = g_aszEUROMed128Boards;
        aszCurrencyTextures = g_aszEUROMed128Currencies;
      }
      aPositions          = g_aEUROMedBoardPositions;
      nNumTextures        = NUM_MEDBOARD_TEXTURES;
      nNumLangTxtrs       = NUM_EURO_MEDIUMLANGUAGES;
      nNumOverlays        = NUM_EURO_MEDIUMBOARDS;
      nNumCurrencyTxtrs   = NUM_EURO_MEDIUMCURRENCIES;
      szType              = TEXT("Medium");
      break;

    case HMD_boardhigh:
      TRACE("Loading texture set for high mesh foreign board\n");
      if (DISPLAY_RenderSettings.bClassic_UseHiResTextures)
      {
        aszLangTextures     = g_aszEUROHigh256Lang;
        aszBoardOverlays    = g_aszEUROHigh256Boards;
        aszCurrencyTextures = g_aszEUROHigh256Currencies;
        bUseHighResTextures = TRUE;
      }
      else
      {
        aszLangTextures     = g_aszEUROHigh128Lang;
        aszBoardOverlays    = g_aszEUROHigh128Boards;
        aszCurrencyTextures = g_aszEUROHigh128Currencies;
      }
      aPositions          = g_aEUROHighBoardPositions;
      nNumTextures        = NUM_HIBOARD_TEXTURES;
      nNumLangTxtrs       = NUM_EURO_HIGHLANGUAGES;
      nNumOverlays        = NUM_EURO_HIGHBOARDS;
      nNumCurrencyTxtrs   = NUM_EURO_HIGHCURRENCIES;
      szType              = TEXT("High");
      break;

    case HMD_board_citymed:
      TRACE("Loading texture set for medium mesh custom board\n");
      ASSERT(-1 == DISPLAY_state.city);
      if (DISPLAY_RenderSettings.bCity_UseHiResTextures)
      {
        aszCityPhotos       = g_aszCity256Photos; 
        aszCityNames        = g_aszCityMed256Textures;
        aszLangTextures     = g_aszEUROMed256Lang;
        aszBoardOverlays    = g_aszEUROMed256Boards;
        aszCurrencyTextures = g_aszEUROMed256Currencies;
        bUseHighResTextures = TRUE;
      }
      else
      {
        aszCityPhotos       = g_aszCity128Photos; 
        aszCityNames        = g_aszCityMed128Textures;
        aszLangTextures     = g_aszEUROMed128Lang;
        aszBoardOverlays    = g_aszEUROMed128Boards;
        aszCurrencyTextures = g_aszEUROMed128Currencies;
      }
      aPositions          = g_aEUROMedCityBoardPositions;
      nNumPhotos          = NUM_CITY_PHOTOS;
      nNumNames           = NUM_MEDCITY_CITIES;
      nNumTextures        = NUM_MEDCITY_TEXTURES;
      nNumLangTxtrs       = NUM_EURO_MEDIUMLANGUAGES;
      nNumOverlays        = NUM_EURO_MEDIUMBOARDS;
      nNumCurrencyTxtrs   = NUM_EURO_MEDIUMCURRENCIES;
      szType              = TEXT("Medium");
      break;

    case HMD_board_cityhigh:
      TRACE("Loading texture set for high mesh custom board\n");
      ASSERT(-1 == DISPLAY_state.city);
      if (DISPLAY_RenderSettings.bCity_UseHiResTextures)
      {
        aszCityPhotos       = g_aszCity256Photos; 
        aszCityNames        = g_aszCityHigh256Textures;
        aszLangTextures     = g_aszEUROHigh256Lang;
        aszBoardOverlays    = g_aszEUROHigh256Boards;
        aszCurrencyTextures = g_aszEUROHigh256Currencies;
        bUseHighResTextures = TRUE;
      }
      else
      {
        aszCityPhotos       = g_aszCity128Photos; 
        aszCityNames        = g_aszCityHigh128Textures;
        aszLangTextures     = g_aszEUROHigh128Lang;
        aszBoardOverlays    = g_aszEUROHigh128Boards;
        aszCurrencyTextures = g_aszEUROHigh128Currencies;
      }
      aPositions          = g_aEUROHighCityBoardPositions;
      nNumPhotos          = NUM_CITY_PHOTOS;
      nNumNames           = NUM_HICITY_CITIES;
      nNumTextures        = NUM_HICITY_TEXTURES;
      nNumLangTxtrs       = NUM_EURO_HIGHLANGUAGES;
      nNumOverlays        = NUM_EURO_HIGHBOARDS;
      nNumCurrencyTxtrs   = NUM_EURO_HIGHCURRENCIES;
      szType              = TEXT("High");
      break;
  }

  // =========================================================================
  // Load the textures

  if (bUseHighResTextures)
    rct.right = rct.bottom = 256;

  SETTIMBANK(0);

  // Load the city property photos
  for (int i = 0; i < nNumPhotos; i++)
  {                   
    _stprintf(szBitmap, TEXT("%sPhotos\\%s"), DISPLAY_state.customBoardPath,
              aszCityPhotos[i]);

    // Load the texture
    pTextureID = LoadBMPTexture(aPositions[i].w1, aPositions[i].w2, szBitmap);

    if (!pTextureID) continue;
    ASSERT(NULL != pTextureID->pTexture);

    // Swap the texture
    if ((*ppMesh)->SubstituteTexture(pTextureID))
      nNumLoaded++;
    else
    {
      TRACE("Texture loaded properly, but failed to swapped into the mesh\n");
      RELEASEPOINTER(pTextureID);
    }
  }

  int nOffset = nNumPhotos;

  // Load the city property names
  for (i = 0; i < nNumNames; i++)
  {                   
    _stprintf(szBitmap, TEXT("%s%s\\%s"), DISPLAY_state.customBoardPath,
              szType, aszCityNames[i]);

    // Load the texture
    pTextureID = LoadBMPTexture(aPositions[nOffset + i].w1,
                                aPositions[nOffset + i].w2, szBitmap);

    if (!pTextureID) continue;
    ASSERT(NULL != pTextureID->pTexture);

    // Swap the texture
    if ((*ppMesh)->SubstituteTexture(pTextureID))
      nNumLoaded++;
    else
    {
      TRACE("Texture loaded properly, but failed to swapped into the mesh\n");
      RELEASEPOINTER(pTextureID);
    }
  }

  // Load language based textures (include property texture templates)
  for (i = nNumNames; i < nNumLangTxtrs; i++)
  {
    _stprintf(szFile, TEXT("Languages\\Lang%02d\\%s\\%s"), nLanguage, szType,
              aszLangTextures[i]);

    if (!LE_DIRINI_ExpandFileName(szFile, szBitmap, LE_DIRINI_GlobalPathList))
      continue;

    // Load the texture
    pTextureID = LoadBMPTexture(aPositions[nOffset + i].w1, 
                                aPositions[nOffset + i].w2, 
                                szBitmap);

    if (!pTextureID) continue;
    ASSERT(NULL != pTextureID->pTexture);

    // Check if we need to load an overlay
    if (i < nNumOverlays)
    {
      // Create the temporary surface so we can construct the overlays
      VERIFY(tmpSurf.Create(*pTextureID->pTexture));

      // Make sure we are using the correct colour key
      if (!pTextureID->pTexture->IsPalettized())
      {
        ddClrKey.dwColorSpaceLowValue   = 
        ddClrKey.dwColorSpaceHighValue  = 0x00007FFF;
      }

      // Blit property texture overlays to language dependent templates
      _stprintf(szFile, TEXT("Boards\\Board%02d\\%s\\%s"), DISPLAY_state.city,
                szType, aszBoardOverlays[i - nOffset]);

      if (!LE_DIRINI_ExpandFileName(szFile, szOverlay, LE_DIRINI_GlobalPathList))
        continue;

      // Load property texture overlay
      hBmp = LoadBMP(szOverlay);
      ASSERT(NULL != hBmp);
      VERIFY(overlay.Create(hBmp));

      overlay.SetColorKey(DDCKEY_SRCBLT, ddClrKey);

      // Copy the overlay to our temp surface
      tmpSurf.BltFast(0, 0, overlay, rct, DDBLTFAST_SRCCOLORKEY);

      // Now copy the temporary surface back to the texture
      // NOTE: you can only use GDI to blit to a texture
      VERIFY(pTextureID->pTexture->GDIBlit(0, 0, tmpSurf, rct));
    }
    // Now check if we need to load the overlay for the GO square
    else if (i == nNumLangTxtrs - 1)
    {
      // Create the temporary surface so we can construct the overlays
      VERIFY(tmpSurf.Create(*pTextureID->pTexture));

      // Make sure we are using the correct colour key
      if (!pTextureID->pTexture->IsPalettized())
      {
        ddClrKey.dwColorSpaceLowValue   = 
        ddClrKey.dwColorSpaceHighValue  = 0x00007FFF;
      }

      // Special Case: Load the currency overlay for the Go square
      _stprintf(szFile, TEXT("Currency\\Curr%02d\\%s\\GO_OVERLAY%d.bmp"),
                DISPLAY_state.system, szType,
                bUseHighResTextures ? 256 : 128);

      if (!LE_DIRINI_ExpandFileName(szFile, szOverlay, LE_DIRINI_GlobalPathList))
        continue;

      hBmp = LoadBMP(szOverlay);
      ASSERT(NULL != hBmp);
      VERIFY(overlay.Create(hBmp));

      overlay.SetColorKey(DDCKEY_SRCBLT, ddClrKey);

      // Copy the overlay to our temp surface
      tmpSurf.BltFast(0, 0, overlay, rct, DDBLTFAST_SRCCOLORKEY);

      // Now copy the temporary surface back to the texture
      // NOTE: you can only use GDI to blit to a texture
      VERIFY(pTextureID->pTexture->GDIBlit(0, 0, tmpSurf, rct));
    }

    // Swap the texture
    if ((*ppMesh)->SubstituteTexture(pTextureID))
      nNumLoaded++;
    else
    {
      TRACE("Texture loaded properly, but failed to swapped into the mesh\n");
      RELEASEPOINTER(pTextureID);
    }
  }

  nOffset += nNumLangTxtrs;

  // Load currency stuff
  for (i = 0; i < nNumCurrencyTxtrs; i++)
  {
    // Format the texture's file name
    _stprintf(szFile, TEXT("Currency\\Curr%02d\\%s\\%s"),
              DISPLAY_state.system, szType, aszCurrencyTextures[i]);

    if (!LE_DIRINI_ExpandFileName(szFile, szOverlay, LE_DIRINI_GlobalPathList))
      continue;

    // Load the texture
    pTextureID = LoadBMPTexture(aPositions[nOffset + i].w1, 
                                aPositions[nOffset + i].w2, szOverlay);

    if (!pTextureID) continue;
    ASSERT(NULL != pTextureID->pTexture);

    // Swap the texture
    if ((*ppMesh)->SubstituteTexture(pTextureID))
      nNumLoaded++;
    else
    {
      TRACE("Texture loaded properly, but failed to swapped into the mesh\n");
      RELEASEPOINTER(pTextureID);
    }
  }

#ifdef _DEBUG
  // Check to make sure we actually loaded all of the textures
  if (nNumLoaded != nNumTextures)
  {
    ::MessageBox(NULL, "Failed to load all of the textures",
                 "WARNING!", MB_OK);
  }
#endif // _DEBUG

  // Cleanup
  SETTIMBANK(DISPLAY_state.VirtualTimPageForAnythingNotPreloaded);
  RELEASESURFACE(overlay);
  RELEASESURFACE(tmpSurf);
}


// ===========================================================================
// Function:    UDUTILS_RenderSpeedTest
//
// Description: Performs a 3D rendering speed test
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    This is a fairly complicated procedure.  We start with the
//              highest quality settings and continue dropping the resolution
//              until we reach a resonable frame rate.  We need to do this for
//              both the classic boards and the city boards.
// ===========================================================================
BOOL UDUTILS_RenderSpeedTest()
{
  float           fFrameRate              = 0.0f;
  LONGLONG        llStart                 = 0;
  LONGLONG        llEnd                   = 0;
  LONGLONG        llFrequency             = 0;
  Mesh**          ppMesh                  = NULL;
  int             i = 0, j                = 0;
  const int       knNumFrames             = 3;
  float           kfMinFrameRate          = 0.0f;
  BOOL            bTestingClassic         = TRUE;
  BOOL            bNotFound               = TRUE;
  WORD            wShift                  = 0;
  WORD            wTmp                    = 0;
  int             nMsgID                  = IDB_TESTMSG01;
  Surface*        pMsgSurf                = NULL;

  MSG             msg;
  D3DVECTOR       vFrom;
  D3DVECTOR       vAt;
  D3DVECTOR       vUp;
  Matrix          mtrxWorld(Matrix::IDENTITY);
  Matrix          mtrxView;
  Matrix          mtrxProjection;
  Matrix          mtrxOldWorld;
  Matrix          mtrxOldView;
  Matrix          mtrxOldProjection;

  D3DRECT         rct     = { 0, 0, 800, 450 };
  RECT            rctSrc  = { 0, 0, 0, 0 };
  POINT           ptPos   = { 0, 0 };
  DDBLTFX         ddBltFx;

  ZeroMemory(&ddBltFx, sizeof(DDBLTFX));
  ddBltFx.dwSize = sizeof(DDBLTFX);

  // Get the 3D device from pc3d
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // If we don't have any hardware, we automatically set everything to
  //  run in software
  if (!p3DDevice->IsHardware())
  {
    ZeroMemory(&DISPLAY_RenderSettings, sizeof(DISPLAY_RenderSettingsStruct));
    return(TRUE);
  }

  // Save the old rendering settings
  p3DDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, mtrxOldWorld);
  p3DDevice->GetTransform(D3DTRANSFORMSTATE_VIEW, mtrxOldView);
  p3DDevice->GetTransform(D3DTRANSFORMSTATE_PROJECTION, mtrxOldProjection);

  // Set our world matrix
  mtrxWorld.Scale(0.1f, 0.1f, 0.1f);
  p3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &mtrxWorld);
  
  // Set the projection matrix
  mtrxProjection.ProjectionMatrix(CLIPPING_PLANE_NEAR, CLIPPING_PLANE_FAR,
                                  ((float)PIDOUBLE)/4);
  p3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &mtrxProjection);

  // Set the ambient light and remove any materials
  p3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0xFFFFFFFF);
  p3DDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, NULL);

  // Save the old viewport
  PVIEWPORT pOldViewport = p3DDevice->GetCurrentViewport();

  // Create ourselves a new viewport
  PVIEWPORT pViewport = pc3D::GetDDrawDriver()->CreateViewport();
  ASSERT(NULL != pViewport);
  p3DDevice->AddViewport(pViewport);

  // Setup the viewport
  VIEWPORTDATA viewData;
  ZeroMemory(&viewData, sizeof(VIEWPORTDATA));
  viewData.dwSize = sizeof(VIEWPORTDATA);
  viewData.dwWidth      = rct.x2;
  viewData.dwHeight     = rct.y2;
  viewData.dvClipX      = -1.0;
  viewData.dvClipY      =  1.0;
  viewData.dvClipWidth  =  2.0;
  viewData.dvClipHeight =  2.0;
  viewData.dvMinZ       =  0.0;
  viewData.dvMaxZ       =  1.0;

  // Set the viewport's properties and set is as the current viewport
  pViewport->SetViewport2(&viewData);
  p3DDevice->SetCurrentViewport(pViewport);

  // Get the render target
  Surface* pRenderTarget = pc3D::GetRenderSurface();
  ASSERT(NULL != pRenderTarget);

  // We need to load the appropriate background according to which
  //  version we are running.
#if USA_VERSION
    WORD wBmpID = IDB_BACKGROUND;
    WORD wOverlayID = IDB_OVERLAY;
#else
    WORD wBmpID = IDB_BACKGROUNDEURO;
    WORD wOverlayID = IDB_OVERLAYEURO;
#endif

  // Load the background surface
  Surface surfBackground(MAKEINTRESOURCE(wBmpID));

  // Load the overlay surface
  Surface surfOverlay(MAKEINTRESOURCE(wOverlayID));

  // Use a WORD pointer to reference the stuff in the render settings
  WORD* pwSettings = (WORD*)(&DISPLAY_RenderSettings);

  // Turny everything on at the start
  *pwSettings = 0x03FF;

  // Get the counter freqency
  if (!QueryPerformanceFrequency((LARGE_INTEGER*)&llFrequency)) goto ABORT_TEST;

  // Display the info message to the user before we start the test
#if !USA_VERSION
  // Things are a tad different for the foreign versions
  // We need to select the appropriate message according to the installed
  //  language
  nMsgID = IDB_TESTMSG02 + iLangId - 2;  
#endif // USA_VERSION

  // Load the message surface
  pMsgSurf = new Surface(MAKEINTRESOURCE(nMsgID));
  ASSERT(NULL != pMsgSurf);
  
  // Get the size of the surface
  pMsgSurf->GetSize(*(SIZE*)((LPVOID)(&rctSrc.right)));

  // We are using GDI to do the blit here to ensure that the message is
  //  actually displayed.
  pRenderTarget->GDIBlit(0, 0, *pMsgSurf, rctSrc);

  // Flip the display so we can see the message
  pc3D::FlipDisplay();

  // Destroy our surface, we don't need it any more
  DESTROYSURFACE(pMsgSurf);

#if CE_ARTLIB_EnableMultitasking
  LE_KEYBRD_AnyKey = 0;
  LE_MOUSE_ClearAllLatched ();
  while (TRUE)
  {
    if (LE_MOUSE_MouseLatched[LEFT_BUTTON])
      break;
    if (LE_KEYBRD_AnyKey)
      break;
    LE_TIMER_Delay (1);
  }
#else // Put in the message pump to handle the keypress
  while (bNotFound) 
  {
    // Get the next message in the queue
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

    switch (msg.message)
    {
      // if it's a quit message, we're outta here 
      case WM_QUIT: PostQuitMessage(msg.wParam);  goto ABORT_TEST;
      case WM_LBUTTONDOWN:
      case WM_CHAR: bNotFound = FALSE; break;
      default:      TranslateMessage(&msg); DispatchMessage(&msg); break;
    }
  }
#endif

  // Flush the message queue so that the key press doesn't abort
  // the intro video playback later on.

  LE_QUEUE_MessageRecord Message;
  while (LE_UIMSG_ReceiveEvent (&Message))
    LE_QUEUE_FreeStorageAssociatedWithMessage (&Message);

  // Start with a clean slate
  DDRAW_ClearMainSurfaces();

  // We need to do this test for both the classic and city boards
  for (bTestingClassic = TRUE; bTestingClassic >= 0; bTestingClassic--, wShift += 4)
  {
    bNotFound = TRUE;

    // This test will run a fair amount faster that actual game play because
    //  we are not rendering tokens and dice, or the 2D user interface so
    //  these values have been inflated to reflect this.
    if (bTestingClassic)
      kfMinFrameRate = 30.0f;
    else
    {
      kfMinFrameRate = 28.0f;

      // Whatever we found for the classic board, start with it as our base
      //  for the city board test.
      wTmp = *pwSettings & 0x030F;
      *pwSettings = ((wTmp << 4) | wTmp) & 0x03FF;

      // If we have disabled the 3D board for the classic board, don't even
      //  bother doing the test for the city board
      if (!DISPLAY_RenderSettings.bUse3DBoard)
        goto ABORT_TEST;
    }

    // Continue until the best render setting are found
    while (bNotFound)
    {
      // Remove the previously loaded board
      if (LED_EI != CurrentBoard)
      {
        // Make sure that the board is removed
        while (LE_DATA_RemoveRef(CurrentBoard)) ;
        CurrentBoard = LED_EI;
      }

      if (bTestingClassic)
        DISPLAY_state.city = 0;
      else
      {
#if USA_VERSION
        DISPLAY_state.city = 1;
#else
        // Set the city so we are forced to load a custom board
        DISPLAY_state.city = -1;

        // Get the path to our sample foreign board
        TCHAR szPath[MAX_PATH];
        _tcscpy(szPath, "SampleBoard\\SampleBoard.brd");

        if (!LE_DIRINI_ExpandFileName(szPath, DISPLAY_state.customBoardPath,
                                      LE_DIRINI_GlobalPathList))
        {
          break;
        }

        // Rip out the filename from the path
        LPTSTR szTmp = _tcsrchr(DISPLAY_state.customBoardPath, '\\');
        szTmp[1] = '\0';
#endif  // USA_VERSION
      }

      UDBOARD_SwitchToBoard(LED_EI, FALSE);

      // Get the board mesh
      ppMesh = (Mesh**)LE_DATA_GetPointer(CurrentBoard);
      ASSERT(NULL != ppMesh && NULL != *ppMesh);

      // Record the start time
      if (!QueryPerformanceCounter((LARGE_INTEGER*)&llStart)) goto ABORT_TEST;

      // Now render a bunch of frames to figure out how fast we can do it
      for (i = 0; i < 39; i++)
      {
        // Setup the camera
        vFrom.x = CameraAngles2D[i][0];
        vFrom.y = CameraAngles2D[i][1];
        vFrom.z = CameraAngles2D[i][2];

        vAt.x = vFrom.x + CameraAngles2D[i][3];
        vAt.y = vFrom.y + CameraAngles2D[i][4];
        vAt.z = vFrom.z + CameraAngles2D[i][5];

        vUp.x = CameraAngles2D[i][6];
        vUp.y = CameraAngles2D[i][7];
        vUp.z = CameraAngles2D[i][8];

        mtrxView.ViewMatrix(vFrom, vAt, vUp);

        // Set the view(camera) matrix
        p3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &mtrxView);

        // Render each camera a couple of times to get a good test
        for (j = 0; j < knNumFrames; j++)
        {
          // Clear the viewport
          pViewport->Clear2(1ul, &rct, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                            0x00000000, 1.0f, 0);

          // If we are still using the background
          if (*pwSettings & 0x0200)
          {
            // Blit the background
            surfBackground.GetSize(*(SIZE*)(&rctSrc.right));
            if (!pRenderTarget->Blt(*((RECT*)((LPVOID)(&rct))),
                                    surfBackground.GetDDSurface(),
                                    rctSrc, 0, ddBltFx))
            {
              // Turn the background bitmap off
              *pwSettings ^= 0x0200;
            }
          }

          // Begin the scene
          p3DDevice->BeginScene();

          // Render the mesh
          (*ppMesh)->Render(p3DDevice);

          // End the scene
          p3DDevice->EndScene();

          // Blit the overlay
          surfOverlay.GetSize(*(SIZE*)(&rctSrc.right));
          ptPos.y = i * knNumFrames + j;
          ptPos.x = ptPos.y * 2;
          pRenderTarget->BltFast(ptPos.x, ptPos.y, surfOverlay, rctSrc, 0);

          // Flip the display
          pc3D::FlipDisplay();

          // Make sure we have a message pump here so the application doesn't hang
          while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
          {
            // if it's a quit message, we're outta here 
            if ((WM_KEYDOWN == msg.message && VK_ESCAPE == (int)msg.wParam) ||
                WM_QUIT == msg.message) 
            {
              // Kill our resources
              RELEASEPOINTER(pViewport);
              RELEASESURFACE(surfBackground);
              RELEASESURFACE(surfOverlay);

              // Re-post the quit message
              PostQuitMessage(msg.wParam);
              return(FALSE);
            } 
            else 
            {
              TranslateMessage(&msg);
              DispatchMessage(&msg);
            }
          }
        }
      }

      // Record the end time
      if (!QueryPerformanceCounter((LARGE_INTEGER*)&llEnd)) goto ABORT_TEST;

      // Calculate the frame rate
      fFrameRate = (float)(knNumFrames * 39) / ((float)(llEnd - llStart) /
                   (float)llFrequency);

      // Did we pass?
      if (kfMinFrameRate > fFrameRate)
      {
        TRACE("TEST RESULTS: %f frames per second\n", fFrameRate);

        // We didn't get a good frame rate.  Try dropping the quality and
        //  do the test again.

#define USING_HIRES_MESH          (*pwSettings & (0x0001 << wShift))
#define USING_HIRES_TEXTURES      (*pwSettings & (0x0002 << wShift))
#define USING_DITHERING           (*pwSettings & (0x0004 << wShift))
#define USING_BILINEARFILTERINTG  (*pwSettings & (0x0008 << wShift))
#define USING_3DBOARD             (*pwSettings & (0x0010 << wShift))

        // First we drop the texture size
        if (USING_HIRES_TEXTURES)  // Are hi-res textures enabled?
          *pwSettings ^= (0x02 << wShift);
        else
        {
          // We have already dropped the texture size.
          //  Try dropping the mesh.
          if (USING_HIRES_MESH)
          {
            *pwSettings ^= (0x01 << wShift); // Turn off hi-res meshes
            *pwSettings |= (0x02 << wShift); // Turn on hi-res textures
          }
          else
          {
            // We have already dropped both the mesh and the textures.  Try
            //  dropping dithering and bilinear filtering.
            if (USING_DITHERING && USING_BILINEARFILTERINTG)
            {
              *pwSettings ^= (0x04 << wShift); // Turn off dithering
              *pwSettings ^= (0x08 << wShift); // Turn off bilinear filtering
              DISPLAY_RenderSettings.bUseBackground = FALSE; // Turn off background
            }
            else
            {
              // Okay we have dropped everything we can.  I guess we'll have
              // to use the 2d Boards
              DISPLAY_RenderSettings.bUse3DBoard = FALSE;
              bNotFound   = FALSE;
            }
          }
        }
#undef USING_HIRES_MESH
#undef USING_HIRES_TEXTURES
#undef USING_DITHERING
#undef USING_BILINEARFILTERINTG
#undef USING_3DBOARD
      }
      else
      {
        // We passed the test.
        bNotFound = FALSE;
      }
    }

    // Spit out a message tell the user what was selected
#ifdef _DEBUG
    if (bTestingClassic)
    {
      char szMsg[MAX_PATH];
      sprintf(szMsg, "Selecting %s mesh board with %s-res textures. FrameRate: %f fps.",
              DISPLAY_RenderSettings.bClassic_UseHiResMesh ? "High" : "Medium",
              DISPLAY_RenderSettings.bClassic_UseHiResTextures ? "High" : "Medium", fFrameRate);

      ::MessageBox(NULL, szMsg, "INFO", MB_OK);
    }
    else
    {
      char szMsg[MAX_PATH];
      sprintf(szMsg, "Selecting %s mesh city board with %s-res textures. FrameRate: %f fps",
              DISPLAY_RenderSettings.bCity_UseHiResMesh ? "High" : "Medium",
              DISPLAY_RenderSettings.bCity_UseHiResTextures ? "High" : "Medium", fFrameRate);

      ::MessageBox(NULL, szMsg, "INFO", MB_OK);
    }
#endif
  }

ABORT_TEST:
  // Restore our matrices
  p3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &mtrxOldWorld);
  p3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &mtrxOldView);
  p3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &mtrxOldProjection);

  // Remove our viewport and set the old one back to the active viewport
  p3DDevice->RemoveViewport(pViewport);
  p3DDevice->SetCurrentViewport(pOldViewport);

  // Release our allocated resources
  RELEASEPOINTER(pViewport);
  RELEASESURFACE(surfBackground);
  RELEASESURFACE(surfOverlay);

  // If we have turned off the 3D board, make sure that we choose the software
  //  3D device
  if (!DISPLAY_RenderSettings.bUse3DBoard)
  {
    // NOTE: This selection will not take into effect until the next time the
    //        application is run.
    DWORD dwDriver;
    DWORD dw3DDevice;

    // Get the selected driver and 3D device from the DirectX.ini file
    if (DXINI_GetSelectedDriver(dwDriver) && 
        DXINI_GetSelected3DDevice(dw3DDevice))
    {
      // Get the number of 3D devices available for the selected driver
      DWORD dwNum3DDevices;
      VERIFY(DXINI_GetNum3DDevices(dwDriver, dwNum3DDevices));

      // Set the device for software
      dw3DDevice = dwDriver ? dwNum3DDevices : 0;

      // Change the setting in the INI file
      DXINI_SetSelected3DDevice(dw3DDevice);
    }
  }
  return(TRUE);
}

// ===========================================================================
// End of File
// ===========================================================================

