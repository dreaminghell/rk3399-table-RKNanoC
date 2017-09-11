/*
********************************************************************************
*                   Copyright (c) 2009,WangBo
*                      All rights reserved.
*
* File Name£º   BlueToothControl.c
*
* Description:
*
* History:      <author>          <time>        <version>
*                 WangBo      2009-4-10       1.0
*    desc:    ORG.
********************************************************************************
*/
#include "SysInclude.h"
#include "USBConfig.h"

#ifdef _USBAUDIO_DECODE_

#include "audio_globals.h"
#include "audio_file_access.h"

#define USB_AUDIO_OUT_BUF_SIZE 512


//------------------global varibles----------------------------------
_ATTR_USB_AUDIO_BSS_  static int32  g_samplerate;
_ATTR_USB_AUDIO_BSS_  static int32  g_channels;
_ATTR_USB_AUDIO_BSS_  static uint32 g_BtBufIndex;
_ATTR_USB_AUDIO_BSS_  static int32  g_outLength;
_ATTR_USB_AUDIO_BSS_  static long   g_timePos;

_ATTR_USB_AUDIO_BSS_  int16   gUsbAudioOutputPtr [2][USB_AUDIO_OUT_BUF_SIZE * 2];

/*
--------------------------------------------------------------------------------
  Function name :  void BlueToothControlStart(uint32 controlID)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/

_ATTR_USB_AUDIO_CODE_
int UsbAudio_open(void)
{
    int ret;
    //need from bt information
    g_samplerate      = 48000;
    g_channels        = 2;
    
    g_outLength       = USB_AUDIO_OUT_BUF_SIZE;
    g_BtBufIndex = 0;

    memset(&gUsbAudioOutputPtr[0][0], 0, 2*2*USB_AUDIO_OUT_BUF_SIZE*sizeof(int16));
    g_timePos += g_outLength;

    return 1;
}

/*
--------------------------------------------------------------------------------
  Function name :  void bt_phonePCM_get_samplerate(uint32 controlID)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int UsbAudio_get_samplerate(void)
{
    return g_samplerate;
}


/*
--------------------------------------------------------------------------------
  Function name :  void bt_phonePCM_get_samplerate(uint32 controlID)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int UsbAudio_get_time(void)
{
    return g_timePos;
}


/*
--------------------------------------------------------------------------------
  Function name :  void BlueToothControlStart(uint32 controlID)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
int UsbAudio_get_buffer(unsigned long ulParam1 , unsigned long ulParam2)
{
	*(int *)ulParam1 = (unsigned long)(&gUsbAudioOutputPtr[g_BtBufIndex^1][0]);
	*(int *)ulParam2 = g_outLength;
    
	return 1;
}

/*
--------------------------------------------------------------------------------
  Function name :  void BlueToothControlStart(uint32 controlID)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/

_ATTR_USB_AUDIO_CODE_
int UsbAudio_decode(void)
{
    int32  i;
    uint32 len;
    uint32 readedbyte;
    uint32 readsize = USB_AUDIO_OUT_BUF_SIZE * sizeof(int16);
    int16 *pbuf = (int16*)gUsbAudioOutputPtr[g_BtBufIndex];
    //int16   BtStreamInputBufferTemp[USB_AUDIO_OUT_BUF_SIZE];

    
    readedbyte = RKFIO_FRead(pbuf, USB_AUDIO_OUT_BUF_SIZE*2*2, NULL);

    if(readedbyte <= 0)
    {
         memset(&gUsbAudioOutputPtr[0][0], 0, 2*2*USB_AUDIO_OUT_BUF_SIZE*sizeof(int16));
         return 0;
    }
  
    g_timePos += g_outLength;
    g_BtBufIndex ^= 1;

    return 1;
}

/*
--------------------------------------------------------------------------------
  Function name :  void BlueToothControlStart(uint32 controlID)
  Author        :  zs
  Description   :

  Input         :  null
  Return        :  TRUE/FALSE
  History       :  <author>         <time>         <version>
                     zs            2009/02/20         Ver1.0
  desc          :   ORG
--------------------------------------------------------------------------------
*/
_ATTR_USB_AUDIO_CODE_
unsigned long UsbAudioFunction(unsigned long ulIoctl, unsigned long ulParam1,
        unsigned long ulParam2, unsigned long ulParam3)
{

    switch (ulIoctl)
    {

        case SUBFN_CODEC_OPEN_DEC:
            {
                UsbAudio_open();
                return 1;
            }

        case SUBFN_CODEC_GETBUFFER:
            {
                 UsbAudio_get_buffer(ulParam1,ulParam2);
                return 1;
            }

        case SUBFN_CODEC_DECODE:
            {
                return UsbAudio_decode();
                //return 1;
            }

        case SUBFN_CODEC_GETSAMPLERATE:
            {
                *(int *)ulParam1 =  UsbAudio_get_samplerate();
                return(1);
            }

        case SUBFN_CODEC_GETCHANNELS:
            {
                *(int *)ulParam1 = 2;
                return 1;
            }

        case SUBFN_CODEC_GETBITRATE:
            {
                *(int *)ulParam1 = 192000;
                return(1);
            }

        case SUBFN_CODEC_GETLENGTH:
            {
                *(int *)ulParam1 = 0xffffff;
                return 1;
            }

        case SUBFN_CODEC_GETTIME:
            {
               // *(int *)ulParam1 = 0; 

                *(int *)ulParam1 = (long long) UsbAudio_get_time() * 1000 / UsbAudio_get_samplerate();
                return 1;
            }

        case SUBFN_CODEC_SEEK:
            {
                return 1;
            }

        case SUBFN_CODEC_CLOSE:
            {
                return 1;
            }

        default:
            {
                return 0;
            }
    }
    return -1;
}

#endif

/*
********************************************************************************
*
*                         End of BlueToothControl.c
*
********************************************************************************
*/

