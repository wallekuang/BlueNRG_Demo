

1. 此Demo 用于演示在任意IO口 模拟串口输出log.  
   供一些客户硬件上已经使用了一路串口，然后又想要通过串口进行调试；
   或者供客户没有引出串口，但后期希望使用串口调试。
   波特率默认是9600， 没有调试好115200，使用115200波特率可能有影响(与蓝牙中断冲突)

2. 原理
	a. 使用GPIO 模拟串口
	b. 增加重写vsprintf
	c. 增加FIFO存储log,控制输出
	
3. 移植到其他工程方法	
	a. 添加相关文件  cassert.c  user_buffer.c   simulate_uart.c  到目标工程
	b. 在程序开始处设置初始化 simulate_uart_init();
	c. 在sistick 中断中 void SysTick_Handler(void)  增加simulate_uart_handler();
	d. 使用COMPrintf()输出日志  用法和printf一样
	e. 如果调试带睡眠的程序，增加log, 需要在睡眠检测函数中增加，因为睡眠systick是不工作，会影响log输出。
		SleepModes App_SleepMode_Check(SleepModes sleepMode)
		{
		  if(SdkEvalComIOTxFifoNotEmpty() || SdkEvalComUARTBusy())
			return SLEEPMODE_RUNNING;
		  
			if(simulate_uart_fifo_len())
				return SLEEPMODE_RUNNING;
			
		  return SLEEPMODE_NOTIMER;
		}