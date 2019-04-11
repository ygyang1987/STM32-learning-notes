#ifndef _BSP_I2CIO_H
#define _BSP_I2CIO_H

#define I2C_CLKSCR_EA // 定义此宏，决定是否等待时钟释放

#include "stm8s.h"
#include "bsp_timer.h"

/* 宏定义 --------------------------------------------------------------------*/
#define PORT_I2C_SCL              GPIOB
#define PORT_I2C_SDA              GPIOB

#define PIN_I2C_SCL               GPIO_PIN_4
#define PIN_I2C_SDA               GPIO_PIN_5

#define I2C_L_SCL                 GPIO_WriteLow(PORT_I2C_SCL, PIN_I2C_SCL)

#ifdef  I2C_CLKSCR_EA //设SCL为高时，如果需要支持外设时钟抓取，此处需要设为1
#define I2C_H_SCL                 I2CIO_WirteHighSCL_Wait()
#else
#define I2C_H_SCL                 GPIO_WriteHigh(PORT_I2C_SCL, PIN_I2C_SCL)
#endif

#define I2C_L_SDA                 GPIO_WriteLow(PORT_I2C_SDA, PIN_I2C_SDA)
#define I2C_H_SDA                 GPIO_WriteHigh(PORT_I2C_SDA, PIN_I2C_SDA)

#define I2C_I_SDA                 GPIO_Init(PORT_I2C_SDA, PIN_I2C_SDA, GPIO_MODE_IN_FL_NO_IT)
#define I2C_R_SDA                 ((uint8_t) GPIO_ReadInputPin(PORT_I2C_SDA, PIN_I2C_SDA) )
#define I2C_O_SDA                 GPIO_Init(PORT_I2C_SDA, PIN_I2C_SDA, GPIO_MODE_OUT_OD_HIZ_SLOW)

#define I2C_I_SCL                 GPIO_Init(PORT_I2C_SCL, PIN_I2C_SCL, GPIO_MODE_IN_FL_NO_IT)
#define I2C_R_SCL                 ((uint8_t) GPIO_ReadInputPin(PORT_I2C_SCL, PIN_I2C_SCL) )
#define I2C_O_SCL                 GPIO_Init(PORT_I2C_SCL, PIN_I2C_SCL, GPIO_MODE_OUT_OD_HIZ_SLOW)

#define I2C_DELAY_FULL  TIMDelay_N10us(2)
#define I2C_DELAY_HALF  TIMDelay_N10us(1)

#define I2C_ACK                   0                        //应答
#define I2C_NOACK                 1                        //非应答

/* 函数声明 ------------------------------------------------------------------*/
void I2CIO_Initializes(void);
//void I2CIO_WirteHighSCL_Wait(void);
//uint8_t I2CIO_ReadSDA(void);

void I2C_Start(void);
void I2C_Stop(void);
void I2C_PutAck(uint8_t Ack);
uint8_t I2C_GetAck(void);

uint8_t I2C_WriteByte(uint8_t data);
uint8_t I2C_ReadByte(uint8_t ack);

#endif

