/*
*********************************************************************************************************
* Company:	Aclara RF Systems, Inc.
* File:		flash.h
* OrigAuthor:	Keith Suhoza
* Date:		01/13/2006
* Portable:     No
*
* Copyright:    2006-2008 Aclara RF Systems, Inc. All rights reserved
*
* Decription: Contains all code to program flash memory
*
* Revisors:     Keith Suhoza   (KAS)
*
*
* Revision History:
* *****************
* 01/13/2006 -- KAS -- Original authoring
*
*********************************************************************************************************
*/
#include "stdint.h"

#ifndef FLASH_H
#define FLASH_H

// Flash Memory Map
#define FLASH_BASE_ADDR		0x10000000
#define CFG1_BASE_ADDR		0x10020000
#define PS_BASE_ADDR		0x10040000
#define TR1_BASE_ADDR		0x10060000
#define TR2_BASE_ADDR		0x10080000
#define TR3_BASE_ADDR		0x100A0000
#define MTU_BASE_ADDR		0x100C0000
#define MTU_HWTYPE_ADDR		0x100CFE00
#define MTU_FWVERSION_ADDR	0x100CFE02
#define MTU_FWTYPE_ADDR		0x100CFE04
#define MTU_AUX_HWTYPE_ADDR		0x100CE002
#define MTU_AUX_FWVERSION_ADDR	0x100CE004
#define MTU_AUX_FWTYPE_ADDR		0x100CE003
#define CFG2_BASE_ADDR		0x100E0000
#define MAIN_CODE_BASE_ADDR 0x10100000
#define MAIN_CODE_TOP_ADDR	0x1027FFFF
#define MAIN_CHKSUM_ADDR  	0x1027FFFC
#define BACKUP_BASE_ADDR	0x10280000
#define FLASH_TOP_ADDR		0x103FFFFF
#define BACKUP_CHKSUM_ADDR  0x103FFFFC

#define FLASH_IMAGE_SIZE   0x00020000

#define PS_INT_BASE_ADDR	0xC800
#define PS_INT_TOP_ADDR		0xFFFF
#define TR_INT_BASE_ADDR	0x4A00
#define TR_INT_TOP_ADDR		0xFFFF
#define J_INT_BASE_ADDR	    0x3E00
#define MTU_INT_BASE_ADDR	0x00000
#define MTU_INT_TOP_ADDR	0x1FFFF

// Normal Page Size
#define C3_FLASH_PAGE_SIZE		0x00010000
#define C3_FLASH_PAGE_MASK		0xFFFF0000
#define NUM_C3_MASK_BITS		16
#define J3_FLASH_PAGE_SIZE		0x00020000
#define J3_FLASH_PAGE_MASK		0xFFFE0000
#define NUM_J3_MASK_BITS		17

#define MAIN_CODE_BASE_ADDR		0x10100000
#define MAIN_CODE_END_ADDR		0x1027FFFF
#define SHADOW_BASE_ADDR		0x10280000
#define SHADOW_END_ADDR			0x103FFFFF
#define NUM_MAIN_C3_SECTORS		24
#define NUM_MAIN_J3_SECTORS		12

// J3 Buffering
#define FLASH_BUFFER_SIZE	0x00000020
#define FLASH_BUFFER_MASK	0xFFFFFFE0

// C3 Boot Block Page Sizes
#define FLASH_BOOT_PAGE_SIZE	0x00002000
#define FLASH_BOOT_PAGE_MASK	0xFFFFE000

// Flash Commands
#define FLASH_READ_ARRAY	0xFF
#define FLASH_READ_ID		0x90
#define FLASH_READ_QUERY	0x98
#define FLASH_READ_STATUS	0x70
#define FLASH_CLEAR_STATUS	0x50
#define FLASH_BUFFER_WRITE	0xE8
#define FLASH_WORD_PROGRAM	0x40
#define FLASH_BLOCK_ERASE1	0x20
#define FLASH_BLOCK_ERASE2	0xD0
#define FLASH_BE_SUSPEND	0xB0
#define FLASH_CONFIG		0xB8
#define FLASH_LOCK_BITS		0x60
#define FLASH_SET_LOCK		0x01
#define FLASH_CLEAR_LOCK	0xD0
#define FLASH_PROTECT		0xC0

#define DONE_BIT			0x0080

// define flash command codes for MICROCHIP_SST39V3201C and CYPRESS_S29GL032N
#define FLASH_SEQ_ADD1     0x555
#define FLASH_SEQ_ADD2     0x2AA
#define FLASH_CODE1        0xAA
#define FLASH_CODE2        0x55
#define ID_IN_CODE         0x90
#define ID_OUT_CODE        0xF0
#define WRITE_CODE         0xA0
#define ERASE_SECTOR_CODE1 0x80
#define CHIP_SECTOR_CODE1  0x10
#define ERASE_SECTOR_CODE2 0x30

// Flash Devices Supported
// J3 Style
#define INTEL_28F320J3		0x00890016
#define INTEL_28F640J3		0x00890017
#define INTEL_28F128J3		0x00890018
#define INTEL_28F256J3		0x0089001D
#define ST_M58LW032D		0x00200016
#define ST_M58LW064D		0x00200017
// C3 Style
#define INTEL_28F320C3B		0x008988C5
#define INTEL_28F640C3B		0x008988CD
#define ST_M29W320DB		0x002022CB
#define ST_M29W640DB		0x002022DF
#define SHARP_LHF32FEZ		0x00B000B5
#define ATMEL_AT49BV320D	0x001090C4
#define MICROCHIP_SST39V3201C   0x00BF235F
#define CYPRESS_S29GL032N       0x0001227E

#define INVALID_FLASH		-1
#define J3_FLASH			0
#define C3_FLASH			1
#define   BOOT_MODE      2

#ifndef BOOT_CODE
// Flash Command header
typedef struct
{
    unsigned long base_addr;
    unsigned long end_addr;
    unsigned char model_number;
    unsigned char slot;
    unsigned short pad;
    unsigned long sector_addr;
} flash_sector_id;

// Commands
extern void unprotect_flash(void);
extern void protect_flash(void);
extern unsigned long read_flash_id(void);
extern int check_flash_id(unsigned long flash_id);
//extern unsigned char upgrade_device(AT91PS_USART pUSART, int slot_addr, long block_addr, uint8_t);
//extern uint8_t handlePeripheralInfoQuery(AT91PS_USART, int);
extern unsigned short checksum(unsigned char *addr, unsigned int count);
extern unsigned short checksum2(unsigned char *addr, unsigned int count);
extern unsigned long rom_checksum(unsigned short *addr, unsigned long count);
extern int program_sector(volatile unsigned short *sect_addr, unsigned short *data_addr, unsigned int num_bytes, int do_delays);
extern int init_flash_prog(unsigned char slot);
extern int buff_flash_prog(unsigned char slot, unsigned long addr, unsigned char *data, int length);
extern int do_flash_prog(unsigned char slot, unsigned char extraACK);
extern int build_query_flash_buffer(unsigned char *data);
extern void program_short(volatile unsigned short *addr, unsigned short value);
//extern void storeCfgInFlash(nvDCUSettings *);
#endif
#endif
