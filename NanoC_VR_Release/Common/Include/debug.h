/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   typedef.h
* 
* Description:  typedef.h -Macros define the data type
*                      
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_


#define DEBUG_TYPE_UART             0 //将debug输出到uart
#define DEBUG_TYPE_WRITE_FILE       1 //将debug信息写到文件中
#define DEBUG_TYPE_PRINT_SCREEN     2 //将debug信息打印到屏幕
#define DEBUG_TYPE_VIRTUAL_UART     3 //将debug信息输出到模拟串口中
#define DEBUG_TYPE_OFF              4 //关闭debug信息



#define TRACE_ENTER(x) 0
#define TRACE_EXIT(x) 

/*
*-------------------------------------------------------------------------------
*
* declare a external definition function for debug,it can call it when need.
*
*-------------------------------------------------------------------------------
*/
extern void __error__(char *pcFilename, unsigned long ulLine);

/*
*-------------------------------------------------------------------------------
*
* define DEGUG
*
*-------------------------------------------------------------------------------
*/

//#define DEBUG

#ifdef DEBUG
#define ASSERT(expr) {                                      \
                         if(!(expr))                        \
                         {                                  \
                             __error__(__FILE__, __LINE__); \
                         }                                  \
                     }
#else
#define ASSERT(expr)
#endif
/*
********************************************************************************
*
*                         End of Debug.h
*
********************************************************************************
*/
#endif
