/*****************************************************************************
 *
 * FILE:        L_BLTWARP.CPP
 * DESCRIPTION: This fine piece of code will stretch a bitmap to a
 * given boundry.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_BltWarp.cpp 35    7/07/99 1:14p Lzou $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_BltWarp.cpp $
 * 
 * 35    7/07/99 1:14p Lzou
 * (1) A blt clip bug is fixed in all warp routines.
 * (2) A (wrong colour) alpha effect bug in 24 bit colour depth is found
 * in function LI_BLTWARP_Hline_8to24 () when DRAW_ALPHA_MMX is on. The
 * plain DRAW_ALPHA works correctly. This bug needs to be fixed.
 * (3) Another blt bug is found in function LI_BLTWARP_Hline_8to16 (),
 * which assumes that each scanline has even number of pixels and always
 * blits two pixels each time without checking how many pixels left to be
 * drawn. This bug also needs to be fixed.
 * 
 * 34    6/16/99 3:07p Fredds
 * Reverted back to 2nd last verion as problem is due to fixed-point
 * roundoff error.
 * 
 * 33    6/16/99 2:33p Fredds
 * Fixed the round-off pixel error in LE_BLTWARP_8to16
 * 
 * 32    6/15/99 12:45p Fredds
 * Fixed irrugular destination width bug.  Good work Li.
 * 
 * 31    6/11/99 12:01p Fredds
 * LE_BLTWARP_24to24 now has MMX support.
 * 
 * 30    6/11/99 11:02a Fredds
 * Added LE_BLTWARP_24to24 for native 24-bit to 24-bit bitmap warping.
 * 
 * 29    6/10/99 5:13p Fredds
 * Added function LE_BLTWARP_16to16 for native 16-bit to 16-bit rotations
 * 
 * 28    6/10/99 11:17a Fredds
 * Added funtion LE_BLTWARP_32to32 for 32-bit textures.
 * 
 * 27    6/08/99 5:48p Fredds
 * Fixed bitmap overdraw bug.  
 * 
 * 26    6/08/99 12:15p Fredds
 * LE_BLTWARP_8to16,24 and 32 SOLID, MASKED and ALPHA BLEND versions all
 * implemented, working and optimized.  Go nuts.
 * 
 * 25    6/07/99 1:31p Fredds
 * LI_HLINE_8to24 with alpha now implemented in MMX.
 * 
 * 24    6/02/99 5:50p Fredds
 * Finished optimizing LI_HLINE_8to24 SOLID and MASKED copy modes.
 * 
 * 23    6/02/99 2:33p Fredds
 * Fixed the drawing of the padded area.  24-bit WARP functions is not in
 * service in this version.
 * 
 * 21    6/02/99 2:25p Fredds
 * Fixed the drawing of pad bytes error.  Good work Alex.
 * 
 * 20    6/01/99 6:16p Fredds
 * Sped-up LI_HLINE_8to32 by only calculating for alpha values between 2
 * and 254.
 * 
 * 19    5/31/99 6:15p Fredds
 * Re-wrote and optimized LI_HLINE_8to32 SOLID copy with mask for classic
 * Pentium (non-MMX).
 * 
 * 18    5/31/99 1:09p Fredds
 * All 16-bit hline code now in optimized assembler.
 * 
 * 17    5/27/99 4:34p Fredds
 * LI_BLTWARP_8to16 optimized and bug free. 
 * 
 * 15    5/26/99 5:19p Fredds
 * Fixed reverse pixel write in LI_HLINE_8to16
 * 
 * 14    5/26/99 1:40p Fredds
 * LI_BLTLINE_8to16 now fully in Pentium assembler.  Optimized for writing
 * 2 pixels at once but not for dual-pipelining.
 * 
 * 13    5/21/99 4:54p Fredds
 * LE_BLTWARP_8to16 is now in assmebly (non-MMX)
 * 
 * 12    5/21/99 2:56p Fredds
 * Removed warnings in C version of LE_BLTWARP_8to16
 * 
 * 11    5/20/99 4:20p Fredds
 * Fixed blue spots in 16-bit routines
 * 
 * 10    5/20/99 2:34p Fredds
 * Fixed the errors from the previous version
 * 
 * 9     5/20/99 1:20p Fredds
 * Optimized LE_BLTWARP_8to32 for MMX and faster alpha blending.
 * 
 * 8     5/14/99 1:27p Fredds
 * LE_BLTWARP_8to32 now does solid copying,masked copying and partial
 * transparency
 * 
 * 7     5/13/99 5:05p Fredds
 * Modified functions to perform different operations based on number of
 * alpha values in the src TAB.
 * 
 * # alpha = 0 --> solid copy
 * # alpha = 1 --> color 0 transparent
 * # alpha > 1 --> color 0 transparent and alpha transparency is applied
 * to all colors in the range of 1 to (# alpha) -1
 * 
 * 6     5/13/99 3:18p Fredds
 * Fixed problem with irregular pitch in 16 and 32-bit mode
 * 
 * 5     5/12/99 4:57p Fredds
 * Modified warp functions to mask color 0.
 * 
 * 4     5/12/99 4:27p Agmsmith
 * Tabs to spaces and master #if statement.
 *
 * 3     5/11/99 12:58p Fredds
 * Now includes LE_BLTWARP_8to24.
 *
 * 2     5/07/99 5:57p Fredds
 * Added LE_BLTWARP_8to16 and 8to32 to stretch and contort a bitmap.
 ****************************************************************************/
#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Internal to this file only
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// D E F I N E S
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DRAW_SOLID 0
#define DRAW_MASK 1
#define DRAW_ALPHA 2
#define DRAW_ALPHA_MMX 3
#define DRAW_SOLID_MMX 4

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// D A T A  S T R U C T U R E S
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct XYPoint
{
   int x,y;
};

struct Edge
{
   int x,tx,ty;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// F U N C T I O N  P R O T O T Y P E S
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_Scan_Edge(Edge *edge, const XYPoint &a, const XYPoint &b, const XYPoint &ta, const XYPoint &tb,UNS16 Dest_Height);

static void LI_BLTWARP_Hline_8to16(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
                            UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op);

static void LI_BLTWARP_Hline_8to32(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
                            UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op);

static void LI_BLTWARP_Hline_8to24(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
                            UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op);

static void LI_BLTWARP_Hline_16to16(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op);

static void LI_BLTWARP_Hline_24to24(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op);

static void LI_BLTWARP_Hline_32to32(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op);

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_8to16
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intent and purposes this is an affine texture mapper.
// NOTE: This function only does SOLID copying.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
//
// REMARKS 
// *  The corners of the source image will always correspond with these 4 points.  i.e: (X0,Y0) will 
//    reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height) and so on.
//
// *  Based on the number of alpha values, it will draw each bitmap differently.
//     Num_Alpha == 0 --> Draw solid
//     Num_Alpha == 1 --> Draw with color 0 as transparent
//     Num_Alpha > 1 --> Draw with color 0 as transparent and color range from 1 to Num_Alpha-1 
//     are drawn with alpha blending.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LE_BLTWARP_8to16(void *Src_Pixel_Pntr,UNS16 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                        LE_BLT_AlphaPaletteEntryRecord *Src_Palette,UNS16 Num_Colors,UNS16 Num_Alpha,
                        void *Dest_Pixel_Pntr,UNS16 Dest_Pitch,UNS16 Dest_Width,UNS16 Dest_Height,
                        INT32 X0,INT32 Y0,INT32 X1,INT32 Y1,INT32 X2,INT32 Y2,INT32 X3,INT32 Y3)
{
   //----------------
   // LOCALS
   //----------------
   register int i;

   UNS8 Op;

   // edge lists
   Edge  fwd[2048];
   Edge  bwd[2048];

   // points to bound to
   XYPoint point[4],tex_point[4];

   //--------------------
   // MAIN CODE
   //--------------------

   // fill in point and texture cords array in counter-clockwise order
   point[0].x=X0; point[0].y=Y0;
   point[1].x=X1; point[1].y=Y1;
   point[2].x=X2; point[2].y=Y2;
   point[3].x=X3; point[3].y=Y3;

   tex_point[0].x=0; tex_point[0].y=0;
   tex_point[1].x=0; tex_point[1].y=Src_Height-1;
   tex_point[2].x=Src_Width-1; tex_point[2].y=Src_Height-1;
   tex_point[3].x=Src_Width-1; tex_point[3].y=0;

   // starting at point 0 iterate thru all points and find the maximum and minimum y value
   int min_y = point[0].y, min_i = 0;
   int max_y = point[0].y, max_i = 0;

   for (i=1; i<4; i++)
   {
      // if the current y point is less than the last recorded minimum, then make this point the new minumum
      if (point[i].y < min_y)
      {
         // save point index
         min_i = i;

         // set new minimum
         min_y = point[min_i].y;
      }

      // if the current y point is greater than the last recorded max, then make this point the new max
      if (point[i].y > max_y)
      {
         // save point index
         max_i = i;

         // set new max
         max_y = point[max_i].y;
      }
   }

   // degenerate case: polygon is flat
   if (min_y == max_y)
      return;

   // scan forward
   i = min_i;
   while (i != max_i)
   {
      int n = (i+1) % 4;
      LI_Scan_Edge(fwd,point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // scan backward
   i = min_i;
   while (i != max_i)
   {
      int n = (i-1+4) % 4;
      LI_Scan_Edge(bwd, point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // set draw mode   
   if (Num_Alpha<=1)
      Op=DRAW_SOLID;   
   else if (Num_Alpha>1)
		Op=DRAW_ALPHA;

   // clip
   int y = (min_y > 0) ? min_y : 0;

   // get screen offset to draw line on
   int offset = y*Dest_Pitch;

   // scan through all y values and fill so long as we are within the polygons and screens space
   while (y <= max_y && y < Dest_Height)
   {
      // draw a scanline from one edge to the other
      if (bwd[y].x < fwd[y].x)
         LI_BLTWARP_Hline_8to16(bwd[y],fwd[y],
         (unsigned char *) Src_Pixel_Pntr,Src_Palette,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);
      else
         LI_BLTWARP_Hline_8to16(fwd[y],bwd[y],
         (unsigned char *) Src_Pixel_Pntr,Src_Palette,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);
      
      // increment current y line
      y++;
       
      // increment destination buffer offset to next line
      offset += Dest_Pitch;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_8to24
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intensive purposes this is an affine texture mapper.
// NOTE: This function only does SOLID copying.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
//
// REMARKS 
// *  The corners of the source image will always correspond with these 4 points.  i.e: (X0,Y0) will 
//    reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height) and so on.
//
// *  Based on the number of alpha values, it will draw each bitmap differently.
//     Num_Alpha == 0 --> Draw solid
//     Num_Alpha == 1 --> Draw with color 0 as transparent
//     Num_Alpha > 1 --> Draw with color 0 as transparent and color range from 1 to Num_Alpha-1 
//     are drawn with alpha blending.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LE_BLTWARP_8to24(void *Src_Pixel_Pntr,UNS16 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                        LE_BLT_AlphaPaletteEntryRecord *Src_Palette,UNS16 Num_Colors,UNS16 Num_Alpha,
                        void *Dest_Pixel_Pntr,UNS16 Dest_Pitch,UNS16 Dest_Width,UNS16 Dest_Height,
                        INT32 X0,INT32 Y0,INT32 X1,INT32 Y1,INT32 X2,INT32 Y2,INT32 X3,INT32 Y3)
{
   //----------------
   // LOCALS
   //----------------
   register int i;

   UNS8 Op;

   // edge lists
   Edge  fwd[2048];
   Edge  bwd[2048];

   // points to bound to
   XYPoint point[4],tex_point[4];

   //--------------------
   // MAIN CODE
   //--------------------
   // fill in point and texture cords array in counter-clockwise order
   point[0].x=X0; point[0].y=Y0;
   point[1].x=X1; point[1].y=Y1;
   point[2].x=X2; point[2].y=Y2;
   point[3].x=X3; point[3].y=Y3;

   tex_point[0].x=0; tex_point[0].y=0;
   tex_point[1].x=0; tex_point[1].y=Src_Height-1;
   tex_point[2].x=Src_Width-1; tex_point[2].y=Src_Height-1;
   tex_point[3].x=Src_Width-1; tex_point[3].y=0;

   // starting at point 0 iterate thru all points and find the maximum and minimum y value
   int min_y = point[0].y, min_i = 0;
   int max_y = point[0].y, max_i = 0;

   for (i=1; i<4; i++)
   {
      // if the current y point is less than the last recorded minimum, then make this point the new minumum
      if (point[i].y < min_y)
      {
         // save point index
         min_i = i;

         // set new minimum
         min_y = point[min_i].y;
      }

      // if the current y point is greater than the last recorded max, then make this point the new max
      if (point[i].y > max_y)
      {
         // save point index
         max_i = i;

         // set new max
         max_y = point[max_i].y;
      }
   }

   // degenerate case: polygon is flat
   if (min_y == max_y)
      return;

   // scan forward
   i = min_i;
   while (i != max_i)
   {
      int n = (i+1) % 4;
      LI_Scan_Edge(fwd,point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // scan backward
   i = min_i;
   while (i != max_i)
   {
      int n = (i-1+4) % 4;
      LI_Scan_Edge(bwd, point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // clip
   int y = (min_y > 0) ? min_y : 0;

   // set draw mode   
   if (Num_Alpha==0)
      Op=DRAW_SOLID;
   else if (Num_Alpha==1)
      Op=DRAW_MASK;
   else if (Num_Alpha>1)
	{
      if (LE_MMX_Is_Available())
			Op=DRAW_ALPHA_MMX;
      else
			Op=DRAW_ALPHA;
	}


   // get logical screen offset to draw line on
   UNS32 offset = y * Dest_Pitch;

   // scan through all y values and fill so long as we are within the polygons and screens space
   while (y <= max_y && y < Dest_Height)
   {
      // draw a scanline from one edge to the other
      if (bwd[y].x < fwd[y].x)
         LI_BLTWARP_Hline_8to24(bwd[y],fwd[y],
         (unsigned char *) Src_Pixel_Pntr,Src_Palette,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8 *) Dest_Pixel_Pntr+offset),Dest_Width,Op);
      else
         LI_BLTWARP_Hline_8to24(fwd[y],bwd[y],
         (unsigned char *) Src_Pixel_Pntr,Src_Palette,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8 *) Dest_Pixel_Pntr+offset),Dest_Width,Op);

      // increment current logical y line
      y++;

      // increment destination buffer offset to next line
      offset += Dest_Pitch;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_8to32
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intensive purposes this is an affine texture mapper.
// NOTE: This function only does SOLID copying.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
//
// REMARKS 
// *  The corners of the source image will always correspond with these 4 points.  i.e: (X0,Y0) will 
//    reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height) and so on.
//
// *  Based on the number of alpha values, it will draw each bitmap differently.
//     Num_Alpha == 0 --> Draw solid
//     Num_Alpha == 1 --> Draw with color 0 as transparent
//     Num_Alpha > 1 --> Draw with color 0 as transparent and color range from 1 to Num_Alpha-1 
//     are drawn with alpha blending.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LE_BLTWARP_8to32(void *Src_Pixel_Pntr,UNS16 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                        LE_BLT_AlphaPaletteEntryRecord *Src_Palette,UNS16 Num_Colors,UNS16 Num_Alpha,
                        void *Dest_Pixel_Pntr,UNS16 Dest_Pitch,UNS16 Dest_Width,UNS16 Dest_Height,
                        INT32 X0,INT32 Y0,INT32 X1,INT32 Y1,INT32 X2,INT32 Y2,INT32 X3,INT32 Y3)
{
   //----------------
   // LOCALS
   //----------------
   register int i;

   UNS8 Op;

   // edge lists
   Edge  fwd[2048];
   Edge  bwd[2048];

   // points to bound to
   XYPoint point[4],tex_point[4];

   //--------------------
   // MAIN CODE
   //--------------------
   // fill in point and texture cords array in counter-clockwise order
   point[0].x=X0; point[0].y=Y0;
   point[1].x=X1; point[1].y=Y1;
   point[2].x=X2; point[2].y=Y2;
   point[3].x=X3; point[3].y=Y3;

   tex_point[0].x=0; tex_point[0].y=0;
   tex_point[1].x=0; tex_point[1].y=Src_Height-1;
   tex_point[2].x=Src_Width-1; tex_point[2].y=Src_Height-1;
   tex_point[3].x=Src_Width-1; tex_point[3].y=0;

   // starting at point 0 iterate thru all points and find the maximum and minimum y value
   int min_y = point[0].y, min_i = 0;
   int max_y = point[0].y, max_i = 0;

   for (i=1; i<4; i++)
   {
      // if the current y point is less than the last recorded minimum, then make this point the new minumum
      if (point[i].y < min_y)
      {
         // save point index
         min_i = i;

         // set new minimum
         min_y = point[min_i].y;
      }

      // if the current y point is greater than the last recorded max, then make this point the new max
      if (point[i].y > max_y)
      {
         // save point index
         max_i = i;

         // set new max
         max_y = point[max_i].y;
      }
   }

   // degenerate case: polygon is flat
   if (min_y == max_y)
      return;

   // scan forward
   i = min_i;
   while (i != max_i)
   {
      int n = (i+1) % 4;
      LI_Scan_Edge(fwd,point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // scan backward
   i = min_i;
   while (i != max_i)
   {
      int n = (i-1+4) % 4;
      LI_Scan_Edge(bwd, point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // clip
   int y = (min_y > 0) ? min_y : 0;

   // set draw mode   
   if (Num_Alpha<=1)
      Op=DRAW_SOLID;
   else if (Num_Alpha>1)
   {
      if (LE_MMX_Is_Available())
        Op=DRAW_ALPHA_MMX;
       else
        Op=DRAW_ALPHA;
   }
   
   // get screen offset to draw line on
   UNS32 offset = y*Dest_Pitch;

   // scan through all y values and fill so long as we are within the polygons and screens space
   while (y <= max_y && y < Dest_Height)
   {
      // draw a scanline from one edge to the other
      if (bwd[y].x < fwd[y].x)
         LI_BLTWARP_Hline_8to32(bwd[y],fwd[y],
         (unsigned char *) Src_Pixel_Pntr,Src_Palette,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);
      else
         LI_BLTWARP_Hline_8to32(fwd[y],bwd[y],
         (unsigned char *) Src_Pixel_Pntr,Src_Palette,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);

      // increment current y line
      y++;

      // increment destination buffer offset to next line
      offset += Dest_Pitch;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_16to16
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LE_BLTWARP_16to16(void *Src_Pixel_Pntr,UNS16 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
 void *Dest_Pixel_Pntr,UNS16 Dest_Pitch,UNS16 Dest_Width,UNS16 Dest_Height,
 INT32 X0,INT32 Y0, INT32 X1,INT32 Y1, INT32 X2,INT32 Y2, INT32 X3,INT32 Y3)
{
   //----------------
   // LOCALS
   //----------------
   register int i;

   UNS8 Op;

   // edge lists
   Edge  fwd[2048];
   Edge  bwd[2048];

   // points to bound to
   XYPoint point[4],tex_point[4];

   //--------------------
   // MAIN CODE
   //--------------------
   // fill in point and texture cords array in counter-clockwise order
   point[0].x=X0; point[0].y=Y0;
   point[1].x=X1; point[1].y=Y1;
   point[2].x=X2; point[2].y=Y2;
   point[3].x=X3; point[3].y=Y3;

   tex_point[0].x=0; tex_point[0].y=0;
   tex_point[1].x=0; tex_point[1].y=Src_Height-1;
   tex_point[2].x=Src_Width-1; tex_point[2].y=Src_Height-1;
   tex_point[3].x=Src_Width-1; tex_point[3].y=0;

   // starting at point 0 iterate thru all points and find the maximum and minimum y value
   int min_y = point[0].y, min_i = 0;
   int max_y = point[0].y, max_i = 0;

   for (i=1; i<4; i++)
   {
      // if the current y point is less than the last recorded minimum, then make this point the new minumum
      if (point[i].y < min_y)
      {
         // save point index
         min_i = i;

         // set new minimum
         min_y = point[min_i].y;
      }

      // if the current y point is greater than the last recorded max, then make this point the new max
      if (point[i].y > max_y)
      {
         // save point index
         max_i = i;

         // set new max
         max_y = point[max_i].y;
      }
   }

   // degenerate case: polygon is flat
   if (min_y == max_y)
      return;

   // scan forward
   i = min_i;
   while (i != max_i)
   {
      int n = (i+1) % 4;
      LI_Scan_Edge(fwd,point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // scan backward
   i = min_i;
   while (i != max_i)
   {
      int n = (i-1+4) % 4;
      LI_Scan_Edge(bwd, point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // clip
   int y = (min_y > 0) ? min_y : 0;

   // set draw mode   
   if (LE_MMX_Is_Available())
		Op=DRAW_SOLID_MMX;
	else
		Op=DRAW_SOLID;
   
   // get screen offset to draw line on
   UNS32 offset = y*Dest_Pitch;

   // scan through all y values and fill so long as we are within the polygons and screens space
   while (y <= max_y && y < Dest_Height)
   {
      // draw a scanline from one edge to the other
      if (bwd[y].x < fwd[y].x)
         LI_BLTWARP_Hline_16to16(bwd[y],fwd[y],
         (unsigned char *) Src_Pixel_Pntr,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);
      else
         LI_BLTWARP_Hline_16to16(fwd[y],bwd[y],
         (unsigned char *) Src_Pixel_Pntr,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);

      // increment current y line
      y++;

      // increment destination buffer offset to next line
      offset += Dest_Pitch;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_24to24
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LE_BLTWARP_24to24(void *Src_Pixel_Pntr,UNS16 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
 void *Dest_Pixel_Pntr,UNS16 Dest_Pitch,UNS16 Dest_Width,UNS16 Dest_Height,
 INT32 X0,INT32 Y0, INT32 X1,INT32 Y1, INT32 X2,INT32 Y2, INT32 X3,INT32 Y3)
{
   //----------------
   // LOCALS
   //----------------
   register int i;

   UNS8 Op;

   // edge lists
   Edge  fwd[2048];
   Edge  bwd[2048];

   // points to bound to
   XYPoint point[4],tex_point[4];

   //--------------------
   // MAIN CODE
   //--------------------
   // fill in point and texture cords array in counter-clockwise order
   point[0].x=X0; point[0].y=Y0;
   point[1].x=X1; point[1].y=Y1;
   point[2].x=X2; point[2].y=Y2;
   point[3].x=X3; point[3].y=Y3;

   tex_point[0].x=0; tex_point[0].y=0;
   tex_point[1].x=0; tex_point[1].y=Src_Height-1;
   tex_point[2].x=Src_Width-1; tex_point[2].y=Src_Height-1;
   tex_point[3].x=Src_Width-1; tex_point[3].y=0;

   // starting at point 0 iterate thru all points and find the maximum and minimum y value
   int min_y = point[0].y, min_i = 0;
   int max_y = point[0].y, max_i = 0;

   for (i=1; i<4; i++)
   {
      // if the current y point is less than the last recorded minimum, then make this point the new minumum
      if (point[i].y < min_y)
      {
         // save point index
         min_i = i;

         // set new minimum
         min_y = point[min_i].y;
      }

      // if the current y point is greater than the last recorded max, then make this point the new max
      if (point[i].y > max_y)
      {
         // save point index
         max_i = i;

         // set new max
         max_y = point[max_i].y;
      }
   }

   // degenerate case: polygon is flat
   if (min_y == max_y)
      return;

   // scan forward
   i = min_i;
   while (i != max_i)
   {
      int n = (i+1) % 4;
      LI_Scan_Edge(fwd,point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // scan backward
   i = min_i;
   while (i != max_i)
   {
      int n = (i-1+4) % 4;
      LI_Scan_Edge(bwd, point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // clip
   int y = (min_y > 0) ? min_y : 0;

   // set draw mode   
   if (LE_MMX_Is_Available())
		Op=DRAW_SOLID_MMX;
	else
		Op=DRAW_SOLID;
   
   // get screen offset to draw line on
   UNS32 offset = y*Dest_Pitch;

   // scan through all y values and fill so long as we are within the polygons and screens space
   while (y <= max_y && y < Dest_Height)
   {
      // draw a scanline from one edge to the other
      if (bwd[y].x < fwd[y].x)
         LI_BLTWARP_Hline_24to24(bwd[y],fwd[y],
         (unsigned char *) Src_Pixel_Pntr,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);
      else
         LI_BLTWARP_Hline_24to24(fwd[y],bwd[y],
         (unsigned char *) Src_Pixel_Pntr,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);

      // increment current y line
      y++;

      // increment destination buffer offset to next line
      offset += Dest_Pitch;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_32to32
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LE_BLTWARP_32to32(void *Src_Pixel_Pntr,UNS16 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
 void *Dest_Pixel_Pntr,UNS16 Dest_Pitch,UNS16 Dest_Width,UNS16 Dest_Height,
 INT32 X0,INT32 Y0, INT32 X1,INT32 Y1, INT32 X2,INT32 Y2, INT32 X3,INT32 Y3)
{
   //----------------
   // LOCALS
   //----------------
   register int i;

   UNS8 Op;

   // edge lists
   Edge  fwd[2048];
   Edge  bwd[2048];

   // points to bound to
   XYPoint point[4],tex_point[4];

   //--------------------
   // MAIN CODE
   //--------------------
   // fill in point and texture cords array in counter-clockwise order
   point[0].x=X0; point[0].y=Y0;
   point[1].x=X1; point[1].y=Y1;
   point[2].x=X2; point[2].y=Y2;
   point[3].x=X3; point[3].y=Y3;

   tex_point[0].x=0; tex_point[0].y=0;
   tex_point[1].x=0; tex_point[1].y=Src_Height-1;
   tex_point[2].x=Src_Width-1; tex_point[2].y=Src_Height-1;
   tex_point[3].x=Src_Width-1; tex_point[3].y=0;

   // starting at point 0 iterate thru all points and find the maximum and minimum y value
   int min_y = point[0].y, min_i = 0;
   int max_y = point[0].y, max_i = 0;

   for (i=1; i<4; i++)
   {
      // if the current y point is less than the last recorded minimum, then make this point the new minumum
      if (point[i].y < min_y)
      {
         // save point index
         min_i = i;

         // set new minimum
         min_y = point[min_i].y;
      }

      // if the current y point is greater than the last recorded max, then make this point the new max
      if (point[i].y > max_y)
      {
         // save point index
         max_i = i;

         // set new max
         max_y = point[max_i].y;
      }
   }

   // degenerate case: polygon is flat
   if (min_y == max_y)
      return;

   // scan forward
   i = min_i;
   while (i != max_i)
   {
      int n = (i+1) % 4;
      LI_Scan_Edge(fwd,point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // scan backward
   i = min_i;
   while (i != max_i)
   {
      int n = (i-1+4) % 4;
      LI_Scan_Edge(bwd, point[i], point[n], tex_point[i], tex_point[n],Dest_Height);
      i = n;
   }

   // clip
   int y = (min_y > 0) ? min_y : 0;

   // set draw mode   
   if (LE_MMX_Is_Available())
		Op=DRAW_SOLID_MMX;
	else
		Op=DRAW_SOLID;
   
   // get screen offset to draw line on
   UNS32 offset = y*Dest_Pitch;

   // scan through all y values and fill so long as we are within the polygons and screens space
   while (y <= max_y && y < Dest_Height)
   {
      // draw a scanline from one edge to the other
      if (bwd[y].x < fwd[y].x)
         LI_BLTWARP_Hline_32to32(bwd[y],fwd[y],
         (unsigned char *) Src_Pixel_Pntr,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);
      else
         LI_BLTWARP_Hline_32to32(fwd[y],bwd[y],
         (unsigned char *) Src_Pixel_Pntr,(UNS32) Src_Pitch,Src_Width,Src_Height,
         ((UNS8*) Dest_Pixel_Pntr)+offset,Dest_Width,Op);

      // increment current y line
      y++;

      // increment destination buffer offset to next line
      offset += Dest_Pitch;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL SUPPORT FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_Scan_Edge
//
// DESCRIPTION: Scans an edge and fills in an edge list for the rasterizer.  This is a generic
// function used by all the warp functions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_Scan_Edge(Edge *edge, const XYPoint &a, const XYPoint &b, const XYPoint &ta, const XYPoint &tb,UNS16 Dest_Height)
{
   // clip
   int min_y = (a.y > 0) ? a.y : 0;
   int max_y = (b.y < Dest_Height) ? b.y : Dest_Height;

   // ? degenerate case: bottom is higher than top
   if (max_y <= min_y)
      return;

   // initialize
   int  dx = (( b.x -  a.x) << 16) / (b.y - a.y);
   int dtx = ((tb.x - ta.x) << 16) / (b.y - a.y);
   int dty = ((tb.y - ta.y) << 16) / (b.y - a.y);

   int  x = ( a.x << 16);
   int tx = (ta.x << 16);
   int ty = (ta.y << 16);

   if (a.y < 0)
   {
      x  -= a.y *  dx;
      tx -= a.y * dtx;
      ty -= a.y * dty;
   }

   // fill edge list by interpolating from min_y to max_y
   do
   {
      edge[min_y].x=(x >> 16); 
		x +=dx;

      edge[min_y].tx=tx; 
		tx += dtx;

      edge[min_y].ty=ty; 
		ty += dty;
   } while (++min_y <= max_y);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_BLTWARP_Hline_8to16
//
// DESCRIPTION: An internal function that draws a textured horizontal line from one edge to
// another.
// NOTE: This function is called internally by LE_BLTWARP_8to16
//
// ARGUMENTS:
// min, max -- The edge span to draw between.
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pallete -- The 8-bit pallete for the source
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Width -- The width of the destination surface.
//
// REMARKS 
// *  Based on the number on the operation value, the line will be drawn differnly
//     Op == 0 --> Draw solid with color 0 as transparent
//     Op == 2 --> Draw with color 0 as transparent and color range from 1 to Num_Alpha-1 
//     are drawn with alpha blending.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_BLTWARP_Hline_8to16(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
                            UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op)
{
   //----------------
   // LOCALS
   //----------------
   // for color table lookup   
	UNS32 Source,Indicies;
	UNS32 Inverse_Alpha,Dest_Pixel_Value;

	// made local so that they are in L1 cache
	UNS32 Red_Mask,Green_Mask,Blue_Mask;

   // clip
   int min_x = (min.x > 0) ? min.x : 0;
   int max_x = (max.x < Dest_Width) ? max.x : Dest_Width;

   if (max_x <= min_x)
      return;

   // initialize 
   int dtx = (max.tx - min.tx) / (max.x - min.x);
   int dty = (max.ty - min.ty) / (max.x - min.x);
   int tx = min.tx;
   int ty = min.ty;


   // if left edge is clipped adjust
   if (min.x < 0)
   {
      tx -= min.x * dtx;
      ty -= min.x * dty;
   }

   // start position
   UNS16 *d = (UNS16*) Dest_Pixel_Pntr + min_x;

   // end position
   UNS16 *e = (UNS16*) Dest_Pixel_Pntr + max_x;

	// set up masks locally so that they are in the data cache
	Red_Mask=LE_BLT_dwRedMask;
	Green_Mask=LE_BLT_dwGreenMask;
	Blue_Mask=LE_BLT_dwBlueMask;


   //--------------------
   // MAIN CODE
   //--------------------

   switch(Op)
   {
   //-----------------------------------------
   // Draws texture as a solid or with color 0
	// masking.  No alpha blending.
   //-----------------------------------------
   case DRAW_SOLID:
		__asm
		{
			// point to the first pixel in this line
			mov	edi,d

DRAW_SOLID_Main_Loop:
			//--------------------------------------------------------------
			// get first 8 bit color index
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			mov	ebx,tx						
			mov	esi,Src_Pixel_Pntr		

			shr	ebx,16						
			mov	ecx,ty						

			mov	eax,Src_Pitch				
			add	esi,ebx						

			shr	ecx,16						
			mul	ecx							
			
			add	esi,eax						
			xor	ebx,ebx						
			
			nop									
			nop									
			
			// BL <- first 8-bit index      		
			mov	bl,byte ptr [esi]			

			//--------------------------------------------------------------------------
			// get second 8 bit color index
			// index=Src_Pixel_Pntr[((tx+dtx) >> 16) + (((ty+dty) >> 16) * Src_Pitch)];
			//--------------------------------------------------------------------------
			push	ebx			 
			mov	ebx,tx		

			mov	ecx,ty					 
			mov	esi,Src_Pixel_Pntr	

			add	ebx,dtx		 
			add	ecx,dty		

			mov	tx,ebx		 
			mov	ty,ecx		
			
			shr	ebx,16			 
			mov	eax,Src_Pitch	
			
			shr	ecx,16		
			add	esi,ebx		

			mul	ecx			

			// add offset to ESI 
			add	esi,eax		
			nop					
			
			nop					
			// restore first index value (which is in BL)
			pop	ebx			

			// BH <- second 8-bit index      					
			mov	bh,byte ptr [esi]		

			or		ebx,ebx
			jz		DRAW_SOLID_Next_Iteration

			//----------------------------------------------------------
			// Get 16-bit color values 
			//
			// BH <- Index of pixel 1
			// BL <- Index of pixel 2
			//----------------------------------------------------------
			mov	edx,ebx

			// get index of the first pixel
			and	edx,0x000000ff
			
			// check if it's zero
			or		edx,edx
			jz		DRAW_SOLID_Skip_First_Pixel

			// point ESI to the source palette
			mov	esi,Src_Palette
			shl	edx,3
			add	esi,edx
			
			// avoid AGI on ESI
			nop	// U
			nop	// V
			nop	// U

			// get 16-bit color into ECX (to avoid prefix penalty)
			mov	ecx,[esi]
			// we just want the low 16-bits
			and	ecx,0x0000ffff

DRAW_SOLID_Skip_First_Pixel:
			mov	edx,ebx
			and	edx,0x0000ff00
			or		edx,edx
			jz		DRAW_SOLID_Draw_Texels		

			// point to source palette lookup table
			mov	esi,Src_Palette
			shr	edx,5
			add	esi,edx

			// shift ECX up so that we can write the low-order word
			shl	ecx,16
			mov	edx,[esi]
			and	edx,0x0000ffff
			or		ecx,edx

			//----------------------------------------------------------
			// Draw texels to destination
			//
			// BH <- texel 1
			// BL <- texel 2
			//----------------------------------------------------------
DRAW_SOLID_Draw_Texels:
			// check if low-index (second pixel) is 0
			or		bl,bl
			jz		DRAW_SOLID_Write_High_Word

			// check if high-index (first pixel) is 0
			or		bh,bh
			jz		DRAW_SOLID_Write_Low_Word

			// neither are 0 so write both
			jmp	DRAW_SOLID_Write_Both
	
DRAW_SOLID_Write_High_Word:
			mov	[edi+2],cx
			jmp	DRAW_SOLID_Next_Iteration

DRAW_SOLID_Write_Low_Word:
			mov	[edi],cx
			jmp	DRAW_SOLID_Next_Iteration
	
DRAW_SOLID_Write_Both:
			rol	ecx,16
			mov	[edi],ecx
			
DRAW_SOLID_Next_Iteration:
			//------------------------------------
			// advance to next texel and pixel
			//------------------------------------
			// get tx and ty
			mov	ebx,tx		
			mov	ecx,ty

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx

			// advance screen pointer by 4 bytes
			// (cause we processed 2 16-bit pixels
			add	edi,4

			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_Main_Loop			
		}
   break;

   //---------------------------------------------------
   // Draws with partial alpha and color 0 masking.
	// 
	// Hand optimized Pentium assembler.  Optimizing was
	// hard because each subsequent line is dependent
	// in one way or another on the previous line(s).
	//
	// Various NOP's were thrown in to synchronize 
   // intructions going through the UV pipes.
	//
	// NOTE: HIGHLY OPTIMIZED BY HAND....IF YOU CAN'T
	// UNDERSTAND THIS CODE, THEN DON'T GO F**KING WITH 
	// IT.  Thanks :)
	//---------------------------------------------------
   case DRAW_ALPHA:
		__asm
		{
			// point to the first pixel in this line
			mov	edi,d

Main_Loop:
			//--------------------------------------------------------------
			// get first 8 bit color index
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			mov	ebx,tx						
			mov	esi,Src_Pixel_Pntr		

			shr	ebx,16						
			mov	ecx,ty						

			mov	eax,Src_Pitch				
			add	esi,ebx						

			shr	ecx,16						
			mul	ecx							
			
			add	esi,eax						
			xor	ebx,ebx						
			
			nop									
			nop									
			
			// BL <- first 8-bit index      		
			mov	bl,byte ptr [esi]			

			//--------------------------------------------------------------------------
			// get second 8 bit color index
			// index=Src_Pixel_Pntr[((tx+dtx) >> 16) + (((ty+dty) >> 16) * Src_Pitch)];
			//--------------------------------------------------------------------------
			push	ebx			 
			mov	ebx,tx		

			mov	ecx,ty					 
			mov	esi,Src_Pixel_Pntr	

			add	ebx,dtx		 
			add	ecx,dty		

			mov	tx,ebx		 
			mov	ty,ecx		
			
			shr	ebx,16			 
			mov	eax,Src_Pitch	
			
			shr	ecx,16		
			add	esi,ebx		

			mul	ecx			

			// add offset to ESI 
			add	esi,eax		
			nop					
			
			nop					
			// restore first index value (which is in BL)
			pop	ebx			

			// BH <- second 8-bit index      		
			mov	bh,byte ptr [esi]		
		
			// compare both pixels and skip em if they are both black
			or		bx,bx						
			jz		Next_Iteration

			//---------------------------------------------------------------
			// read 2 pixels from the destination (stored as RGB RGB) and
			// store it for later use.
			// 
			//
			// NOTE: This will cause a significant pipeline stall when
			// reading from a video memory surface, especially the front
			// buffer but not so much from the back buffer.  This is due
			// to the fact that video memory is not cached on most systems.  
			// Some motherboards support cacheable video memory and so it 
			// will be faster on those machines.
			//---------------------------------------------------------------
			mov	eax,[edi]

			// preserve EBX (index values forfor later use)
			// BL -> First index value
			// BH -> Second index value
			mov	Indicies,ebx

			// preserve read value
			mov	Dest_Pixel_Value,eax
			
			/////////////////////////////////////////////////////////////////
			// PROCESS FIRST PIXEL 
			//
			// BL -> Index value for lookup table conversions
			/////////////////////////////////////////////////////////////////
			// ECX - destination pixels
			xor	ecx,ecx

			or		bl,bl
			jz		Skip_First_Pixel

			//--------------------------------------------------------
			// determine Inverse_Alpha
			// Inverse_Alpha=(UNS32) 255-Src_Palette[index].dwAlpha;
			//--------------------------------------------------------
			// clear BH (which stores the second index cause we want the first one)
			xor	bh,bh

			// point ESI to the source palette
			mov	esi,Src_Palette
			
			// calculate the offset (while avoiding AGI on ESI)
			shl	ebx,3		
			mov	ecx,255
			
			// add offset into table to ESI
			add	esi,ebx									
			nop								
			
			nop
			nop
			
			// get alpha value and determine the inverse value
			mov	eax,[esi].dwAlpha
			nop								
			
			sub	ecx,eax					
			mov	Inverse_Alpha,ecx		
			
			//--------------------------------------------------------
			// get 16-bit color         
			// Source = (UNS32) Src_Palette[index].lowDWORD.dwColour;
			//--------------------------------------------------------
			// get source pixel
			mov	ebx,[esi]

			// if the inverse is 0 then just write the source value and 
			// skip to the next pixel.
			or		ecx,ecx
			jnz	Process_Pixel_1

			// straight combine pixel 1 with the destination
			or		ecx,ebx
			jmp	Skip_First_Pixel

Process_Pixel_1:
			// prepare CX (which will be the destination pixel to write)
			xor	ecx,ecx

			// store source pixel for later use
			mov	Source,ebx
						
			// get the destination pixel value
			mov	ebx,Dest_Pixel_Value

			//------------------------
			// PROCESS RED COMPONENT
			//------------------------
			// EAX <- (Dest_Red * Inverse_Alpha)/255
			mov	eax,ebx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			mov	esi,Red_Mask	// UV -> U-pipe
			and	eax,esi			// UV -> V-pipe
			
			mul	Inverse_Alpha	// NP
			
			nop						// UV -> U-pipe
			nop						// UV -> V-pipe
			
			shr	eax,8				// PU -> U-pipe
			mov	edx,Source		// UV -> V-pipe
			
			// EAX <- destination RED component
			// EDX <- source RED component
			and	eax,esi			// UV -> U-pipe										
			and	edx,esi			// UV -> V-pipe
			
			// add destination RED and source RED
			add	eax,edx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			// mask the red component
			and	eax,esi			// UV -> U-pipe
			nop						// UV -> V-pipe	
			
			// combine with ECX (final pixel value)
			or		ecx,eax			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			//------------------------
			// PROCESS GREEN COMPONENT
			//------------------------
			// EAX <- (Dest_Green * Inverse_Alpha)/255
			mov	eax,ebx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			mov	esi,Green_Mask	// UV -> U-pipe
			and	eax,esi			// UV -> V-pipe
			
			mul	Inverse_Alpha	// NP
			
			nop						// UV -> U-pipe
			nop						// UV -> V-pipe
			
			shr	eax,8				// PU -> U-pipe
			mov	edx,Source		// UV -> V-pipe
			
			// EAX <- destination GREEN component
			// EDX <- source GREEN component
			and	eax,esi			// UV -> U-pipe										
			and	edx,esi			// UV -> V-pipe
			
			// add destination GREEN and source GREEN
			add	eax,edx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			// mask the red component
			and	eax,esi			// UV -> U-pipe
			nop						// UV -> V-pipe	
			
			// combine with ECX (final pixel value)
			or		ecx,eax			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			//------------------------
			// PROCESS BLUE COMPONENT
			//------------------------
			// EAX <- (Dest_Blue * Inverse_Alpha)/255
			mov	eax,ebx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			mov	esi,Blue_Mask	// UV -> U-pipe
			and	eax,esi			// UV -> V-pipe
			
			mul	Inverse_Alpha	// NP
			
			nop						// UV -> U-pipe
			nop						// UV -> V-pipe
			
			shr	eax,8				// PU -> U-pipe
			mov	edx,Source		// UV -> V-pipe
			
			// EAX <- destination BLUE component
			// EDX <- source BLUE component
			and	eax,esi			// UV -> U-pipe										
			and	edx,esi			// UV -> V-pipe
			
			// add destination BLUE and source BLUE
			add	eax,edx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			// mask the red component
			and	eax,esi			// UV -> U-pipe
			nop						// UV -> V-pipe	
			
			// combine with ECX (final pixel value)
			or		ecx,eax			// UV -> U-pipe
			nop						// UV -> V-pipe
			
Skip_First_Pixel:
			/////////////////////////////////////////////////////////////////
			// PROCESS SEOND PIXEL 
			//
			// BH -> Index value for lookup table conversions
			/////////////////////////////////////////////////////////////////		
			// restore EBX (which holds the index values)
			mov	ebx,Indicies

			// check if the index is 0
			or		bh,bh
			jz		Skip_Second_Pixel

			//--------------------------------------------------------
			// determine Inverse_Alpha
			// Inverse_Alpha=(UNS32) 255-Src_Palette[index].dwAlpha;
			//--------------------------------------------------------
			// mov BH to BL
			shr	ebx,8

			// point ESI to the source palette
			mov	esi,Src_Palette

			// preserve ECX (which holds the destination pixel)
			push	ecx
			
			// calculate the offset (while avoiding AGI on ESI)
			shl	ebx,3			
			mov	ecx,255		
			
			// add offset into table to ESI
			add	esi,ebx									
			nop								
			
			nop
			nop
			
			// get alpha value and determine the inverse value
			mov	eax,[esi].dwAlpha
			nop								
			
			// 255-Alpha
			sub	ecx,eax					
			mov	Inverse_Alpha,ecx		

			// restore ECX (which is the destination pixel to write)
			pop	ecx
			
			//--------------------------------------------------------
			// get 16-bit color         
			// Source = (UNS32) Src_Palette[index].lowDWORD.dwColour;
			//--------------------------------------------------------
			// shift the low-word up so that we can write to the low-word
			shl	ecx,16		

			// get source pixel
			mov	ebx,[esi]

			// if inverse alpha is 0 then skip the calculations
			mov	eax,Inverse_Alpha
			or		eax,eax
			jnz	Process_Pixel_2

			// just OR the source pixel with the destination
			or		ecx,ebx
			jmp	Skip_Second_Pixel


Process_Pixel_2:
			// store source pixel
			mov	Source,ebx
			
			// get the destination pixel value
			mov	ebx,Dest_Pixel_Value			

			// shift down the high word to get the second pixel
			shr	ebx,16

			//------------------------
			// PROCESS RED COMPONENT
			//------------------------
			// EAX <- (Dest_Red * Inverse_Alpha)/255
			mov	eax,ebx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			mov	esi,Red_Mask	// UV -> U-pipe
			and	eax,esi			// UV -> V-pipe
			
			mul	Inverse_Alpha	// NP
			
			nop						// UV -> U-pipe
			nop						// UV -> V-pipe
			
			shr	eax,8				// PU -> U-pipe
			mov	edx,Source		// UV -> V-pipe
			
			// EAX <- destination RED component
			// EDX <- source RED component
			and	eax,esi			// UV -> U-pipe										
			and	edx,esi			// UV -> V-pipe
			
			// add destination RED and source RED
			add	eax,edx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			// mask the red component
			and	eax,esi			// UV -> U-pipe
			nop						// UV -> V-pipe	
			
			// combine with ECX (final pixel value)
			or		ecx,eax			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			//------------------------
			// PROCESS GREEN COMPONENT
			//------------------------
			// EAX <- (Dest_Green * Inverse_Alpha)/255
			mov	eax,ebx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			mov	esi,Green_Mask	// UV -> U-pipe
			and	eax,esi			// UV -> V-pipe
			
			mul	Inverse_Alpha	// NP
			
			nop						// UV -> U-pipe
			nop						// UV -> V-pipe
			
			shr	eax,8				// PU -> U-pipe
			mov	edx,Source		// UV -> V-pipe
			
			// EAX <- destination GREEN component
			// EDX <- source GREEN component
			and	eax,esi			// UV -> U-pipe										
			and	edx,esi			// UV -> V-pipe
			
			// add destination GREEN and source GREEN
			add	eax,edx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			// mask the red component
			and	eax,esi			// UV -> U-pipe
			nop						// UV -> V-pipe	
			
			// combine with ECX (final pixel value)
			or		ecx,eax			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			//------------------------
			// PROCESS BLUE COMPONENT
			//------------------------
			// EAX <- (Dest_Blue * Inverse_Alpha)/255
			mov	eax,ebx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			mov	esi,Blue_Mask	// UV -> U-pipe
			and	eax,esi			// UV -> V-pipe
			
			mul	Inverse_Alpha	// NP
			
			nop						// UV -> U-pipe
			nop						// UV -> V-pipe
			
			shr	eax,8				// PU -> U-pipe
			mov	edx,Source		// UV -> V-pipe
			
			// EAX <- destination BLUE component
			// EDX <- source BLUE component
			and	eax,esi			// UV -> U-pipe										
			and	edx,esi			// UV -> V-pipe
			
			// add destination BLUE and source BLUE
			add	eax,edx			// UV -> U-pipe
			nop						// UV -> V-pipe
			
			// mask the red component
			and	eax,esi			// UV -> U-pipe
			nop						// UV -> V-pipe	
			
			// combine with ECX (final pixel value)
			or		ecx,eax			// UV -> U-pipe
			nop						// UV -> V-pipe


Skip_Second_Pixel:
			// get the indices
			mov	ebx,Indicies

			// check if low-index (second pixel) is 0
			or		bl,bl
			jz		Write_High_Word

			// check if high-index (first pixel) is 0
			or		bh,bh
			jz		Write_Low_Word

			// neither are 0 so write both
			jmp	Write_Both

Write_High_Word:
			mov	[edi+2],cx
			jmp	Next_Iteration

Write_Low_Word:
			mov	[edi],cx
			jmp	Next_Iteration
	
Write_Both:
			rol	ecx,16
			mov	[edi],ecx

Next_Iteration:			
			// get tx and ty
			mov	ebx,tx		
			mov	ecx,ty

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx

			// advance screen pointer by 4 bytes
			add	edi,4

			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		Main_Loop
		}
	break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_BLTWARP_Hline_8to24
//
// DESCRIPTION: An internal function that draws a textured horizontal line from one edge to
// another.
// NOTE: This function is called internally by LE_BLTWARP_8to24
//
// ARGUMENTS:
// min, max -- The edge span to draw between.
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pallete -- The 8-bit pallete for the source
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Width -- The width of the destination surface.
//
// REMARKS 
// *  Based on the number on the operation value, the line will be drawn differnly
//     Op == 0 --> Draw solid
//     Op == 1 --> Draw with color 0 as transparent
//     Op == 2 --> Draw with color 0 as transparent and color range from 1 to Num_Alpha-1 
//     are drawn with alpha blending.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_BLTWARP_Hline_8to24(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
                            UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op)
{
   //----------------
   // LOCALS
   //----------------
   // for color table lookup
   DWORD index;
	UNS32 Inverse_Alpha,Source_RGB;   
   UNS32 Texel1;

	// texel and pixel stream arrays
	UNS8 Texel_Stream_Array[16]; 
	UNS32 *Texel_Stream = (UNS32 *) &Texel_Stream_Array[0];

	UNS8 Pixel_Stream_Array[12];
	UNS32 *Pixel_Stream = (UNS32 *) &Pixel_Stream_Array[0];

	// for fast video mem writes
	UNS8 Burst_Stream_Array[12];
	UNS32 *Burst_Stream = (UNS32 *) &Burst_Stream_Array[0];

   // clip to physical screen
   int min_x = (min.x > 0) ? min.x : 0;
   int max_x = (max.x < Dest_Width) ? max.x : Dest_Width;

   if (max_x <= min_x)
      return;

   // initialize
   int dtx = (max.tx - min.tx) / (max.x - min.x);
   int dty = (max.ty - min.ty) / (max.x - min.x);

   int tx = min.tx;
   int ty = min.ty;

   // if left edge is clipped adjust
   if (min.x < 0)
   {
      tx -= min.x * dtx;
      ty -= min.x * dty;
   }

   // start position
   UNS8 *d = Dest_Pixel_Pntr + min_x * 3;

   // end position
   UNS8 *e = Dest_Pixel_Pntr + max_x * 3;

   //--------------------
   // MAIN CODE
   //--------------------
   switch(Op)
   {
   //-----------------------------------------
   // Draws a solid texture with no masking.
   //-----------------------------------------
   case DRAW_SOLID:
		__asm
		{
			// set MOVSD to move in forward direction
			cld

			// determine length of hline
			mov	ecx,e
			sub	ecx,d

			// check if it's greater than 11
			cmp	ecx,11
			jg		short DRAW_SOLID_Write_DWORD

			// point to the screen and write
			mov	edi,d
			jmp	DRAW_SOLID_Write_Byte_Stream
		
			//------------------------------------------------------------
			// greater than 11 (12 +) so enter optimized loop that will
			// write 3 DWORDS at a time
			//
			// ECX <-- Number of pixels to plot
			// EDI <-- Pointer to incoming texel storage
			//------------------------------------------------------------			
DRAW_SOLID_Write_DWORD:
			mov	edi,Pixel_Stream

			//-----------------
			// READ TEXEL 1
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			mov   ebx,tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			mov   ebx,ty
			shr   ebx,16
			mov   eax,Src_Pitch
			mul   ebx
			add   esi,eax

			// get index
			mov	ebx,[esi]

			// get 24-bit pixel
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// copy RGB triple to memory
			push	ecx
			mov	ecx,3
			rep	movsb
			pop	ecx

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx
			
			//-----------------
			// READ TEXEL 2
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			shr   edx,16
			mov   eax,Src_Pitch
			mul   edx
			add   esi,eax

			// get index
			mov	ebx,[esi]

			// get 24-bit pixel
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// copy RGB triple to memory
			push	ecx
			mov	ecx,3
			rep	movsb
			pop	ecx

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx


			//-----------------
			// READ TEXEL 3
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			shr   edx,16
			mov   eax,Src_Pitch
			mul   edx
			add   esi,eax

			// get index
			mov	ebx,[esi]

			// get 24-bit pixel
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// copy RGB triple to memory
			push	ecx
			mov	ecx,3
			rep	movsb
			pop	ecx

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx
			
			//-----------------
			// READ TEXEL 4
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			shr   edx,16
			mov   eax,Src_Pitch
			mul   edx
			add   esi,eax

			// get index
			mov	ebx,[esi]

			// get 24-bit pixel
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// copy RGB triple to memory
			push	ecx
			mov	ecx,3
			rep	movsb
			pop	ecx

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx

			//---------------------------------
			// blast stream to the destination
			//---------------------------------
			push	ecx			
			mov	esi,Pixel_Stream
  			mov	edi,d
			mov	ecx,3
			rep	movsd
			pop	ecx

			// add 12 to destination pointer
			add	d,12

			// decrement ECX by 12
			sub	ecx,12

			// check if we're still over or equal 12 and if so, then fetch another
			// source RGB quadriple
			cmp	ecx,12
			jge	DRAW_SOLID_Write_DWORD	
		
			// is ECX 0? (meaning we're done)
			or		ecx,ecx
			jz		DRAW_SOLID_Done

			//----------------------------------------------------
			// Write remaining bytes directly to the destination
			//
			// ECX <-- Number of bytes to write
			// EDI <-- Offset on destination surface
			//----------------------------------------------------
DRAW_SOLID_Write_Byte_Stream:
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			mov   ebx,tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			mov   ebx,ty
			shr   ebx,16
			mov   eax,Src_Pitch
			mul   ebx
			add   esi,eax

			// get index
			mov	ebx,[esi]

			// get 24-bit pixel
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// copy RGB triple to destination
			push	ecx
			mov	ecx,3
			rep	movsb
			pop	ecx
			
			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx

			// decrement counter
			sub	ecx,3
			jnz	DRAW_SOLID_Write_Byte_Stream

DRAW_SOLID_Done:
		}
   break;


   //-----------------------------------------
   // Draws and masks out color 0
   //-----------------------------------------
   case DRAW_MASK:
      // draw
      while (d <= e)
      {
         //-------------------------
         // READ TEXELS
         //-------------------------
         // get 8 bit color index
         index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
         
         // mask out color 0
         if (index!=0)
         {
            // look up 24-bit texel in table
            Texel1 = (UNS32) Src_Palette[index].lowDWORD.dwColour;
            
            //-------------------------
            // WRITE PIXELS
            //-------------------------
            __asm
            {
               mov   edi,d
                  
               // get first texel value
               mov   eax,Texel1
               mov   [edi],ax
               ror   eax,16
               mov   [edi+2],al
            }
         }
         
         // increment texture map offsets
         tx += dtx;
         ty += dty;
         
         // increment screen pointer
         d+=3;
      }
   break;


   //----------------------------------------------------
   // Draw texture with partial alpha blending.
   //----------------------------------------------------
   case DRAW_ALPHA:
      __asm
		{
			mov	edi,d

DRAW_ALPHA_Main_Loop:
			//---------------------------------------------------------------
			// get 8 bit color index
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//---------------------------------------------------------------
			mov   esi,Src_Pixel_Pntr
            
			mov   ebx,tx
			shr   ebx,16
			mov   ecx,ty
			add   esi,ebx

			mov   eax,Src_Pitch
			shr   ecx,16
			mul   ecx
			add   esi,eax
			
			// get 8-bit color index
			mov	edx,[esi]
			and	edx,0x000000ff
			
			// if it's zero, then skip this pixel
			or		edx,edx
			jz		DRAW_ALPHA_Next_Iteration
			       		
			//-----------------------------------------------------------------
			// Source RGB=Src_Palette[index].lowDWORD.dwColour;
			//
			// ESI <-- index
			//----------------------------------------------------------------
			shl	edx,3 			
			mov	esi,Src_Palette
			add	esi,edx

			// EBX <- source color
			mov	ebx,[esi]
					
			//---------------------------------------------------------------
			// determine the inverse alpha for this color.
			// Inverse_Alpha=255-Src_Palette[index].dwAlpha;            
			//
			// ESI <-- index
			//----------------------------------------------------------------
			mov	ecx,[esi].dwAlpha

			// check if source alpha is 255
			cmp	ecx,255
			jl 	short DRAW_ALPHA_Check_For_0_Alpha

			// if it is 255 then just write the source RGB to the destination
			jmp	DRAW_ALPHA_Draw_Texel

DRAW_ALPHA_Check_For_0_Alpha:
			or		ecx,ecx
			jz		DRAW_ALPHA_Next_Iteration

			// store the source RGB and determine the inverse alpha value
			mov	eax,255
			mov	Source_RGB,ebx
			sub	eax,ecx

			// read a value from the destination
			mov	ebx,[edi]

			mov	Inverse_Alpha,eax

			//---------------------------------------------------------
			// read the 32-bit RGB value from the destination surface
			// stored as 0 R G B
			//---------------------------------------------------------
			// store the value 2 more times on the stack for later
			push	ebx
			push	ebx
			
			pop	ecx

			// mask the destination BLUE
			and	ebx,0x000000ff

			// mask the destination GREEN
			and	ecx,0x0000ff00

			// move to the HI-word of ECX
			shl	ecx,8

			// combine BLUE and GREEN and multiply by inverse alpha
			or		ebx,ecx
			mov	eax,Inverse_Alpha
			mul	ebx
			
			// extract the BLUE and divide by 256
			push	eax
			xor	ebx,ebx
			and	eax,0x0000ffff
			shr	eax,8

			// store to EBX
			mov	ebx,eax

			// extract the GREEN and divide by 256
			pop	eax
			and	eax,0xffff0000
			shr	eax,24
			shl	eax,8
			or		ebx,eax

			// mask the destination RED and multiply
			pop	ecx
			and	ecx,0x00ff0000
			mov	eax,Inverse_Alpha
			mul	ecx
			shr	eax,24
			shl	eax,16

			// combine with BLUE and GREEN
			or		ebx,eax

			// add destination RGB and source RGB
			add	ebx,Source_RGB

DRAW_ALPHA_Draw_Texel:
			// write
			mov	[edi],bx
			ror	ebx,16
			mov	[edi+2],bl
			
DRAW_ALPHA_Next_Iteration:         
			// get tx and ty
			mov	ebx,tx		
			mov	ecx,ty

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx

			// advance screen pointer by 3 bytes
			add	edi,3

			// store new y offset
			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_ALPHA_Main_Loop	
		}			
   break;


   //----------------------------------------------------
   // Draw texture with partial alpha blending using the
	// awesome (?) power of MMX
	//
	// Texel: Pixels from the source texture.s
	// Pixel: Pixels from the destination surface.
   //----------------------------------------------------
   case DRAW_ALPHA_MMX:
      // draw
      __asm
		{
			// set MOVSD to move in forward direction
			cld

			// determine length of hline
			mov	ecx,e
			sub	ecx,d

			// check if it's greater than 11
			cmp	ecx,11
			jg		short DRAW_ALPHA_MMX_Process_DWORD

			// less than 12 so process RGB triples individually
			mov	edi,d
			jmp	DRAW_ALPHA_MMX_Process_RGB_Triple
		
			//------------------------------------------------------------
			// greater than 11 (12 +) so enter optimized loop that will
			// process 4 RGB triples at a time.
			//
			// ECX <-- Number of pixels to process
			// EDI <-- Pointer to incoming texel storage
			//------------------------------------------------------------			
DRAW_ALPHA_MMX_Process_DWORD:
			mov	edi,Texel_Stream

			//-----------------
			// READ TEXEL 1
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			mov   ebx,tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			mov   ebx,ty
			shr   ebx,16
			mov   eax,Src_Pitch
			mul   ebx
			add   esi,eax

			// get 8-bit index
			mov	ebx,[esi]

			// convert index for palette table lookup
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// get the 0RGB value to EAX
			mov	eax,[esi]

			// mov RGB values up 8 bits
			shl	eax,8

			// get the alpha value to EBX (BL)
			mov	ebx,[esi+4]

			// combine the both of em so that
			// EAX = RGBA
			or		eax,ebx

			// store RGBA to the texel stream and advance EDI to 
			// point to the next entry
			stosd

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx	

			//-----------------
			// READ TEXEL 2
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			shr   edx,16
			mov   eax,Src_Pitch
			mul   edx
			add   esi,eax

			// get 8-bit index
			mov	ebx,[esi]

			// convert index for palette table lookup
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// get the 0RGB value to EAX
			mov	eax,[esi]

			// mov RGB values up 8 bits
			shl	eax,8

			// get the alpha value to EBX (BL)
			mov	ebx,[esi+4]

			// combine the both of em so that
			// EAX = RGBA
			or		eax,ebx

			// store RGBA to the texel stream and advance EDI to 
			// point to the next entry
			stosd

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx	

			//-----------------
			// READ TEXEL 3
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			shr   edx,16
			mov   eax,Src_Pitch
			mul   edx
			add   esi,eax

			// get 8-bit index
			mov	ebx,[esi]

			// convert index for palette table lookup
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// get the 0RGB value to EAX
			mov	eax,[esi]

			// mov RGB values up 8 bits
			shl	eax,8

			// get the alpha value to EBX (BL)
			mov	ebx,[esi+4]

			// combine the both of em so that
			// EAX = RGBA
			or		eax,ebx

			// store RGBA to the texel stream and advance EDI to 
			// point to the next entry
			stosd

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx	

			//-----------------
			// READ TEXEL 4
			//-----------------
			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			shr   edx,16
			mov   eax,Src_Pitch
			mul   edx
			add   esi,eax

			// get 8-bit index
			mov	ebx,[esi]

			// convert index for palette table lookup
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// get the 0RGB value to EAX
			mov	eax,[esi]

			// mov RGB values up 8 bits
			shl	eax,8

			// get the alpha value to EBX (BL)
			mov	ebx,[esi+4]

			// combine the both of em so that
			// EAX = RGBA
			or		eax,ebx

			// store RGBA to the texel stream and advance EDI to 
			// point to the next entry
			stosd

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx	

			//-------------------------------------------------
			// Copy 4 destination pixels into (cached) memory
			//
			// ESI <- Starting dest offset
			// EDI <- Pixel stream storage
			//-------------------------------------------------
			push	ecx
			mov	esi,d
			mov	edi,Pixel_Stream
			mov	ecx,3
			rep	movsd
			pop	ecx

			//--------------------------------------------------
			// ALPHA BLEND PIXELS
			//
			// ESI - Texel_Stream -> 4 source RGBA quads
			// ESI - Pixel_Stream -> 4 destination RGB triples
			// EBX - Pointer to destination surface
			//--------------------------------------------------
			mov	esi,Texel_Stream
			mov	edi,Pixel_Stream
			mov	ebx,d

			push	ecx
			//--------------------------------
			// PROCESS FIRST TEXEL/PIXEL PAIR
			//--------------------------------		
			// EAX <- source RGBA
			// ESI+=4 (to next entry)
			lodsd

			// MM7 <- source RGBA
			movd	MM7,eax

			// CL <- 255-alpha
			mov	ecx,255
			xor	edx,edx
			sub	ecx,eax

			// shift down by 8
			psrlq	MM7,8
			
			// mask the alpha value
			and	ecx,0x000000ff

			// check if alpha is 255 and if so then skip it
			cmp	ecx,0xff
			je		DRAW_ALPHA_MMX_Process_TEXEL_2

			// MM1 <- destination RGBR
			movd	MM1,[edi]

			or		edx,ecx

			// clear MM2 (used for interleave operation)
			pxor	MM2,MM2

			rol	edx,8

			// interleave destination RGB components with 0's starting with MM1.
			punpcklbw   MM1,MM2

			or		edx,ecx
			rol	edx,8

			or		edx,ecx

			// load MM0 with the inversse alpha values (EBX <- AAAA)
			movd	MM0,edx

			// interleave inverse alpha values with 0's starting with MM0
			punpcklbw   MM0,MM2
						
			// multiply destination RGB components by 255-alpha
			// and store result to MM1
			pmullw   MM1,MM0
			
			// pack-shift words in MM1 right by 8 to divide by 256
			psrlw MM1,8        
			
			// pack words back into bytes and store to hi-dword of MM2				
			packuswb MM2,MM1
			
			// shift right to move to lower dword
			psrlq MM2,32
			
			// MM2 --> (Dest_RGB * Inverse_Alpha)/8 (byte format)
			// MM7 --> Source_RGB
			
			// add MM7 (source RGB) with MM1 ((Dest_RGB * Inverse_Alpha) >> 8)
			paddb	MM7,MM2

			// write MM7 to the destination surface
			movd	[ebx],MM7

DRAW_ALPHA_MMX_Process_TEXEL_2:
			//--------------------------------
			// PROCESS SECOND TEXEL/PIXEL PAIR
			//--------------------------------		
			// EAX <- source RGBA
			// ESI+=4 (to next entry)
			lodsd

			// MM7 <- source RGBA
			movd	MM7,eax

			// CL <- 255-alpha
			mov	ecx,255
			xor	edx,edx
			sub	ecx,eax

			// shift down by 8
			psrlq	MM7,8
			
			// mask the alpha value
			and	ecx,0x000000ff	

			// check if alpha is 255 and if so then skip it
			cmp	ecx,0xff
			je		DRAW_ALPHA_MMX_Process_TEXEL_3

			// MM1 <- destination RGBR
			movd	MM1,[edi+3]

			or		edx,ecx

			// clear MM2 (used for interleave operation)
			pxor	MM2,MM2

			rol	edx,8

			// interleave destination RGB components with 0's starting with MM1.
			punpcklbw   MM1,MM2

			or		edx,ecx
			rol	edx,8

			or		edx,ecx

			// load MM0 with the inversse alpha values (EBX <- AAAA)
			movd	MM0,edx

			// interleave inverse alpha values with 0's starting with MM0
			punpcklbw   MM0,MM2
						
			// multiply destination RGB components by 255-alpha
			// and store result to MM1
			pmullw   MM1,MM0
			
			// pack-shift words in MM1 right by 8 to divide by 256
			psrlw MM1,8        
			
			// pack words back into bytes and store to hi-dword of MM2				
			packuswb MM2,MM1
			
			// shift right to move to lower dword
			psrlq MM2,32
			
			// MM2 --> (Dest_RGB * Inverse_Alpha)/8 (byte format)
			// MM7 --> Source_RGB
			
			// add MM7 (source RGB) with MM1 ((Dest_RGB * Inverse_Alpha) >> 8)
			paddb	MM7,MM2

			// write MM7 to the destination surface
			movd	[ebx+3],MM7

DRAW_ALPHA_MMX_Process_TEXEL_3:			
			//--------------------------------
			// PROCESS THIRD TEXEL/PIXEL PAIR
			//--------------------------------		
			// EAX <- source RGBA
			// ESI+=4 (to next entry)
			lodsd

			// MM7 <- source RGBA
			movd	MM7,eax

			// CL <- 255-alpha
			mov	ecx,255
			xor	edx,edx
			sub	ecx,eax

			// shift down by 8
			psrlq	MM7,8
			
			// mask the alpha value
			and	ecx,0x000000ff	

			// check if alpha is 255 and if so then skip it
			cmp	ecx,0xff
			je		DRAW_ALPHA_MMX_Process_TEXEL_4

			// MM1 <- destination RGBR
			movd	MM1,[edi+6]

			or		edx,ecx

			// clear MM2 (used for interleave operation)
			pxor	MM2,MM2

			rol	edx,8

			// interleave destination RGB components with 0's starting with MM1.
			punpcklbw   MM1,MM2

			or		edx,ecx
			rol	edx,8

			or		edx,ecx

			// load MM0 with the inversse alpha values (EBX <- AAAA)
			movd	MM0,edx

			// interleave inverse alpha values with 0's starting with MM0
			punpcklbw   MM0,MM2
						
			// multiply destination RGB components by 255-alpha
			// and store result to MM1
			pmullw   MM1,MM0
			
			// pack-shift words in MM1 right by 8 to divide by 256
			psrlw MM1,8        
			
			// pack words back into bytes and store to hi-dword of MM2				
			packuswb MM2,MM1
			
			// shift right to move to lower dword
			psrlq MM2,32
			
			// MM2 --> (Dest_RGB * Inverse_Alpha)/8 (byte format)
			// MM7 --> Source_RGB
			
			// add MM7 (source RGB) with MM1 ((Dest_RGB * Inverse_Alpha) >> 8)
			paddb	MM7,MM2

			// write MM7 to the destination surface
			movd	[ebx+6],MM7

DRAW_ALPHA_MMX_Process_TEXEL_4:
			//--------------------------------
			// PROCESS FOURTH TEXEL/PIXEL PAIR
			//--------------------------------		
			// EAX <- source RGBA
			// ESI+=4 (to next entry)
			lodsd

			// MM7 <- source RGBA
			movd	MM7,eax

			// CL <- 255-alpha
			mov	ecx,255
			xor	edx,edx
			sub	ecx,eax

			// shift down by 8
			psrlq	MM7,8
			
			// mask the alpha value
			and	ecx,0x000000ff	

			// check if alpha is 255 and if so then skip it
			cmp	ecx,0xff
			je		DRAW_ALPHA_MMX_Skip_Texel_4

			// MM1 <- destination RGBR
			movd	MM1,[edi+9]

			or		edx,ecx

			// clear MM2 (used for interleave operation)
			pxor	MM2,MM2

			rol	edx,8

			// interleave destination RGB components with 0's starting with MM1.
			punpcklbw   MM1,MM2

			or		edx,ecx
			rol	edx,8

			or		edx,ecx

			// load MM0 with the inversse alpha values (EBX <- AAAA)
			movd	MM0,edx

			// shift MM1 down by 8 (0RGB)
			// psrlq	MM1,8

			// interleave inverse alpha values with 0's starting with MM0
			punpcklbw   MM0,MM2
						
			// multiply destination RGB components by 255-alpha
			// and store result to MM1
			pmullw   MM1,MM0
			
			// pack-shift words in MM1 right by 8 to divide by 256
			psrlw MM1,8        
			
			// pack words back into bytes and store to hi-dword of MM2				
			packuswb MM2,MM1
			
			// shift right to move to lower dword
			psrlq MM2,32
			
			// MM2 --> (Dest_RGB * Inverse_Alpha)/8 (byte format)
			// MM7 --> Source_RGB
			
			// add MM7 (source RGB) with MM1 ((Dest_RGB * Inverse_Alpha) >> 8) 
			paddb	MM7,MM2

			// write result to the destination surface
			movd	eax,MM7
			mov	[ebx+9],ax
			ror	eax,16
			mov	[ebx+11],al

DRAW_ALPHA_MMX_Skip_Texel_4:
			// restore ECX to byte counter
			pop	ecx

			// add 12 to destination pointer
			add	d,12

			// decrement byte counter by 12
			sub	ecx,12

			// check if we're still over or equal 12 bytes and if so, then fetch another
			// source RGB quadriple
			cmp	ecx,12
			jge	DRAW_ALPHA_MMX_Process_DWORD
		
			// is ECX 0? (meaning we're done)
			or		ecx,ecx
			jz		DRAW_ALPHA_MMX_Done

			//----------------------------------------------------
			// Process individual RGB triples
			//
			// ECX <-- Number of bytes left to process
			//----------------------------------------------------
DRAW_ALPHA_MMX_Process_RGB_Triple:
			mov	edi,d

DRAW_ALPHA_MMX_Process_RGB_Triple_Main_Loop:
			// preserve byte counter
			push	ecx

			// point ESI to texture
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			mov   ebx,tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			mov   ebx,ty
			shr   ebx,16
			mov   eax,Src_Pitch
			mul   ebx
			add   esi,eax

			// get 8-bit index
			mov	ebx,[esi]

			// convert index for palette table lookup
			mov	esi,Src_Palette
			and	ebx,0x000000ff
			shl	ebx,3
			add	esi,ebx

			// MM7 <- get the source 0RGB
			movd	MM7,[esi]

			// get the alpha value to EAX (AL)
			mov	eax,[esi+4]

			//-------------------------
			// alpha blend RGB triple
			//
			// ESI <- source RGB
			// ESI+4 <- alpha factor
			//-------------------------
			// CL <- 255-alpha
			mov	ecx,255
			xor	edx,edx
			sub	ecx,eax

			// mask the a0lpha value
			and	ecx,0x000000ff	

			// check if alpha is 255 and if so then skip it
			cmp	ecx,0xff
			je		DRAW_ALPHA_MMX_Skip_Single_Write

			// OR in the alpha value 
			or		edx,ecx
			rol	edx,8

			or		edx,ecx
			rol	edx,8

			or		edx,ecx

			// MM1 <- destination RGBR
			movd	MM1,[edi]

			// load MM0 with the inversse alpha values (EBX <- AAAA)
			movd	MM0,edx

			// clear MM2 (used for interleave operation)
			pxor	MM2,MM2

			// interleave inverse alpha values with 0's starting with MM0
			punpcklbw   MM0,MM2
			
			// interleave destination RGB components with 0's starting with MM1.
			punpcklbw   MM1,MM2
			
			// multiply destination RGB components by 255-alpha
			// and store result to MM1
			pmullw   MM1,MM0
			
			// pack-shift words in MM1 right by 8 to divide by 256
			psrlw MM1,8        
			
			// pack words back into bytes and store to hi-dword of MM2				
			packuswb MM2,MM1
			
			// shift right to move to lower dword
			psrlq MM2,32
			
			// MM2 --> (Dest_RGB * Inverse_Alpha)/8 (byte format)
			// MM7 --> Source_RGB
			
			// add MM7 (source RGB) with MM1 ((Dest_RGB * Inverse_Alpha) >> 8)
			paddb MM7,MM2

			// write MM7 to the destination surface
			movd	eax,MM7
			mov	[edi],ax
			ror	eax,16
			mov	[edi+2],al
			
DRAW_ALPHA_MMX_Skip_Single_Write:
			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx

			// advance destination index to next pixel
			add	edi,3

			// decrement counter
			pop	ecx
			sub	ecx,3
			jnz	DRAW_ALPHA_MMX_Process_RGB_Triple_Main_Loop

DRAW_ALPHA_MMX_Done:

			// clear the MMX state
			emms
		}
   break;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_BLTWARP_Hline_8to32
//
// DESCRIPTION: An internal function that draws a textured horizontal line from one edge to
// another.
// NOTE: This function is called internally by LE_BLTWARP_8to32
//
// ARGUMENTS:
// min, max -- The edge span to draw between.
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pallete -- The 8-bit pallete for the source
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Width -- The width of the destination surface.
//
// REMARKS 
// *  Based on the number on the operation value, the line will be drawn differnly
//     Op == 0 --> Draw solid
//     Op == 1 --> Draw with color 0 as transparent
//     Op == 2 --> Draw with color 0 as transparent and color range from 1 to Num_Alpha-1 
//     are drawn with alpha blending.
//     Op == 3 --> Alpha blend with MMX
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_BLTWARP_Hline_8to32(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
                            UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op)
{
   //----------------
   // LOCALS
   //----------------
   // work vars
   UNS32 Inverse_Alpha,Source_RGB;

   // clip to the logical screen
   int min_x = (min.x > 0) ? min.x : 0;
   int max_x = (max.x < Dest_Width) ? max.x : Dest_Width;

   // make sure it ain't flat
   if (max_x <= min_x)
      return;

   // initialize the step vars which is the ratio of the length of a texture side to the
   // length of the corresponding polygon's side.
   int dtx = (max.tx - min.tx) / (max.x - min.x);
   int dty = (max.ty - min.ty) / (max.x - min.x);

   // starting texture co-ordinates
   int tx = min.tx;
   int ty = min.ty;

   // if left edge is clipped adjust
   if (min.x < 0)
   {
      tx -= min.x * dtx;
      ty -= min.x * dty;
   }

   // start position
   UNS32 *d = (UNS32*) Dest_Pixel_Pntr + min_x;

   // end position
   UNS32 *e = (UNS32*) Dest_Pixel_Pntr + max_x;

   //--------------------
   // MAIN CODE
   //--------------------
   switch(Op)
   {
   //-----------------------------------------
   // Draws texture as a solid or with color 0
	// masking.  No alpha blending.
   //-----------------------------------------
   case DRAW_SOLID:
		__asm
		{
			// point to the first pixel in this line
			mov	edi,d

DRAW_SOLID_Main_Loop:
			//--------------------------------------------------------------
			// get 8 bit color index
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			mov	ebx,tx						
			mov	esi,Src_Pixel_Pntr		

			shr	ebx,16						
			mov	ecx,ty						

			mov	eax,Src_Pitch				
			add	esi,ebx						

			shr	ecx,16						
			mul	ecx							
			
			add	esi,eax						
			xor	ebx,ebx						
			
			nop									
			nop									
			
			// BL <- first 8-bit index      		
			mov	bl,byte ptr [esi]			

			or		ebx,ebx
			jnz	DRAW_SOLID_Draw
			add	edi,4
			jmp	DRAW_SOLID_Next_Iteration

DRAW_SOLID_Draw:
			//----------------------------------------------------------
			// Get 32-bit color value
			//
			// BL <- index 
			//----------------------------------------------------------
			// point ESI to the source palette
			shl	ebx,3
			mov	esi,Src_Palette			
			add	esi,ebx

			//----------------------------------------------------------
			// Draw texel to destination
			// ESI -> EDI
			//----------------------------------------------------------
			nop
			nop
			nop

			movsd
			
DRAW_SOLID_Next_Iteration:
			//------------------------------------
			// advance to next texel and pixel
			//------------------------------------
			mov	ecx,ty
			mov	ebx,tx		

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx

			// advance screen pointer by 4 bytes
			// add	edi,4

			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_Main_Loop			
		}
   break;
   

   //-----------------------------------------------------------------------------
   // Perform alpha blending using MMX extensions
	// NOTE: Interleaving is done to zero extend byte values into words so that
	// we can contain the overflow from the multiplication of the inverse_alpha
	// value and the destination RGB components
	//
	// NOTE: This code won't make much sense as it has be re-arranged such that 
	// most of the instructions pair in the UV pipes.  If you can't understand it
	// then you shouldn't be trying to change....SO DON'T.  :)
   //-----------------------------------------------------------------------------
   case DRAW_ALPHA_MMX:
      // draw
      __asm
		{
			mov	edi,d

DRAW_ALPHA_MMX_Main_Loop:
			//---------------------------------------------------------------
			// get 8 bit color index
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//---------------------------------------------------------------
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			mov   ebx,tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			mov   ebx,ty
			shr   ebx,16
			mov   eax,Src_Pitch
			mul   ebx
			add   esi,eax
			
			// get 8-bit color index
			mov	edx,[esi]
			and	edx,0x000000ff
			
			// if it's zero, then skip this pixel
			or		edx,edx
			jz		DRAW_ALPHA_MMX_Next_Iteration
			       		
			//-----------------------------------------------------------------
			// get the source and destination RGB components and store to MM7
			// Source_RGB=Src_Palette[index].lowDWORD.dwColour;
			//
			// EBX <-- index
			// MM7 <-- Source Color
			//----------------------------------------------------------------
 			mov	esi,Src_Palette
			shl	edx,3
			add	esi,edx

			// store the source color
			movd	MM7,[esi]
	
			// get the alpha value
			mov	ecx,[esi].dwAlpha
			// is it 255?
			cmp	ecx,255
			// if not then continue
			jl		short DRAW_ALPHA_MMX_Check_0_Alpha

			// it is so just draw the source pixel
			jmp	DRAW_ALPHA_MMX_Draw_Texel
			
DRAW_ALPHA_MMX_Check_0_Alpha:
			// if alpha is 0 then just skip the pixel altogether
			or		ecx,ecx
			jz		DRAW_ALPHA_MMX_Next_Iteration

			//---------------------------------------------------------------
			// determine the inverse alpha for this color.
			// 256 - Src_Palette[index].dwAlpha;            
			//----------------------------------------------------------------
			mov	ebx,255
			mov	ecx,[esi].dwAlpha
			xor	eax,eax
			sub	ebx,ecx

			// fill EAX with inverse alpha values
			or	   eax,ebx
			rol   eax,8
			or	   eax,ebx
			rol   eax,8
			or	   eax,ebx
			rol   eax,8
			or	   eax,ebx

			nop
			
			// Load MM1 with the destination RGB components			
			// -----------------------------
			// | 0000 | 00Dr | 00Dg | 00Db |
			// -----------------------------
			// |   HI QWORD  |  LO QWORD   |
			// -----------------------------             
			movd MM1,[edi]
					
			// load MM0 with the inverse alpha component by interleaving
			// with 2 registers.  The first reg is all 0's.  The second 
			// is loaded with Invserse_Alpha.  It will look like this when
			// the operation is completed.
			// -----------------------------
			// | 00IA | 00IA | 00IA | 00IA |
			// -----------------------------
			// |   HI QWORD  |  LO QWORD   |
			// -----------------------------
			
			// load MM0 with the Inverse_Alpha values
			movd  MM0,eax
			
			// set MM2 to 0 (used for next 2 interleave operations)
			pxor  MM2,MM2
			
			// interleave inverse alpha values with 0's starting with MM0
			punpcklbw   MM0,MM2
			
			// interleave destination RGB components with 0's starting with MM1.
			punpcklbw   MM1,MM2
			
			// multiply destination RGB components by 255-alpha
			// and store result to MM1
			pmullw   MM1,MM0
			
			// pack-shift MM1 right by 8 to divide by 256
			psrlw MM1,8        
			
			// pack words back into bytes and store to hi-dword of MM2				
			packuswb MM2,MM1
			
			// shift right to move to lower dword
			psrlq MM2,32
			
			// MM2 --> (Dest_RGB * Inverse_Alpha)/8 (byte format)
			// MM7 --> Source_RGB
			
			// add MM7 (source RGB) with MM1 ((Dest_RGB * Inverse_Alpha) >> 8)
			// store result to MM0 which will be in interleaved word format
			paddw MM7,MM2
			

DRAW_ALPHA_MMX_Draw_Texel:
			// write to surface
			movd  [edi],MM7

DRAW_ALPHA_MMX_Next_Iteration:         
			// get tx and ty
			mov	ebx,tx		
			mov	ecx,ty

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx

			// advance screen pointer by 4 bytes
			add	edi,4

			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_ALPHA_MMX_Main_Loop
		
			emms
		}
		break;


   //----------------------------------------------------
   // Draw texture with partial alpha blending.
   //----------------------------------------------------
   case DRAW_ALPHA:
      __asm
		{
			mov	edi,d

DRAW_ALPHA_Main_Loop:
			//---------------------------------------------------------------
			// get 8 bit color index
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//---------------------------------------------------------------
			mov   esi,Src_Pixel_Pntr
            
         // calc tx
			mov   ebx,tx
			shr   ebx,16
			add   esi,ebx
				
			// calc ty
			mov   ebx,ty
			shr   ebx,16
			mov   eax,Src_Pitch
			mul   ebx
			add   esi,eax
			
			// get 8-bit color index
			mov	edx,[esi]
			and	edx,0x000000ff
			
			// if it's zero, then skip this pixel
			or		edx,edx
			jz		DRAW_ALPHA_Next_Iteration
			       		
			//-----------------------------------------------------------------
			// Source RGB=Src_Palette[index].lowDWORD.dwColour;
			//
			// ESI <-- index
			//----------------------------------------------------------------
 			mov	esi,Src_Palette
			shl	edx,3
			add	esi,edx

			// EBX <- source color
			mov	ebx,[esi]
					
			//---------------------------------------------------------------
			// determine the inverse alpha for this color.
			// Inverse_Alpha=255-Src_Palette[index].dwAlpha;            
			//
			// ESI <-- index
			//----------------------------------------------------------------
			mov	ecx,[esi].dwAlpha

			// check if source alpha is 255
			cmp	ecx,255
			jl 	short DRAW_ALPHA_Check_For_0_Alpha

			// if it is 255 then just write the source RGB to the destination
			jmp	DRAW_ALPHA_Draw_Texel

DRAW_ALPHA_Check_For_0_Alpha:
			or		ecx,ecx
			jz		DRAW_ALPHA_Next_Iteration

			// store the source RGB and determine the inverse alpha value
			mov	eax,255
				mov	Source_RGB,ebx
			sub	eax,ecx
			mov	Inverse_Alpha,eax

			//---------------------------------------------------------
			// read the 32-bit RGB value from the destination surface
			// stored as 0 R G B
			//---------------------------------------------------------
			mov	ebx,[edi]

			// store the value 2 more times on the stack for later
			push	ebx
			push	ebx
			
			// mask the destination BLUE
			and	ebx,0x000000ff

			// mask the destination GREEN
			pop	ecx
			and	ecx,0x0000ff00
			// move to the HI-word of ECX
			shl	ecx,8

			// combine BLUE and GREEN and multiply by inverse alpha
			or		ebx,ecx
			mov	eax,Inverse_Alpha
			mul	ebx
			
			// extract the BLUE and divide by 256
			push	eax
			xor	ebx,ebx
			and	eax,0x0000ffff
			shr	eax,8

			// store to EBX
			mov	ebx,eax

			// extract the GREEN and divide by 256
			pop	eax
			and	eax,0xffff0000
			shr	eax,24
			shl	eax,8
			or		ebx,eax

			// mask the destination RED and multiply
			pop	ecx
			and	ecx,0x00ff0000
			mov	eax,Inverse_Alpha
			mul	ecx
			shr	eax,24
			shl	eax,16

			// combine with BLUE and GREEN
			or		ebx,eax

			// add destination RGB and source RGB
			add	ebx,Source_RGB

DRAW_ALPHA_Draw_Texel:
			// write
			mov	[edi],ebx
			
DRAW_ALPHA_Next_Iteration:         
			// get tx and ty
			mov	ebx,tx		
			mov	ecx,ty

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx

			// advance screen pointer by 4 bytes
			add	edi,4

			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_ALPHA_Main_Loop
		
			emms
		}			
   break;
   }  // SWITCH
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_BLTWARP_Hline_16to16
//
// DESCRIPTION: An internal function that draws a textured horizontal line from one edge to
// another.
// NOTE: This function is called internally by LE_BLTWARP_16to16
//
// ARGUMENTS:
// min, max -- The edge span to draw between.
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels

// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Width -- The width of the destination surface.
//
// REMARKS 
// *  Based on the operation value, the hline will be drawn differently.
//     Op == 0 --> Draw solid
//     Op == 2 --> Alpha blend using a global alpha value
//     Op == 3 --> Alpha blend with MMX using a global alpha value
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_BLTWARP_Hline_16to16(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op)
{
   //----------------
   // LOCALS
   //----------------  
   // clip
   int min_x = (min.x > 0) ? min.x : 0;
   int max_x = (max.x < Dest_Width) ? max.x : Dest_Width;

   if (max_x <= min_x)
      return;

   // initialize 
   int dtx = (max.tx - min.tx) / (max.x - min.x);
   int dty = (max.ty - min.ty) / (max.x - min.x);

   int tx = min.tx;
   int ty = min.ty;

   // if left edge is clipped adjust
   if (min.x < 0)
   {
      tx -= min.x * dtx;
      ty -= min.x * dty;
   }

   // start position
   UNS16 *d = (UNS16*) Dest_Pixel_Pntr + min_x;

   // end position
   UNS16 *e = (UNS16*) Dest_Pixel_Pntr + max_x;

   //--------------------
   // MAIN CODE
   //--------------------
   switch(Op)
   {
   //-----------------------------------------
   // Draws straight to the destination with
	// no masking or blending
   //-----------------------------------------
   case DRAW_SOLID:
		__asm
		{
			cld

			// point to the first pixel in this line
			mov	edi,d

DRAW_SOLID_Main_Loop:
			//--------------------------------------------------------------
			// get first 16-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			// add (tx >> 16)*2
			mov	ebx,tx		
			mov	esi,Src_Pixel_Pntr		
			shr	ebx,16			
			mov	ecx,ty			
			shl	ebx,1
			mov	eax,Src_Pitch

			// add (ty >> 16) * Src_Pitch			
			shr	ecx,16
			add	esi,ebx			
			mul	ecx
			add	esi,eax

			mov	ecx,ty

			// avoid AGI
			nop
			nop

			// ESI -> current texel
			mov	ebx,[esi]

			// preserve
			push	ebx

			//-----------------------
			// advance to next texel 
			//-----------------------		
			mov	ebx,tx		

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx
			mov	ty,ecx

			//--------------------------------------------------------------
			// get second 16-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			// add (tx >> 16)*2
			mov	ebx,tx		
			mov	esi,Src_Pixel_Pntr		
			shr	ebx,16			
			mov	ecx,ty			
			shl	ebx,1
			mov	eax,Src_Pitch

			// add (ty >> 16) * Src_Pitch			
			shr	ecx,16
			add	esi,ebx			
			mul	ecx
			add	esi,eax

			// restore first pixel
			pop	ebx

			// ESI -> current texel (second one)
			mov	eax,[esi]

			// mask low word
			and	ebx,0x0000ffff

			// move to top word (cause this one gets written second)
			shl	eax,16

			// combine em
			or		eax,ebx
			
			//------------------------------------
			// advance to next texel and pixel
			//------------------------------------
			mov	ecx,ty
			mov	ebx,tx		

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx
			mov	ty,ecx
	
			//----------------------------------------------------------
			// Transfer 32-bit value from source bitmap to destination
			// surface.
			//
			// EAX <- 16-bit pixel pair
			// EDI <- pointer to current offset in destination surface
			//----------------------------------------------------------
			stosd
			
			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_Main_Loop			
		}
   break;


   //-----------------------------------------
   // Draws straight to the destination with
	// no masking or blending with MMX.
   //-----------------------------------------
   case DRAW_SOLID_MMX:
		__asm
		{
			cld

			// point to the first pixel in this line
		 	mov	edi,d
			nop

			//-------------------------------------------
			// set up MMX regs to be our texture offsets
			// MM0 -  Ty   Tx
			// MM1 - DTy  DTx
			//-------------------------------------------
			movd	MM0,ty
			movd	MM2,tx

			psllq	MM0,32
			movd	MM1,dty

			por	MM0,MM2
			psllq	MM1,32
						 
			movd	MM2,dtx			
			por	MM1,MM2

DRAW_SOLID_MMX_Main_Loop:
			//--------------------------------------------------------------
			// get first 32-bit color value
 			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			movq	MM7,MM0						
			mov   esi,Src_Pixel_Pntr		
			
			movd  ebx,MM0						
			psrlq	MM7,48						
            
         // calc tx			
			shr   ebx,16						
			mov   eax,Src_Pitch				
	
			shl	ebx,1			
			nop

			movd  ecx,MM7		
			add   esi,ebx						
					
			// calc ty			
			mul   ecx
 
			// ESI -> current texel
			mov	ebx,[esi+eax]

			//-----------------------
			// advance to next texel
			//-----------------------
			paddd	MM0,MM1

			// preserve first texel value
			push	ebx

			//--------------------------------------------------------------
			// get second 32-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			movq	MM7,MM0						
			mov   esi,Src_Pixel_Pntr		
			
			movd  ebx,MM0						
			psrlq	MM7,48						
            
         // calc tx			
			shr   ebx,16						
			mov   eax,Src_Pitch				
	
			shl	ebx,1							
			add   esi,ebx						
					
			// calc ty			
			movd  ecx,MM7						
			mul   ecx
			add   esi,eax

			//-----------------------
			// advance to next texel
			//-----------------------
			paddd	MM0,MM1

			// restore first texel
			pop	ebx

			// ESI -> current texel (second one)
			mov	eax,[esi]

			// mask low word
			and	ebx,0x0000ffff

			// move to top word (cause this one gets written second)
			shl	eax,16

			// combine em
			or		eax,ebx
			
			//----------------------------------------------------------
			// Transfer 32-bit value from source bitmap to destination
			// surface.
			//
			// EAX <- 16-bit pair
			// EDI <- pointer to current offset in destination surface
			//----------------------------------------------------------
			stosd
			
			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_MMX_Main_Loop			

			emms
		}
   break;

   }  // SWITCH
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_BLTWARP_Hline_32to32
//
// DESCRIPTION: An internal function that draws a textured horizontal line from one edge to
// another.
// NOTE: This function is called internally by LE_BLTWARP_32to32
//
// ARGUMENTS:
// min, max -- The edge span to draw between.
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels

// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Width -- The width of the destination surface.
//
// REMARKS 
// *  Based on the operation value, the hline will be drawn differently.
//     Op == 0 --> Draw solid
//     Op == 2 --> Alpha blend using a global alpha value
//     Op == 3 --> Alpha blend with MMX using a global alpha value
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_BLTWARP_Hline_32to32(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op)
{
   //----------------
   // LOCALS
   //----------------
   // clip to the logical screen
   int min_x = (min.x > 0) ? min.x : 0;
   int max_x = (max.x < Dest_Width) ? max.x : Dest_Width;

   // make sure it ain't flat
   if (max_x <= min_x)
      return;

   // initialize the step vars which is the ratio of the length of a texture side to the
   // length of the corresponding polygon's side.
   int dtx = (max.tx - min.tx) / (max.x - min.x);
   int dty = (max.ty - min.ty) / (max.x - min.x);

   // starting texture co-ordinates
   int tx = min.tx;
   int ty = min.ty;

   // if left edge is clipped adjust
   if (min.x < 0)
   {
      tx -= min.x * dtx;
      ty -= min.x * dty;
   }

   // start position
   UNS32 *d = (UNS32*) Dest_Pixel_Pntr + (min_x);

   // end position
   UNS32 *e = (UNS32*) Dest_Pixel_Pntr + (max_x);

   //--------------------
   // MAIN CODE
   //--------------------
   switch(Op)
   {
   //-----------------------------------------
   // Draws straight to the destination with
	// no masking or blending
   //-----------------------------------------
   case DRAW_SOLID:
		__asm
		{
			cld

			// point to the first pixel in this line
			mov	edi,d

DRAW_SOLID_Main_Loop:
			//--------------------------------------------------------------
			// get 32-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			// add (tx >> 16)*4
			mov	ebx,tx		
			mov	esi,Src_Pixel_Pntr		
			shr	ebx,16			
			mov	ecx,ty			
			shl	ebx,2
			mov	eax,Src_Pitch

			// add (ty >> 16) * Src_Pitch
			
			shr	ecx,16
			add	esi,ebx			
			mul	ecx
			add	esi,eax
			
			//----------------------------------------------------------
			// Transfer 32-bit value from source bitmap to destination
			// surface.
			//
			// ESI <- index to 32-bit source pixel
			// EDI <- pointer to current offset in destination surface
			//----------------------------------------------------------
			movsd
			
			//------------------------------------
			// advance to next texel and pixel
			//------------------------------------
			mov	ecx,ty
			mov	ebx,tx		

			// increment texture co-ordinates to next texl			
			// tx+=dtx
			add	ebx,dtx

			// ty+=dty
			add	ecx,dty

			// store new values
			mov	tx,ebx
			mov	ty,ecx

			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_Main_Loop			
		}
   break;


   //-----------------------------------------
   // Draws straight to the destination with
	// no masking or blending with MMX.
   //-----------------------------------------
   case DRAW_SOLID_MMX:
		__asm
		{
			cld

			// point to the first pixel in this line
		 	mov	edi,d

			//-------------------------------------------
			// set up MMX regs to be our texture offsets
			// MM0 -  Ty   Tx
			// MM1 - DTy  DTx
			//-------------------------------------------
			movd	MM0,ty
			movd	MM1,dty

			psllq	MM0,32
			psllq	MM1,32

			movd	MM2,tx
			por	MM0,MM2
						 
			movd	MM2,dtx			
			por	MM1,MM2

DRAW_SOLID_MMX_Main_Loop:
			//--------------------------------------------------------------
			// get 32-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			movq	MM7,MM0						
			mov   esi,Src_Pixel_Pntr		
			
			movd  ebx,MM0						
			psrlq	MM7,48						
            
         // calc tx			
			shr   ebx,16						
			mov   eax,Src_Pitch				
	
			shl	ebx,2							
			add   esi,ebx						
					
			// calc ty			
			movd  ecx,MM7						
			mul   ecx
			add   esi,eax

			//-----------------------
			// advance to next texel
			//-----------------------
			paddd	MM0,MM1
			
			//----------------------------------------------------------
			// Transfer 32-bit value from source bitmap to destination
			// surface.
			//
			// ESI <- index to 32-bit source pixel
			// EDI <- pointer to current offset in destination surface
			//----------------------------------------------------------
			movsd
			
			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_MMX_Main_Loop			

			emms
		}
   break;

   }  // SWITCH
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LI_BLTWARP_Hline_24to24
//
// DESCRIPTION: An internal function that draws a textured horizontal line from one edge to
// another.
// NOTE: This function is called internally by LE_BLTWARP_24to24
//
// ARGUMENTS:
// min, max -- The edge span to draw between.
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels

// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Width -- The width of the destination surface.
//
// REMARKS 
// *  Based on the operation value, the hline will be drawn differently.
//     Op == 0 --> Draw solid
//     Op == 2 --> Alpha blend using a global alpha value
//     Op == 3 --> Alpha blend with MMX using a global alpha value
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LI_BLTWARP_Hline_24to24(const Edge &min,const Edge &max,
                            unsigned char *Src_Pixel_Pntr,UNS32 Src_Pitch,UNS16 Src_Width,UNS16 Src_Height,
                            UNS8 *Dest_Pixel_Pntr,UNS16 Dest_Width,
                            UNS8 Op)
{
   //----------------
   // LOCALS
   //----------------
	// texel and pixel stream arrays
	UNS8 Texel_Stream_Array[16]; 
	UNS32 *Texel_Stream = (UNS32 *) &Texel_Stream_Array[0];

	UNS8 Pixel_Stream_Array[12];
	UNS32 *Pixel_Stream = (UNS32 *) &Pixel_Stream_Array[0];

	// for fast video mem writes
	UNS8 Burst_Stream_Array[12];
	UNS32 *Burst_Stream = (UNS32 *) &Burst_Stream_Array[0];

   // clip to physical screen
   int min_x = (min.x > 0) ? min.x : 0;
   int max_x = (max.x < Dest_Width) ? max.x : Dest_Width;

   if (max_x <= min_x)
      return;

   // initialize
   int dtx = (max.tx - min.tx) / (max.x - min.x);
   int dty = (max.ty - min.ty) / (max.x - min.x);

   int tx = min.tx;
   int ty = min.ty;

   // if left edge is clipped adjust
   if (min.x < 0)
   {
      tx -= min.x * dtx;
      ty -= min.x * dty;
   }

   // start position
   UNS8 *d = Dest_Pixel_Pntr + min_x * 3;

   // end position
   UNS8 *e = (Dest_Pixel_Pntr + max_x * 3) - 3;

   //--------------------
   // MAIN CODE
   //--------------------
   switch(Op)
   {
   //-----------------------------------------
   // Draws a solid texture with no masking.
   //-----------------------------------------
   case DRAW_SOLID:
		__asm
		{
			// set MOVSD to move in forward direction
			cld

			// point to the screen and write
			mov	edi,d
		
			//------------------------------------------------------------
			// Inner loop.  Works by copying 4 bytes from source to dest
			// then decrementing EDI by 1 so that moves by 3 bytes to the
			// next offset.
			//	
			// Instruction pairing in effect.  If you don't understand
			// what's going on below, then chances are, you shouldn't be
			// f**king wit it.
			//------------------------------------------------------------			
DRAW_SOLID_Write_DWORD:
			mov   ebx,tx
			mov   esi,Src_Pixel_Pntr
            
			shr   ebx,16
			mov   ecx,ty

			mov   eax,Src_Pitch
			add   esi,ebx

			shr   ecx,16
			add   esi,ebx
			
			add   esi,ebx			
			
			mul   ecx
			nop

			add   esi,eax

			movsd
			nop

			dec	edi

			// advance texture offsets
			mov	ebx,tx		
			mov	edx,ty
			add	ebx,dtx
			add	edx,dty
			mov	tx,ebx
			mov	ty,edx
			
			cmp	edi,e
			jl		DRAW_SOLID_Write_DWORD		

			//----------------------
			// copy last RGB triple
			//----------------------
			mov   ebx,tx
			mov   esi,Src_Pixel_Pntr
            
			shr   ebx,16
			mov   ecx,ty

			mov   eax,Src_Pitch
			add   esi,ebx

			shr   ecx,16
			add   esi,ebx
			
			add   esi,ebx			
			
			mul   ecx
			nop

			add   esi,eax

			mov	ecx,3
			movsb
		}
   break;


   //-----------------------------------------
   // Draws a solid texture with no masking 
	// but using the awesome force of 
	// EMMA-EMMA-X
   //-----------------------------------------
   case DRAW_SOLID_MMX:
		__asm
		{
			cld

			// point to the first pixel in this line
		 	mov	edi,d

			//-------------------------------------------
			// set up MMX regs to be our texture offsets
			// MM0 -  Ty   Tx
			// MM1 - DTy  DTx
			//-------------------------------------------
			movd	MM0,ty
			movd	MM1,dty

			psllq	MM0,32
			psllq	MM1,32

			movd	MM2,tx
			por	MM0,MM2
						 
			movd	MM2,dtx			
			por	MM1,MM2

DRAW_SOLID_MMX_Main_Loop:
			//--------------------------------------------------------------
			// get 32-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			movq	MM7,MM0					// U
			mov   esi,Src_Pixel_Pntr	// V
			
			movd  ebx,MM0					// U
			psrlq	MM7,48					// V
            
         // calc tx			
			shr   ebx,16					// U
			mov   eax,Src_Pitch			// V
	
			movd  ecx,MM7					// U
			add	esi,ebx					// V

			add	esi,ebx
			add   esi,ebx						
												
			mul   ecx
			nop

			add   esi,eax

			//-----------------------
			// advance to next texel
			//-----------------------
			paddd	MM0,MM1
			
			//----------------------------------------------------------
			// Transfer 32-bit value from source bitmap to destination
			// surface.  But dec EDI by one to offset the dest surface
			// increments of 3.
			//
			// ESI <- index to 32-bit source pixel
			// EDI <- pointer to current offset in destination surface
			//----------------------------------------------------------
			movsd

			dec	edi
			
			// check if we hit the end of this line
			cmp	edi,e
			jl		DRAW_SOLID_MMX_Main_Loop			

			//--------------------------------------------------------------
			// get 32-bit color value
			// index=Src_Pixel_Pntr[(tx >> 16) + ((ty >> 16) * Src_Pitch)];
			//--------------------------------------------------------------
			movq	MM7,MM0						
			mov   esi,Src_Pixel_Pntr		
			
			movd  ebx,MM0						
			psrlq	MM7,48						
            
         // calc tx			
			shr   ebx,16						
			mov   eax,Src_Pitch				
	
			add	esi,ebx
			add	esi,ebx
			add   esi,ebx						
					
			// calc ty			
			movd  ecx,MM7						
			mul   ecx
			add   esi,eax
		
			//----------------------------------------------------------
			// copy single RGB triple
			//----------------------------------------------------------
			mov	ecx,3
			rep	movsb

			// clear MMX state
			emms		
		}
	break;
	}
}

#endif // CE_ARTLIB_EnableSystemGrafix




#if 0

#endif