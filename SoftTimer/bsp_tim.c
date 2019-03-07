#include "bsp_tim.h"

#if SOFT_TIM_NUM>0

static volatile SoftTim_t g_STim[SOFT_TIM_NUM];

void BSP_SoftTimInit() //请根据实际情况修改此函数
{
	g_STim[STIM_DISCHGR].Max = 15000; // 15s
	g_STim[STIM_DISCHGR].Sta = _BSPTIM_STOP;//_BSPTIM_RUNING_ONCE;
	
	g_STim[STIM_FORCMANL].Max = 12000; // 12s
	g_STim[STIM_FORCMANL].Sta = _BSPTIM_STOP;//_BSPTIM_RUNING_ONCE;
}

/* ----------------------------------------------------

----------------------------------------------------- */
uint8_t BSP_SoftTimReadResetFlag(uint8_t i)
{
  if(g_STim[i].Flag ==1)
	{
	  g_STim[i].Flag =0;
		return 1;
	}
	else
	{
	  return 0;
	}
}

/* ----------------------------------------------------

----------------------------------------------------- */
void BSP_SoftTimAbort(uint8_t i)
{
  g_STim[i].Sta = _BSPTIM_STOP;
}

/* ----------------------------------------------------

----------------------------------------------------- */
/* If the state of a softTim is not STOP, calling these resume functions will do nothing. */
void BSP_SoftTimResumeOnce(uint8_t i)
{
	if(g_STim[i].Sta != _BSPTIM_STOP)
		return;
	g_STim[i].Cnt = 0;
  g_STim[i].Sta = _BSPTIM_RUNING_ONCE;
}

void BSP_SoftTimResumeRepeat(uint8_t i)
{
	if(g_STim[i].Sta != _BSPTIM_STOP)
		return;
	g_STim[i].Cnt = 0;
  g_STim[i].Sta = _BSPTIM_RUNING_REPEAT;
}


/* ----------------------------------------------------
Handler
-------------------------------------------------------
Change each timer's state.
Set Flag if it reach time.
The Flag MUST BE RESET BY USER after read,
BSP_SoftTimReadResetFlag() can be used to do this.
----------------------------------------------------- */

void BSP_SoftTimHandler()
{
	uint8_t i;
	for(i=0;i<SOFT_TIM_NUM;++i)
	{
		if (g_STim[i].Sta == _BSPTIM_STOP)
		{
			g_STim[i].Cnt = 0;
		}
		else
		{
			if(g_STim[i].Flag !=1 )
			{
				if(++(g_STim[i].Cnt) >= g_STim[i].Max)
				{
					g_STim[i].Flag =1;
					g_STim[i].Cnt = 0;
					if(g_STim[i].Sta == _BSPTIM_RUNING_ONCE)
					{
						g_STim[i].Sta = _BSPTIM_STOP;
					}
				}
			}
		}
	}
}

#endif /* ifdef SOFT_TIM_NUM>0 */
