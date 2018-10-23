/*****************************************************************
*
*   FILE:           MMX.C
*   DESCRIPTION:    MMX routines, including detection and blitting
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*****************************************************************/

#include "l_inc.h"

/************************************************************/

#if CE_ARTLIB_EnableSystemMMX

// The following holds the bytes for the CPUID opcode, followed by a ret instruction.
// The assembler does not support the CPUID instruction yet.
BYTE LI_MMX_CPUID_Instruction[] =
{ 0x0F, 0xA2,   // CPUID instruction
  0xC3          // ret opcode.
};


// The following variable will be set to TRUE during initialization if
// the CPU is MMX enabled.
BOOL    LI_MMX_Available = FALSE;

/*****************************************************************************
 *                           LI_MMX_InitSystem                               *
 *                                                                           *
 *      Initializes MMX system.  Sets LI_MMX_Available to TRUE if CPU is     *
 *  MMX enabled, FALSE otherwise.                                            *
 *****************************************************************************/

void LI_MMX_InitSystem(void)
{
    LI_MMX_Available = LI_MMX_DetectMMX ();

return;
}

/*****************************************************************************
 *                           LI_MMX_DetectMMX                                *
 * Input:   void                                                             *
 * Output:  BOOL - TRUE if CPU supports MMX, FALSE in not.                   *
 *****************************************************************************/

BOOL LI_MMX_DetectMMX (void)
{
    BOOL mmx_supported;

    __asm
    {
       // Detect whether the computer we are running under is at least a
       // Pentium.  If so, we can detect for MMX.  If not, no MMX available.

       // Check for 386.
                pushfd            // push original EFLAGS
                pop     eax           // and pop them into EAX
                mov     ecx, eax      // save original EFLAGS
                xor     eax, 40000h   // flip AC bit in EFLAGS
                push    eax          // save new EFLAGS value on stack
                popfd             // replace current EFLAGS value

                pushfd            // Get flags again - see if the flip
                                  // stayed, or was rejected

                pop     eax           // store new EFLAGS in EAX

                xor     eax, ecx      // Are the flags changed from the original?
                                  // Compare the two.
                jz      No_MMX_Support   // It is a 386?

        // Check for 486.
                mov     eax, ecx      // Get original flags
                xor     eax, 200000h  // CPUID instruction supported flag.
                push    eax          // Set the flags with this bit.
                popfd             // If the change to the bit stays, means CPUID
                pushfd            // instruction is supported, and we have a Pentium.

                pop     eax           // Get new flags into eax.
                xor     eax, ecx      // Have the flags changed?

                jz      No_MMX_Support  // A 486?  No MMX available.

        // Chip supports the CPUID instruction.  Use it to check for MMX.

                mov     eax, 1  // Request feature flags from CPUID

       //                CPUID      // Get CPU flags - Note: Assembler does not
                                    // support it yet.  Hence calling it artificially.
                mov     ebx, offset LI_MMX_CPUID_Instruction
                call    ebx

                test    edx, 00800000h // MMX available?
                jz      No_MMX_Support

                mov     mmx_supported, TRUE     // MMX available.
                jmp     Finished_detection

    No_MMX_Support:
                mov     mmx_supported, FALSE    // MMX not available.

Finished_detection:
    }

return mmx_supported;
//return FALSE;  // Mike: Force a failure.
}


/*****************************************************************************
 *                           LI_MMX_Is_Available                             *
 *                                                                           *
 *   Return:  BOOL - TRUE if system supports MMX, FALSE otherwise, as        *
 *                   detected by LI_MMX_DetectMMX at system initialization.  *
 *****************************************************************************/

BOOL    LE_MMX_Is_Available(void)
{

return LI_MMX_Available;
}

#if CE_ARTLIB_BlitUseOldRoutines // Unused MMX code for blitting.
/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt8to16_MMX
*
*   Description:  blts from an 8 bit paletted bitmap to a 16 bit native
*                 bitmap, using alpha channel data in the palette to
*                 generate transparent pixels, using mmx opcodes
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 16 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/

void LI_MMXAlphaBitBlt8to16(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    register LPLONG lpColorTable,
    int nAlphaInColorTable)
{
    LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;  // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;       // source bitmap width in bytes
    int nPixelsAcrossToBlt;         // blt width in pixels
    int nLinesToBlt;                  // blt height in pixels
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                          // Hacking my way around it.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
    nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits +
                            (nYBltStartCoordinate * nXDestinationWidthInBytes);
    lpDestinationBeginBlt += nXBltStartCoordinate * 2;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    // Need this to look up the jump array in assembly code.
    jump_array_point = jump_array;

    // Assembly inline code for writing the pixels.
    __asm
    {
        // First, save registers we'll be messing up that would not normally
        // be messed up.

        push    ESI
        push    EDI

        cmp     byte ptr [set_offsets], 0
        jnz     StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov     eax, jump_array_point
            mov     ebx, 4
            mov     dword ptr [eax], OFFSET Write_Alpha_Data
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov     set_offsets, 1

StartRoutine:
        // We need to set up the loops, and all the registers.
        // Store # of pixels across in mm4.
        movd    mm4, dword ptr [nPixelsAcrossToBlt]

        // Store location of jump table in high dword of mm4.
        movd    mm0, jump_array_point
        psllq   mm0, 32
        por     mm4, mm0

        // In mm6, we'll put nAlphaInColorTable, and  lpColorTable in mm5.
        movd    mm6, dword ptr [nAlphaInColorTable]
        movd    mm5, dword ptr [lpColorTable]

        // We'll put the # of lines to write in high mm5.
        movd    mm0, dword ptr [nLinesToBlt]          // Load the 2 counters
        psllq   mm0, 32
        por     mm5, mm0

        // We need LE_BLT_dwRedMask, LE_BLT_dwGreenMask, and LE_BLT_dwBlueMask.
        // We'll put them in mm7, with wRedMask16 low word, wGreenMask16 next,
        // and wBlueMask16 the third word.
        mov     eax, 0ffffh
        movd    mm0, eax
        movd    mm7, dword ptr [LE_BLT_dwRedMask]
        movd    mm2, dword ptr [LE_BLT_dwGreenMask]
        pand    mm7, mm0
        movd    mm1, dword ptr [LE_BLT_dwBlueMask]
        pand    mm2, mm0
        pand    mm1, mm0
        psllq   mm2, 16
        psllq   mm1, 32
        por     mm7, mm2
        por     mm7, mm1

        // Now we are ready to start the loop

ScanLineLoop:
        // Get the lines count from second dword of mm5
        movq    mm0, mm5
        psrlq   mm0, 32
        movd    eax, mm0
        cmp     eax, 1
        jl  EndOfBlit

            // Subtract one from the lines we have left.
            mov     eax, 01h
            movd    mm0, eax
            psllq   mm0, 32
            psubw   mm5, mm0

            // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
            // Start the counter of # of pixels wrote in ecx
            xor     ecx, ecx
            mov edi, dword ptr [lpDestinationBeginBlt]


        StartLineBlit:
                movd    eax, mm4    // compare current count with nPixelsAcrossToBlt
                cmp     ecx, eax
                jge     EndOfLineBlit

                // Get the next pixels, if we need to.
                test    ecx, 07h
                jz      GetNextPixels
                jmp     DontGetNextPixels

        GetNextPixels:
                // Get the next 8 pixels.
                movq    mm3, [esi]
                add     esi, 8

        DontGetNextPixels:
                // Move the next pixel into al.
                movd    eax, mm3
                psrlq   mm3, 8      // Shift the store pixels by 8 bits, so we get
                                    // the next pixel the next time we get here

                // Mask the lower 8 bits - that is the next pixel.
                and     eax, 0ffh

                // Check to make sure it is not transparent.
                cmp     eax, 0
                jz      FinishedWithPixel

                    // Now check to see that the pixel is not greater than nAlphaInColorTable.
                    movd    ebx, mm6
                    cmp     eax, ebx
                    jl      CheckForAlphaModes

                        // Not special - just look up in color table.
                        shl     eax, 3      // Multiply by 8.
                        movd    ebx, mm5    // Get location of lpColorTable
                        add     ebx, eax

                        // Now copy a word over.
                        mov     eax, dword ptr [ebx]
                        mov     [edi], ax

                        inc     edi         /* Repeating this code here for speed. */
                        inc     ecx
                        inc     edi
                        jmp     StartLineBlit

            CheckForAlphaModes:
                        // Get the alpha pixel, and set up for alpha tests.
                        shl     eax, 3          // 8 bytes for each alpha entry in color table
                        movd    ebx, mm5        // Get location of lpColorTable
                        add     eax, ebx
                        mov     edx, eax        // Get location of destination data in edx

                        // Setup mm1 to have 3 copies of destination in first 3 words.
                        mov     ebx, dword ptr [edx]    // store  destination word in bx
                        add     edx, 4                  // Go to location of alpha byte
                        and     ebx, 0ffffh             // Isolate the destination word in ebx
                        movd    mm1, ebx                // copy it into mm1

                        movq    mm2, mm1    // Make mm1 have 3 copies of destination word
                        psllq   mm1, 16     // in successive words.
                        por     mm1, mm2
                        psllq   mm2, 32
                        por     mm1, mm2

                        // Setup mm0 to have 3 copies of source in first 3 words.
                        mov     ebx, dword ptr [edi]    // Get source word into dx
                        and     ebx, 0ffffh             // isolate source word in dx
                        movd    mm0, ebx                // Put it into mm0
                        movq    mm2, mm0                // and or it 3 times.
                        psllq   mm0, 16
                        por     mm0, mm2
                        psllq   mm2, 32
                        por     mm0, mm2

                        // Mask for red, green, and blue.
                        pand    mm1, mm7
                        pand    mm0, mm7

                        // Load up the alpha byte in eax
                        mov     eax, dword ptr [edx]    // store alpha byte in eax
                        and     eax, 0ffh               // isolate the alpha byte

                        // Jump to the appropriate function
                        shr     eax, 4                  // Alpha bytes are ?F.  Get rid of the F.
                        movq    mm2, mm4                // Get the jump table location
                        inc     eax                     // Look up table starts with a dummy in 0.
                        psrlq   mm2, 32                 // Jump table in high dword
                        shl     eax, 2                  // 4 bytes of address data for each alpha option

                        movd    edx, mm2                // Put jump table in edx
                        add     eax, edx                // Get index into table
                        mov     eax, dword ptr [eax]    // Get address location in eax

                        jmp     eax                     // jump to correct alpha conversion

                DO_ALPHA_OPAQUE06_25:

                            // Doing dest + source - source >> 4.
                            paddw   mm1, mm0
                            psrlw   mm0, 4
                            psubw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE12_50:

                            // Doing dest + source - source >> 3.
                            paddw   mm1, mm0
                            psrlw   mm0, 3
                            psubw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE18_75:

                            // Doing dest + source >> 1 + source >> 2 + source >> 4
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 2
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE25_00:

                            // Doing dest + source >> 1 + source >> 2
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 1
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE31_25:

                            // Doing dest + source >> 1 + source >> 3 + source >> 4
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 2
                            paddw   mm1, mm0
                            psrlw   mm0, 1
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE37_50:
                            // Doing dest + source >> 1 + source >> 3
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 2
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE43_75:
                            // Doing dest + source >> 1 + source >> 4
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 3
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE50_00:

                            // Doing dest + source >> 1
                            psrlw   mm0, 1
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE56_25:

                            // Doing dest + source >> 1 - source >> 4
                            psrlw   mm0, 1
                            paddw   mm1, mm0
                            psrlw   mm0, 3
                            psubw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE62_50:

                            // Doing dest + source >> 2 + source >> 3
                            psrlw   mm0, 2
                            paddw   mm1, mm0
                            psrlw   mm0, 1
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE68_75:

                            // Doing dest + source >> 2 + source >> 4
                            psrlw   mm0, 2
                            paddw   mm1, mm0
                            psrlw   mm0, 2
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE75_00:

                            // Doing dest + source >> 2
                            psrlw   mm0, 2
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE81_25:

                            // Doing dest + source >> 3 + source >> 4
                            psrlw   mm0, 3
                            paddw   mm1, mm0
                            psrlw   mm0, 1
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE87_50:

                            // Doing dest + source >> 3
                            psrlw   mm0, 3
                            paddw   mm1, mm0

                            jmp Finish_Alpha_Conversion

                DO_ALPHA_OPAQUE93_75:

                            // Doing dest + source >> 4
                            psrlw   mm0, 4
                            paddw   mm1, mm0

                            // Now finish with conversion.

        Finish_Alpha_Conversion:
                            // Do a final mask
                            pand    mm1, mm7

                            // Now add the red, green, blue sections together.
                            movq    mm0, mm1
                            psrlq   mm0, 16
                            por     mm1, mm0
                            psrlq   mm0, 16
                            por     mm1, mm0

                            // Get the results into eax
                            movd    eax, mm1
                            and     eax, 0ffffh

                            // Now write the word.
        Write_Alpha_Data:
                      // Write the new pixel.
                    mov     word ptr [edi], ax

    FinishedWithPixel:

            inc     edi         // Increase our destination pointer
            inc     ecx         // Increase pixels done counter
            inc     edi         // Increase our destination pointer - twice - here for pairing
            jmp     StartLineBlit

EndOfLineBlit:
            // Increase pointers to source and destination to point
            // to next line.
        mov eax, dword ptr [nXDestinationWidthInBytes]
        add dword ptr [lpDestinationBeginBlt], eax

        mov eax, dword ptr [nXSourceWidthInBytes]
        add dword ptr [lpSourceBeginBlt], eax

        jmp ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop     EDI
        pop     ESI

        EMMS        // Empty MMX state - allows floating point to work.
    };

}


/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt8to32
*
*   Description:  blts from an 8 bit paletted bitmap to a 32 bit native
*                 bitmap, using alpha channel data in the palette to
*                 generate transparent pixels, using mmx opcodes
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 32 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      LE_BLT_dwRedMask - mask of red bits for 32 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 32 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 32 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/

void LI_MMXAlphaBitBlt8to32(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    register LPLONG lpColorTable,
    int nAlphaInColorTable)
{
    LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;  // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;       // source bitmap width in bytes
    int nPixelsAcrossToBlt;         // blt width in pixels
    int nLinesToBlt;                  // blt height in pixels
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
    nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits +
                            (nYBltStartCoordinate * nXDestinationWidthInBytes);
    lpDestinationBeginBlt += nXBltStartCoordinate * 4;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    // Need this to look up the jump array in assembly code.
    jump_array_point = jump_array;

    // Assembly inline code for writing the pixels.
    __asm
    {
        // First, save registers we'll be messing up that would not normally
        // be messed up.

        push    ESI
        push    EDI

        cmp     byte ptr [set_offsets], 0
        jnz     StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov     eax, jump_array_point
            mov     ebx, 4
            mov     dword ptr [eax], OFFSET Write_Alpha_Data
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov     set_offsets, 1

StartRoutine:
        // We need to set up the loops, and all the registers.
        // Store # of pixels across in mm4.
        movd    mm4, dword ptr [nPixelsAcrossToBlt]

        // Store location of jump table in high dword of mm4.
        movd    mm0, jump_array_point
        psllq   mm0, 32
        por     mm4, mm0

        // In mm6, we'll put nAlphaInColorTable, and  lpColorTable in mm5.
        movd    mm6, dword ptr [nAlphaInColorTable]
        movd    mm5, dword ptr [lpColorTable]

        // We'll put the # of lines to write in high mm5.
        movd    mm0, dword ptr [nLinesToBlt]          // Load the 2 counters
        psllq   mm0, 32
        por     mm5, mm0

        // We need LE_BLT_dwRedMask, LE_BLT_dwGreenMask, and LE_BLT_dwBlueMask.
        // We'll put them red and green in mm7, with LE_BLT_dwBlueMask
        // in high part of mm6.
        movd    mm7, dword ptr [LE_BLT_dwRedMask]
        movd    mm2, dword ptr [LE_BLT_dwGreenMask]
        movd    mm1, dword ptr [LE_BLT_dwBlueMask]
        psllq   mm2, 32
        psllq   mm1, 32
        por     mm7, mm2
        por     mm6, mm1

        // Now we are ready to start the loop

ScanLineLoop:
        // Get the lines count from second dword of mm5
        movq    mm0, mm5
        psrlq   mm0, 32
        movd    eax, mm0
        cmp     eax, 1
        jl  EndOfBlit

            // Subtract one from the lines we have left.
            mov     eax, 01h
            movd    mm0, eax
            psllq   mm0, 32
            psubw   mm5, mm0

            // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
            // Start the counter of # of pixels wrote in ecx
            xor     ecx, ecx
            mov edi, dword ptr [lpDestinationBeginBlt]


        StartLineBlit:
                movd    eax, mm4    // compare current count with nPixelsAcrossToBlt
                cmp     ecx, eax
                jge     EndOfLineBlit

                // Get the next pixels, if we need to.
                test    ecx, 07h
                jz      GetNextPixels
                jmp     DontGetNextPixels

        GetNextPixels:
                // Get the next 8 pixels.
                movq    mm3, [esi]
                add     esi, 8

        DontGetNextPixels:
                // Move the next pixel into al.
                movd    eax, mm3
                psrlq   mm3, 8      // Shift the store pixels by 8 bits, so we get
                                    // the next pixel the next time we get here

                // Mask the lower 8 bits - that is the next pixel.
                and     eax, 0ffh

                // Check to make sure it is not transparent.
                cmp     eax, 0
                jz      FinishedWithPixel

                    // Now check to see that the pixel is not greater than nAlphaInColorTable.
                    movd    ebx, mm6
                    cmp     eax, ebx
                    jl      CheckForAlphaModes

                        // Not special - just look up in color table.
                        shl     eax, 3      // Multiply by 8.
                        movd    ebx, mm5    // Get location of lpColorTable
                        add     ebx, eax

                        // Now copy a word over.
                        mov     eax, dword ptr [ebx]
                        mov     dword ptr [edi], eax

                        inc     ecx
                        add     edi, 4
                        jmp     StartLineBlit

            CheckForAlphaModes:
                        // Get the alpha pixel, and set up for alpha tests.
                        shl     eax, 3          // 8 bytes for each alpha entry in color table
                        movd    ebx, mm5        // Get location of lpColorTable
                        add     eax, ebx

                        // Setup mm0 to hold destination dword, while
                        // ebx to hold source dword.
                        movd    mm0, dword ptr [eax]
                        mov     ebx, dword ptr [edi]
                        add     eax, 4

                        // Load up the alpha byte in eax
                        mov     eax, dword ptr [eax]    // store alpha byte in eax
                        and     eax, 0ffh               // isolate the alpha byte

                        // Jump to the appropriate function
                        shr     eax, 4                  // Alpha bytes are ?F.  Get rid of the F.
                        movq    mm2, mm4                // Get the jump table location
                        inc     eax                     // Look up table starts with a dummy in 0.
                        psrlq   mm2, 32                 // Jump table in high dword
                        shl     eax, 2                  // 4 bytes of address data for each alpha option

                        movd    edx, mm2                // Put jump table in edx
                        add     eax, edx                // Get index into table
                        mov     eax, dword ptr [eax]    // Get address location in edx

                        jmp     eax                     // jump to correct alpha conversion

                DO_ALPHA_OPAQUE06_25:

                            // Doing dest + source - source >> 4.
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 4.
                            psrld   mm1, 4

                            // And subtract source from dest
                            psubd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // Add them together
                            paddd   mm0, mm1

                            // Shift source by 4.
                            psrld   mm1, 4

                            // And subtract
                            psubd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE12_50:

                            // Doing dest + source - source >> 3.
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3.
                            psrld   mm1, 3

                            // And subtract source from dest
                            psubd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // Add them together
                            paddd   mm0, mm1

                            // Shift source by 3.
                            psrld   mm1, 3

                            // And subtract
                            psubd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE18_75:

                            // Doing dest + source >> 1 + source >> 2 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE25_00:

                            // Doing dest + source >> 1 + source >> 2
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE31_25:

                            // Doing dest + source >> 1 + source >> 3 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2.
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 1
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 1
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE37_50:
                            // Doing dest + source >> 1 + source >> 3
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE43_75:
                            // Doing dest + source >> 1 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3
                            psrld   mm1, 3

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3
                            psrld   mm1, 3

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE50_00:

                            // Doing dest + source >> 1
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE56_25:

                            // Doing dest + source >> 1 - source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3
                            psrld   mm1, 3

                            // Subtract source from dest
                            psubd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3.
                            psrld   mm1, 3

                            // Subtract source from dest
                            psubd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE62_50:

                            // Doing dest + source >> 2 + source >> 3
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE68_75:

                            // Doing dest + source >> 2 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE75_00:

                            // Doing dest + source >> 2
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE81_25:

                            // Doing dest + source >> 3 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE87_50:

                            // Doing dest + source >> 3
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE93_75:

                            // Doing dest + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 4
                            psrld   mm1, 4

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 4
                            psrld   mm1, 4

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            // Now write the word.
        Write_Alpha_Data:
                      // Write the new pixel.
                    mov     dword ptr [edi], eax

    FinishedWithPixel:

            inc     ecx         // Increase pixels done counter
            add     edi, 4      // Increase the destination pointer.
            jmp     StartLineBlit

EndOfLineBlit:
            // Increase pointers to source and destination to point
            // to next line.
        mov     eax, dword ptr [nXDestinationWidthInBytes]
        add     dword ptr [lpDestinationBeginBlt], eax

        mov     eax, dword ptr [nXSourceWidthInBytes]
        add     dword ptr [lpSourceBeginBlt], eax

        jmp     ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop     EDI
        pop     ESI

        EMMS        // Empty MMX state - allows floating point to work.
    };

}


/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt8to24
*
*   Description:  blts from an 8 bit paletted bitmap to a 24 bit native
*                 bitmap, using alpha channel data in the palette to
*                 generate transparent pixels, using mmx opcodes
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 24 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/

void LI_MMXAlphaBitBlt8to24(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    register LPLONG lpColorTable,
    int nAlphaInColorTable)
{
    LPBYTE lpDestinationBeginBlt;       // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;      // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;            // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;           // source bitmap width in bytes
    int nPixelsAcrossToBlt;             // blt width in pixels
    int nLinesToBlt;                    // blt height in pixels
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                        // Hacking my way around it.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 3) + 3) & 0xfffffffc;
    nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits +
                                                    (nYBltStartCoordinate * nXDestinationWidthInBytes);
    lpDestinationBeginBlt += nXBltStartCoordinate * 3;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    // Need this to look up the jump array in assembly code.
    jump_array_point = jump_array;

    // Assembly inline code for writing the pixels.
    __asm
    {
        // First, save registers we'll be messing up that would not normally
        // be messed up.

        push    ESI
        push    EDI

        cmp     byte ptr [set_offsets], 0
        jnz     StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov     eax, jump_array_point
            mov     ebx, 4
            mov     dword ptr [eax], OFFSET Write_Alpha_Data
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add     eax, ebx
            mov     dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add     eax, ebx
            mov     dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov     set_offsets, 1

StartRoutine:
        // We need to set up the loops, and all the registers.
        // Store # of pixels across in mm4.
        movd    mm4, dword ptr [nPixelsAcrossToBlt]

        // Store location of jump table in high dword of mm4.
        movd    mm0, jump_array_point
        psllq   mm0, 32
        por     mm4, mm0

        // In mm6, we'll put nAlphaInColorTable, and  lpColorTable in mm5.
        movd    mm6, dword ptr [nAlphaInColorTable]
        movd    mm5, dword ptr [lpColorTable]

        // We'll put the # of lines to write in high mm5.
        movd    mm0, dword ptr [nLinesToBlt]          // Load the 2 counters
        psllq   mm0, 32
        por     mm5, mm0

        // We need the red, green and blue masks.
        // We'll put red and green in mm7, with blue
        // in high part of mm6.
        mov     eax, 0ffh       // red mask
        mov     ebx, 0ff00h     // green mask
        mov     ecx, 0ff0000h   // blue mask
        movd    mm7, eax
        movd    mm2, ebx
        movd    mm1, ecx
        psllq   mm2, 32
        psllq   mm1, 32
        por     mm7, mm2
        por     mm6, mm1

        // Now we are ready to start the loop

ScanLineLoop:
        // Get the lines count from second dword of mm5
        movq    mm0, mm5
        psrlq   mm0, 32
        movd    eax, mm0
        cmp     eax, 1
        jl  EndOfBlit

            // Subtract one from the lines we have left.
            mov     eax, 01h
            movd    mm0, eax
            psllq   mm0, 32
            psubw   mm5, mm0

            // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
            // Start the counter of # of pixels wrote in ecx
            xor     ecx, ecx
            mov edi, dword ptr [lpDestinationBeginBlt]


        StartLineBlit:
                movd    eax, mm4    // compare current count with nPixelsAcrossToBlt
                cmp     ecx, eax
                jge     EndOfLineBlit

                // Get the next pixels, if we need to.
                test    ecx, 07h
                jz      GetNextPixels
                jmp     DontGetNextPixels

        GetNextPixels:
                // Get the next 8 pixels.
                movq    mm3, [esi]
                add     esi, 8

        DontGetNextPixels:
                // Move the next pixel into al.
                movd    eax, mm3
                psrlq   mm3, 8      // Shift the store pixels by 8 bits, so we get
                                    // the next pixel the next time we get here

                // Mask the lower 8 bits - that is the next pixel.
                and     eax, 0ffh

                // Check to make sure it is not transparent.
                cmp     eax, 0
                jz      FinishedWithPixel

                    // Now check to see that the pixel is not greater than nAlphaInColorTable.
                    movd    ebx, mm6
                    cmp     eax, ebx
                    jl      CheckForAlphaModes

                        // Not special - just look up in color table.
                        shl     eax, 3      // Multiply by 8.
                        movd    ebx, mm5    // Get location of lpColorTable
                        add     ebx, eax

                        // Now copy 3 bytes over.
                        mov     eax, dword ptr [ebx]
                        mov     word ptr [edi], ax
                        inc     edi
                        shr     eax, 16
                        inc     edi
                        inc     ecx     // might as well increase the pixel counter here.
                        mov     byte ptr [edi], al

                        inc     edi
                        jmp     StartLineBlit

            CheckForAlphaModes:
                        // Get the alpha pixel, and set up for alpha tests.
                        shl     eax, 3          // 8 bytes for each alpha entry in color table
                        movd    ebx, mm5        // Get location of lpColorTable
                        add     eax, ebx

                        // Setup mm0 to hold destination dword, while
                        // ebx to hold source dword.
                        movd    mm0, dword ptr [eax]
                        mov     ebx, dword ptr [edi]
                        add     eax, 4

                        // Load up the alpha byte in eax
                        mov     eax, dword ptr [eax]    // store alpha byte in eax
                        and     eax, 0ffh               // isolate the alpha byte

                        // Jump to the appropriate function
                        shr     eax, 4                  // Alpha bytes are ?F.  Get rid of the F.
                        movq    mm2, mm4                // Get the jump table location
                        inc     eax                     // Look up table starts with a dummy in 0.
                        psrlq   mm2, 32                 // Jump table in high dword
                        shl     eax, 2                  // 4 bytes of address data for each alpha option

                        movd    edx, mm2                // Put jump table in edx
                        add     eax, edx                // Get index into table
                        mov     eax, dword ptr [eax]    // Get address location in edx

                        jmp     eax                     // jump to correct alpha conversion

                DO_ALPHA_OPAQUE06_25:

                            // Doing dest + source - source >> 4.
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 4.
                            psrld   mm1, 4

                            // And subtract source from dest
                            psubd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // Add them together
                            paddd   mm0, mm1

                            // Shift source by 4.
                            psrld   mm1, 4

                            // And subtract
                            psubd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE12_50:

                            // Doing dest + source - source >> 3.
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3.
                            psrld   mm1, 3

                            // And subtract source from dest
                            psubd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // Add them together
                            paddd   mm0, mm1

                            // Shift source by 3.
                            psrld   mm1, 3

                            // And subtract
                            psubd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE18_75:

                            // Doing dest + source >> 1 + source >> 2 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE25_00:

                            // Doing dest + source >> 1 + source >> 2
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE31_25:

                            // Doing dest + source >> 1 + source >> 3 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2.
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 1
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Shift source by 1
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE37_50:
                            // Doing dest + source >> 1 + source >> 3
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE43_75:
                            // Doing dest + source >> 1 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3
                            psrld   mm1, 3

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3
                            psrld   mm1, 3

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE50_00:

                            // Doing dest + source >> 1
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE56_25:

                            // Doing dest + source >> 1 - source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3
                            psrld   mm1, 3

                            // Subtract source from dest
                            psubd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 1
                            psrld   mm1, 1

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 3.
                            psrld   mm1, 3

                            // Subtract source from dest
                            psubd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE62_50:

                            // Doing dest + source >> 2 + source >> 3
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE68_75:

                            // Doing dest + source >> 2 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 2
                            psrld   mm1, 2

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE75_00:

                            // Doing dest + source >> 2
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 2
                            psrld   mm1, 2

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE81_25:

                            // Doing dest + source >> 3 + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Shift source by 1.
                            psrld   mm1, 1

                            // Add source to dest
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE87_50:

                            // Doing dest + source >> 3
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 3
                            psrld   mm1, 3

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            jmp Write_Alpha_Data

                DO_ALPHA_OPAQUE93_75:

                            // Doing dest + source >> 4
                            // First do red & green - have mm0
                            // hold 2 copies of destination, and mm1
                            // hold 2 copies of source.
                            movd    eax, mm0    // store dest in eax, need another register
                            movq    mm1, mm0
                            psllq   mm0, 32     // prepare to or in the 2nd copy
                            movd    mm2, ebx    // start creating 2 copies of source
                            por     mm0, mm1

                            movd    mm1, ebx    // Finish 2nd copy of source in mm1
                            psllq   mm2, 32
                            pand    mm0, mm7    // mask destination bits for red + green
                            por     mm1, mm2
                            pand    mm1, mm7    // mask source bits for red + green

                            // shift source by 4
                            psrld   mm1, 4

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask the result
                            pand    mm0, mm7

                            // And or it together.
                            movq    mm1, mm0
                            psrlq   mm0, 32
                            por     mm1, mm0

                            // Save the result in edx
                            movd    edx, mm1

                            // Now do blue.
                            movq    mm2, mm6    // Get the mask for blue.
                            movd    mm0, eax    // destination in mm0
                            psrlq   mm2, 32     // mask is in high mm2.
                            movd    mm1, ebx    // source in mm1

                            // Mask source and dest.
                            pand    mm0, mm2
                            pand    mm1, mm2

                            // shift source by 4
                            psrld   mm1, 4

                            // add source to dest.
                            paddd   mm0, mm1

                            // Mask one final time.
                            pand    mm0, mm2

                            // And or it into our result
                            movd    eax, mm0
                            or      eax, edx

                            // Now write the word.
        Write_Alpha_Data:
                      // Write the new pixel.
                    mov     word ptr [edi], ax
                    shr     eax, 16
                    mov     byte ptr [edi + 2], al

    FinishedWithPixel:

            inc     ecx         // Increase pixels done counter
            add     edi, 3      // Increase the destination pointer.
            jmp     StartLineBlit

EndOfLineBlit:
        // Increase pointers to source and destination to point
        // to next line.
        mov     eax, dword ptr [nXDestinationWidthInBytes]
        add     dword ptr [lpDestinationBeginBlt], eax

        mov     eax, dword ptr [nXSourceWidthInBytes]
        add     dword ptr [lpSourceBeginBlt], eax

        jmp     ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop     EDI
        pop     ESI

        EMMS        // Empty MMX state - allows floating point to work.
    };

}

void LI_MMXAlphaBitBlt16to16(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    int nAlphaValue)
{
    LPBYTE  lpDestinationBeginBlt;      //destination Scan Line Pointer
    int     nXDestinationWidthInBytes;  //destination Bitmap Width
    LPBYTE  lpSourceBeginBlt;           //source Scan Line Pointer
    int     nXSourceWidthInBytes;       //source Bitmap Width
    int     nPixelsAcrossToBlt;         //# pixels across to blt
    int nLinesToBlt;                    //# lines down to blt
    int     wPureGreen;                 //Pure green color cache
    static DWORDLONG wPureGreen64;      //Pure green 64 bits.
    static DWORDLONG wRedMask64;        //Red mask
    static DWORDLONG wGreenMask64;      //Green mask
    static DWORDLONG wBlueMask64;       //Blue Mask
    static BOOL set_offsets = FALSE;    //Check to see if offsets set
    static DWORD    jump_array[17];     //Keeps offsets to alpha
                                        //routines we jump to
    DWORD * jump_array_point;           //So inline assembly can find
                                        //the Jump Array

    wPureGreen = LI_BLT16ConvertColorRefTo16BitColor(
            LE_GRAFIX_MakeColorRef( 0, 255, 0 ));
    //Setup variables
    nXDestinationWidthInBytes = (( nXDestinationWidthInPixels * 2 ) + 3 )
                    & 0xFFFFFFFC;
    nXSourceWidthInBytes = (( nXSourceWidthInPixels * 2 ) + 3 )
                    & 0xFFFFFFFC;
    jump_array_point = jump_array;
    //Source and destination widths are built like this because they are
    //DIBitmaps and are aligned along 4 byte boundaries...
    lpDestinationBeginBlt = lpDestinationBits +
                ( nYBltStartCoordinate *
                nXDestinationWidthInBytes ) +
                ( nXBltStartCoordinate * 2 );
    lpSourceBeginBlt = lpSourceBits +
                ( lprSourceBltRect->top *
                nXSourceWidthInBytes ) +
                ( lprSourceBltRect->left * 2 );
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    __asm
    {
        //AG: Code borrowed from the 8 -> 16 Alpha blt. (thanks AS)
        // First, save registers we'll be messing up that would not normally
        // be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            //Setting up the red, green, blue and pure green masks..

            movd mm0, wPureGreen
            psllq mm0, 48
            movq mm4, mm0
            psrlq mm0, 16
            por mm4, mm0
            psrlq mm0, 16
            por mm4, mm0
            psrlq mm0, 16
            por mm4, mm0
            movq wPureGreen64, mm4
            movd mm0, LE_BLT_dwRedMask
            psllq mm0, 48
            movq mm5, mm0
            psrlq mm0, 16
            por mm5, mm0
            psrlq mm0, 16
            por mm5, mm0
            psrlq mm0, 16
            por mm5, mm0
            movq wRedMask64, mm5
            movd mm0, LE_BLT_dwGreenMask
            psllq mm0, 48
            movq mm6, mm0
            psrlq mm0, 16
            por mm6, mm0
            psrlq mm0, 16
            por mm6, mm0
            psrlq mm0, 16
            por mm6, mm0
            movq wGreenMask64, mm6
            movd mm0, LE_BLT_dwBlueMask
            psllq mm0, 48
            movq mm7, mm0
            psrlq mm0, 16
            por mm7, mm0
            psrlq mm0, 16
            por mm7, mm0
            psrlq mm0, 16
            por mm7, mm0
            movq wBlueMask64, mm7

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        // Start by setting up the 64 bit versions of the red/green/blue masks.
            movq mm5, wRedMask64
            movq mm6, wGreenMask64
            movq mm7, wBlueMask64
        // Set up address of jump in EDX
            mov eax, nAlphaValue
            shr eax, 4
            inc eax
            shl eax, 2
            add eax, jump_array_point
            mov edx, dword ptr [eax]

        // We need to set up the loops.  EBX contains nLinesToBlt
            mov ebx, dword Ptr [nLinesToBlt]

ScanLineLoop:

        // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
        // Use EAX as a counter of how many pixels we
        // have written. Reset it for the next line. In the middle here for pairing
            xor eax, eax
            mov edi, dword ptr [lpDestinationBeginBlt]

            StartOfLineBlit:
            //We will be eating 4 pixels per go around here. Not necessarily writing
            //them all back....AG
            // Get the next 4 pixels. (source and destination)
                movq mm0, [esi]
                add esi, 8
                movq mm1, [edi]

            // mm0 has up to 4 valid source pixels....
            // mm1 has up to 4 valid destination pixels..
            // EDX has the address to jump to in our jump table
            // EAX is our pixel counter
            // EBX is our line counter
            // ESI are our source bits.
            // EDI are our destination bits
            // mm5, mm6, mm7 have the red, green and blue 64 bit masks (4 16 bit masks)
            // mm2, mm3 are scratch
            // mm4 will contain the pixels to be written at the end of this.
            //WHIEW....AG.

            // Now, jump to edx (jump table value)

                jmp edx

            DO_ALPHA_OPAQUE06_25:
                //Do red first....
                //Source...Using mm4 to clear it with new data.
                movq mm4, mm0
                pand mm4, mm5

                psrlw mm4, 4

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                paddusw mm4, mm2
                psrlw mm2, 4
                psubusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm3, mm0
                pand mm3, mm6

                psrlw mm3, 4

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                paddusw mm3, mm2
                psrlw mm2, 4
                psubusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm3, mm0
                pand mm3, mm7

                psrlw mm3, 4

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                paddusw mm3, mm2
                psrlw mm2, 4
                psubusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE12_50:
                //Do red first
                //Source...
                movq mm4, mm0
                pand mm4, mm5

                psrlw mm4, 3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                paddusw mm4, mm2
                psrlw mm2, 3
                psubusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm3, mm0
                pand mm3, mm6

                psrlw mm3, 3

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                paddusw mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm3, mm0
                pand mm3, mm7

                psrlw mm3, 3

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                paddusw mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE18_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 3
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 2
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 3
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 3
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE25_00:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 2
                movq mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 1
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 2
                movq mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 2
                movq mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE31_25:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 2
                movq mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 2
                paddusw mm4, mm2
                psrlw mm2, 1
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE37_50:

                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 2
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 2
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE43_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 3
                psubusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 3
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 3
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 3
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE50_00:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm4, mm0
                pand mm4, mm5

                psrlw mm4, 1

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm3, mm0
                pand mm3, mm6

                psrlw mm3, 1

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm3, mm0
                pand mm3, mm7

                psrlw mm3, 1

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE56_25:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 3
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 1
                paddusw mm4, mm2
                psrlw mm2, 3
                psubusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE62_50:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 2
                paddusw mm4, mm2
                psrlw mm2, 1
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE68_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 2
                paddusw mm4, mm2
                psrlw mm2, 2
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE75_00:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 2
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE81_25:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 3
                paddusw mm4, mm2
                psrlw mm2, 1
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 3
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 3
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE87_50:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                movq mm4, mm3
                psrlw mm3, 3
                psubusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 3
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 3
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 3
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE93_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                movq mm4, mm3
                psrlw mm3, 4
                psubusw mm4, mm3

                //Destination
                movq mm2, mm1
                pand mm2, mm5

                psrlw mm2, 4
                paddusw mm4, mm2

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                movq mm3, mm2
                psrlw mm2, 4
                psubusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm6

                psrlw mm2, 4
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                movq mm3, mm2
                psrlw mm2, 4
                psubusw mm3, mm2

                //Destination
                movq mm2, mm1
                pand mm2, mm7

                psrlw mm2, 4
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

        Write_Alpha_Data:
            //Okay, we've done the math for four pixels, but do we have
            //any that were transparent?
            pcmpeqw mm0, wPureGreen64
            //mm0 now contains a bit mask masking out all except those that
            //are pure green
            pand mm1, mm0
            //mm1 (background pixels) now contains only those pixels that are
            //transparent
            pand mm0, mm4
            //mm0 now contains only those pixels from the mixed that are invalid.
            pxor mm0, mm4
            //mm0 now has 0x0000 in the pixels that were pure green
            por mm0, mm1
            //Or the old background pixels into the holes in the forground..
            //mm0 now contains the data we want to write...

            //Now, check to see if we're close to the end of the line.
            add eax, 4
            cmp eax, nPixelsAcrossToBlt
            jg LessThanFourPixels
            //If not less than four pixels, write the whole thing...
            movq [edi], mm0
            je EndOfLineBlit    //Those were the last four pixels.
            add edi, 8
            jmp StartOfLineBlit
        LessThanFourPixels:
            //This is the end of the line, and there are either 1, 2 or 3
            //pixels to write....check on this now.
            sub eax, nPixelsAcrossToBlt
            //eax now contains a number [1 2 3]
            //if eax=1 3 pixels
            //       2 2 pixels
            //       3 1 pixel.
            cmp eax, 3
            je OnlyOnePixel
            movd [edi], mm0
            cmp eax, 1
            jne EndOfLineBlit
            add edi, 4
            //Shift the two pixels we've written out.
            psrlq mm0, 32
        OnlyOnePixel:
            //We can't just move 16 bits from the MMX...move it to the
            //cx register and stuff it through that way.
            movd ecx, mm0
            mov [edi], cx
            //To the end of line BLT.

EndOfLineBlit:
        //Jump and compare here for jump prediction (follow thru)
        cmp ebx, 1
        je  EndOfBlit

        // Subtract one from the lines we have left.
        dec ebx

        mov edi, dword ptr [nXDestinationWidthInBytes]
        add dword ptr [lpDestinationBeginBlt], edi

        mov esi, dword ptr [nXSourceWidthInBytes]
        add dword ptr [lpSourceBeginBlt], esi

        jmp ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop EDI
        pop ESI
        //Reset state of FPU
        EMMS
    };

}


/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt24to24
*
*   Description:  blts from a 24 bit bitmap to a 24 bit bitmap,
*         using a passed transparency value and MMX inline assembly
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - destination bitmap width
*         nXBltStartCoordinate - X coord, ul corner of blt RECT
*         nYBltStartCoordinate - Y coord, ul corner of blt RECT
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - source bitmap width
*         lprSourceBltRect - source blt RECT
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXAlphaBitBlt24to24(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    int nAlphaValue)
{
    LPBYTE lpDestinationBeginBlt;       // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;      // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;            // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;           // source bitmap width in bytes
    int nPixelsAcrossToBlt;             // blt width in pixels
    int nLinesToBlt;                    // blt height in pixels
    DWORD dwPureGreen;                  // Pure Green Cache..
    DWORD dw24BitMask=0x00FFFFFF;       // 32 bit 24 bit color mask.
    DWORD GarbageMask=0xFFFF0000;       // 32 bit garbage mask.
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                        // Hacking my way around it.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = (( nXDestinationWidthInPixels * 3 ) + 3 ) & 0xfffffffc;
    nXSourceWidthInBytes = (( nXSourceWidthInPixels * 3 ) + 3 ) & 0xfffffffc;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits +
                            (nYBltStartCoordinate * nXDestinationWidthInBytes);
    lpDestinationBeginBlt += nXBltStartCoordinate * 3;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left * 3;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    dwPureGreen = LI_BLT24ConvertColorRefTo24BitColor(LE_GRAFIX_MakeColorRef(0,255,0));

    jump_array_point = jump_array;

    __asm
    {

        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        //Set mm3 equal to where we need to jump to.
        mov eax, nAlphaValue
        shr eax, 4
        inc eax
        shl eax, 2
        add eax, jump_array_point
        mov eax, dword ptr [eax]
        movd mm3, eax
        //Set up masks in MMX registers.
        movd mm7, GarbageMask
        psllq mm7, 32
        movd mm5, nPixelsAcrossToBlt
        movd mm4, nLinesToBlt

        //Set masks up in registers.
        mov ebx, dwPureGreen
        mov ecx, dw24BitMask

        // Assume at least one line to blt(will change later if necessary)

        // Set source pointer.
        mov esi, lpSourceBeginBlt
        // Set destination pointer
        mov edi, lpDestinationBeginBlt
        //Assume at least one pixel to blt (ie: minimum blt >= (1,1))
StartLineBlt:
        //Okay, to recap where everything is:
        //MM7   high garbage mask
        //MM6   Garbage bits
        //MM5   Number of Pixels across
        //MM4   Number of Lines to blt
        //MM3   Jump Table Point
        //MM2   Write Point.
        //MM1   Destination Pixel
        //MM0   Source Pixel
        //EAX   Pixels across counter.
        //EBX   Pure Green
        //ECX   24 bit mask
        //EDX   Scratch Space

        movd eax, mm5

GetNextPixel:
        //Snag the first pixel DX
        mov edx, [esi]
        add esi, 3
        and edx, ecx
        cmp edx, ebx
        je DoneWithPixel
        //Move the destination bits into mm1 unpacking them...Clear mm1 first
        //And move the source pixel into mm0 unpacking it as well.
        movd mm0, edx
        pxor mm1, mm1
        punpcklbw mm1, [edi]
        pxor mm2, mm2
        punpcklbw mm0, mm2
        //Shift the destination into proper place
        psrlq mm1, 8
        //Save the garbage...
        movq mm7, mm6
        pand mm6, mm1
        //Move the jump point into edx
        movd edx, mm3
        //Jump to point in jump table.
        jmp edx
DO_ALPHA_OPAQUE06_25:
        //Shift and add source
        psrlw mm0, 4
        movq mm2, mm0

        //Shift and add destination
        paddusw mm2, mm1
        psrlw mm1, 4
        psubusw mm2, mm1

        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE12_50:
        //Source
        psrlw mm0, 3
        movq mm2, mm0

        //Destination
        paddusw mm2, mm1
        psrlw mm1, 3
        psubusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE18_75:
        //Source
        psrlw mm0, 3
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write it.
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE25_00:
        //Source
        psrlw mm0, 2
        movq mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE31_25:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE37_50:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE43_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 3
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE50_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE56_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 3
        psubusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE62_50:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 2
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE68_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 2
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE75_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE81_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 3
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE87_50:
        //Source
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Destination
        psrlw mm1, 3
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE93_75:
        //Source
        movq mm2, mm0
        psrlw mm0, 4
        psubusw mm2, mm0

        //Destination
        psrlw mm1, 4
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

Write_Alpha_Data:
        //Restore Garbage
        por mm2, mm7
        pxor mm2, mm7
        por mm2, mm6

        //Re-pack words into bytes
        packuswb mm2, mm2

        //Move into destination
        movd [edi], mm2
DoneWithPixel:
        add edi, 3

        dec eax
        jz EndLineBlt

        jmp GetNextPixel
EndLineBlt:
        //Move from mm4 (lines down left) to edx
        movd edx, mm4
        dec edx
        jz EndBlt
        //Put back the lines down left into mm4
        movd mm4, edx
        //Get source and destination pointers, and increment them
        mov esi, lpSourceBeginBlt
        mov edi, lpDestinationBeginBlt
        add esi, nXSourceWidthInBytes
        add edi, nXDestinationWidthInBytes
        mov lpSourceBeginBlt, esi
        mov lpDestinationBeginBlt, edi
        jmp StartLineBlt
EndBlt:
        //Restore pointers...
        pop EDI
        pop ESI
        //Restore FPU
        EMMS
    };
}


/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt32to32
*
*   Description:  blts from a 32 bit bitmap to a 32 bit bitmap,
*         using a passed transparency value and MMX inline assembly
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - destination bitmap width
*         nXBltStartCoordinate - X coord, ul corner of blt RECT
*         nYBltStartCoordinate - Y coord, ul corner of blt RECT
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - source bitmap width
*         lprSourceBltRect - source blt RECT
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXAlphaBitBlt32to32(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    int nAlphaValue)
{
    LPBYTE lpDestinationBeginBlt;       // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;      // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;            // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;           // source bitmap width in bytes
    int nPixelsAcrossToBlt;             // blt width in pixels
    int nLinesToBlt;                    // blt height in pixels
    DWORD dwPureGreen;                  // Pure Green Cache..
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                        // Hacking my way around it.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
    nXSourceWidthInBytes = nXSourceWidthInPixels * 4;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits +
                            (nYBltStartCoordinate * nXDestinationWidthInBytes);
    lpDestinationBeginBlt += nXBltStartCoordinate * 4;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left * 4;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    dwPureGreen = LI_BLT32ConvertColorRefTo32BitColor(LE_GRAFIX_MakeColorRef(0,255,0));

    jump_array_point = jump_array;

    __asm
    {

        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        //Set mm3 equal to where we need to jump to.
        mov eax, nAlphaValue
        shr eax, 4
        inc eax
        shl eax, 2
        add eax, jump_array_point
        mov eax, dword ptr [eax]
        movd mm3, eax
        //Set up masks in MMX registers.
        movd mm5, nPixelsAcrossToBlt
        movd mm4, nLinesToBlt

        //Set masks up in registers.
        mov ebx, dwPureGreen

        // Assume at least one line to blt(will change later if necessary)

        // Set source pointer.
        mov esi, lpSourceBeginBlt
        // Set destination pointer
        mov edi, lpDestinationBeginBlt
        //Assume at least one pixel to blt (ie: minimum blt >= (1,1))
StartLineBlt:
        //Okay, to recap where everything is:
        //MM7   high garbage mask
        //MM6   Garbage bits
        //MM5   Number of Pixels across
        //MM4   Number of Lines to blt
        //MM3   Jump Table Point
        //MM2   Write Point.
        //MM1   Destination Pixel
        //MM0   Source Pixel
        //EAX   Pixels across counter.
        //EBX   Pure Green
        //ECX   Scratch Space
        //EDX   Scratch Space

        movd eax, mm5

GetNextPixel:
        //Snag the first pixel DX
        mov edx, [esi]
        add esi, 4
        cmp edx, ebx
        je DoneWithPixel
        //Move the destination bits into mm1 unpacking them...Clear mm1 first
        //And move the source pixel into mm0 unpacking it as well.
        movd mm0, edx
        pxor mm1, mm1
        punpcklbw mm1, [edi]
        pxor mm2, mm2
        punpcklbw mm0, mm2
        //Shift the destination into proper place
        psrlq mm1, 8

        //Move the jump point into edx
        movd edx, mm3
        //Jump to point in jump table.
        jmp edx
DO_ALPHA_OPAQUE06_25:
        //Shift and add source
        psrlw mm0, 4
        movq mm2, mm0

        //Shift and add destination
        paddusw mm2, mm1
        psrlw mm1, 4
        psubusw mm2, mm1

        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE12_50:
        //Source
        psrlw mm0, 3
        movq mm2, mm0

        //Destination
        paddusw mm2, mm1
        psrlw mm1, 3
        psubusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE18_75:
        //Source
        psrlw mm0, 3
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write it.
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE25_00:
        //Source
        psrlw mm0, 2
        movq mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE31_25:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE37_50:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE43_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 3
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE50_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE56_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 1
        paddusw mm2, mm1
        psrlw mm1, 3
        psubusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE62_50:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 2
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE68_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 2
        paddusw mm2, mm1
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE75_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 2
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE81_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Destination
        psrlw mm1, 3
        paddusw mm2, mm1
        psrlw mm1, 1
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE87_50:
        //Source
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Destination
        psrlw mm1, 3
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE93_75:
        //Source
        movq mm2, mm0
        psrlw mm0, 4
        psubusw mm2, mm0

        //Destination
        psrlw mm1, 4
        paddusw mm2, mm1

        //Write
        jmp Write_Alpha_Data

Write_Alpha_Data:
        //Re-pack words into bytes
        packuswb mm2, mm2

        //Move into destination
        movd [edi], mm2
DoneWithPixel:
        add edi, 4

        dec eax
        jz EndLineBlt

        jmp GetNextPixel
EndLineBlt:
        //Move from mm4 (lines down left) to edx
        movd edx, mm4
        dec edx
        jz EndBlt
        //Put back the lines down left into mm4
        movd mm4, edx
        //Get source and destination pointers, and increment them
        mov esi, lpSourceBeginBlt
        mov edi, lpDestinationBeginBlt
        add esi, nXSourceWidthInBytes
        add edi, nXDestinationWidthInBytes
        mov lpSourceBeginBlt, esi
        mov lpDestinationBeginBlt, edi
        jmp StartLineBlt
EndBlt:
        //Restore pointers...
        pop EDI
        pop ESI
        //Restore FPU
        EMMS
    };
}

/*******************************************************************************
*
*   Name:         LI_MMXFade16
*
*   Description:  blts 16 bitmap to a 16 bitmap, fading to black (0,0,0).
*         using a passed transparency value.
*         ie: (putting the bitmap over a black background making the
*         image more and more transparent (or less and less for a
*         fade in)
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - width in pixels of the destination
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - width in pixels of the source
*         lprSourceBltRect - source blit rectangle (in case source is bigger
*                   than the destination, need to clip it)
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXFade16(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    int nAlphaValue)
{
    LPBYTE  lpDestinationBeginBlt;      //destination Scan Line Pointer
    int     nXDestinationWidthInBytes;  //destination Bitmap Width
    LPBYTE  lpSourceBeginBlt;           //source Scan Line Pointer
    int     nXSourceWidthInBytes;       //source Bitmap Width
    int     nPixelsAcrossToBlt;         //# pixels across to blt
    int     nLinesToBlt;                //# lines down to blt
    int     wPureGreen;                 //Pure green color cache
    static DWORDLONG wPureGreen64;      //Pure green 64 bits.
    static DWORDLONG wRedMask64;        //Red mask
    static DWORDLONG wGreenMask64;      //Green mask
    static DWORDLONG wBlueMask64;       //Blue Mask
    static BOOL set_offsets = FALSE;    //Check to see if offsets set
    static DWORD    jump_array[17];     //Keeps offsets to alpha
                                        //routines we jump to
    DWORD * jump_array_point;           //So inline assembly can find
                                        //the Jump Array

    wPureGreen = LI_BLT16ConvertColorRefTo16BitColor(
            LE_GRAFIX_MakeColorRef( 0, 255, 0 ));
    //Setup variables
    nXDestinationWidthInBytes = (( nXDestinationWidthInPixels * 2 ) + 3 )
                    & 0xFFFFFFFC;
    nXSourceWidthInBytes = (( nXSourceWidthInPixels * 2 ) + 3 )
                    & 0xFFFFFFFC;
    jump_array_point = jump_array;
    //Source and destination widths are built like this because they are
    //DIBitmaps and are aligned along 4 byte boundaries...
    lpDestinationBeginBlt = lpDestinationBits;
    lpSourceBeginBlt = lpSourceBits +
                ( lprSourceBltRect->top *
                nXSourceWidthInBytes ) +
                ( lprSourceBltRect->left * 2 );
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    __asm
    {
    //AG: Code borrowed from the 8 -> 16 Alpha blt. (thanks AS)
        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            //Setting up the red, green, blue and pure green masks..

            movd mm0, wPureGreen
            psllq mm0, 48
            movq mm4, mm0
            psrlq mm0, 16
            por mm4, mm0
            psrlq mm0, 16
            por mm4, mm0
            psrlq mm0, 16
            por mm4, mm0
            movq wPureGreen64, mm4
            movd mm0, LE_BLT_dwRedMask
            psllq mm0, 48
            movq mm5, mm0
            psrlq mm0, 16
            por mm5, mm0
            psrlq mm0, 16
            por mm5, mm0
            psrlq mm0, 16
            por mm5, mm0
            movq wRedMask64, mm5
            movd mm0, LE_BLT_dwGreenMask
            psllq mm0, 48
            movq mm6, mm0
            psrlq mm0, 16
            por mm6, mm0
            psrlq mm0, 16
            por mm6, mm0
            psrlq mm0, 16
            por mm6, mm0
            movq wGreenMask64, mm6
            movd mm0, LE_BLT_dwBlueMask
            psllq mm0, 48
            movq mm7, mm0
            psrlq mm0, 16
            por mm7, mm0
            psrlq mm0, 16
            por mm7, mm0
            psrlq mm0, 16
            por mm7, mm0
            movq wBlueMask64, mm7

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        // Start by setting up the 64 bit versions of the red/green/blue masks.
            movq mm5, wRedMask64
            movq mm6, wGreenMask64
            movq mm7, wBlueMask64
        // Set up address of jump in EDX
            mov eax, nAlphaValue
            shr eax, 4
            inc eax
            shl eax, 2
            add eax, jump_array_point
            mov edx, dword ptr [eax]

        // We need to set up the loops.  EBX contains nLinesToBlt
            mov ebx, dword Ptr [nLinesToBlt]

ScanLineLoop:

        // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
        // Use EAX as a counter of how many pixels we
        // have written. Reset it for the next line. In the middle here for pairing
            xor eax, eax
            mov edi, dword ptr [lpDestinationBeginBlt]

            StartOfLineBlit:
            //We will be eating 4 pixels per go around here. Not necessarily writing
            //them all back....AG
            // Get the next 4 pixels. (source and destination)
                movq mm0, [esi]
                add esi, 8
            // mm0 has up to 4 valid source pixels....
            // EDX has the address to jump to in our jump table
            // EAX is our pixel counter
            // EBX is our line counter
            // ESI are our source bits.
            // EDI are our destination bits
            // mm5, mm6, mm7 have the red, green and blue 64 bit masks (4 16 bit masks)
            // mm1, mm2, mm3 are scratch
            // mm4 will contain the pixels to be written at the end of this.
            //WHIEW....AG.

            // Now, jump to edx (jump table value)

                jmp edx

            DO_ALPHA_OPAQUE06_25:
                //Do red first....
                //Source...Using mm4 to clear it with new data.
                movq mm4, mm0
                pand mm4, mm5

                psrlw mm4, 4

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm3, mm0
                pand mm3, mm6

                psrlw mm3, 4

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm3, mm0
                pand mm3, mm7

                psrlw mm3, 4

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE12_50:
                //Do red first
                //Source...
                movq mm4, mm0
                pand mm4, mm5

                psrlw mm4, 3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm3, mm0
                pand mm3, mm6

                psrlw mm3, 3

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm3, mm0
                pand mm3, mm7

                psrlw mm3, 3

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE18_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 3
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 3
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 3
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE25_00:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 2
                movq mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 2
                movq mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 2
                movq mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE31_25:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 2
                movq mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE37_50:

                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 2
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 2
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE43_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 3
                psubusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE50_00:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm4, mm0
                pand mm4, mm5

                psrlw mm4, 1

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm3, mm0
                pand mm3, mm6

                psrlw mm3, 1

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm3, mm0
                pand mm3, mm7

                psrlw mm3, 1

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE56_25:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 3
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 3
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE62_50:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE68_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE75_00:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE81_25:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                psrlw mm3, 1
                movq mm4, mm3
                psrlw mm3, 1
                paddusw mm4, mm3
                psrlw mm3, 2
                paddusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                psrlw mm2, 1
                movq mm3, mm2
                psrlw mm2, 1
                paddusw mm3, mm2
                psrlw mm2, 2
                paddusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE87_50:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                movq mm4, mm3
                psrlw mm3, 3
                psubusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                movq mm3, mm2
                psrlw mm2, 3
                psubusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

            DO_ALPHA_OPAQUE93_75:
                //Do red first
                //Source...Make sure it ends up in mm4
                movq mm3, mm0
                pand mm3, mm5

                movq mm4, mm3
                psrlw mm3, 4
                psubusw mm4, mm3

                //Mask result red
                pand mm4, mm5

                //Green
                //Source...
                movq mm2, mm0
                pand mm2, mm6

                movq mm3, mm2
                psrlw mm2, 4
                psubusw mm3, mm2

                //Mask result green
                pand mm3, mm6

                //Combine green and red.
                por mm4, mm3

                //Now blue...
                //Source
                movq mm2, mm0
                pand mm2, mm7

                movq mm3, mm2
                psrlw mm2, 4
                psubusw mm3, mm2

                //Mask result blue.
                pand mm3, mm7

                //Combine blue with red and green.
                por mm4, mm3

                jmp Write_Alpha_Data

        Write_Alpha_Data:
            //Now, check to see if we're close to the end of the line.
            add eax, 4
            cmp eax, nPixelsAcrossToBlt
            jg LessThanFourPixels
            //If not less than four pixels, write the whole thing...
            movq [edi], mm4
            je EndOfLineBlit    //Those were the last four pixels.
            add edi, 8
            jmp StartOfLineBlit
        LessThanFourPixels:
            //This is the end of the line, and there are either 1, 2 or 3
            //pixels to write....check on this now.
            sub eax, nPixelsAcrossToBlt
            //eax now contains a number [1 2 3]
            //if eax=1 3 pixels
            //       2 2 pixels
            //       3 1 pixel.
            cmp eax, 3
            je OnlyOnePixel
            movd [edi], mm4
            cmp eax, 1
            jne EndOfLineBlit
            add edi, 4
            //Shift the two pixels we've written out.
            psrlq mm3, 32
        OnlyOnePixel:
            //We can't just move 16 bits from the MMX...move it to the
            //cx register and stuff it through that way.
            movd ecx, mm4
            mov [edi], cx
            //To the end of line BLT.

EndOfLineBlit:
        //Jump and compare here for jump prediction (follow thru)
        cmp ebx, 1
        je  EndOfBlit

        // Subtract one from the lines we have left.
        dec ebx

        mov edi, dword ptr [nXDestinationWidthInBytes]
        add dword ptr [lpDestinationBeginBlt], edi

        mov esi, dword ptr [nXSourceWidthInBytes]
        add dword ptr [lpSourceBeginBlt], esi

        jmp ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop EDI
        pop ESI
        //Reset state of FPU
        EMMS
    };

}

/*******************************************************************************
*
*   Name:         LI_MMXFade24
*
*   Description:  blts 24 bitmap to a 24 bitmap, fading to black (0,0,0).
*         using a passed transparency value.
*         ie: (putting the bitmap over a black background making the
*         image more and more transparent (or less and less for a
*         fade in)
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - width in pixels of the destination
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - width in pixels of the source
*         lprSourceBltRect - source blit rectangle (in case source is bigger
*                   than the destination, need to clip it)
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXFade24(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    int nAlphaValue)
{
    LPBYTE lpDestinationBeginBlt;       // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;      // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;            // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;           // source bitmap width in bytes
    int nPixelsAcrossToBlt;             // blt width in pixels
    int nLinesToBlt;                    // blt height in pixels
    DWORD dwPureGreen;                  // Pure Green Cache..
    DWORD dw24BitMask=0x00FFFFFF;       // 32 bit 24 bit color mask.
    DWORD GarbageMask=0xFFFF0000;       // 32 bit garbage mask.
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                        // Hacking my way around it.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = (( nXDestinationWidthInPixels * 3 ) + 3 ) & 0xfffffffc;
    nXSourceWidthInBytes = (( nXSourceWidthInPixels * 3 ) + 3 ) & 0xfffffffc;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left * 3;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    dwPureGreen = LI_BLT24ConvertColorRefTo24BitColor(LE_GRAFIX_MakeColorRef(0,255,0));

    jump_array_point = jump_array;

    __asm
    {

        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        //Set mm3 equal to where we need to jump to.
        mov eax, nAlphaValue
        shr eax, 4
        inc eax
        shl eax, 2
        add eax, jump_array_point
        mov eax, dword ptr [eax]
        movd mm3, eax
        //Set up masks in MMX registers.
        movd mm7, GarbageMask
        psllq mm7, 32
        movd mm5, nPixelsAcrossToBlt
        movd mm4, nLinesToBlt

        //Set masks up in registers.
        mov ebx, dwPureGreen
        mov ecx, dw24BitMask

        // Assume at least one line to blt(will change later if necessary)

        // Set source pointer.
        mov esi, lpSourceBeginBlt
        // Set destination pointer
        mov edi, lpDestinationBeginBlt
        //Assume at least one pixel to blt (ie: minimum blt >= (1,1))
StartLineBlt:
        //Okay, to recap where everything is:
        //MM7   high garbage mask
        //MM6   Garbage bits
        //MM5   Number of Pixels across
        //MM4   Number of Lines to blt
        //MM3   Jump Table Point
        //MM2   Write Point.
        //MM1   Destination Pixel
        //MM0   Source Pixel
        //EAX   Pixels across counter.
        //EBX   Pure Green
        //ECX   24 bit mask
        //EDX   Scratch Space

        movd eax, mm5

GetNextPixel:
        //Snag the first pixel and unpack into mm0
        movd mm0, [esi]
        pxor mm1, mm1
        punpcklbw mm0, mm1
        add esi, 3

        //Save the garbage...
        movq mm7, mm6
        pand mm6, mm0

        //Move the jump point into edx
        movd edx, mm3
        //Jump to point in jump table.
        jmp edx

DO_ALPHA_OPAQUE06_25:
        //Shift and add source
        psrlw mm0, 4
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE12_50:
        //Source
        psrlw mm0, 3
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE18_75:
        //Source
        psrlw mm0, 3
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write it.
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE25_00:
        //Source
        psrlw mm0, 2
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE31_25:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE37_50:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE43_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE50_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE56_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE62_50:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE68_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE75_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE81_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE87_50:
        //Source
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE93_75:
        //Source
        movq mm2, mm0
        psrlw mm0, 4
        psubusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

Write_Alpha_Data:
        //Restore Garbage
        por mm2, mm7
        pxor mm2, mm7
        por mm2, mm6

        //Re-pack words into bytes
        packuswb mm2, mm2

        //Move into destination
        movd [edi], mm2

        add edi, 3

        dec eax
        jz EndLineBlt

        jmp GetNextPixel
EndLineBlt:
        //Move from mm4 (lines down left) to edx
        movd edx, mm4
        dec edx
        jz EndBlt
        //Put back the lines down left into mm4
        movd mm4, edx
        //Get source and destination pointers, and increment them
        mov esi, lpSourceBeginBlt
        mov edi, lpDestinationBeginBlt
        add esi, nXSourceWidthInBytes
        add edi, nXDestinationWidthInBytes
        mov lpSourceBeginBlt, esi
        mov lpDestinationBeginBlt, edi
        jmp StartLineBlt
EndBlt:
        //Restore pointers...
        pop EDI
        pop ESI
        //Restore FPU
        EMMS
    };
}


/*******************************************************************************
*
*   Name:         LI_MMXFade32
*
*   Description:  blts 32 bitmap to a 32 bitmap, fading to black (0,0,0).
*         using a passed transparency value.
*         ie: (putting the bitmap over a black background making the
*         image more and more transparent (or less and less for a
*         fade in)
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - width in pixels of the destination
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - width in pixels of the source
*         lprSourceBltRect - source blit rectangle (in case source is bigger
*                   than the destination, need to clip it)
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXFade32(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    LPBYTE lpSourceBits,
    int nXSourceWidthInPixels,
    LPRECT lprSourceBltRect,
    int nAlphaValue)
{
    LPBYTE lpDestinationBeginBlt;       // pointer to destination scanline position to begin blt
    int nXDestinationWidthInBytes;      // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;            // pointer to source scanline position to begin blt
    int nXSourceWidthInBytes;           // source bitmap width in bytes
    int nPixelsAcrossToBlt;             // blt width in pixels
    int nLinesToBlt;                    // blt height in pixels
//    DWORD dwPureGreen;                  // Pure Green Cache..
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                        // Hacking my way around it.

    // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
    nXSourceWidthInBytes = nXSourceWidthInPixels * 4;

    // calculate start position of blt in destination bitmap
    lpDestinationBeginBlt = lpDestinationBits;

    // calculate start position of blt in source bitmap
    lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
    lpSourceBeginBlt += lprSourceBltRect->left * 4;

    // calculate dimensions of blt
    nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
    nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    jump_array_point = jump_array;

    __asm
    {

        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        //Set mm3 equal to where we need to jump to.
        mov eax, nAlphaValue
        shr eax, 4
        inc eax
        shl eax, 2
        add eax, jump_array_point
        mov eax, dword ptr [eax]
        movd mm3, eax
        //Set up masks in MMX registers.
        movd mm5, nPixelsAcrossToBlt
        movd mm4, nLinesToBlt

        // Assume at least one line to blt(will change later if necessary)

        // Set source pointer.
        mov esi, lpSourceBeginBlt
        // Set destination pointer
        mov edi, lpDestinationBeginBlt
        //Assume at least one pixel to blt (ie: minimum blt >= (1,1))
StartLineBlt:
        //Okay, to recap where everything is:
        //MM5   Number of Pixels across
        //MM4   Number of Lines to blt
        //MM3   Jump Table Point
        //MM2   Write Point.
        //MM1   Destination Pixel
        //MM0   Source Pixel
        //EAX   Pixels across counter.
        //ECX   Scratch Space
        //EDX   Scratch Space

        movd eax, mm5

GetNextPixel:
        pxor mm0, mm0
        punpcklbw mm0, [esi]
        //Shift the pixels into the proper place
        psrlq mm0, 8
        add esi, 4

        //Move the jump point into edx
        movd edx, mm3
        //Jump to point in jump table.
        jmp edx
DO_ALPHA_OPAQUE06_25:
        //Shift and add source
        psrlw mm0, 4
        movq mm2, mm0

        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE12_50:
        //Source
        psrlw mm0, 3
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE18_75:
        //Source
        psrlw mm0, 3
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write it.
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE25_00:
        //Source
        psrlw mm0, 2
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE31_25:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE37_50:
        //Source
        psrlw mm0, 2
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE43_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE50_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE56_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 3
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE62_50:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE68_75:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE75_00:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE81_25:
        //Source
        psrlw mm0, 1
        movq mm2, mm0
        psrlw mm0, 1
        paddusw mm2, mm0
        psrlw mm0, 2
        paddusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE87_50:
        //Source
        movq mm2, mm0
        psrlw mm0, 3
        psubusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

DO_ALPHA_OPAQUE93_75:
        //Source
        movq mm2, mm0
        psrlw mm0, 4
        psubusw mm2, mm0

        //Write
        jmp Write_Alpha_Data

Write_Alpha_Data:
        //Re-pack words into bytes
        packuswb mm2, mm2

        //Move into destination
        movd [edi], mm2

        add edi, 4

        dec eax
        jz EndLineBlt

        jmp GetNextPixel
EndLineBlt:
        //Move from mm4 (lines down left) to edx
        movd edx, mm4
        dec edx
        jz EndBlt
        //Put back the lines down left into mm4
        movd mm4, edx
        //Get source and destination pointers, and increment them
        mov esi, lpSourceBeginBlt
        mov edi, lpDestinationBeginBlt
        add esi, nXSourceWidthInBytes
        add edi, nXDestinationWidthInBytes
        mov lpSourceBeginBlt, esi
        mov lpDestinationBeginBlt, edi
        jmp StartLineBlt
EndBlt:
        //Restore pointers...
        pop EDI
        pop ESI
        //Restore FPU
        EMMS
    };
}


#endif // Unused MMX blitter code.

#endif // if CE_ARTLIB_EnableSystemMMX
