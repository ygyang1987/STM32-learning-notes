/**
  ******************************************************************************
  GPIO模拟I2C驱动，支持从机时钟抓取。代码核心操作用宏编写，方便不同平台移植。
  注意GPIO的输入输出状态需要频繁切换。
  ******************************************************************************/
#include "bsp_i2cio.h" 

/************************************************
I2C初始化，均配置为输出并释放
*************************************************/
void I2CIO_Initializes(void)
{
  I2C_O_SCL;
  I2C_H_SCL; 
  I2C_O_SDA;
  I2C_H_SDA;
  I2C_DELAY_FULL;
}

/************************************************
作为主机释放SCL，并检测；阻塞直到其确实释放
REF: https://www.i2c-bus.org/clock-stretching/
*************************************************/
void I2CIO_WirteHighSCL_Wait(void)
{
  GPIO_WriteHigh(PORT_I2C_SCL, PIN_I2C_SCL);
  I2C_I_SCL;// 等待时钟抓取结束
  while(I2C_R_SCL==1) 
  {
    ;
  }
  I2C_O_SCL;
}

/************************************************
I2C开始位（S与RS）
*************************************************/
void I2C_Start(void)
{
  I2C_H_SDA;
  I2C_DELAY_HALF;
  I2C_H_SCL;
  I2C_DELAY_FULL;
  I2C_L_SDA;
  I2C_DELAY_FULL;
  I2C_L_SCL;
}

/************************************************
I2C停止位（P）
*************************************************/
void I2C_Stop(void)
{
  I2C_L_SCL;
  I2C_L_SDA;
  I2C_DELAY_HALF;
  I2C_H_SCL;
  I2C_DELAY_FULL;
  I2C_H_SDA;
  I2C_DELAY_FULL;
}

/************************************************
功    能 ： I2C主机应答(或不应答)
参    数 ： I2C_ACK :应答
            I2C_NOACK :不应答
*************************************************/
void I2C_PutAck(uint8_t Ack)
{
  I2C_L_SCL;
  if(I2C_ACK == Ack)
  {
    I2C_L_SDA;
  }
  else
  {
    I2C_H_SDA;
  }
  I2C_DELAY_HALF;
  
  I2C_H_SCL;
  I2C_DELAY_HALF;

  I2C_L_SCL;
}

/************************************************
函数名称 ： I2C_GetAck
功    能 ： I2C主机查询从机有无应答（若无，发送一个停止位）
参    数 ： 无
返 回 值 ： I2C_ACK :从机应答
           I2C_NOACK :从机无应答
*************************************************/
uint8_t I2C_GetAck(void)
{
  uint8_t RetryTimes=0;
  I2C_H_SDA;
  I2C_I_SDA;
  I2C_DELAY_HALF;
  I2C_H_SCL;
  I2C_DELAY_HALF;
  while(I2C_R_SDA==I2C_NOACK)
  {
    I2C_DELAY_HALF;
    ++RetryTimes;
    if(RetryTimes>=2) // 可调节以灵活匹配实际从机的ACK
    {
      I2C_O_SDA;
      I2C_Stop(); // 确定无应答后，发送停止位
      return I2C_NOACK;
    }
  }
  I2C_O_SDA;
  I2C_H_SDA;
  I2C_L_SCL;
  return I2C_ACK; 
  
}
//以上为基本位操作

/************************************************
I2C主机写一字节，并听取应答
参    数 ： data --- 待写的数据
返 回 值 ： I2C_ACK ----- 从机应答
            I2C_NOACK --- 从机无应答
*************************************************/
uint8_t I2C_WriteByte(uint8_t data)
{
  uint8_t cnt;
  
  for(cnt=0; cnt<8; ++cnt)
  {
    I2C_L_SCL;
    if(data & 0x80)
    {
      I2C_H_SDA;
    }
    else
    {
      I2C_L_SDA;
    }
    data <<= 1;
    I2C_DELAY_HALF;
    I2C_H_SCL;
    I2C_DELAY_HALF;
    I2C_L_SCL;
    I2C_DELAY_HALF;
  }
  return I2C_GetAck(); //读取应答位

}

/************************************************
I2C主机读一字节，并决定是否应答
参    数 ： ack - 读取后，是应答(或者非应答)位
返 回 值 ： 读到的一字节数据
*************************************************/
uint8_t I2C_ReadByte(uint8_t ack)
{
  uint8_t cnt;
  uint8_t data=0;

  I2C_H_SDA;
  I2C_I_SDA;
  for(cnt=0; cnt<8; ++cnt)
  {
    I2C_L_SCL;
    I2C_DELAY_HALF;
    I2C_H_SCL;
    data <<= 1;
    if(I2C_R_SDA)
    {
      ++data;  //等同于(data |= 0x01)但指令更快
    }
    I2C_DELAY_HALF;
  }
  I2C_O_SDA;
  I2C_PutAck(ack);
  
  return data;
}

/* Eof */
