/**
  ******************************************************************************
  * @file    nand_flash.c
  * @brief   ���״���GD5F2MG7 256MB Nand Flash�Ļ���QSPI���ߵײ�IO
  * @version <version> <time>   <author>          <desc>
  *          0.0.1     25/03/17 Jiaolong  Wu      ��ҳ���Ͷ�ҳд������ʹ��
  ******************************************************************************
  */
#include "main.h"
#include "nand_flash.h"

#include <string.h>
/*��
	\brief ����GD5F2GM7��ID, 
	       C8�ǳ���ID��
	       DID1���豸ID
*/
static uint8_t MID = 0xC8;
#if defined GD5F2GM7UExxG
static uint8_t DID1 = 0x92;
#elif defined GD5F2GM7RExxG
static uint8_t DID1 = 0x82;
#endif


extern QSPI_HandleTypeDef hqspi;
nand_flash_info_buff_t nand_flash_info_buff = {0};

/* Private function prototypes -----------------------------------------------*/
static uint8_t nand_flash_get_status(uint8_t status_reg_addr);
static uint8_t nand_flash_clear_feature_reg(uint8_t feature);
static uint8_t nand_flash_set_feature_reg(uint8_t feature);
static uint8_t nand_flash_read_feature_reg(void);
static uint8_t nand_flash_get_protection_reg(void);
static uint8_t nand_flash_set_protection_reg(uint8_t reg_val);
static uint8_t nand_flash_get_id(uint16_t* id);
static uint8_t nand_flash_get_param_page(uint8_t*);
static uint8_t nand_flash_get_check_uid(uint8_t*);
static uint8_t nand_flash_page_read(uint32_t column_addr);




/*��
	\brief ��ȡGD5F2GM7��״̬�Ĵ���
	\param[in] status_reg_addr -- ״̬�Ĵ�����ַ
	\param[in] mask_bit -- ״̬�Ĵ�������
	\param[out] None
	\retval 0 
	\version 0.0.1
*/
static uint8_t nand_flash_get_status(uint8_t status_reg_addr)
{
	QSPI_CommandTypeDef s_cmd = {0};
	uint8_t status = 0;
	s_cmd.Instruction = GET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	s_cmd.Address = status_reg_addr;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive(&hqspi, &status, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);


	return status;
}

/*!
	\brief ��ȡGD5F2GM7��feature�Ĵ���
	\param[in] None
	\param[out] None
	\retval feature�Ĵ�������ֵ
	\version 0.0.1
*/
static uint8_t nand_flash_read_feature_reg(void)
{
	uint8_t reg_val = 0;
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = GET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = REG_FEATURES1;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	HAL_QSPI_Receive(&hqspi, &reg_val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	return reg_val;
}

/*!
	\brief ����GD5F2GM7 feature �Ĵ���
	\param[in] feature -- feature�Ĵ�������ֵ
	\param[out] None
	\retval 0 -- ���óɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
static uint8_t nand_flash_set_feature_reg(uint8_t feature)
{
	QSPI_CommandTypeDef s_cmd = {0};
	uint8_t reg_val = 0;
	uint8_t ret = 1;
	/*
		feature1�Ĵ���
		bit        7         6        5		  4       3       2         1        0 
	     	| OTP_PRT | OTP_EN | RESERVED | ECC_EN | BPL | RESERVED | RESERVED | QE |
	*/
	// ��ȡfeatures1�Ĵ���ֵ
	reg_val = nand_flash_read_feature_reg();
	reg_val |= feature;

	// ����features1�Ĵ���
	s_cmd.Instruction = SET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = REG_FEATURES1;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Transmit(&hqspi, &reg_val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	// �ض�
	reg_val = 0;
	reg_val = nand_flash_read_feature_reg();
	if ((reg_val & feature) == feature)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	return ret;
}

/*!
	\brief ����GD5F2GM7 feature �Ĵ���
	\param[in] feature -- feature�Ĵ�������ֵ
	\param[out] None
	\retval 0 -- ���óɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
static uint8_t nand_flash_clear_feature_reg(uint8_t feature)
{
	QSPI_CommandTypeDef s_cmd = {0};
	uint8_t reg_val = 0;
	uint8_t ret = 1;
	/*
		feature1�Ĵ���
		bit        7         6        5		  4       3       2         1        0 
	     	| OTP_PRT | OTP_EN | RESERVED | ECC_EN | BPL | RESERVED | RESERVED | QE |
	*/
	// ��ȡfeatures1�Ĵ���ֵ
	reg_val = nand_flash_read_feature_reg();
	reg_val &= ~feature;

	// ����features1�Ĵ���
	s_cmd.Instruction = SET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = REG_FEATURES1;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Transmit(&hqspi, &reg_val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	// �ض�
	reg_val = 0;
	reg_val = nand_flash_read_feature_reg();
	if ((reg_val & feature) == 0)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	return ret;
}

/*!
	\brief ��ȡGD5F2GM7��protection�Ĵ���
	\param[in] None
	\param[out] None
	\retval protection�Ĵ�������ֵ
	\version 0.0.1
*/
static uint8_t nand_flash_get_protection_reg(void)
{
	QSPI_CommandTypeDef s_cmd = {0};
	uint8_t reg_val = 0;
	s_cmd.Instruction = GET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = REG_PROTECTION;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive(&hqspi, &reg_val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	return reg_val;
}

/*!
	\brief ����GD5F2GM7��protection�Ĵ���
	\param[in] reg_val -- protection�Ĵ�������ֵ
	\param[out] None
	\retval 0 -- ���óɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
static uint8_t nand_flash_set_protection_reg(uint8_t reg_val)
{
	QSPI_CommandTypeDef s_cmd = {0};
	uint8_t ret = 1;
	s_cmd.Instruction = SET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = REG_PROTECTION;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Transmit(&hqspi, &reg_val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	if (nand_flash_get_protection_reg() == reg_val)
	{
		ret = 0;
	}

	s_cmd.Instruction = GET_FEATURES_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = REG_STATUS2;
	s_cmd.AddressSize = QSPI_ADDRESS_8_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = 1;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive(&hqspi, &reg_val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	return ret;
}

/*!
	\brief ��ȡGD5F2GM7��ID
	\param[in] None
	\param[out] id -- ��ȡ����ID
	\retval 0 -- ��ȡ�ɹ� 1 -- ��ȡʧ��
	\version 0.0.1
*/
static uint8_t nand_flash_get_id(uint16_t* id)
{
	uint8_t ret = 0;
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = READ_ID_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.DummyCycles = 8;
	s_cmd.NbData = 2;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}

	// ����ID
	if (HAL_QSPI_Receive(&hqspi, (uint8_t *)id, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}

static uint8_t nand_flash_get_check_uid(uint8_t* uid)
{
	uint8_t ret = 1;
	QSPI_CommandTypeDef s_cmd = {0};

	// ����OTP_EN
	if (nand_flash_OTP_Enable())
	{
		nand_flash_OTP_Disable();
		return ret;
	}

	// ��ȡUID
	if (nand_flash_page_read(ADDR_UID))
	{
		return ret;
	}
	s_cmd.Instruction = READ_CACHE_x1_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = ADDR_UID;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.AlternateBytes = 0;
	s_cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	s_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_1_LINE;

	s_cmd.NbData = 32;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	HAL_QSPI_Receive_DMA(&hqspi, uid);
	HAL_Delay(1);

	nand_flash_OTP_Disable();
	// ���UID 
	for (int i = 0; i < 16; i++)
	{
		if ((uid[i] ^ uid[i + 16]) != 0xFF)
		{
			return ret;
		}
	}
	return 0;
}

/*!
	\brief ��ȡGD5F2GM7�Ĳ������õ�ǰ128�ֽ�
	\param[in] None
	\param[out] param_buff -- ��ȡ���Ĳ�������
	\retval 0 -- ��ȡ�ɹ� 1 -- ��ȡʧ��
	\version 0.0.1
*/
static uint8_t nand_flash_get_param_page(uint8_t* param_buff)
{
	uint8_t ret = 1;
	uint8_t dummy = 0;
	QSPI_CommandTypeDef s_cmd = {0};

	// ����OTP_EN
	if (nand_flash_OTP_Enable())
	{
		nand_flash_OTP_Disable();
		return ret;
	}

	// ��ȡPARAM_PAGE
	if (nand_flash_page_read(ADDR_PARAM_PAGE))
	{
		return 1;
	}

	s_cmd.Instruction = READ_CACHE_x1_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = 0;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;


	s_cmd.AlternateBytes = 0;
	s_cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	s_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_1_LINE;

	s_cmd.NbData = 128;
	s_cmd.DataMode = QSPI_DATA_1_LINE;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive_DMA(&hqspi, param_buff);
	HAL_Delay(2);
	nand_flash_OTP_Disable();
	
	return 0;
}



/*!
	\brief GD5F2GM7 �����ݰᵽCache Registers
	\param[in] column_addr -- ����ʼ��ַ��ÿ��block��Page��ַ
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
static uint8_t nand_flash_page_read(uint32_t column_addr)
{
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = PAGE_READ_TOCACHE_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	s_cmd.Address = column_addr;
	s_cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;

	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	
	// ��ѯ״̬
	if (nand_flash_status_reg_check_bit(0x00))
	{
		return 1;
	}
	
	return 0;
}

/*!
	\brief GD5F2GM7 ʹ��OTP Region ��д
	\param[in] None
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_OTP_Enable(void)
{
	return nand_flash_set_feature_reg(OTP_REGION_EN_CMD);
}

/*!
	\brief GD5F2GM7 �ر�OTP Region ��д
	\param[in] None
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_OTP_Disable(void)
{
	nand_flash_clear_feature_reg(OTP_REGION_EN_CMD);
	return nand_flash_set_feature_reg(OTP_REGION_PRT_CMD);
}

/*!
	\brief GD5F2GM7 ���״̬�Ĵ���bitλ
	\param[in] mask_bit -- ״̬�Ĵ�������
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_status_reg_check_bit(uint8_t mask_bit)
{
	uint8_t ret = 1;
	uint8_t retry = 255;
	uint8_t status = 0;

	if (mask_bit == IN_OP)
	{
		do 
		{
			status = nand_flash_get_status(REG_STATUS1);
			if ((status & 0x01) == 0)
			{
				ret = 0;
				break;
			}
		} while (retry--);
	}
	else if (mask_bit == WEL)
	{
		do 
		{
			status = nand_flash_get_status(REG_STATUS1);
			if ((status & mask_bit) != 0)
			{
				ret = 0;
				break;
			}
		} while (retry--);
	}
	else
	{
		do 
		{
			status = nand_flash_get_status(REG_STATUS1);
			if ((status & 0x01) == 0 && (status & mask_bit) == 0)
			{
				ret = 0;
				break;
			}
			else if ((status & mask_bit) != 0) 	// Error Occur
			{
				ret = 1;
				break;
			}
		} while (retry--);
	}
	
	return ret;
}

/*!
	\brief GD5F2GM7����λ, ʹDevice����IDLE״̬
	\param[in] None
	\param[out] None
	\retval None
	\version 0.0.1
*/
void nand_flash_softreset(void)
{
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = RESET_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_Delay(1);
}

/*!
	\brief GD5F2GM7���ϵ縴λ
	\param[in] None
	\param[out] None
	\retval None
	\version 0.0.1
*/
void nand_flash_poweronreset(void)
{
	QSPI_CommandTypeDef s_cmd = {0};
	
	s_cmd.Instruction = ENABLE_POWER_ON_RESET_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	
	s_cmd.Instruction = POWER_ON_RESET_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;	
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_Delay(2);
}





/*!
	\brief ��ʼ��GD5F2GM7
	\param[in]
	\param[out]
	\retval 0 -- ��ʼ���ɹ� 1 -- ��ʼ��ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_initialize(void)
{
	uint8_t ret = 1;
	QSPI_CommandTypeDef s_cmd = {0};

	union 
	{
		uint8_t byte[2];
		uint16_t word;
	} id = {0};

	
	// ��λ
	nand_flash_poweronreset();

	// ��ȡID
	nand_flash_get_id(&id.word);
	if (id.byte[0] == MID && id.byte[1] == DID1)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}


	// ����feature
	nand_flash_set_feature_reg(QE_BIT);

	// ��ȡUID
	nand_flash_get_check_uid(&nand_flash_info_buff.uid[0]);
	
	// ��ȡparam page
	nand_flash_get_param_page(&nand_flash_info_buff.param_page[0]);
	
	// ����PROTECTION
	// uint8_t temp_val = protection_reg_val | 0x80;
	nand_flash_set_protection_reg(0x00);

//	nand_flash_erase_all_block();
	
	return ret;
}

/*!
	\brief GD5F2GM7 дʹ�� (Program Load)
	\param[in] None
	\param[out] None
	\retval None
	\version 0.0.1
*/
void nand_flash_write_enable(void)
{
	uint8_t status = 0, a,b =2;
	QSPI_CommandTypeDef s_cmd = {0};
	
	while(HAL_QSPI_GetState(&hqspi) != HAL_QSPI_STATE_READY)
	{
		;
	}
	s_cmd.Instruction = WRITE_ENABLE_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	if (nand_flash_status_reg_check_bit(WEL))
	{
		return;
	}
}

/*!
	\brief GD5F2GM7 д��ֹ (Program Load)
	\param[in] None
	\param[out] None
	\retval None
	\version 0.0.1
*/
void nand_flash_write_disable(void)
{
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = WRITE_DISABLE_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

/*!
	\brief GD5F2GM7 ��ʼд��
	\param[in] addr -- д��ĵ�ַ
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_start_program(uint32_t addr)
{
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = PROGRAM_EXE_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = addr;
	s_cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	// ��ѯ״̬
	if (nand_flash_status_reg_check_bit(P_FAIL))
	{
		return 1;
	}
	
	return 0;
}

/*!
	\brief GD5F2GM7 ����һ��block
	\param[in] raw_addr -- block�׵�ַ
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_erase_block(uint32_t raw_addr)
{
	QSPI_CommandTypeDef s_cmd = {0};
	
	nand_flash_write_enable();

	s_cmd.Instruction = BLOCK_ERASE_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_cmd.Address = raw_addr;
	s_cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);

	// ��ѯ״̬,һ��3ms��һ��block
	if (nand_flash_status_reg_check_bit(E_FAIL))
	{
		return 1;
	}

	return 0;
}

/*!
	\brief GD5F2GM7 ��������block
	\param[in] raw_addr -- block�׵�ַ
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_erase_all_block(void)
{
	uint8_t ret;
	uint16_t erase_cnt = 0;
	// ����block 0 ~ 2047
	for (uint16_t i = 0; i < BLOCK_COUNT; i++)
	{
	 	union {
	 		uint32_t ra;
	 		struct {
	 			uint32_t resvered : 6;
	 			uint32_t block_addr : 11;
	 			uint32_t reserved : 15;
	 		} bits;
	 	} raw_addr = {0};
	 	raw_addr.bits.block_addr = i;
	 	ret = nand_flash_erase_block(raw_addr.ra);
	 	if (!ret)
	 	{
	 		erase_cnt++;
	 	}	
	}

	return ret;
}

/*!
	\brief GD5F2GM7 дһҳ����
	\param[in] addr -- д��ĵ�ַ
	\param[in] prog_cmd -- д������
	\param[in] len -- д��ĳ���, һ��Ϊһ��ҳ�Ĵ�С
	\param[in] pbuff -- д�������
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_write_page(uint32_t addr, uint8_t prog_cmd, uint8_t *pbuff, uint32_t len)
{
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = prog_cmd;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	s_cmd.Address = 0;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = len;
	if (prog_cmd == PROGRAM_LOAD_CMD || prog_cmd == PROGRAM_LOAD_RANDOM_DATA_CMD) {
		s_cmd.DataMode = QSPI_DATA_1_LINE;
	} else if (prog_cmd == PROGRAM_LOAD_x4_CMD || prog_cmd == PROGRAM_LOAD_RANDOM_DATA_x4_CMD) {
		s_cmd.DataMode = QSPI_DATA_4_LINES;
	} 
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
#ifdef USE_QSPI_DMA
	HAL_QSPI_Transmit_DMA(&hqspi, pbuff);
#elif USE_QSPI_IT
	HAL_QSPI_Transmit_IT(&hqspi, pbuff);
#else
	HAL_QSPI_Transmit(&hqspi, pbuff, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
#endif
	nand_flash_write_enable();

	if (nand_flash_start_program(addr))
	{
		nand_flash_write_disable();
		return 1;
	}
	nand_flash_write_disable();
	return 0;
}

/*!
	\brief дGD5F2GM7�Ķ�ҳ����
	\param[in] addr -- д�ĵ�ַ
	\param[in] count -- д��ҳ��
	\param[out] pbuff -- д������
	\retval 0 -- д�ɹ� 1 -- дʧ��
	\version 0.0.1
*/
uint8_t nand_flash_write_multi_page(uint32_t addr, uint8_t* pbuff, uint32_t count)
{
	while (count--)
	{
		nand_flash_write_page(addr, PROGRAM_LOAD_RANDOM_DATA_x4_CMD, pbuff, PAGE_SIZE);
		addr++; // ƫ�Ƶ���һ��page
		// pbuff += PAGE_SIZE;
	}
	return 0;
}


/*!
	\brief ��ȡGD5F2GM7��һҳ����
	\param[in] addr -- ��ȡ�ĵ�ַ
	\param[in] rd_cache_cmd -- ��ȡ����
	\param[in] len -- ��ȡ�ĳ���, һ��Ϊһ��ҳ�Ĵ�С
	\param[out] pbuff -- ��ȡ������
	\retval 0 -- ��ȡ�ɹ� 1 -- ��ȡʧ��  2 -- ECC����
	\version 0.0.1
*/
uint8_t nand_flash_read_page_from_cache(uint32_t addr, uint8_t rd_cache_cmd, uint8_t *pbuff, uint32_t len)
{
	QSPI_CommandTypeDef s_cmd = {0};

	// ��ȡ���ݵ�Cache Registers
	if (nand_flash_page_read(addr))
	{
		return 1;
	}

	s_cmd.Instruction = rd_cache_cmd;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	// zhe��ĵ�ַ��������2KB Cache Registers�е�ƫ�Ƶ�ַ
	// һ����㿪ʼ��ȡlen���ֽھ�OK
	s_cmd.Address = 0;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.NbData = len;
	
	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	switch (rd_cache_cmd)
	{
		case READ_CACHE_x1_CMD:
			s_cmd.DummyCycles = 8;
			s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
			s_cmd.DataMode = QSPI_DATA_1_LINE;
			break;

		case READ_CACHE_x2_CMD:
			s_cmd.DummyCycles = 8;
			s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
			s_cmd.DataMode = QSPI_DATA_2_LINES;
			break;

		case READ_CACHE_x4_CMD:
			s_cmd.DummyCycles = 8;
			s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
			s_cmd.DataMode = QSPI_DATA_4_LINES;
			break;

		case READ_CACHE_DUAL_CMD:
			s_cmd.DummyCycles = 4;
			s_cmd.AddressMode = QSPI_DATA_2_LINES;
			s_cmd.DataMode = QSPI_DATA_2_LINES;
			break;

		case READ_CACHE_QUAD_CMD:
			s_cmd.DummyCycles = 4;
			s_cmd.AddressMode = QSPI_ADDRESS_4_LINES;
			s_cmd.DataMode = QSPI_DATA_4_LINES;
			break;

		case READ_CACHE_QUADDTR_CMD:
			s_cmd.DummyCycles = 8;
			s_cmd.AddressMode = QSPI_ADDRESS_4_LINES;
			s_cmd.DataMode = QSPI_DATA_4_LINES;
			s_cmd.DdrMode = QSPI_DDR_MODE_ENABLE;
			break;
		default:
			break;
	}
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
#ifdef USE_QSPI_DMA
	HAL_QSPI_Receive_DMA(&hqspi, (uint8_t *)pbuff);
#elif USE_QSPI_IT
	HAL_QSPI_Receive_IT(&hqspi, (uint8_t *)pbuff);
#else
	HAL_QSPI_Receive(&hqspi, (uint8_t *)pbuff, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
#endif
	
	
	while(HAL_QSPI_GetState(&hqspi) != HAL_QSPI_STATE_READY);

	// ��ȡECC Status
	uint8_t status = nand_flash_get_status(REG_STATUS1);
	if (status >= 0x20)
	{
		return 2;
	}

	return 0;
}


/*!
	\brief ��ȡGD5F2GM7�Ķ�ҳ����
	\param[in] addr -- ��ȡ�ĵ�ַ
	\param[in] count -- ��ȡ��ҳ��
	\param[out] pbuff -- ��ȡ������
	\retval 0 -- ��ȡ�ɹ� 1 -- ��ȡʧ��
	\version 0.0.1
*/
uint8_t nand_flash_read_multi_page(uint32_t addr, uint8_t* pbuff, uint32_t count)
{
	uint8_t res = 1;
	while (count--)
	{
		res = nand_flash_read_page_from_cache(addr, READ_CACHE_QUAD_CMD, pbuff, PAGE_SIZE);
		addr++;
		// pbuff += PAGE_SIZE;
	}

	return 0;
}







/*!
	\brief GD5F2GM7���ڲ�����Ǩ�ƣ���ҳΪ��λ
	\param[in] src_addr -- Դҳ��ַ
	\param[in] dest_count -- Ŀ��ҳ��ַ
	\retval 0 -- Ǩ�Ƴɹ� 1 -- Ǩ��ʧ��
	\version 0.0.1
*/
// uint8_t temp[PAGE_SIZE+64] = {0};
uint8_t nand_flash_internal_page_data_move(uint32_t src_addr, uint32_t dest_addr)
{
	uint8_t res = 1;
	uint8_t temp[64] = {0};
	// ��ȡspare area
	nand_flash_read_page_spare(src_addr, temp, 64);
	nand_flash_write_page_spare(dest_addr, temp, 64);
	
	
	
	// �Ȱ�src_addr�����ݶ���cache
	res = nand_flash_page_read(src_addr);

	

	// �ٰ�dest_addr������д���¿�ҳ��
	nand_flash_write_enable();
	if (nand_flash_start_program(dest_addr))
	{
		nand_flash_write_disable();
		return 1;
	}

	


	// �ض�������
	// nand_flash_read_page_from_cache(dest_addr, READ_CACHE_QUAD_CMD, temp, PAGE_SIZE+64);
	// memset(temp, 0, PAGE_SIZE+64);

	return res;
}


/*!
	\brief GD5F2GM7���ڲ�����Ǩ�ƣ��Կ�Ϊ��λ
	\param[in] src_block -- Դ���ַ
	\param[in] dest_block -- Ŀ����ַ
	\param[in] ignore_page -- ��Ҫ���Ե�ҳ
	\retval 0 -- Ǩ�Ƴɹ� 1 -- Ǩ��ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_internal_block_move(uint16_t src_block, uint16_t dest_block, uint8_t ignore_page)
{
	uint8_t res = 1;
	uint8_t move_succ = 0;
	uint32_t sa, da;
	sa = src_block * BLOCK_SIZE;
	da = dest_block * BLOCK_SIZE;

	//�Ȳ�dest_addr���ڵĿ�
	res = nand_flash_erase_block(da);
	for (uint8_t i = 0; i < BLOCK_SIZE; i++)
	{
		if (ignore_page == i)
			continue;
		res = nand_flash_internal_page_data_move(sa + i, da + i);
		if (!res)
		{
			move_succ++;
		}
	}

	return move_succ;
}


/*!
	\brief GD5F2GM7�Ļ����⣬���0xFF������0xFF���ǻ���
	\param[in] addr -- ���ַ
	\retval ������ֵ
	\version 0.0.1
*/
uint8_t nand_flash_bad_block_check(uint32_t addr)
{
	uint8_t bad_mark = 0x55;
	uint8_t res = 1;
	QSPI_CommandTypeDef s_cmd = {0};
	res = nand_flash_page_read(addr);

	s_cmd.Instruction = READ_CACHE_QUAD_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	// 0x800�� spare area���׵�ַ�������Ǿ��������ַ
	s_cmd.Address = BAD_BLOCK_FLAG;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.NbData = 1;
	
	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;


	s_cmd.DummyCycles = 4;
	s_cmd.AddressMode = QSPI_ADDRESS_4_LINES;
	s_cmd.DataMode = QSPI_DATA_4_LINES;
	
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive_DMA(&hqspi, (uint8_t *)&bad_mark);

	
	
	while(HAL_QSPI_GetState(&hqspi) != HAL_QSPI_STATE_READY);
	return bad_mark;
}

/*!
	\brief GD5F2GM7��ҳECC��ȡ
	\param[in] addr -- ҳ��ַ
	\param[in] ecc -- ��ECCֵ��buffer
	\retval 0 -- ��ȡ�ɹ� 1 -- ��ȡʧ��
	\version 0.0.1
*/
uint8_t nand_flash_read_page_ecc(uint32_t addr, uint8_t* pbuff)
{
	uint8_t res = 1;
	QSPI_CommandTypeDef s_cmd = {0};
	res = nand_flash_page_read(addr);

	s_cmd.Instruction = READ_CACHE_QUAD_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	// 0x840�� ECC�ĵ�ַ
	s_cmd.Address = ECC_CODE;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.NbData = 64;
	
	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;


	s_cmd.DummyCycles = 4;
	s_cmd.AddressMode = QSPI_ADDRESS_4_LINES;
	s_cmd.DataMode = QSPI_DATA_4_LINES;
	
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive_DMA(&hqspi, (uint8_t *)pbuff);

	
	
	while(HAL_QSPI_GetState(&hqspi) != HAL_QSPI_STATE_READY);
	return res;
}


/*!
	\brief GD5F2GM7��ҳspare�����ȡ
	\param[in] addr -- ҳ��ַ
	\param[in] pbuff -- ��spare����ֵ��buffer
	\retval 0 -- ��ȡ�ɹ� 1 -- ��ȡʧ��
	\version 0.0.1
*/
uint8_t nand_flash_read_page_spare(uint32_t addr, uint8_t* pbuff, uint8_t len)
{
	uint8_t res = 1;
	QSPI_CommandTypeDef s_cmd = {0};
	res = nand_flash_page_read(addr);


	
	s_cmd.Instruction = READ_CACHE_QUAD_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

	// 0x800�� spare area���׵�ַ�������Ǿ��������ַ
	s_cmd.Address = BAD_BLOCK_FLAG;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.NbData = len;
	
	
	
	s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;


	s_cmd.DummyCycles = 4;
	s_cmd.AddressMode = QSPI_ADDRESS_4_LINES;
	s_cmd.DataMode = QSPI_DATA_4_LINES;
	
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	HAL_QSPI_Receive_DMA(&hqspi, (uint8_t *)pbuff);

	
	
	while(HAL_QSPI_GetState(&hqspi) != HAL_QSPI_STATE_READY);
	return res;
}

/*!
	\brief GD5F2GM7 дspare����
	\param[in] addr -- д��ĵ�ַ
	\param[in] prog_cmd -- д������
	\param[in] len -- д��ĳ���, һ��Ϊһ��ҳ�Ĵ�С
	\param[in] pbuff -- д�������
	\param[out] None
	\retval 0 -- �����ɹ� 1 -- ����ʧ��
	\version 0.0.1
*/
uint8_t nand_flash_write_page_spare(uint32_t addr, uint8_t *pbuff, uint32_t len)
{
	QSPI_CommandTypeDef s_cmd = {0};

	s_cmd.Instruction = PROGRAM_LOAD_RANDOM_DATA_x4_CMD;
	s_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	
	// 0x800�� spare area���׵�ַ�������Ǿ��������ַ
	s_cmd.Address = BAD_BLOCK_FLAG;
	s_cmd.AddressSize = QSPI_ADDRESS_16_BITS;
	s_cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	s_cmd.NbData = len;
	s_cmd.DataMode = QSPI_DATA_4_LINES; 
	HAL_QSPI_Command(&hqspi, &s_cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
#ifdef USE_QSPI_DMA
	HAL_QSPI_Transmit_DMA(&hqspi, pbuff);
#elif USE_QSPI_IT
	HAL_QSPI_Transmit_IT(&hqspi, pbuff);
#else
	HAL_QSPI_Transmit(&hqspi, pbuff, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
#endif
	nand_flash_write_enable();

	if (nand_flash_start_program(addr))
	{
		nand_flash_write_disable();
		return 1;
	}
	nand_flash_write_disable();
	return 0;
}