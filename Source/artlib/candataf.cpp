/*****************************************************************************
 * FILE:        candataf.cpp
 *
 * DESCRIPTION: A 3D "CAN" file format reader memory mapper implementation
 *              for a data ID source.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/LibraryCode/candataf.cpp 4     11/26/98 3:05p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/LibraryCode/candataf.cpp $
 * 
 * 4     11/26/98 3:05p Agmsmith
 * Updated for ArtLib99.
 * 
 * 3     10/06/98 2:23p Agmsmith
 * Added library header and #defines to turn off this module if not used.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix3D

#include <assert.h>
#include <memory.h>
#include "l_can.h"



#define MAXMAPS 4
typedef struct {
  BOOL inUse;
  int  canID;
  int  dataID;
  int  filePos;
  int  totalSize;
} DataIDMap;
DataIDMap maps[MAXMAPS];

BOOL dmmOpen(struct MapperAction *m);
BOOL dmmClose(struct MapperAction *m);
BOOL dmmRead(struct MapperAction *m);
BOOL dmmMap(struct MapperAction *m);


// Implementations:

BOOL dmmOpen(struct MapperAction *m) {
  int slot;

  // Find a slot
  for (slot = 0; slot < MAXMAPS; slot++)
    if (!maps[slot].inUse)
      break;
  assert(slot != MAXMAPS);

  // Load from the datafile
  LE_DATA_AddRef(m->dataID);

  // Mark the slot as being in use.
  maps[slot].inUse = TRUE;
  maps[slot].canID = m->canID;
  maps[slot].dataID = m->dataID;
  maps[slot].filePos = 0;
  maps[slot].totalSize = LE_DATA_GetLoadedSize (maps[slot].dataID);

  return TRUE;
}

BOOL dmmClose(struct MapperAction *m) {
  int slot;

  // Find the slot
  for (slot = 0; slot < MAXMAPS; slot++)
    if (maps[slot].canID == m->canID)
      break;
  assert((slot != MAXMAPS) && (maps[slot].inUse));

  // Clean it up
  LE_DATA_RemoveRef(maps[slot].dataID);
  maps[slot].inUse = FALSE;
  return TRUE;
}

BOOL dmmRead(struct MapperAction *m) {
  int slot;
  char *mem;

  // Find the slot
  for (slot = 0; slot < MAXMAPS; slot++)
    if (maps[slot].canID == m->canID)
      break;
  assert((slot != MAXMAPS) && (maps[slot].inUse));

  // Check for "end of file"
  if (maps[slot].filePos >= maps[slot].totalSize) {
    m->bytes = 0;
    return FALSE;
  }

  // Clip the length of bytes read, if necessary.
  if (maps[slot].filePos + m->bytes > maps[slot].totalSize)
    m->bytes = maps[slot].totalSize - maps[slot].filePos;

  if (m->dest) {
    // Show me the data
    mem = (char *)LE_DATA_Use (maps[slot].dataID);

    // Act like it's being read from a sequential file
    memcpy(m->dest, mem + maps[slot].filePos, m->bytes);
  }
  maps[slot].filePos += m->bytes;
  return TRUE;
}

BOOL dmmMap(struct MapperAction *m) {
  int slot;
  char *mem;

  // Find the slot
  for (slot = 0; slot < MAXMAPS; slot++)
    if (maps[slot].canID == m->canID)
      break;
  assert((slot != MAXMAPS) && (maps[slot].inUse));

  // Check for "end of file"
  if (m->offset >= maps[slot].totalSize) {
    m->bytes = 0;
    return FALSE;
  }

  // Clip the length of bytes read, if necessary.
  maps[slot].filePos = m->offset;
  if (m->offset + m->bytes > maps[slot].totalSize)
    m->bytes = maps[slot].totalSize - m->offset;

  // Show me the data
  mem = (char *) LE_DATA_Use (maps[slot].dataID);
  maps[slot].filePos = m->offset + m->bytes;

  // Set a pointer to the part that the caller wants
  m->dest = mem + m->offset;

  return TRUE;
}

BOOL CAN_DataIDMemoryMapper(struct MapperAction *m) {
  static BOOL firstTime = TRUE;

  // Initialize here, same as how CAN_FileMemoryMapper does it.
  if (firstTime) {
    memset(maps, sizeof(DataIDMap) * MAXMAPS, 0);
    firstTime = FALSE;
  }

  // Do stuff
  switch (m->action) {
    case CAN_MAPPER_OPEN:
      return dmmOpen( m );
    case CAN_MAPPER_MAP:
      return dmmMap( m );
    case CAN_MAPPER_READ:
      return dmmRead( m );
    case CAN_MAPPER_CLOSE:
      return dmmClose( m );
    default:
      assert(0); // bad action
  };

  return FALSE;
}

#endif // CE_ARTLIB_EnableSystemGrafix3D
