/******************************************************************************
 * fw_upgrade.h - Firmware upgrade module definitions
 ******************************************************************************
 * ORIGIN DATE:  June 20, 2005
 * TESTBENCH:    A0004 (Rev. A)
 * AUTHOR:       Keith Suhoza
 * E-MAIL:       ksuhoza@aclaratech.com
 *****************************************************************************/

#ifndef _BS_UPGRADE_H_
#define _BS_UPGRADE_H_

#define BOOT_START_ADDR     0x00000000
#define BOOT_END_ADDR		0x00002FFF
#define INT_SRAM_START_ADDR 0x00200000

#define CODE1_START_ADDR    0x10100000
#define CODE1_END_ADDR    	0x1027FFFF
#define CODE1_CHKSUM_ADDR  	0x1027FFFC
#define CODE2_START_ADDR    0x10280000
#define CODE2_END_ADDR    	0x103FFFFF
#define CODE2_CHKSUM_ADDR   0x103FFFFC
#define MAIN_BASE_ADDR      0x20004000

#define asc_to_hex(a) ((a) & 0x40) ? (((a) & 0x0f) + 9) : ((a) & 0x0f)
#define RS232_STOP_CHAR 0x0a

#define BS_BLINK_RECS	0x0f

/*
 * S-record field positions.
 */
#define		BC_STRT					2
#define		BC_LEN					2
#define		BC_END					BC_STRT + BC_LEN
#define		ADDR_STRT				BC_END
#define 	ADDR_LEN				8
#define 	CS_LEN					2

#define 	S0_RECORD	0
#define		S3_RECORD	3
#define		S7_RECORD	7

/*
 * Parsed s-records go here.
 */
typedef struct
{
	unsigned char	type;		// Record type S0, 3, or 7 (only support S3S7 format)
	unsigned char	data_bc;	// Data byte count.
	unsigned long	address;	// Start address of record.
	unsigned char	data[32];	// Actual code data bytes.
} s_record;

#define IO_DBG_RS_OFF_N	0x00400000
#define IO_TEST_LED1	0x10000000
#define IO_TEST_LED2	0x20000000

#define BAUD_VALUE (50000000 / (115200 * 16))
#define debug_RS232_on()	(AT91C_BASE_PIOD->PIO_SODR = IO_DBG_RS_OFF_N)

#endif
