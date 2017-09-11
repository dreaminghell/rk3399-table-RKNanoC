#ifndef _H_AUDIO_FADE
#define _H_AUDIO_FADE

//#ifdef  _RK_EQ_

typedef struct
{
	long length;
	long begin_offset;
	long cur_offset;
	int  is_finished;
	int  is_in_or_out;
	

	long dx,dy,p,dobDy,dobD,cur_x,cur_y,x2,y2;
	int  is_xyswap;
}fade_type;

#define FADE_IN     0
#define FADE_OUT    1
#define FADE_NULL   -1

//initialization.
//begin:the frist specimen serial number,len:length  type: 0-fade in 1-fade out.
void FadeInit(long begin,long len,int type);
long FadeDoOnce();

//#endif

#endif