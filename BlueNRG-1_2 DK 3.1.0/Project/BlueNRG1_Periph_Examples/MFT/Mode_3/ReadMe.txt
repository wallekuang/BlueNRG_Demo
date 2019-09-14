/**
  @page MTFX_Mode_3 Mode 3 Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    MFTX/Mode_3/readme.txt 
  * @author  MEMS Application Team
  * @version V1.0.0
  * @date    September-2014
  * @brief   Description of the example code functionality.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim


@par Example Description 

This example demonstrate the possibility of multi-function timers configured in mode 3 (dual independent timer/counter). In this example the MFTX Timer/Counter 1 is used to generate 1kHz signal and Timer/Counter 2 to count external clock signal.

@par Detail Description

In mode 3 (dual independent timer/counter), the timer/counter is configured to operate as a dual independent system timer or dual external-event counter. In addition, Timer/Counter 1 can generate a 50% duty cycle PWM signal on the TnA pin. The TnB pin can be used as an external-event input or pulse-accumulate input, and serve as the clock source to either Timer/Counter 1 or Timer/Counter 2. Both counters can also be operated from the prescaled system clock.

In this example MFTX3 is used. 8MHz RC oscilator is enabled and selected as clock signal for Timer/Counter 1. This clock is divided by 8. Timer/Counter 2 uses external clock on TnB pin. Sensitivity to rising edge is selected for TnB input. Timer/Counter 1 is preset and reloaded to 499 so the frequency of the output signal is 1kHz. Timer/Counter 2 is preset and reloaded to 4.

The GPIO 6 pin is internally connected to TnB input (MFTX0).

The GPIO 10 pin is configured as output and configuret to be PWM output from Timer/Counter 1.
The GPIO 9 pin is configured as output and is toggled in the main program according variable which is changed in TnD interrupt rutine.

Interrupt TnA is enabled. This interrupt occures at underflow of Timer/Counter1. 
Interrupt TnD in enabled. This interrupt occures at underflow of Timer/Counter2.

@par Notes

In this example, system clock is configured to default value of 18 MHz.

@par How to use it

If any squere signal (up to 300 kHz) is connected to GPIO 6 then on the GPIO 9 is generated signal with 1/10 frequncy of the input signal.
1kHz square signal is continously generated at GPIO 10.
   
 * <h3><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h3>
 */