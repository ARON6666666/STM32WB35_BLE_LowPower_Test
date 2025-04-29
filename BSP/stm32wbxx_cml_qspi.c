/**
  ******************************************************************************
  * @file    stm32wbxx_cml_qspi.c
  * @brief   QUADSPI��������
  * @version <version> <time>   <author>          <desc>
  *          0.0.1     25/04/14 Jiaolong  Wu      ��ʼ�汾
  ******************************************************************************
  */


#include "stm32wbxx_cml_qspi.h"

// QSPI��������
QSPI_HandleTypeDef hqspi;


/*!
	\brief STM32WBxx QSPIʱ�ӳ�ʼ��
	\param[in] is_enabled 1 -- ʹ�� 0 -- ����
	\param[out] None
	\retval none
	\version 001 250414 JIAOLONG WU ��ʼ���汾
*/
void CML_QSPI_InitClock(uint8_t is_enabled)
{
	if (is_enabled)
		__HAL_RCC_QUADSPI_CLK_ENABLE();
	else
		__HAL_RCC_QUADSPI_CLK_DISABLE();
}

/*!
	\brief STM32WBxx QSPI�ж�IRQ����
	\param[in] is_enabled 1 -- ʹ�� 0 -- ����
	\param[out] None
	\retval none
	\version 001 250414 JIAOLONG WU ��ʼ���汾
*/
void CML_QSPI_InitIRQ(uint8_t is_enabled)
{
	if (is_enabled)
	{
		NVIC_SetPriority(QUADSPI_IRQn, 
					NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 
					1, 
					0));
		NVIC_EnableIRQ(QUADSPI_IRQn);
	}
	else
	{
		NVIC_DisableIRQ(QUADSPI_IRQn);
	}
}

/*!
	\brief STM32WBxx QSPI������������
	\param[in] None
	\param[out] None
	\retval none
	\version 001 250414 JIAOLONG WU ��ʼ���汾
*/
void CML_QSPI_InitGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/**QUADSPI GPIO Configuration
    PB8     ------> QUADSPI_BK1_IO1
    PB9     ------> QUADSPI_BK1_IO0
    PA2     ------> QUADSPI_BK1_NCS
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3
    PA7     ------> QUADSPI_BK1_IO2
    */
//	QSPI_GPIO_CLK_ENABLE();
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_7|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/*!
	\brief STM32WBxx QSPI��������
	\param[in] clockprescaler -- QSPIʱ�ӷ�Ƶ��0 - 255
    \param[in] flashsize  -- �ܷ�flash��ַ���ߴ�С�����ݷ��ʵ�flash�ĵ�ַ����λ��ȷ��
	\param[in] clockmode -- QSPI_CLOCK_MODE_0 or QSPI_CLOCK_MODE_3
    \param[in] cfg_mode  -- �ж�  DMA����
	\param[out] None
	\retval none
	\version 001 250414 JIAOLONG WU ��ʼ���汾
*/
void CML_QSPI_InitCfg(uint32_t ClockPrescaler, uint32_t FlashSize, uint32_t ClockMode)
{
	CML_QSPI_InitClock(1);
	CML_QSPI_InitGPIO();
	
	hqspi.Instance = QUADSPI;
	hqspi.Init.ClockPrescaler = ClockPrescaler;
	hqspi.Init.FifoThreshold = 1;
	hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
	hqspi.Init.FlashSize = FlashSize;
	hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
	hqspi.Init.ClockMode = ClockMode;
	HAL_QSPI_Init(&hqspi);
	CML_QSPI_InitIRQ(1);
}