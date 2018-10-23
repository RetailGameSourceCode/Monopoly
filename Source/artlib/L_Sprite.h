/*******************************************************************************
*
*   FILE:         L_SPRITE.H
*   AUTHOR:       DCF
*   DESCRIPTION:
*
*   (C) Copyright 1995 Artech Digital Entertainment.  All rights reserved.
*
*******************************************************************************/

#ifndef SPRITE_H
#define SPRITE_H

typedef struct tagSpriteFrame
{
  unsigned int isVisible : 1;
  unsigned int isInterrupt : 1;
  unsigned int isLoop : 1;
  unsigned int isModified : 1;
    /* Flags controlling the sprite for this frame. The isVisible flag
    determines whether or not the sprite is shown on screen. The isInterrupt
    flag means that the image for this frame will interrupt immediately the
    current image playing for the sprite. If it is FALSE, the next image
    will be chained to the end of the current image. If isLoop is TRUE, the
    image for this frame will loop. Only makes sense if using sequences. If
    isModified is TRUE, this frame will become the sprites current frame on
    the next call to LE_SPRITE_UpdateSprites(). */

  short x,y;
    /* Position of the sprite for this frame. The position is relative
    to the top left corner of the graphic used to display the sprite. */

#if CE_ARTLIB_EnableSystemAnim
  unsigned short width, height;
    /* Dimensions of the sprite image. Used for scaling the bitmap
    image. These have no effect if the sprite is a Sequence. A value of 0
    means the sprite will be the same size as the image it is using for
    display. The width must not be less than 2. */
#elif CE_ARTLIB_EnableSystemSequencer
  TYPE_Scale2D scale;
    /* Scaling factor. This value is used for the new renderer that uses
    a scaling factor rather than width and height dimensions for scaling.
    There is a new function LE_SPRITE_SetScale() which can be used to
    adjust the size of the image. */
#endif

  LE_DATA_DataId image;
    /* Image used to display the sprite for this frame. Can be a TAB, TCB,
    an object created with LE_GRAFIX_ObjectCreate() or a sequence. */

} SPRITEFRAME, *LPSPRITEFRAME;


typedef void FUNC_MODIFYSPRITE( LPSPRITEFRAME, void * );
typedef FUNC_MODIFYSPRITE *PFUNC_MODIFYSPRITE;

typedef struct tagSprite
{
  DWORD priority;
    /* Priority of the button. This value is used internally in the
    animation engine to determine the Z order of objects on the screen.
    Higher priority objects appear in front of lower priority objects. */

  PFUNC_MODIFYSPRITE callback;
    /* Optional callback function used to control the display of the
    sprite. This routine will be called during LE_SPRITE_UpdateSprites().
    The user has the option of modifying the parameters for the sprite
    which will be applied as soon as the function returns. The first
    parameter is a pointer to the next sprite frame. The user can make
    changes directly to this structure. The second parameter is optional
    user data. */

  void *callbackFunctionData;
    /* Optional user defined data that is passed to the callback function
    if one is defined. */

#if CE_ARTLIB_EnableSystemSequencer
  LE_REND_RenderSlot renderSlot;
    /*  Render slot where this sprite image will be rendered. By default,
    this value will be 0, which is usually the default render slot if
    you are using only one. However if you are using multiple ones, you
    pretty well have to set this value. Use LE_SPRITE_SetRenderSlot(). */
#endif

  SPRITEFRAME frames[2];
    /* Frames holding information about the sprite. The first frame
    holds the current display state while the second contains changes
    that will be applied to the first on the next call to
    SPRITE_UpdateSprites(). After the call, frames[0] will be the
    same as frames[1]. */

} LE_SPRITE_Record, *LE_SPRITE_Pointer;


// ==========================================================================
//
// Function prototypes
//
// ==========================================================================


void LE_SPRITE_Initialize( LE_SPRITE_Pointer sprite, DWORD priority );
  /* Prepare a sprite for display. This routine will not show the sprite
  on the screen ( use SPRITE_Show ). You define an initial position
  and image for the sprite. You do not have to use the routines
  SPRITE_StartModification and SPRITE_EndModification for this call. */


void LE_SPRITE_SetVisible( LE_SPRITE_Pointer sprite, BOOL isVisible );
  /* Set the visibility status of the sprite. If isVisible is TRUE, the
  sprite will be displayed on the screen. If it is FALSE, it will be
  removed but not destroyed. The sprites new visibility status will
  be activated on the next call to SPRITE_UpdateSprites(). */


void LE_SPRITE_GetPosition( LE_SPRITE_Pointer sprite, short *x, short *y );
void LE_SPRITE_SetPosition( LE_SPRITE_Pointer sprite,
                            short             x,
                            short             y,
                            BOOL              isOffset );
  /* Set a new position for the sprite. The values of 'x' and 'y' are
  the screen coordinates of the upper left corner of the current image
  used to display the sprite. If the value of isOffset is TRUE, the
  values of 'x' and 'y' are treated as offsets from the sprites
  current position. The sprite will only be updated on the next call
  to SPRITE_UpdateSprites(). */


#if CE_ARTLIB_EnableSystemAnim
  void LE_SPRITE_SetWidth( LE_SPRITE_Pointer sprite, unsigned short width );
  void LE_SPRITE_SetHeight( LE_SPRITE_Pointer sprite, unsigned short height );
    /* Sets the dimensions for the sprite. If the dimensions for the sprite
    do not match the dimensions of its current image, the sprite will be
    scaled as necessary. If the current image is a Sequence, these values
    are ignored. A value of 0 means to keep the sprite the same size as
    the image. */

#elif CE_ARTLIB_EnableSystemSequencer
  void LE_SPRITE_SetScale( LE_SPRITE_Pointer sprite, TYPE_Scale2D scale );
    /* Define a new scaling factor for the sprite. A value of 1.0 for
    means the image is not changed, 0.5 means half the size, 2.0 for
    twice the size etc. Currently, calling this function will only
    affect the image if you are running the new sequencer, renderer. */

  void LE_SPRITE_SetRenderSlot( LE_SPRITE_Pointer sprite, LE_REND_RenderSlot slot );
    /* Sets the render slot for this sprite. By default, the render
    slot will be 0 which is suitable in most cases. If you are using
    multiple render slots, always set them for the sprites. */
#endif


void LE_SPRITE_SetCallbackFunction( LE_SPRITE_Pointer   sprite,
                                    PFUNC_MODIFYSPRITE  func,
                                    void *              data );
  /* Define a callback function for this sprite with optional function
  data. This routine will be called during each call to LE_SPRITE_UpdateSprites
  that involve this sprite. The user can modify the next frame and
  it will be updated immediately. If you wish to cancel the callback
  function, call this routine again with NULL as a function pointer. */


void LE_SPRITE_SetImage( LE_SPRITE_Pointer  sprite,
                         LE_DATA_DataId     image,
                         BOOL               isLoop,
                         BOOL               isInterrupt );
  /* Set a new image for the sprite. The image can be either a TCB, TAB
  a sequence or an object created with LE_GRAFIX_ObjectCreate(). If the
  isLoop parameter is TRUE, and the new image is a sequence, the
  sequence will loop ( duh! ). If the new image is not a sequence, this
  parameter is ignored. If the 'isInterrupt' parameter is TRUE, the
  sprite will change its appearance immediately, otherwise it will
  be chained to the currently running image. This parameter really
  only makes sense if the current image is a sequence. The new image
  will come into affect during the next call to LE_SPRITE_UpdateSprites(). */


void LE_SPRITE_ForceRedraw( LE_SPRITE_Pointer sprite );
  /* Forces the sprite to update and therefore redraw itself during the
  next call to LE_SPRITE_UpdateSprites() regardless if the image or the
  position has not changed. This is useful if you want to alter the
  existing image a sprite is using without changing its position. */


void LE_SPRITE_UpdateSprites( LE_SPRITE_Pointer sprites, int numSprites );
  /* Apply changes made to all sprites contained in the passed array.
  The value of 'numSprites' is the number of sprites contained
  in the passed array. This routine will temporarily pause the
  Animation Engine so all changes will be applied at the same time. */

#endif
