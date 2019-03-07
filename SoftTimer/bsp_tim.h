/**
  *  这里是根据硬件定时器自定义扩展的一类软定时器
	*  它可随时启动单次计时，或循环计时。
	*  与硬件定时器不同的是，在计时完成后，它不会自动重置。而是置位标志位。
	*  在计时完成前用户查询标志位没影响，
	*  而在计时完成后，用户查询过标志位后标志位自动复位。如果位循环模式则重新开始计时。
	*  因不会重复触发，可适用于前后台查询式状态机编程架构中。
	*
	*  BSP_SoftTimReadResetFlag 为读标志位并复位函数。
  *  BSP_SoftTimAbort 为立刻停止计时器。但它不会复位标志位。
	*  BSP_SoftTimResumeOnce 为开始单次计时。如果计时已经开始过，则无效。
	*  BSP_SoftTimResumeRepeat 为开始重复计时。如果计时已经开始过，则无效。
	
	*  目前为了兼顾效率与封装性，暂将全部软定时器采用同一个数据结构数组管理，使用宏定义下标区分。
  *  而数据结构变量封装为静态，仅通过接口函数访问修改。
  
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __bsp_tim_H
#define __bsp_tim_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "tim.h"
#include <stdint.h>
#define SOFT_TIM_NUM 2

#define STIM_DISCHGR   0
#define STIM_FORCMANL  1


/* Private defines -----------------------------------------------------------*/
	
typedef enum
{
	_BSPTIM_STOP=0,
	_BSPTIM_RUNING_ONCE,
	_BSPTIM_RUNING_REPEAT
} SoftTimSta_t;

typedef struct
{
	volatile SoftTimSta_t Sta;
	volatile uint8_t Flag;  /* Flag */
	volatile uint32_t Cnt;	/* Counter */
	volatile uint32_t Max;	/* Counter Max */
} SoftTim_t;

/* Prototypes ----------------------------------------------------------------*/

void     BSP_SoftTimInit(void);
void     BSP_SoftTimHandler(void);
void     BSP_SoftTimAbort(uint8_t i);
void     BSP_SoftTimResumeOnce(uint8_t i);
void     BSP_SoftTimResumeRepeat(uint8_t i);
uint8_t  BSP_SoftTimReadResetFlag(uint8_t i);

#ifdef __cplusplus
}
#endif

#endif /*__tim_H */


/****END OF FILE****/
