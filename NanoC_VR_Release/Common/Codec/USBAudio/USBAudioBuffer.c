/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name£º   netbuffer.c
*
* Description:
*
* History:      <author>          <time>        <version>
*                move           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "SysInclude.h"
#include "USBConfig.h"

#ifdef _USBAUDIO_DECODE_

#include "USBAudioBuffer.h"

typedef union
{
  uint8   b8[USBAUDIO_BUFSIZE];
  uint16 b16[USBAUDIO_BUFSIZE/2];
  uint32 b32[USBAUDIO_BUFSIZE/4];
}USBBUFFER;


_ATTR_USB_AUDIO_CODE_ static USBBUFFER vs_buf;
_ATTR_USB_AUDIO_BSS_ static volatile unsigned int vs_bufhead, vs_buftail;
_ATTR_USB_AUDIO_BSS_ static volatile unsigned long vs_offset;


_ATTR_USB_AUDIO_CODE_
void usbAudio_buf_puts(const unsigned char *s, unsigned int len)
{
    unsigned int head;
	int getlen = len;
    if(usbAudio_buf_free_size() < len)
    {
        return;
    }
    head = vs_bufhead;
    while(len--)
    {
        vs_buf.b8[head++] = *s++;
        if (head == USBAUDIO_BUFSIZE)
        {
			head = 0;
		}
    }
    vs_bufhead = head;
}

_ATTR_USB_AUDIO_CODE_
unsigned int usbAudio_buf_size(void)
{
	return USBAUDIO_BUFSIZE;
}

_ATTR_USB_AUDIO_CODE_
unsigned int usbAudio_buf_free_size(void)
{
	unsigned int head, tail;

	head = vs_bufhead;//the vaild data begin form positon 'vs_buftail' and end in position 'vs_bufhead'.
	tail = vs_buftail;

	if (head > tail)
	{
		return (USBAUDIO_BUFSIZE - (head - tail) - 1);
	}
	else if(head < tail)
	{
		return (tail - head - 1);
	}

	return (USBAUDIO_BUFSIZE - 1);
}

_ATTR_USB_AUDIO_CODE_
unsigned int usbAudio_buf_len(void)
{
	unsigned int head, tail;

	head = vs_bufhead;//the vaild data begin form positon 'vs_buftail' and end in position 'vs_bufhead'.
	tail = vs_buftail;

	if(head > tail)
	{
		return (head-tail);
	}
	else if(head < tail)
	{
		return (USBAUDIO_BUFSIZE-(tail-head));
	}

	return 0;
}


_ATTR_USB_AUDIO_CODE_
void usbAudio_buf_reset(void)
{
	vs_bufhead = 0;
	vs_buftail = 0;
    vs_offset = 0;
	return;
}

/*
 * audio file system interface..
 */

_ATTR_USB_AUDIO_CODE_
unsigned long RKUSB_FLength(char *in)
{
	return 0xfffffff;
}


_ATTR_USB_AUDIO_CODE_
unsigned short RKUSB_FRead(unsigned char *b, unsigned short s, void *f)
{
    unsigned int tail;
    int readed, i;

    readed = usbAudio_buf_len();

    if (readed > s)
    	readed = s;

    tail = vs_buftail;

    i = 0;
    while (i < readed)
    {
	    b[i++] = vs_buf.b8[tail++];
	    if (tail == USBAUDIO_BUFSIZE)
	    {
		      tail = 0;
	    }
    }
    vs_buftail = tail;

    vs_offset += readed;
  return readed;
}


_ATTR_USB_AUDIO_CODE_
unsigned char RKUSB_FSeek(unsigned long offset, unsigned char Whence, char Handle)
{

    return 0 ;

}

_ATTR_USB_AUDIO_CODE_
unsigned long RKUSB_FTell(void *in)
{
	return vs_buftail;
}

_ATTR_USB_AUDIO_CODE_
unsigned char RKUSB_FClose(char Handle)
{
	return 0;
}


_ATTR_USB_AUDIO_CODE_
int RKUSB_FEOF(void *f)
{
	return 0;
}

#endif

