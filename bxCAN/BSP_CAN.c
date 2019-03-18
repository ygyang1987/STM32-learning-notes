#include "can.h"
extern CAN_HandleTypeDef hcan;

/** *******************************************************
Filter config with the macros:
IDE: =0 for STD,   =1 for EXT.
RTR: =0 for DATA,  =1 for REMOTE. 
******************************************************** */
/* SCALE32 handles ExtId: */
#define SCL32_EXT_HIGH(_ExtId_)         ( ((_ExtId_) << 3 >> 16) & 0xFFFF )
#define SCL32_EXT_DATA_LOW(_ExtId_)     ( ((_ExtId_) << 3) & 0xFFF8 | (1<<2) | (0<<1) ) /* [IDE RTR 0] for last 3 bits */
#define SCL32_EXT_REMOTE_LOW(_ExtId_)   ( ((_ExtId_) << 3) & 0xFFF8 | (1<<2) | (1<<1) )

/* SCALE32 handles StdId: */
#define SCL32_STD_HIGH(_StdId_)         ( ((_StdId_) << 5) & 0xFFE0 )
#define SCL32_STD_DATA_LOW(_StdId_)     ( (0<<2) | (0<<1) ) /* [IDE RTR 0] for last 3 bits */
#define SCL32_STD_REMOTE_LOW(_StdId_)   ( (0<<2) | (1<<1) )

/* SCALE16 handles StdId: */
#define SCL16_STD_DATA(_StdId_)         ( ((_StdId_) << 5) & 0xFFE0 | (0<<3) | (0<<4) ) /* [ RTR IDE EXIT[17:15] ] for last 5 bits */
#define SCL16_STD_REMOTE(_StdId_)       ( ((_StdId_) << 5) & 0xFFE0 | (0<<3) | (1<<4) )

/* SCALE16 should not handle ExtId so there is no SCL16_EXTID marco. */

/** *******************************************************
Start
Filter config. 
Start CAN peripheral.
Start Activate Notification.
******************************************************** */
void BSP_hcanRxCfgStart(void)
{
	CAN_FilterTypeDef filterCfg;
  uint32_t filterId = 0x12345678;
  uint32_t maskId   = 0x00000000;  /*Bits: =1 for must-match,  =0 for not-care */
	
  filterCfg.FilterBank = 0;  /*!< Specifies the filter bank which will be initialized.*/
  /* For single CAN instance(14 dedicated filter banks), Min_Data = 0 and Max_Data = 13.
     For dual CAN instances(28 filter banks shared), Min_Data = 0 and Max_Data = 27. */
  {
    /* ATTENTION: Choose one mode. Your choice must match the configuration below. */
	  filterCfg.FilterMode  = /*!<  @ref CAN_filter_mode */
                            CAN_FILTERMODE_IDMASK;  
//                            CAN_FILTERMODE_IDLIST;  /*!< Identifier list mode */
    filterCfg.FilterScale = /*!<  @ref CAN_filter_scale */
//                            CAN_FILTERSCALE_16BIT;  /*!< Two 16-bit filters */
                            CAN_FILTERSCALE_32BIT;  /*!< One 32-bit filter  */
		
		/* ATTENTION: Modify these variables to accommodate different IDs: */
//    uint32_t filterId = 0x12345678;

    /* ATTENTION: Modify these marcos to accommodate different modes: */
    filterCfg.FilterIdHigh     = SCL32_EXT_HIGH(filterId);
    filterCfg.FilterIdLow      = SCL32_EXT_DATA_LOW(filterId);
    filterCfg.FilterMaskIdHigh = SCL32_EXT_HIGH(maskId);
    filterCfg.FilterMaskIdLow  = SCL32_EXT_DATA_LOW(maskId);
  }

  filterCfg.FilterFIFOAssignment =  /*!< Specifies the FIFO (0 or 1U); @ref CAN_filter_FIFO */
                                    CAN_FILTER_FIFO0;
//                                    CAN_FILTER_FIFO1;
	
  filterCfg.FilterActivation =   /*!< Enable or disable the filter; @ref CAN_filter_activation */
//                                    CAN_FILTER_DISABLE /*!< Disable filter */
                                    CAN_FILTER_ENABLE;

  filterCfg.SlaveStartFilterBank = 14; /*Select the start filter bank for the slave CAN instance.*/
/*^For single CAN instances, this parameter is meaningless.
   For dual CAN instances, filter banks with lower index are assigned to master CAN instance, 
	                whereas those with greater index are assigned to slave CAN instance. */ //<- But what if equal to it ? 
/* This parameter must be a number between Min_Data = 0 and Max_Data = 27. */	

  if( HAL_OK != HAL_CAN_ConfigFilter(&hcan, &filterCfg) )
  {
	  Error_Handler();
  }

  /* Start the CAN peripheral */
  if ( HAL_OK != HAL_CAN_Start(&hcan) )
  {
    Error_Handler();
  }

  /* Activate CAN RX notification */
  if ( HAL_OK != HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) )
  {
    Error_Handler();
  }
  return;
}

/** *******************************************************
TX
******************************************************** */
uint32_t KPChgr_canTx( void )
{
	uint32_t TxMailboxRt;  // @arg CAN_Tx_Mailboxes
  
  KP_Charger.TxHd.IDE   =  /*!< @ref CAN_Identifier_Type */
//                   CAN_ID_STD; /*!< Standard Id */
                   CAN_ID_EXT;  /*!< Extended Id */
//  KP_Charger.TxHd.StdId = (uint32_t) 0x000;       /*!< Min_Data = 0 and Max_Data = 0x7FF */
  KP_Charger.TxHd.ExtId = (uint32_t) 0x12345678;  /*!< Min_Data = 0 and Max_Data = 0x1FFFFFFF */
  KP_Charger.TxHd.RTR   =  /*!< type of the frame @ref CAN_remote_transmission_request */
                   CAN_RTR_DATA; /*!< Data frame */
//                   CAN_RTR_REMOTE; /*!< Remote frame */
  KP_Charger.TxHd.DLC   = 8;  /*!< Length of the frame. Min_Data = 0 and Max_Data = 8 */
  KP_Charger.TxHd.TransmitGlobalTime = DISABLE;
  
  KP_Charger.TxDat[0] = (uint8_t) 0xF1;
  KP_Charger.TxDat[1] = (uint8_t) 0xF2;
  KP_Charger.TxDat[2] = (uint8_t) 0xF3;
  KP_Charger.TxDat[3] = (uint8_t) 0xF4;
  KP_Charger.TxDat[4] = (uint8_t) 0xF5;
  KP_Charger.TxDat[5] = (uint8_t) 0xF6;
  KP_Charger.TxDat[6] = (uint8_t) 0xF7;
  KP_Charger.TxDat[7] = (uint8_t) 0xF8;
  
  if( HAL_CAN_GetTxMailboxesFreeLevel( &hcan ) != 0 )
	{
    if( HAL_OK != HAL_CAN_AddTxMessage(&hcan, &(KP_Charger.TxHd), (KP_Charger.TxDat), &TxMailboxRt) )
    {
      return 0xFFFFFFFF; /* Fail to Tx */
    }
	}
	else
	{
    return 0xFFFFFFFE; /* TxMailboxes not free */
	}
  return TxMailboxRt; /* Success. Return TxMailbox @arg CAN_Tx_Mailboxes. */
}


/** *******************************************************
RX
Enable "USB low priority or CAN RX0 interrupts" and then
 place this functon in void HAL_CAN_RxFifo0MsgPendingCallback().
******************************************************** */
uint32_t KPChgr_canRx(void)
{
  if(HAL_OK != HAL_CAN_GetRxMessage( &hcan, CAN_RX_FIFO0, &(KP_Charger.RxHd), KP_Charger.RxDat) )
	{
    return 0xFFFFFFFF; /* Fail to Rx */
	}
  return 0;
}

/**
***************************************************************************
  * @brief  Rx Fifo 0 message pending callback in non blocking mode
  * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
	***************************************************************************
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
  KPChgr_canRx();
}

/****END OF FILE****/




