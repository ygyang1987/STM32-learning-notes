#include "main.h"
#include "stm32f1xx_hal.h"

/**
CubeMx生成的代码中定义了CAN_HandleTypeDef hcan，
但其成员中的 pTxMsg, pRxMsg, pRx1Msg 几个指针变量，
经过实验验证并没有被任何HAL库的CAN初始化函数进行初始化。
也就是说它们都是野指针，禁止直接使用。
用户需要在使用前自行设置静态或动态变量后，并将上述指针初始化。
（关于变量中的data成员，是包含在结构体内的数组，非指针，不必担心野指针问题）
以下是一个例子：
*/

static CanTxMsgTypeDef sCanTxMessage;  // For bxCAN TxMsg
static CanRxMsgTypeDef sCanRxMessage;  // For bxCAN RxMsg (in FIFO0)
static CanRxMsgTypeDef sCanRx1Message; // For bxCAN Rx1Msg (in FIFO1)

void hcanPointerConfig(CAN_HandleTypeDef *p_hcan)
{
  p_hcan->pTxMsg  = &sCanTxMessage;
  p_hcan->pRxMsg  = &sCanRxMessage;
  p_hcan->pRx1Msg = &sCanRx1Message;
}

/**
之后，hcan.pTxMsg虽然已初始化，
但它指向的内存变量如 sCanTxMessage，在调用发送接口函数前仍需要被正确赋值。
经过查找，并没有HAL库的接口函数会规范化地对该变量赋值，
所以，其内容全部由用户充分理解后，自行进行设置的。
以下是一个例子：
*/
void hcanTxMsgConfig(CAN_HandleTypeDef *p_hcan)
{
  p_hcan->pTxMsg-> StdId = (uint32_t) 0x123;       /*!< Min_Data = 0 and Max_Data = 0x7FF */
  p_hcan->pTxMsg-> ExtId = (uint32_t) 0x12345678;  /*!< Min_Data = 0 and Max_Data = 0x1FFFFFFF */
	
  p_hcan->pTxMsg-> IDE   =  /*!< @ref CAN_Identifier_Type */
//                      	 CAN_ID_STD; /*!< Standard Id */
                      	   CAN_ID_EXT;  /*!< Extended Id */
	
  p_hcan->pTxMsg-> RTR   =  /*!< @ref CAN_remote_transmission_request */
                        	 CAN_RTR_DATA; /*!< Data frame */
//                      	 CAN_RTR_REMOTE; /*!< Remote frame */
	
  p_hcan->pTxMsg-> DLC   = 8;  /*!< Min_Data = 0 and Max_Data = 8 */

  p_hcan->pTxMsg-> Data[0] = (uint8_t) 0x11;  /*!< Min_Data = 0 and Max_Data = 0xFF */
  p_hcan->pTxMsg-> Data[1] = (uint8_t) 0x12;
  p_hcan->pTxMsg-> Data[2] = (uint8_t) 0x13;
  p_hcan->pTxMsg-> Data[3] = (uint8_t) 0x14;
  p_hcan->pTxMsg-> Data[4] = (uint8_t) 0x51;
  p_hcan->pTxMsg-> Data[5] = (uint8_t) 0x61;
  p_hcan->pTxMsg-> Data[6] = (uint8_t) 0x71;
  p_hcan->pTxMsg-> Data[7] = (uint8_t) 0x81;
}


/**
设置单个过滤器的一个例子。用IdMask与32位模式的配置函数，其中调用了公式自动计算：
*
void hcanFilterCfg_IdMaskScale32(CAN_HandleTypeDef *p_hcan)
{
  CAN_FilterConfTypeDef filterCfg;

  #if defined(STM32F105xC) || defined(STM32F107xC)
  filterCfg.FilterNumber = 0;  /*!< Min_Data = 0 and Max_Data = 27. */
  #else
  filterCfg.FilterNumber = 0;  /*!< Min_Data = 0 and Max_Data = 13. */
  #endif
	
  filterCfg.FilterMode  = /*!<  @ref CAN_filter_mode */
                        	 CAN_FILTERMODE_IDMASK;  /*!< Identifier mask mode */
//                      	 CAN_FILTERMODE_IDLIST;  /*!< Identifier list mode */
													 
  filterCfg.FilterScale  = /*!<  @ref CAN_filter_scale */
//                      	 CAN_FILTERSCALE_16BIT;  /*!< Two 16-bit filters */
                      	   CAN_FILTERSCALE_32BIT;  /*!< One 32-bit filter  */
	
  if( filterCfg.FilterMode == CAN_FILTERMODE_IDMASK && filterCfg.FilterScale == CAN_FILTERSCALE_32BIT)
  {
    uint32_t filterId = 0x12345000;
    uint32_t maskId   = 0xFFFFF000;
    /* Modify these two variables above,
       do not modify the following automatic calculation formulas: */
    filterCfg.FilterIdHigh     = (filterId << 3 >> 16) & 0xFFFF;
    filterCfg.FilterIdLow      = (filterId << 3) & (0xFFF8 | CAN_ID_EXT);
    filterCfg.FilterMaskIdHigh = (maskId << 3 >> 16) & 0xFFFF;
    filterCfg.FilterMaskIdLow  = (maskId << 3) & (0xFFF8 | CAN_ID_EXT);
  }

  filterCfg.FilterFIFOAssignment =  /*!< @ref CAN_filter_FIFO */
//                      	 CAN_FILTER_FIFO0;  /*!< Filter FIFO 0 assignment for filter x */
                      	   CAN_FILTER_FIFO1;  /*!< Filter FIFO 1 assignment for filter x */

  filterCfg.FilterActivation = ENABLE;  /*!<  ENABLE or DISABLE. */

  filterCfg.BankNumber = 13;  /*!< Min_Data = 0 and Max_Data = 28. */ 
  /* A filter with its number greater than or equal to this value is assigned to DMA2	*/
	
  if( HAL_OK != HAL_CAN_ConfigFilter(p_hcan, &filterCfg) )
	{
	  return; /* Or set an error handler here. */
	};

}

