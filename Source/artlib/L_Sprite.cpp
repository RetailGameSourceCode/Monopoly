/*******************************************************************************
*
*   FILE:         L_SPRITE.CPP
*   AUTHOR:       DCF
*   DESCRIPTION:
*                 For a detailed description of sprites, see the SPRITE.H
*                 file.
*
*   (C) Copyright 1998 Artech Digital Entertainment.  All rights reserved.
*
*******************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemSprite
/*****************************************************************************
 *
 * static void LI_SPRITE_AdjustDisplay( LE_SPRITE_Pointer sprite, LE_DATA_DataId graphic )
 *
 *  Description:
 *    Internal routine that modifies the display of the sprite. It simply
 *    determines the type of the current display object, the type of the
 *    new display graphic ( if any ) and makes the proper adjustments.
 *    This means stopping the current object in the correct manner, starting
 *    the new display graphic in the correct manner and setting/resetting
 *    any display flags. This routine will modify the SPR_F_VISIBLE
 *    flag depending on what image is shown.
 *
 *  Parameters:
 *    sprite    - Pointer to sprite object to modify.
 *    graphic   - New graphic for sprite. Can be LED_EI, in which case
 *                the sprite becomes hidden.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

static void LI_SPRITE_AdjustDisplay( LE_SPRITE_Pointer sprite )
{
  LE_DATA_DataType oldType = LE_DATA_DataUnknown;
  LE_DATA_DataType newType = LE_DATA_DataUnknown;
  #if CE_ARTLIB_EnableSystemAnim
    unsigned short width, height;
    RECT tempRect;
    DWORD sequenceFlags;
  #elif CE_ARTLIB_EnableSystemSequencer
    BOOL bResult;
    LE_DATA_DataId dataID;
    UNS16 dataPriority;
  #endif

  // First, determine the types of the current and new display objects.
  // If the sprite is not visible, the displaying will not be
  // empty but will be the last state of the sprite, therefore the old
  // graphic has to be LED_EI.
  if ( sprite->frames[0].image != LED_EI )
    oldType = LE_DATA_GetLoadedDataType( sprite->frames[0].image );
  if ( sprite->frames[1].image != LED_EI )
    newType = LE_DATA_GetLoadedDataType( sprite->frames[1].image );

  // Override. If the current sprite is not visible.
  if ( !( sprite->frames[0].isVisible ) )
    oldType = (LE_DATA_DataType)LE_DATA_DataUnknown;
  // Override. If the new sprite state is not visible.
  if ( !( sprite->frames[1].isVisible ) )
    newType = (LE_DATA_DataType)LE_DATA_DataUnknown;

  // Now, perform action based on previous and next images.
  switch( oldType )
  {
  // Previous item was nothing.
  case LE_DATA_DataUnknown:
      switch( newType )
      {
        // LED_EI -> TAB.
        case LE_DATA_DataUAP:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_TCBwidth( sprite->frames[1].image ) : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_TCBheight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM_AddToRegisterBitmapList( sprite->frames[1].image, sprite->priority, &tempRect, FALSE );
          #elif CE_ARTLIB_EnableSystemSequencer
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      LED_EI,
                                      0,
                                      FALSE,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;

        // LED_EI -> Object
        case LE_DATA_DataNative:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_ReturnObjectWidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_ReturnObjectHeight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM_AddToRegisterBitmapList( sprite->frames[1].image, sprite->priority, &tempRect, TRUE );
          #elif CE_ARTLIB_EnableSystemSequencer
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      LED_EI,
                                      0,
                                      FALSE,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;

        // LED_EI -> Sequence
        case LE_DATA_DataChunky:
          #if CE_ARTLIB_EnableSystemAnim
            sequenceFlags = ( sprite->frames[1].isLoop ) ? LOOP_SEQUENCE : 0;
            LE_ANIM_AddToStartSequenceList( sprite->frames[1].image,
                                            sprite->priority,
                                            LED_EI,
                                            0,
                                            sequenceFlags,
                                            0,
                                            sprite->frames[1].x,
                                            sprite->frames[1].y,
                                            0,
                                            0,
                                            LED_EI,
                                            0,
                                            FALSE );
          #elif CE_ARTLIB_EnableSystemSequencer
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      LED_EI,
                                      0,
                                      FALSE,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;
      }
      break;


  // Previous item was a TAB or TCB.
  case LE_DATA_DataUAP:
      switch( newType )
      {
        // TAB -> TAB. Change images and/or position.
        case LE_DATA_DataUAP:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_TCBwidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_TCBheight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM2_AddToChangeBitmapList( sprite->frames[0].image,
                                            sprite->priority,
                                            FALSE,
                                            &tempRect,
                                            sprite->frames[1].image,
                                            sprite->priority,
                                            FALSE );
          #elif CE_ARTLIB_EnableSystemSequencer
            // If the image is different, start the new image chained off the old one.
            bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                             (UNS16)sprite->priority,
                                             &dataID,
                                             &dataPriority );
            if ( sprite->frames[1].image != sprite->frames[0].image )
            {
              LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                      (UNS16)sprite->priority,
                                      (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                      (UNS16)(bResult ? dataPriority : 0),
                                      FALSE, FALSE,
                                      FALSE, TRUE, FALSE );
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        (bResult ? dataID : LED_EI),
                                        (UNS16)sprite->priority,
                                        !bResult,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
            // Otherwise, just move the image. If this image has a chainee, it is not
            // yet playing, so don't move it. This is kind of awkward but it is the
            // best I can do.
            else
            {
              if ( !bResult )
              {
                LE_SEQNCR_MoveXYSR( sprite->frames[0].image,
                                    (UNS16)sprite->priority,
                                    sprite->frames[1].x,
                                    sprite->frames[1].y,
                                    sprite->frames[1].scale,
                                    0.0F );
                LE_SEQNCR_ForceRedraw( sprite->frames[1].image, (UNS16)sprite->priority );
              }
            }
          #endif
          break;

        // TAB -> Object.
        case LE_DATA_DataNative:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_ReturnObjectWidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_ReturnObjectHeight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM2_AddToChangeBitmapList( sprite->frames[0].image,
                                            sprite->priority,
                                            FALSE,
                                            &tempRect,
                                            sprite->frames[1].image,
                                            sprite->priority,
                                            TRUE );
          #elif CE_ARTLIB_EnableSystemSequencer
            bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                             (UNS16)sprite->priority,
                                             &dataID,
                                             &dataPriority );
            LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                    (UNS16)sprite->priority,
                                    (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                    (UNS16)(bResult ? dataPriority : 0),
                                    FALSE, FALSE,
                                    FALSE, TRUE, FALSE );
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      (bResult ? dataID : LED_EI),
                                      (UNS16)sprite->priority,
                                      !bResult,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;

        // TAB -> Sequence.
        case LE_DATA_DataChunky:
          #if CE_ARTLIB_EnableSystemAnim
            sequenceFlags = CHAIN_SEQ_TO_BITMAP;
            sequenceFlags |= ( sprite->frames[1].isLoop ) ? LOOP_SEQUENCE : 0;
            LE_ANIM_AddToStartSequenceList( sprite->frames[1].image,
                                            sprite->priority,
                                            LED_EI,
                                            0,
                                            sequenceFlags,
                                            0,
                                            sprite->frames[1].x,
                                            sprite->frames[1].y,
                                            0,
                                            0,
                                            sprite->frames[0].image,
                                            sprite->priority,
                                            FALSE );
          #elif CE_ARTLIB_EnableSystemSequencer
            bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                             (UNS16)sprite->priority,
                                             &dataID,
                                             &dataPriority );
            LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                    (UNS16)sprite->priority,
                                    (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                    (UNS16)(bResult ? dataPriority : 0),
                                    FALSE, FALSE,
                                    FALSE, TRUE, FALSE );
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      (bResult ? dataID : LED_EI),
                                      (UNS16)sprite->priority,
                                      !bResult,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;

        // TAB -> LED_EI
        default:
          #if CE_ARTLIB_EnableSystemAnim
            LE_ANIM_AddToRemoveNativeBitmapList( sprite->priority, sprite->frames[0].image );
          #elif CE_ARTLIB_EnableSystemSequencer
            LE_SEQNCR_Stop( sprite->frames[0].image, (UNS16)sprite->priority );
          #endif
          break;
      }
      break;


  // Previous item was a sequence.
  case LE_DATA_DataChunky:
      switch( newType )
      {
        // Sequence - > TAB
        case LE_DATA_DataUAP:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_TCBwidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_TCBheight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM3_AddToRegisterBitmapList( sprite->frames[1].image,
                                              sprite->priority,
                                              &tempRect,
                                              FALSE,
                                              sprite->frames[0].image,
                                              sprite->priority,
                                              sprite->frames[1].isInterrupt,
                                              TRUE );
          #elif CE_ARTLIB_EnableSystemSequencer
            if ( sprite->frames[1].isInterrupt )
            {
              bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                               (UNS16)sprite->priority,
                                               &dataID,
                                               &dataPriority );
              LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                      (UNS16)sprite->priority,
                                      (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                      (UNS16)(bResult ? dataPriority : 0),
                                      FALSE, FALSE,
                                      FALSE, TRUE, FALSE );
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        (bResult ? dataID : LED_EI),
                                        (UNS16)sprite->priority,
                                        !bResult,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
            else
            {
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        sprite->frames[0].image,
                                        (UNS16)sprite->priority,
                                        FALSE,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
          #endif
          break;

        // Sequence -> Object
        case LE_DATA_DataNative:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_ReturnObjectWidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_ReturnObjectHeight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM3_AddToRegisterBitmapList( sprite->frames[1].image,
                                              sprite->priority,
                                              &tempRect,
                                              TRUE,
                                              sprite->frames[0].image,
                                              sprite->priority,
                                              sprite->frames[1].isInterrupt,
                                              TRUE );
          #elif CE_ARTLIB_EnableSystemSequencer
            if ( sprite->frames[1].isInterrupt )
            {
              bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                               (UNS16)sprite->priority,
                                               &dataID,
                                               &dataPriority );
              LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                      (UNS16)sprite->priority,
                                      (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                      (UNS16)(bResult ? dataPriority : 0),
                                      FALSE, FALSE,
                                      FALSE, TRUE, FALSE );
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        (bResult ? dataID : LED_EI),
                                        (UNS16)sprite->priority,
                                        !bResult,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
            else
            {
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        sprite->frames[0].image,
                                        (UNS16)sprite->priority,
                                        FALSE,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
          #endif
          break;

        // Sequence -> Sequence.
        case LE_DATA_DataChunky:
          #if CE_ARTLIB_EnableSystemAnim
            sequenceFlags = CHAIN_SEQUENCE;
            sequenceFlags |= ( sprite->frames[1].isInterrupt ) ? CHAIN_SEQ_INTERRUPT : 0;
            sequenceFlags |= ( sprite->frames[1].isLoop      ) ? LOOP_SEQUENCE : 0;
            LE_ANIM_AddToStartSequenceList( sprite->frames[1].image,
                                            sprite->priority,
                                            sprite->frames[0].image,
                                            sprite->priority,
                                            sequenceFlags,
                                            0,
                                            sprite->frames[1].x,
                                            sprite->frames[1].y,
                                            0,
                                            0,
                                            LED_EI,
                                            0,
                                            FALSE );
          #elif CE_ARTLIB_EnableSystemSequencer
            if ( sprite->frames[1].isInterrupt )
            {
              bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                               (UNS16)sprite->priority,
                                               &dataID,
                                               &dataPriority );
              LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                      (UNS16)sprite->priority,
                                      (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                      (UNS16)(bResult ? dataPriority : 0),
                                      FALSE, FALSE,
                                      FALSE, TRUE, FALSE );
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        (bResult ? dataID : LED_EI),
                                        (UNS16)sprite->priority,
                                        !bResult,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
            else
            {
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        sprite->frames[0].image,
                                        (UNS16)sprite->priority,
                                        FALSE,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
          #endif
          break;

        // Sequence -> LED_EI
        default:
          #if CE_ARTLIB_EnableSystemAnim
            // Special case. If the interrupt flag is not set, let the sequence
            // complete and stop it at the end. We can do this by chaining the
            // LED_EI image to the end of the sequence.
            if ( sprite->frames[1].isInterrupt == FALSE )
            {
              SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                  sprite->frames[1].x + 1, sprite->frames[1].y + 1 );
              LE_ANIM3_AddToRegisterBitmapList( sprite->frames[1].image,
                                                sprite->priority,
                                                &tempRect,
                                                TRUE,
                                                sprite->frames[0].image,
                                                sprite->priority,
                                                FALSE,
                                                TRUE );
            }
            // Otherwise, just stop the sequence.
            else
            {
              LE_ANIM_AddToStopSequenceList( sprite->frames[0].image,
                                             sprite->priority,
                                             TRUE );
            }
          #elif CE_ARTLIB_EnableSystemSequencer
            // If the interrupt flag is set, just stop the sequence immediately.
            // If it is not set, change the ending action.
            if ( sprite->frames[1].isInterrupt )
              LE_SEQNCR_Stop( sprite->frames[0].image, (UNS16)sprite->priority );
            else
              LE_SEQNCR_SetEndingAction( sprite->frames[0].image,
                               (UNS16)sprite->priority,
                               (LE_SEQNCR_EndingAction)LE_SEQNCR_EndingActionStop );
          #endif
          break;
      }
      break;


  // Previous item was an object.
  case LE_DATA_DataNative:
      switch( newType )
      {
        // Object -> TAB.
        case LE_DATA_DataUAP:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_TCBwidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_TCBheight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM2_AddToChangeBitmapList( sprite->frames[0].image,
                                            sprite->priority,
                                            TRUE,
                                            &tempRect,
                                            sprite->frames[1].image,
                                            sprite->priority,
                                             FALSE );
          #elif CE_ARTLIB_EnableSystemSequencer
            bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                             (UNS16)sprite->priority,
                                             &dataID,
                                             &dataPriority );
            LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                    (UNS16)sprite->priority,
                                    (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                    (UNS16)(bResult ? dataPriority : 0),
                                    FALSE, FALSE,
                                    FALSE, TRUE, FALSE );
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      (bResult ? dataID : LED_EI),
                                      (UNS16)sprite->priority,
                                      !bResult,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;

        // Object -> Object.
        case LE_DATA_DataNative:
          #if CE_ARTLIB_EnableSystemAnim
            width  = ( sprite->frames[1].width  == 0 ) ? LE_GRAFIX_ReturnObjectWidth( sprite->frames[1].image )  : sprite->frames[1].width;
            height = ( sprite->frames[1].height == 0 ) ? LE_GRAFIX_ReturnObjectHeight( sprite->frames[1].image ) : sprite->frames[1].height;
            SetRect( &tempRect, sprite->frames[1].x, sprite->frames[1].y,
                                sprite->frames[1].x + width, sprite->frames[1].y + height );
            LE_ANIM2_AddToChangeBitmapList( sprite->frames[0].image,
                                            sprite->priority,
                                            TRUE,
                                            &tempRect,
                                            sprite->frames[1].image,
                                            sprite->priority,
                                            TRUE );
          #elif CE_ARTLIB_EnableSystemSequencer
            // If the image is different, start the new image chained off the old one.
            bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                 (UNS16)sprite->priority,
                                 &dataID,
                                 &dataPriority );
            if ( sprite->frames[1].image != sprite->frames[0].image )
            {
              LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                      (UNS16)sprite->priority,
                                      (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                      (UNS16)(bResult ? dataPriority : 0),
                                      FALSE, FALSE,
                                      FALSE, TRUE, FALSE );
              LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                        (UNS16)sprite->priority,
                                        (bResult ? dataID : LED_EI),
                                        (UNS16)sprite->priority,
                                        !bResult,
                                        sprite->frames[1].x,
                                        sprite->frames[1].y,
                                        sprite->frames[1].scale,
                                        0.0F,
                                        sprite->renderSlot );
            }
            // Otherwise, just move the image.
            else
            {
              if ( !bResult )
              {
                LE_SEQNCR_MoveXYSR( sprite->frames[0].image,
                                    (UNS16)sprite->priority,
                                    sprite->frames[1].x,
                                    sprite->frames[1].y,
                                    sprite->frames[1].scale,
                                    0.0F );
                LE_SEQNCR_ForceRedraw( sprite->frames[0].image, (UNS16)sprite->priority );
              }
            }
          #endif
          break;

        // Object -> Sequence
        case LE_DATA_DataChunky:
          #if CE_ARTLIB_EnableSystemAnim
            sequenceFlags = CHAIN_SEQ_TO_BITMAP;
            sequenceFlags |= ( sprite->frames[1].isLoop ) ? LOOP_SEQUENCE : 0;
            LE_ANIM_AddToStartSequenceList( sprite->frames[1].image,
                                            sprite->priority,
                                            LED_EI,
                                            0,
                                            sequenceFlags,
                                            0,
                                            sprite->frames[1].x,
                                            sprite->frames[1].y,
                                            0,
                                            0,
                                            sprite->frames[0].image,
                                            sprite->priority,
                                            TRUE );
          #elif CE_ARTLIB_EnableSystemSequencer
            bResult = LE_SEQNCR_FindChainee( sprite->frames[0].image,
                                             (UNS16)sprite->priority,
                                             &dataID,
                                             &dataPriority );
            LE_SEQNCR_StopTheWorks( sprite->frames[0].image,
                                    (UNS16)sprite->priority,
                                    (LE_DATA_DataId)(bResult ? dataID : LED_EI),
                                    (UNS16)(bResult ? dataPriority : 0),
                                    FALSE, FALSE,
                                    FALSE, TRUE, FALSE );
            LE_SEQNCR_StartCXYSRSlot( sprite->frames[1].image,
                                      (UNS16)sprite->priority,
                                      (bResult ? dataID : LED_EI),
                                      (UNS16)sprite->priority,
                                      !bResult,
                                      sprite->frames[1].x,
                                      sprite->frames[1].y,
                                      sprite->frames[1].scale,
                                      0.0F,
                                      sprite->renderSlot );
          #endif
          break;

        // Object -> LED_EI.
        default:
          #if CE_ARTLIB_EnableSystemAnim
            LE_ANIM_AddToRemoveNativeBitmapList( sprite->priority, sprite->frames[0].image );
          #elif CE_ARTLIB_EnableSystemSequencer
            LE_SEQNCR_Stop( sprite->frames[0].image, (UNS16)sprite->priority );
          #endif
          break;
      }
      break;
  }

  sprite->frames[0] = sprite->frames[1];
}


/*****************************************************************************
 *
 * void LE_SPRITE_Initialize( LE_SPRITE_Pointer sprite, DWORD priority )
 *
 *  Description:
 *    Initialize a sprite. This call will set up the sprite so it can
 *    be displayed and modified. It just basically sets up all
 *    default values for the structure members and assigns initial
 *    data supplied by the user. The sprite is not visible after this
 *    call.
 *
 *  Parameters:
 *    sprite    - Pointer to sprite object.
 *    priority  - Priority of sprite. This is needed in calls to
 *                display sequences or objects.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_Initialize( LE_SPRITE_Pointer sprite, DWORD priority )
{
  // Reset all the structure members.
  sprite->frames[0].isVisible   = FALSE;
  sprite->frames[0].isInterrupt = FALSE;
  sprite->frames[0].isLoop      = FALSE;
  sprite->frames[0].isModified  = FALSE;
  sprite->frames[0].x           = 0;
  sprite->frames[0].y           = 0;
#if CE_ARTLIB_EnableSystemAnim
  sprite->frames[0].width       = 0;
  sprite->frames[0].height      = 0;
#elif CE_ARTLIB_EnableSystemSequencer
  sprite->frames[0].scale       = 1.0F;
  sprite->renderSlot            = 0;
#endif
  sprite->frames[0].image       = LED_EI;
  sprite->frames[1]             = sprite->frames[0];
  sprite->priority              = priority;
  sprite->callback              = NULL;
  sprite->callbackFunctionData  = NULL;
}


/*****************************************************************************
 *
 * void LE_SPRITE_ForceRedraw( LE_SPRITE_Pointer sprite )
 *
 *  Description:
 *    Forces the sprite to redraw on next update call. This only sets the
 *    isModified flag to TRUE.
 *    This should not cause any problems ( hopefully ).
 *
 *  Parameters:
 *    sprite  - Sprite pointer to modify.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_ForceRedraw( LE_SPRITE_Pointer sprite )
{
  sprite->frames[1].isModified = TRUE;
}


/*****************************************************************************
 *
 * void LE_SPRITE_SetVisible( LE_SPRITE_Pointer sprite, BOOL isVisible )
 *
 *  Description:
 *    Sets the visibility status of the sprite. If the sprite is already
 *    in that visibility mode, no action is performed.
 *
 *  Parameters:
 *    sprite    - Pointer to sprite.
 *    isVisible - TRUE for visible, FALSE for hidden.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_SetVisible( LE_SPRITE_Pointer sprite, BOOL isVisible )
{
  if ( sprite->frames[0].isVisible != (unsigned int)isVisible ||
       sprite->frames[1].isVisible != (unsigned int)isVisible )
  {
    sprite->frames[1].isModified = TRUE;
    sprite->frames[1].isVisible  = isVisible;
  }
}


/*****************************************************************************
 *
 * void LE_SPRITE_SetPosition( LE_SPRITE_Pointer sprite,
 *                             short             x,
 *                             short             y,
 *                             BOOL              isOffset )
 *
 *  Description:
 *    Set a new position for the sprite. The position can be absolute
 *    or relative to the current position depending on the setting of
 *    the isOffset parameter.
 *
 *  Parameters:
 *    sprite    - Pointer to sprite.
 *    x, y      - New sprite position or offset.
 *    isOffset  - If TRUE, x and y are screen positions. If FALSE,
 *                x and y are offsets from the current position.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_GetPosition( LE_SPRITE_Pointer sprite, short *x, short *y )
{
  *x = sprite->frames[1].x;
  *y = sprite->frames[1].y;
}

void LE_SPRITE_SetPosition( LE_SPRITE_Pointer sprite,
                            short             x,
                            short             y,
                            BOOL              isOffset )
{
  if ( isOffset )
  {
    sprite->frames[1].x += x;
    sprite->frames[1].y += y;
  }
  else
  {
    sprite->frames[1].x = x;
    sprite->frames[1].y = y;
  }
  if ( sprite->frames[0].x != sprite->frames[1].x ||
       sprite->frames[0].y != sprite->frames[1].y )
    sprite->frames[1].isModified = TRUE;
}


/*****************************************************************************
 *
 * void LE_SPRITE_SetWidth( LE_SPRITE_Pointer sprite, unsigned short width )
 * void LE_SPRITE_SetHeight( LE_SPRITE_Pointer sprite, unsigned short height )
 *
 *  Description:
 *    Sets the dimensions for the sprite. Used for scaling the image used
 *    to display the sprite. This will not work with sequences as they
 *    have a mind of their own. Currently will not allow either a width or
 *    a height of 0.
 *
 *  Parameters:
 *    sprite          - Pointer to sprite to modify.
 *    width, height   - New dimension.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

#if CE_ARTLIB_EnableSystemAnim

void LE_SPRITE_SetWidth( LE_SPRITE_Pointer sprite, unsigned short width )
{
  if ( width != 1 )
  {
    if ( sprite->frames[0].width != width || sprite->frames[1].width != width )
    sprite->frames[1].width = width;
  }
}

void LE_SPRITE_SetHeight( LE_SPRITE_Pointer sprite, unsigned short height )
{
  if ( height != 1 )
  {
    if ( sprite->frames[0].height != height || sprite->frames[1].height != height )
      sprite->frames[1].height = height;
  }
}

#elif CE_ARTLIB_EnableSystemSequencer

void LE_SPRITE_SetScale( LE_SPRITE_Pointer sprite, TYPE_Scale2D scale )
{
  if ( sprite->frames[1].scale != scale || sprite->frames[0].scale != scale )
  {
    sprite->frames[1].scale = scale;
    sprite->frames[1].isModified = TRUE;
  }
}

void LE_SPRITE_SetRenderSlot( LE_SPRITE_Pointer sprite, LE_REND_RenderSlot slot )
{
  /*
  ** When specifying a render slot, it has to be of type
  ** LE_SEQNCR_RenderSlotSet. To do this, bit shift the render slot
  ** number.
  */
  sprite->renderSlot = ( 1<<slot );
}

#endif


/*****************************************************************************
 *
 * void LE_SPRITE_SetCallbackFunction( LE_SPRITE_Pointer   sprite,
 *                                     PFUNC_MODIFYSPRITE  func,
 *                                     void *              data )
 *
 *  Description:
 *    Set the callback function for the sprite.
 *
 *  Parameters:
 *    sprite    - Pointer to a sprite.
 *    func      - Pointer to callback function.
 *    data      - Optional user data for function.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_SetCallbackFunction( LE_SPRITE_Pointer   sprite,
                                    PFUNC_MODIFYSPRITE  func,
                                    void *              data )
{
  sprite->callback = func;
  sprite->callbackFunctionData = data;
}


/*****************************************************************************
 *
 * void LE_SPRITE_SetImage( LE_SPRITE_Pointer sprite,
 *                          LE_DATA_DataId    image,
 *                          BOOL              isLoop,
 *                          BOOL              isInterrupt )
 *
 *  Description:
 *    Sets a new display image for the sprite. Can be either a TAB\TCB
 *    a sequence or an object. If the new image is a sequence and the
 *    isLoop parameter is TRUE, the sequence will loop. If the value
 *    of isInterrupt is TRUE, the current display image will be
 *    terminated immediately and the new image will start.
 *
 *  Parameters:
 *    sprite      - Pointer to a sprite.
 *    image       - New image pointer.
 *    isLoop      - TRUE if the new image loops.
 *    isInterrupt - TRUE if the current image stops immediately.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_SetImage( LE_SPRITE_Pointer sprite,
                         LE_DATA_DataId    image,
                         BOOL              isLoop,
                         BOOL              isInterrupt )
{
  if ( sprite->frames[0].image != image || sprite->frames[1].image != image )
  {
    sprite->frames[1].isModified = TRUE;
    sprite->frames[1].image = image;
    sprite->frames[1].isLoop = isLoop;
    sprite->frames[1].isInterrupt = isInterrupt;
  }
}


/*****************************************************************************
 *
 * void LE_SPRITE_UpdateSprites( LE_SPRITE_Pointer sprites, int numSprites )
 *
 *  Description:
 *    Update an array of sprites. This effectively applies all changes
 *    made between calls to this function and modifies the sprites
 *    being displayed.
 *
 *  Parameters:
 *    sprites     - Pointer to the start of an array of sprites.
 *    numSprites  - Number of sprites in the passed array.
 *
 *  Returns:
 *    None.
 *
 *****************************************************************************/

void LE_SPRITE_UpdateSprites( LE_SPRITE_Pointer sprites, int numSprites )
{
  int i;

  #if CE_ARTLIB_EnableSystemAnim

    LE_ANIM_PrepareStopSequenceList();
    LE_ANIM_StartRemoveBitmapList();
    LE_ANIM_PrepareStartSequenceList();
    LE_ANIM_StartRegisterBitmapList();
    LE_ANIM_StartChangeBitmapList();

  #elif CE_ARTLIB_EnableSystemSequencer

    LE_SEQNCR_CollectCommands();

  #endif

  for( i=0; i<numSprites; i++ )
  {
    // If a callback function is defined, call it.
    if ( sprites[i].callback != NULL )
      sprites[i].callback( &( sprites[i].frames[1] ), sprites[i].callbackFunctionData );

    // Only process sprite if it has its modified flag set.
    if ( sprites[i].frames[1].isModified )
    {
      // Clear the isModified flag.
      sprites[i].frames[1].isModified = FALSE;

      // If next frame is empty, change the visibility status.
      if ( sprites[i].frames[1].image == LED_EI )
        sprites[i].frames[1].isVisible = FALSE;

      // Don't update the sprite if it is hidden and is not being shown.
      if ( sprites[i].frames[0].isVisible || sprites[i].frames[1].isVisible )
        LI_SPRITE_AdjustDisplay( &( sprites[i] ) );
    }
  }

  #if CE_ARTLIB_EnableSystemAnim

    LE_ANIM_EndChangeBitmapList();
    LE_ANIM_EndRegisterBitmapList();
    LE_ANIM_RunStartSequenceList();
    LE_ANIM_EndRemoveBitmapList();
    LE_ANIM_RunStopSequenceList();
    LE_ANIM_Sync();

  #elif CE_ARTLIB_EnableSystemSequencer

    LE_SEQNCR_ExecuteCommands();

  #endif
}



#endif /* CE_ARTLIB_EnableSystemSprite */
