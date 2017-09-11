/*
********************************************************************************
*                   Copyright (c) 2008, Rockchip
*                         All rights reserved.
*
* File Name£º   audio_file_access.c
* 
* Description:  Audio File Operation Interface
*
* History:      <author>          <time>        <version>       
*             Vincent Hsiung    2009-01-08         1.0
*    desc:    ORG.
********************************************************************************
*/

//#include "../AudioConfig.h"
#include "audio_main.h"

#include <stdio.h>
#include <string.h>
#include "FsInclude.h"
#include "File.h"

//#define SIMULATION
//#define	MEMFILE_SIM

typedef unsigned int size_t;

_ATTR_AUDIO_BSS_
size_t   (*RKFIO_FRead)(void * /*buffer*/, size_t /*length*/,FILE *) ;

_ATTR_AUDIO_BSS_
int      (*RKFIO_FSeek)(long int /*offset*/, int /*whence*/ , FILE * /*stream*/);

_ATTR_AUDIO_BSS_
long int (*RKFIO_FTell)(FILE * /*stream*/);

_ATTR_AUDIO_BSS_
size_t   (*RKFIO_FWrite)(void * /*buffer*/, size_t /*length*/,FILE * /*stream*/);

_ATTR_AUDIO_BSS_
unsigned long (*RKFIO_FLength)(FILE *in /*stream*/);
_ATTR_AUDIO_BSS_
int      (*RKFIO_FClose)(FILE * /*stream*/);

_ATTR_AUDIO_BSS_
int (*RKFIO_FEof)(FILE *in);


_ATTR_AUDIO_BSS_
FILE *pRawFileCache,*pFlacFileHandleBake;

#ifdef SIMULATION
size_t   RKFRead_sim(void * b, size_t s,FILE * f)
{
    int ret = 0;
    ret = fread(b, 1 , s , f);
    return ret;
}

int      RKFWrite_sim(void * b, size_t s,FILE * f)
{
    int ret = 0;
    ret = fwrite(b, 1 , s , f);
    return ret;
}
#endif


#ifdef MEMFILE_SIM
//--mp3encoded data------------------------------------------------------
const unsigned long mp3data[]={
//mao
	//#include "mao.dat"
         0
//lesson1 (mono)
//	#include "lesson1.dat"

//mp1_t
//	#include "mp1_t.dat"

//mp2_t
//	#include "mp2_t.dat"

//1k LR 32k
//	#include "1klr_32.dat"

//Scan Freq
//	#include "scanfreq.dat"

//1k_32.mp3
/*
	#include "1k_32.dat"
//*/
};
//-----------------------------------------------------------------------

static int file_pos = 0;
size_t RKmem_FRead(void *b, size_t s,FILE *f)
{    
	int readed = 0;
	char *p = (char*) &mp3data[0];
	char *b1 = (char*) b;
	
	while(readed < s)
	{
		if ((s - readed)>(sizeof(mp3data)-file_pos))
		{
			memcpy(b1,&p[file_pos],sizeof(mp3data)-file_pos);
			readed += sizeof(mp3data)-file_pos;
			b1 += sizeof(mp3data)-file_pos;
			file_pos = 0;
		}
		else
		{
			memcpy(b1,&p[file_pos],s - readed);
			file_pos += s - readed;
			b1 += s - readed;
			readed += s - readed;
			
			
			//file_pos = 0;
		}
	}
	
	return readed;
}

static int wfile_pos = 0;
size_t RKmem_FWrite(void *ptr, size_t n,FILE *stream)
{
/*
	int saved = 0;
	char *p = (char*)ptr;
	while(saved < n)
	{
		out_samples_buff[wfile_pos&(32*1024-1)] = *p++;
		wfile_pos++;
		saved++;
	}
*/
}
#endif

_ATTR_AUDIO_TEXT_
unsigned long RKFLength(FILE *in)
{
    return (FileInfo[(int)in].FileSize); // modified by huweiguo, 09/04/11

    //return 0x851121;
}

_ATTR_AUDIO_TEXT_
unsigned long RKFTell(FILE *in)
{
    return (FileInfo[(int)in].Offset);
}

//these two functions are specified to FLAC decoder
_ATTR_FLACDEC_TEXT_
int FLAC_FileSeekFast(int offset, int clus, FILE *in)
{
	FileInfo[(int)in].Offset = offset;
	FileInfo[(int)in].Clus   = clus;
	return 0;
}
_ATTR_FLACDEC_TEXT_
int FLAC_FileGetSeekInfo(int *pOffset, int *pClus, FILE *in)
{
	*pOffset = FileInfo[(int)in].Offset;
	*pClus   = FileInfo[(int)in].Clus;
	return 0;
}

_ATTR_AUDIO_TEXT_
void FileFuncInit(void)
{
	RKFIO_FLength = RKFLength;
#ifdef SIMULATION
    RKFIO_FRead=RKFRead_sim;
    RKFIO_FWrite=RKFWrite_sim;
    RKFIO_FSeek=fseek;
    RKFIO_FTell=ftell;
    //RKFIO_FClose=fclose;
#endif    	
#if 0//fdef MEMFILE_SIM
	RKFIO_FRead = RKmem_FRead;
	RKFIO_FWrite = RKmem_FWrite;
#else
	RKFIO_FRead = FileRead;
	//RKFIO_FWrite = FileWrite;  
	RKFIO_FSeek = FileSeek;
	RKFIO_FTell = RKFTell;
	RKFIO_FClose = FileClose;

#endif
}

