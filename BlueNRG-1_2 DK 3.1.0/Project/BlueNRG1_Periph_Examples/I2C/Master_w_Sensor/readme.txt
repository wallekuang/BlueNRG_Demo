/**
  @page I2C_Examples I2C Examples
  @section I2C_Master I2C_Master
  
  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    I2C/Master/readme.txt 
  * @author  MEMS Application Team
  * @version V1.0.0
  * @date    September-2014
  * @brief   I2C master example code.
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
This code provides an example how the I2C could be configured in master mode and used to transmit
and receive the data from the slave using the I2C status register flag polling.

@par Detail Description

In this example the WHO_AM_I register content of a sensor connected on the I2C is read.
Pins GPIO4 and GPIO5 are configured as outputs so a LED could be connected there.
The I2C1 is configured in the master mode.

As soon as the device configuration is finished the I2C communication is initiated
and the content of the WHO_AM_I register of the connected sensor is read. 
 - If it matches the WHO_AM_I register content of the expected sensor the GPIO4 pin is set.
 - If the received WHO_AM_I register content doesn't match the expected value the GPIO5 is set.
 - If the I2C operation is aborted due to the occurrence of some event both GPIO4 and GPIO5 pins remain in the reset state.
 
 
@par Notes
In this example the system clock is configured to the default value of 18 MHz. 


@par How to use it
Connect pull-up resistors (4k7) between GPIO7, GPIO8 and VDD. Then power up the device and download 
the example code. After the reset the I2C communication is initiated and the GPIO4
or GPIO5 pin is set based on the WHO_AM_I register content received from the slave device.
 - If the WHO_AM_I register content matches the WHO_AM_I register content of the expected sensor the GPIO4 pin is set.
 - If the received WHO_AM_I register content doesn't match the expected value the GPIO5 is set.
 - If the I2C operation is aborted due to the occurrence of some event both GPIO4 and GPIO5 pins remain in the reset state.

 The code above is repeated in the infinite loop so the WHO_AM_I register value is read periodically.
  
 * <h3><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h3>
 */


