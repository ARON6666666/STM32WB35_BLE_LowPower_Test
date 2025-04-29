/**
  ******************************************************************************
  * @file    nand_ftl.h
  * @brief   ���״���GD5F2MG7 256MB Nand Flash�ļĴ�����ַ�����Լ�IO����
  * @version <version> <time>   <author>          <desc>
  *          0.0.1     25/03/17  Jiaolong  Wu     ��ʼ�汾
  ******************************************************************************
  */
#ifndef _NAND_FLASH_
#define _NAND_FLASH_


#include <stdint.h>


// ����QSPI���ݴ��䷽ʽ
#define USE_QSPI_DMA
// #define USE_QSPI_IT
// #define USE_QSPI_NORMAL


// ����NAND Flash �ͺ�
#define GD5F2GM7UExxG
// #define GD5F2GM7RExxG

#ifdef GD5F2GM7UExxG
#define BLOCK_SIZE							64
#define BLOCK_COUNT							2048
#define PAGE_SIZE							2048
#define PAGE_COUNT							(BLOCK_COUNT * BLOCK_SIZE)
#elif GD5F2GM7RExxG

#endif




// �Ĵ�����ַ
#define REG_PROTECTION						0xA0
#define REG_FEATURES1						0xB0
#define REG_STATUS1							0xC0
#define REG_REATRUES2						0xD0
#define REG_STATUS2							0xF0

/* дȨ�޲��� */ 
#define WRITE_ENABLE_CMD					0x06
#define WRITE_DISABLE_CMD					0x04

/* ���ú�״̬�Ĵ�����д���� */
#define GET_FEATURES_CMD					0x0F
#define SET_FEATURES_CMD					0x1F

/* ������ */
#define PAGE_READ_TOCACHE_CMD				0x13
#define READ_CACHE_x1_CMD				    0x03 //or 0BH
#define READ_CACHE_x2_CMD					0x3B
#define READ_CACHE_x4_CMD					0x6B
#define READ_CACHE_DUAL_CMD					0xBB
#define READ_CACHE_QUAD_CMD					0xEB
#define READ_CACHE_QUADDTR_CMD				0xEE
#define READ_ID_CMD							0x9F
#define READ_PARAM_PAGE_CMD					0x13
#define READ_UID_CMD						0x13

/* Program ���� */
#define PROGRAM_LOAD_CMD					0x02
#define PROGRAM_LOAD_x4_CMD					0x32
#define PROGRAM_EXE_CMD						0x10
#define PROGRAM_LOAD_RANDOM_DATA_CMD		0x84
#define PROGRAM_LOAD_RANDOM_DATA_x4_CMD		0xC4 //or 34H

/* Erase ���� */
#define BLOCK_ERASE_CMD						0xD8

/* RESET ���� */
#define RESET_CMD							0xFF
#define ENABLE_POWER_ON_RESET_CMD			0x66
#define POWER_ON_RESET_CMD					0x99

/* feature ���� */
#define QE_BIT								0x01

/* OTP Region ���� */
#define OTP_REGION_EN_CMD					0x40
#define OTP_REGION_PRT_CMD					0x80

/* ��ַ���� */
#define ADDR_UID							0x000000
#define ADDR_PARAM_PAGE						0x000001
#define BAD_BLOCK_FLAG						0x0800
#define ECC_CODE							0x0840


enum status_reg_mask_code
{
	IN_OP = 0x00,
	WEL = 0x02,
	E_FAIL = 0x04,
	P_FAIL = 0x08,
	ECCS0 = 0x10,
	ECCS1 = 0x20,
} ;


typedef struct
{
	uint8_t	uid[32];
	uint8_t	param_page[128];
} nand_flash_info_buff_t;
extern nand_flash_info_buff_t nand_flash_info_buff;


#pragma pack(1)
typedef struct
{
	uint32_t page_size;
	uint16_t spare_byte_per_page;
	uint32_t partial_page_size;
	uint16_t spare_byte_per_partial_page;
	uint32_t block_size;
	uint32_t block_count;
	uint8_t luns;
} nand_flash_info_t;
#pragma pack()





// gd5f2gm7 function prototype
uint8_t nand_flash_initialize(void);

void nand_flash_softreset(void);
void nand_flash_poweronreset(void);

uint8_t nand_flash_status_reg_check_bit(uint8_t mask_bit);

uint8_t nand_flash_OTP_Enable(void);
uint8_t nand_flash_OTP_Disable(void);

uint8_t nand_flash_erase_block(uint32_t raw_addr);
uint8_t nand_flash_erase_all_block(void);

uint8_t nand_flash_read_page_from_cache(uint32_t addr, uint8_t rd_cache_cmd, uint8_t *pbuff, uint32_t len);
uint8_t nand_flash_read_multi_page(uint32_t addr, uint8_t* pbuff, uint32_t count);

uint8_t nand_flash_write_page(uint32_t column_addr, uint8_t prog_cmd, uint8_t *pbuff, uint32_t len);
uint8_t nand_flash_write_multi_page(uint32_t addr, uint8_t* pbuff, uint32_t count);

uint8_t nand_flash_internal_page_data_move(uint32_t src_addr, uint32_t dest_addr);
uint8_t nand_flash_internal_block_move(uint16_t src_block, uint16_t dest_block, uint8_t ignore_page);

uint8_t nand_flash_bad_block_check(uint32_t addr);
uint8_t nand_flash_read_page_ecc(uint32_t addr, uint8_t* pbuff);
uint8_t nand_flash_read_page_spare(uint32_t addr, uint8_t* pbuff, uint8_t len);
uint8_t nand_flash_write_page_spare(uint32_t addr, uint8_t *pbuff, uint32_t len);
#endif
