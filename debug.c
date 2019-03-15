/**
  ******************************************************************************
  debug.c
	用于配置putc重定义函数，以及DebugPrintf函数。
	应用于CubeMX生成工程。也可灵活修改重定义方式或用于移植。
  ******************************************************************************
*/
#include <stdio.h>
#include "usart.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF); //用UART2进行printf重定向。
  return ch;
}

#ifdef DEBUG
#define DebugPrintf(fmt,args...) printf(fmt,##args)
//#define DebugPrintf(fmt,args...) printf("%s(%d)"#fmt"\r\n", __FILE__, __LINE__, ##args);
#else
#define DebugPrintf(fmt,...)  do{}while(0)
#endif
