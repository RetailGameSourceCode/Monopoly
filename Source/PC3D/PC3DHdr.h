// ===========================================================================
// Module:      PC3DHdr.h
//
// Description: Precompiled header file for the PC3D library
//
// Created:     April 20, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_PC3DHDR
#define INC_PC3DHDR

#include "Common.h"
#include "..\ArtLib\L_DXTypes.h"
#include "Debug.h"
#include "D3DUtils.h"
#include "MemManager.h"

#include "Array.h"
#include "FlexVert.h"
#include "IPoint.h"
#include "IRect.h"
#include "Shared.h"
#include "Tag.h"
#include "VectList.h"
#include "Vertex.h"
#include "HMDData.h"

class frame;  // Old frame class
class Frame;  // New Frame class
class mesh;   // Old mesh class
class Mesh;   // new Mesh class

#ifdef USE_OLD_FRAME
typedef frame         FrameType;
typedef mesh          MeshType;
#else
typedef Frame         FrameType;
typedef Mesh          MeshType;
#endif

#include "Surface.h"
#include "PC3D.h"


#endif // INC_PC3DHDR
// ===========================================================================
