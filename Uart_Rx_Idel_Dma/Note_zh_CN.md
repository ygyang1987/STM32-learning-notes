# 为何采用空闲中断

STM32的串口，无硬件FIFO，为了存放大量的收发数据，一般用软件缓存配合用中断完成。但是若数据量大速率高，则频繁的中断也使得CPU占用开销变大，从而可能影响整机实时性。若结合DMA，则可大大解放CPU。

DMA发送：发送是主动发起的，因此数据长度一般已知的，帧间隔也已知。DMA接口函数基本可胜任发送操作，使用上不存在大问题。

DMA接收：接收时属于被动方，实际应用中主要问题在于两点：“如何判断帧间隔时间”与“如何实现不定长度字节接收”。由于DMA接收完成前不通知CPU，故配合串口空闲中断可解决这方面的问题。

---
# 要点
## 串口空闲中断如何配置开启/关闭？
若用标准库，使用以下函数在适当场合开启：
```
USART_ITConfig(USART1, USART_IT_IDLE, ENABLE) // 句柄可自行修改
USART_ITConfig(USART1, USART_IT_IDLE, DISABLE)
```
若用HAL库，使用以下函数在适当场合开启（注意CubeMx中暂无此中断项目的配置）：
```
__HAL_UART_ENABLE_IT(huart1, UART_IT_IDLE); // 句柄可自行修改
__HAL_UART_DISABLE_IT(huart1, UART_IT_IDLE); 
```

## 空闲中断如何判断是否发生？
与收发中断类似，中断都是同一个IQR；而在IQR中可以通过查找标志位，或中断标志位来判断：
如标准库判断ITStatus：
```
USART_GetITStatus(USART1, USART_IT_IDLE) == SET
```
HAL库判断FLAG：
```
__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) 
```

## 空闲中断触发后，是否需要软件手动清除标志？ 
经过翻查资料，答案为：需要（硬件不自动清零）。且注意，无法采用库函数 USART_ClearFlag() 或 USART_ClearItPending() 来清除IDEL标志。
一般方法是需要先读取USART_SR，再读取USART_DR。
若用标准库，即：
``` 
{__IO uint32_t tmp;  tmp = USART1->SR;  tmp = USART1->DR;}
```

HAL库则有现成的清该标志的宏（当然你追踪进去，实际展开后就会发现跟上面基本是一样的）：
```
__HAL_UART_CLEAR_IDLEFLAG(&huart1);
```

## 空闲中断发生后，如何判断已经接收了多少数据？
数据存放在在DMA句柄，寄存器CNDTR中（因为数据归DMA的寄存器管理，因此需要调用的是DMA句柄）。但注意这个值是“剩余多少个字节未传输完”。

若调试跟踪这个值，可发现DMA接收一个数据，它自减1。当减到0就触发DMA中断。

现在，我们用串口空闲中断把DMA的接收过程中止，而DMA总数据量则是事先软件设置的，相减就能得到接收了多少个数据。

标准库中例子如下：
```
RxDataSize = DmaBufferSize - DMA1_Channel5->CNDTR;
```
而HAL库则也有宏：
``` C
RxDataSize = DmaBufferSize - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx)
```
