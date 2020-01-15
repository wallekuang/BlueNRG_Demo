/**
  ******************************************************************************
  * @file    cassert.h
  * @author  AMG GCSA Lucien
  * @version V1.0.0
  * @date    10/23/2018
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  ******************************************************************************
  */
#ifndef __CASSERT_H__
#define __CASSERT_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>



int COMPrintf(const char* fmt, ...);

int COMPrintf_hexdump(uint8_t *parr,uint8_t len);



#endif
