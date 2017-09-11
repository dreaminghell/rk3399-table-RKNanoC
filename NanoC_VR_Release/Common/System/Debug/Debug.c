#include <stdio.h>
#include "sysinclude.h"

#define DEBUG_TIME_LEN  8

uint8  DebugType = 0;
uint16 DebugXpos = 0;
uint16 DebugYpos = 0;

//#define _UART_FILE_DEBUG_
//#define _VIRTUAL_UART_DEBUG_
//#define _PRINT_SCREEN_DEBUG_

#ifdef _UART_FILE_DEBUG_


int8   hDebugFile = -1;
int8   DebugBuffer[512];
uint32 DebugBufferOffset = 0;
#endif

void debugfileopen(void)
{   
    #ifdef _UART_FILE_DEBUG_
    hDebugFile = FileOpen("\\","DEBUGLOGTXT","R");
    //printf("hDebugFile = %d\n", hDebugFile);
    if (hDebugFile >= 0)
    {
        FileClose(hDebugFile);
        FileDelete("\\","DEBUGLOGTXT");
        //printf("FileDelete: hDebugFile = %d\n", hDebugFile);
    }
    
    DebugBufferOffset = 0;
    hDebugFile        = -1;
    hDebugFile = FileCreate("\\","DEBUGLOGTXT");
    if (hDebugFile < 0)
    {
        //printf("Debug Creat File Error!!!\n");
    }  

    #endif
}

void debugfileclose(void)
{
    #ifdef _UART_FILE_DEBUG_
    if (hDebugFile != -1)
    {
        if (DebugBufferOffset)
        {
            FileWrite(DebugBuffer, DebugBufferOffset, hDebugFile);
        }
        
        FileClose(hDebugFile);
        hDebugFile = -1;   

        #if (NAND_DRIVER == 1)
        FtlWrBlkFree();
        #endif
    }

    #endif
}


/*
--------------------------------------------------------------------------------
  Function name : void VirtualUartWrite(char ch)
  Author        : 
  Description   : Virtual Uart Write a byte
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#define BAUD_RATE_115200    ((uint32)(8680))        // ns: (1000000 / 115200) * 1000
        
#ifdef _VIRTUAL_UART_DEBUG_

void VirtualUartWrite(char ch)
{
    uint32 i;
    char   temp = ch;

    //start timer delay
    SystickDelayStart();
    
    //start bit
    Gpio_SetPinLevel(VIRTUAL_UART_TX, GPIO_LOW);
    SystickDelaynS(BAUD_RATE_115200);

    //send data
    for (i = 0; i < 8; i++)
    {
        if (temp & 0x01)
        {
            Gpio_SetPinLevel(VIRTUAL_UART_TX, GPIO_HIGH);
        }
        else
        {
            Gpio_SetPinLevel(VIRTUAL_UART_TX, GPIO_LOW);
        }
        SystickDelaynS(BAUD_RATE_115200);
        temp = temp >> 1;
    }

    //stop bit
    Gpio_SetPinLevel(VIRTUAL_UART_TX, GPIO_HIGH);
    SystickDelaynS(BAUD_RATE_115200);
}
#endif

void debugfilewrite(int8 ch, uint32 timeout)
{
    #ifdef _UART_FILE_DEBUG_
    DebugBuffer[DebugBufferOffset++] = ch;
    
    if (DebugBufferOffset >= 512)
    {
        DebugBufferOffset = 0;
        FileWrite(DebugBuffer, 512, hDebugFile);
    }
    #endif
}

void DebugSwitch(uint8 type)
{
    //type = 0: uart debug
    //type = 1: write to file
    //type = 2: print to LCD

    if(type == DEBUG_TYPE_UART)
    {
        GpioMuxSet(GPIOPortB_Pin5,IOMUX_GPIOB5_UART_TXD);    
    }
    if ((type == DEBUG_TYPE_WRITE_FILE) && (DebugType != DEBUG_TYPE_WRITE_FILE))
    {
        debugfileopen();
        DelayMs(500);
    }
    
    if ((type != DEBUG_TYPE_WRITE_FILE) && (DebugType == 1))
    {
        debugfileclose();
    }

    if(type == DEBUG_TYPE_VIRTUAL_UART)
    {
        #ifdef _VIRTUAL_UART_DEBUG_
        GpioMuxSet(VIRTUAL_UART_TX,IOMUX_GPIOC6_IO);
        Gpio_SetPinDirection(VIRTUAL_UART_TX, GPIO_OUT);
        Gpio_SetPinLevel(VIRTUAL_UART_TX, GPIO_HIGH);

        #endif
    }
    DebugType = type;
}

int fputc(int ch, FILE *f)
{
    UINT16 Temp_BKColor, Temp_Color;
    UINT16 Temp_TxtDrawMode;            
    uint8  dgbBuffer[16];
    uint32 tmpcnt, i;

    if (DebugType == DEBUG_TYPE_UART)         //uart debug
    {
        if (ch == '\n')
        {
            UARTWriteByte('\r', 1000);
            
            tmpcnt = SysTickCounter;
            for (i = 0; i < 16; i++)
            {
                dgbBuffer[i] = tmpcnt % 10;
                tmpcnt = tmpcnt / 10;
                if (tmpcnt == 0)
                {
                    break;
                }
            }
            if (i < 2)
            {
                i++;
                dgbBuffer[i] = 0;
            }
            
            UARTWriteByte('[', 1000);
            i++;
            while(i != 0)
            {
                i--;
                UARTWriteByte(dgbBuffer[i]+0x30, 1000);
                if (i == 2)
                {
                    UARTWriteByte('.', 1000);
                }
            }
            UARTWriteByte(']', 1000);
        }
        UARTWriteByte(ch, 1000);
    }

    if (DebugType == DEBUG_TYPE_WRITE_FILE)         //write to file
    {
        #ifdef _UART_FILE_DEBUG_
        if (ch == '\n')
        {
            debugfilewrite('\r', 1000);
            
            tmpcnt = SysTickCounter;
            for (i = 0; i < 16; i++)
            {
                dgbBuffer[i] = tmpcnt % 10;
                tmpcnt = tmpcnt / 10;
                if (tmpcnt == 0)
                {
                    break;
                }
            }
            if (i < 2)
            {
                i++;
                dgbBuffer[i] = 0;
            }
            
            debugfilewrite('[', 1000);
            i++;
            while(i != 0)
            {
                i--;
                debugfilewrite(dgbBuffer[i]+0x30, 1000);
                if (i == 2)
                {
                    debugfilewrite('.', 1000);
                }
            }
            debugfilewrite(']', 1000);
        }
        
        debugfilewrite(ch, 1000);

        #endif
    }
    
    if (DebugType == DEBUG_TYPE_PRINT_SCREEN)         //print to screen
    {
        #ifdef _PRINT_SCREEN_DEBUG_
        if (ch == '\n')
        {
            Temp_Color = LCD_GetColor();
            LCD_SetColor(COLOR_PURPLE);
            Temp_BKColor = LCD_GetBkColor();
            LCD_SetBkColor(COLOR_BLACK);
            Temp_TxtDrawMode=LCD_SetTextMode(LCD_DRAWMODE_NORMAL);
    
            LcdContext.DispPosX = DebugXpos;
            LcdContext.DispPosY = DebugYpos;
            LCD_NFDispUnicodeChar(' ');
            DebugXpos += LCD_GetCharXSize(' ');
            LCD_NFDispUnicodeChar(' ');

            LCD_SetTextMode(Temp_TxtDrawMode);
            LCD_SetBkColor(Temp_BKColor);
            LCD_SetColor(Temp_Color);
            
            DebugXpos  = 0;
            DebugYpos += AS_CHAR_YSIZE_12;
            if (DebugYpos > LCD_HEIGHT)
            {
                DebugYpos = 0;
            }
        }
        else
        {
            Temp_Color = LCD_GetColor();
            LCD_SetColor(COLOR_PURPLE);
            Temp_BKColor = LCD_GetBkColor();
            LCD_SetBkColor(COLOR_BLACK);
            Temp_TxtDrawMode=LCD_SetTextMode(LCD_DRAWMODE_NORMAL);
    
            LcdContext.DispPosX = DebugXpos;
            LcdContext.DispPosY = DebugYpos;
            LCD_NFDispUnicodeChar(ch);
            DebugXpos += LCD_GetCharXSize(ch);

            LCD_SetTextMode(Temp_TxtDrawMode);
            LCD_SetBkColor(Temp_BKColor);
            LCD_SetColor(Temp_Color);
        }

        #endif
    }
	
	if(DebugType == DEBUG_TYPE_VIRTUAL_UART)
	{   
        #ifdef _VIRTUAL_UART_DEBUG_
    	if (ch == '\n')
        {        
            tmpcnt = SysTickCounter;
            for (i = 0; i < DEBUG_TIME_LEN; i++)
            {
                dgbBuffer[i] = tmpcnt % 10;
                tmpcnt = tmpcnt / 10;
            }
            
            VirtualUartWrite('\r');
            VirtualUartWrite('\n');
            VirtualUartWrite('[');
            for (i = 0; i < DEBUG_TIME_LEN; i++)
            {
                VirtualUartWrite(dgbBuffer[DEBUG_TIME_LEN - 1 -i]+0x30);
                if (DEBUG_TIME_LEN - 1 -i == 2)
                {
                    VirtualUartWrite('.');
                }
            }
            VirtualUartWrite(']');
            
            return OK;
        }
        
        VirtualUartWrite(ch);

        #endif
	}
    
}

