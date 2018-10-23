/*
** ZImplode.c
**
** Layer on top of ZLIB "DEFLATE" compression library
** which emulates the pk implode and explode library
** interface.
**
**  Author:
**    For praise: Pierre Bernatchez
**    For blame:  Richard M. Nixon
*/
#include <assert.h>
#include "zimplode.h"
#include "basictyp.h"
#include "zlib.h"

/*
** portions of buffer space used for reading/writing
*/
#define CMP_IN_SUB_BUFFER_SIZE  ( 3 * CMP_BUFFER_SIZE / 4 )
#define CMP_OUT_SUB_BUFFER_SIZE ( CMP_BUFFER_SIZE / 4 )

#define EXP_IN_SUB_BUFFER_SIZE  ( EXP_BUFFER_SIZE / 4 )
#define EXP_OUT_SUB_BUFFER_SIZE ( 3 * EXP_BUFFER_SIZE / 4 )


unsigned int implode(
   unsigned int (*read_buf)(char *buf, unsigned int *size, void *param),
   void         (*write_buf)(char *buf, unsigned int *size, void *param),
   char         *work_buf,
   void         *param,
   unsigned int *type,
   unsigned int *dsize)
  {
  z_stream sZlibStream ;
  CHR * szReadBuff ;
  CHR * szWriteBuff ;
  UNS32 iReadBuffSize ;
  UNS32 iWriteBuffSize ;
  UNS32 iReadBuffBytes ;
  UNS32 iWriteBuffBytes ;
  INT32 iRet ;
  IBOOL iIsEofRead, iIsEofWrite;
  /*
  ** Using user supplied buffer for both our io buffers
  */
  iWriteBuffSize = CMP_OUT_SUB_BUFFER_SIZE ;
  iReadBuffSize = CMP_IN_SUB_BUFFER_SIZE ;
  szWriteBuff = (CHR *) work_buf ;
  szReadBuff = szWriteBuff + iWriteBuffSize;
  /*
  ** Init zlib deflate
  */
  sZlibStream.zalloc = Z_NULL ; /* Use the default memory allocation functions */
  sZlibStream.zfree  = Z_NULL ;
  sZlibStream.opaque = Z_NULL ;
  sZlibStream.data_type = Z_UNKNOWN ; /* Z_BINARY, Z_ASCII, Z_UNKNOWN */
  iRet = deflateInit2( & sZlibStream,
                (int)8,     /* 0-9,increasing compress level/speed tradeoff */
                Z_DEFLATED, /* Compression method. Only deflate supported so far */
                15,         /* 8 .. 15 */  /* Log base 2 of history window size, 32K as per PNG */
                (int)8,     /* Memory usage indicator 1-9 increasing speed, compression, and memory footprint */
                (int)0      /* 0,1,2 -> DEFAULT_STRATEGY, FILTERED, HUFFMAN_ONLY */
                );
  if ( iRet )
    return CMP_INVALID_MODE ; /* Init failed unexpectedly */

  /*
  ** Loop until all the data has been output
  */
  sZlibStream.avail_in = 0 ;
  sZlibStream.next_in  = (void *)szReadBuff ;
  sZlibStream.avail_out = iWriteBuffSize ;
  sZlibStream.next_out  = (void *)szWriteBuff ;
  iIsEofRead  = IFALSE ;
  iIsEofWrite = IFALSE ;
  while ( ! iIsEofWrite )
    {
    if( sZlibStream.avail_in == 0 && ! iIsEofRead )
      {
      /*
      ** Input when we're out of uncompressed data
      */
      iReadBuffBytes = read_buf( szReadBuff, & iReadBuffSize, param );
      assert( iReadBuffBytes <= iReadBuffSize );
      if( iReadBuffBytes )
        {
        sZlibStream.avail_in = iReadBuffBytes ;
        sZlibStream.next_in  = (void *)szReadBuff ;
        }
      else
        iIsEofRead = ITRUE ;
      }
    iRet = deflate( &sZlibStream, (iIsEofRead ? Z_FINISH :Z_NO_FLUSH) );

    switch( iRet )
      {
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
      return CMP_BAD_DATA ; /* data problem */
    case Z_MEM_ERROR:
    case Z_BUF_ERROR:
      return CMP_INVALID_DICTSIZE; /* mem problem */
    case Z_OK:
      break; /* OK.. more compression to go */
    case Z_STREAM_END:
      iIsEofWrite = ITRUE; /* OK .. compression done */
      break;
    default :
      return CMP_ABORT ; /* Other problem */
      }

    if( sZlibStream.avail_out < iWriteBuffSize )
      {
      /*
      ** Output any compressed data as soon as we get it.
      */
      iWriteBuffBytes = iWriteBuffSize - sZlibStream.avail_out ;
      sZlibStream.avail_out = iWriteBuffSize ;
      sZlibStream.next_out  = (void *)szWriteBuff ;
      write_buf( szWriteBuff, & iWriteBuffBytes, param );
      }
    }
  /*
  ** End it
  */
  iRet = deflateEnd(&sZlibStream) ;
  if( iRet )
    return CMP_BAD_DATA ;
  else
    return CMP_NO_ERROR;
  }

unsigned int explode(
   unsigned int (*read_buf)(char *buf, unsigned  int *size, void *param),
   void         (*write_buf)(char *buf, unsigned  int *size, void *param),
   char         *work_buf,
   void         *param)
  {
  z_stream sZlibStream ;
  CHR * szReadBuff ;
  CHR * szWriteBuff ;
  UNS32 iReadBuffSize ;
  UNS32 iWriteBuffSize ;
  UNS32 iReadBuffBytes ;
  UNS32 iWriteBuffBytes ;
  INT32 iRet ;
  IBOOL iIsEofRead, iIsEofWrite;
  /*
  ** Using user supplied buffer for both our io buffers
  */
  iWriteBuffSize = EXP_OUT_SUB_BUFFER_SIZE ;
  iReadBuffSize = EXP_IN_SUB_BUFFER_SIZE ;
  szWriteBuff = (CHR *) work_buf ;
  szReadBuff = szWriteBuff + iWriteBuffSize;
  /*
  ** Init zlib inflate
  */
  sZlibStream.zalloc = Z_NULL ; /* Use the default memory allocation functions */
  sZlibStream.zfree = Z_NULL ;
  sZlibStream.opaque = Z_NULL ;
  sZlibStream.data_type = Z_UNKNOWN ; /* Z_BINARY, Z_ASCII, Z_UNKNOWN */
  iRet = inflateInit2( & sZlibStream,
                15  /* Log base 2 of window buffer, 32K as per PNG */
                );
  if ( iRet )
    return CMP_INVALID_MODE ; /* Init failed unexpectedly */

  /*
  ** Loop until all the data has been output
  */
  sZlibStream.avail_in = 0 ;
  sZlibStream.next_in  = (void *)szReadBuff ;
  sZlibStream.avail_out = iWriteBuffSize ;
  sZlibStream.next_out  = (void *)szWriteBuff ;
  iIsEofRead  = IFALSE ;
  iIsEofWrite = IFALSE ;
  while ( ! iIsEofWrite )
    {
    if( sZlibStream.avail_in == 0 && ! iIsEofRead )
      {
      /*
      ** Input when we're out of compressed data
      */
      iReadBuffBytes = read_buf( szReadBuff, & iReadBuffSize, param );
      assert( iReadBuffBytes <= iReadBuffSize );
      if( iReadBuffBytes )
        {
        sZlibStream.avail_in = iReadBuffBytes ;
        sZlibStream.next_in  = (void *)szReadBuff ;
        }
      else
        iIsEofRead = ITRUE ;
      }
    iRet = inflate( &sZlibStream, (iIsEofRead ? Z_FINISH :Z_NO_FLUSH) );
    switch( iRet )
      {
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
      return CMP_BAD_DATA ; /* data problem */
    case Z_MEM_ERROR:
    case Z_BUF_ERROR:
      return CMP_INVALID_DICTSIZE; /* mem problem */
    case Z_OK:
      break; /* OK.. more compression to go */
    case Z_STREAM_END:
      iIsEofWrite = ITRUE; /* OK .. compression done */
      break;
    default :
      return CMP_ABORT ; /* Other problem */
      }

    if( sZlibStream.avail_out < iWriteBuffSize )
      {
      /*
      ** Output any uncompressed data as soon as we get it.
      */
      iWriteBuffBytes = iWriteBuffSize - sZlibStream.avail_out ;
      sZlibStream.avail_out = iWriteBuffSize ;
      sZlibStream.next_out  = (void *)szWriteBuff ;
      write_buf( szWriteBuff, & iWriteBuffBytes, param );
      }
    }
  /*
  ** End it
  */
  iRet = inflateEnd(&sZlibStream) ;
  if( iRet )
    return CMP_BAD_DATA ;
  else
    return CMP_NO_ERROR;
  }
