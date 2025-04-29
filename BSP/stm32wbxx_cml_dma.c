


#include "stm32wbxx_cml_dma.h"




void CML_DMA_initClock(DMA_TypeDef* DMAx)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
	if (DMAx == DMA1) {
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	} else if (DMAx == DMA2) {
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
	}
}

// QSPI DMA控制数据
DMA_HandleTypeDef hdma_quadspi;
/*!
    \brief      配置QSPI的DMA通道，只需一个通道，并且使用HAL库
    \param[in]  DMA_TypeDef *DMAx
                QSPI_HandleTypeDef* qspiHandle
    \param[out] none
    \retval     none
    \version    V001 250227 ARON  初始版本
*/
void CML_DMA_QSPI_Init(QSPI_HandleTypeDef* qspiHandle, DMA_TypeDef* DMAx)
{
	CML_DMA_initClock(DMAx);
	hdma_quadspi.Instance = DMA1_Channel1;
    hdma_quadspi.Init.Request = DMA_REQUEST_QUADSPI;
    hdma_quadspi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_quadspi.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_quadspi.Init.MemInc = DMA_MINC_ENABLE;
    hdma_quadspi.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_quadspi.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_quadspi.Init.Mode = DMA_NORMAL;
    hdma_quadspi.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_quadspi) != HAL_OK)
    {
      HAL_DMA_DeInit(&hdma_quadspi);
    }

    __HAL_LINKDMA(qspiHandle,hdma,hdma_quadspi);
	
	NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}