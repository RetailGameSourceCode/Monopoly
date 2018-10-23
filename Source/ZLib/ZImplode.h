/*
** ZImplode.h
**  Emulation of the PKZIP library API
**      using zlib.
** CAVEAT
**    The zlib api has an procedure called crc32 which is
**      similar to the one in the pk API.  BUT
**      the order of the arguments are DIFFERENT.
*/
#ifdef __cplusplus
   extern "C" {
#endif

unsigned int implode(
   unsigned int (*read_buf)(char *buf, unsigned int *size, void *param),
   void         (*write_buf)(char *buf, unsigned int *size, void *param),
   char         *work_buf,
   void         *param,
   unsigned int *type,
   unsigned int *dsize);


unsigned int explode(
   unsigned int (*read_buf)(char *buf, unsigned  int *size, void *param),
   void         (*write_buf)(char *buf, unsigned  int *size, void *param),
   char         *work_buf,
   void         *param);

unsigned long crc32(unsigned long crc, const unsigned char *buf, unsigned int len);
// PKWare: unsigned long crc32(char *buffer, unsigned int *size, unsigned long *old_crc);
// Note: different argument order than PKWare equivalent.

#ifdef __cplusplus
   }                         // End of 'extern "C"' declaration
#endif


#define CMP_BUFFER_SIZE    32768
#define EXP_BUFFER_SIZE    16384

#define CMP_BINARY             0
#define CMP_ASCII              1

#define CMP_NO_ERROR           0
#define CMP_INVALID_DICTSIZE   1
#define CMP_INVALID_MODE       2
#define CMP_BAD_DATA           3
#define CMP_ABORT              4
