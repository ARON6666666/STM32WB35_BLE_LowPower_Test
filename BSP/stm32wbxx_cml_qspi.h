/**
  ******************************************************************************
  * @file    stm32wbxx_cml_qspi.c
  * @brief   QUADSPI驱动函数
  * @version <version> <time>   <author>          <desc>
  *          0.0.1     25/04/14 Jiaolong  Wu      初始版本
  ******************************************************************************
  */

#ifndef STM32WBXX_CML_QSPI_H_
#define STM32WBXX_CML_QSPI_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32wbxx.h"




void CML_QSPI_InitClock(uint8_t is_enabled);
void CML_QSPI_InitIRQ(uint8_t is_enabled);
void CML_QSPI_InitGPIO(void);
void CML_QSPI_InitCfg(uint32_t ClockPrescaler, uint32_t FlashSize, uint32_t ClockMode);


#ifdef __cplusplus
 }
#endif


#endif


