#ifndef _USBAUDIO_BUFFER_H_
#define _USBAUDIO_BUFFER_H_
#include "SysInclude.h"


#define USBAUDIO_BUFSIZE                     (1024 * 12*3) // 30 kBytes

//----- PROTOTYPES -----
extern void         usbAudio_buf_puts(const unsigned char *s, unsigned int len);
extern unsigned int usbAudio_buf_size(void);
extern unsigned int usbAudio_buf_free_size(void);
extern unsigned int usbAudio_buf_len(void);
extern void         usbAudio_buf_reset(void);
/*
 *-----wifi audio buffer function interfaces.
*/
extern unsigned long  RKUSB_FLength(char *in);
extern unsigned short RKUSB_FRead(unsigned char *b, unsigned short s, void *f);
extern unsigned char  RKUSB_FSeek(unsigned long offset, unsigned char Whence, char Handle);
extern unsigned long  RKUSB_FTell(void *in);
extern unsigned char  RKUSB_FClose(char Handle);
extern int RKUSB_FEOF(void *f);

#endif //_USBAUDIO_BUFFER_H_