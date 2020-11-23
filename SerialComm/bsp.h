/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                             (c) Copyright 2003, Micrium, Inc., Weston, FL
*                         (c) Copyright 2003, JIDEC Consultants, Sherbrooke, QC
*                                          All Rights Reserved
*
*                                             Cogent CSB337
*                                         Board Support Package
*
* File         : BSP.H
* Originally by: Jean J. Labrosse
* Modified by  : Jean-Denis Hatier
*
*********************************************************************************************************
*/
#ifndef BSP_H
#define BSP_H
#include "def.h"

/*
*********************************************************************************************************
*                                          SYSTEM DEFINITIONS
*********************************************************************************************************
*/
// System Clocks
#define  AT91RM9200_CLK         328

#ifdef EVALBRD

#define  AT91C_MASTER_CLOCK     (55296000)
#define  MCK                    (55296000)

#else

#define  AT91C_MASTER_CLOCK     (75000000)
#define  MCK                    (75000000)

#endif

// SDRAM timing in ns
#define SDRAM_WRITERECOVER  15                                  // Recovery time after write measured in nS (NO AUTO precharge)
#define SDRAM_ROWCYCLE      100                                 // Delay between Refresh and Active Command (nS)
#define SDRAM_ROWPRECHARGE  20                                  // Precharge delay until another Command (nS)
#define SDRAM_ROWCOLDELAY   20                                  // Delay time between Active command and Read/Write (nS)
#define SDRAM_ACT2PRECHARGE 50                                  // Delay time between Active command and Precharge (nS)
#define SDRAM_EXITSELFREFSH 100                                 // Time required for self refresh exit until Active (nS)

// M-Clock in MHz
#define CLKFREQ				75
#define SLOWCLK				10
#define AT91C_SLOW_CLOCK    10000000


// MMU
#define MMU_TT_SIZE				4096
#define INTERNAL_MEMORY_START	0x000
#define INTERNAL_MEMORY_END		0x004
#define FLASH_MEMORY_START		0x100
#define FLASH_MEMORY_END		0x104
#define SDRAM_MEMORY_START		0x200
#define SDRAM_MEMORY_END		0x20F
#define PSRAM_MEMORY_START		0x300
#define PSRAM_MEMORY_END		0x310
#define PERIPHERAL_MEMORY		0xFFF

#define MMU_RW_SUPERVISOR		(1 << 10)
#define MMU_SUPERVISOR_MODE		(0x1f << 4)
#define MMU_WT_CACHABLE         (1 << 3)
#define MMU_1MB_SIZE			(0x02)

// System I/O
#ifdef _9975REVA
    // PortA Defines
#define IO_SPI_IN		0x00000001
#define IO_SPI_OUT		0x00000002
#define IO_SPI_CLK		0x00000004
#define IO_SPI_CS0_N	0x00000008
#define IO_CB_ON_OFF	0x00000008 // Alias for IO_SPI_CS0_N.
#define IO_PA4			0x00000010
#define IO_P_TXD		0x00000020
#define IO_P_RXD		0x00000040
#define IO_EREFCLK		0x00000080
#define IO_ETXEN		0x00000100
#define IO_ETX0			0x00000200
#define IO_ETX1			0x00000400
#define IO_ECRSDV		0x00000800
#define IO_ERX0			0x00001000
#define IO_ERX1			0x00002000
#define IO_ERXER		0x00004000
#define IO_EMDC			0x00008000
#define IO_EMDIO		0x00010000
#define IO_TXD0			0x00020000
#define IO_RXD0			0x00040000
#define IO_PA19			0x00080000
#define IO_CTS0			0x00100000
#define IO_PA21			0x00200000
#define IO_R_RXD		0x00400000
#define IO_R_TXD		0x00800000
#define IO_RTC_IRQ_N	0x01000000
#define IO_RTC_DATA		0x02000000
#define IO_RTC_CLK		0x04000000
#define IO_FORCE_CALL	0x08000000
#define IO_TEST_LED1	0x10000000
#define IO_TEST_LED2	0x20000000
#define IO_DBG_RXD		0x40000000
#define IO_BMS			0x80000000

    // PortB Defines
#define IO_PB0			0x00000001
#define IO_PB1			0x00000002
#define IO_PB2			0x00000004
#define IO_SRXD0		0x00000008
#define IO_SRXC0		0x00000010
#define IO_SRXF0		0x00000020
#define IO_PB6			0x00000040
#define IO_PB7			0x00000080
#define IO_PB8			0x00000100
#define IO_SRXD1		0x00000200
#define IO_SRXC1		0x00000400
#define IO_SRXF1		0x00000800
#define IO_STXD2		0x00001000
#define IO_STXC2		0x00002000
#define IO_STXF2		0x00004000
#define IO_SRXD2		0x00008000
#define IO_SRXC2		0x00010000
#define IO_SRXF2		0x00020000
#define IO_RI1			0x00040000
#define IO_DTR1			0x00080000
#define IO_TXD1			0x00100000
#define IO_RXD1			0x00200000
#define IO_PB22			0x00400000
#define IO_DCD1			0x00800000
#define IO_CTS1			0x01000000
#define IO_DSR1			0x02000000
#define IO_RTS1			0x04000000
#define IO_PB27			0x08000000
#define IO_PF_N			0x10000000
#define IO_DATA_AVAIL	0x20000000

    // PortC Defines
#define IO_BFCK			0x00000001
#define IO_BFRDY		0x00000002
#define IO_BFAVD		0x00000004
#define IO_BFWP_N		0x00000008
#define IO_BFOE_N		0x00000010
#define IO_BFWE_N		0x00000020
#define IO_PC6			0x00000040
#define IO_A23			0x00000080
#define IO_A24			0x00000100
#define IO_PC9			0x00000200
#define IO_PC10			0x00000400
#define IO_PC11			0x00000800
#define IO_PC12			0x00001000
#define IO_PC13			0x00002000
#define IO_PC14			0x00004000
#define IO_PC15			0x00008000
#define IO_D16			0x00010000
#define IO_D17			0x00020000
#define IO_D18			0x00040000
#define IO_D19			0x00080000
#define IO_D20			0x00100000
#define IO_D21			0x00200000
#define IO_D22			0x00400000
#define IO_D23			0x00800000
#define IO_D24			0x01000000
#define IO_D25			0x02000000
#define IO_D26			0x04000000
#define IO_D27			0x08000000
#define IO_D28			0x10000000
#define IO_D29			0x20000000
#define IO_D30			0x40000000
#define IO_D31			0x80000000

    // PortD Defines
#define IO_COM_OFF_N	0x00000001
#define IO_RS_SEL1		0x00000002
#define IO_RS_SEL2		0x00000004
#define IO_COM_RS_OFF_N 0x00000008
#define IO_PD4			0x00000010
#define IO_PD5			0x00000020
#define IO_DBG_TXD		0x00000040
#define IO_EREFCLK		0x00000080
#define IO_PD8			0x00000100
#define IO_PD9			0x00000200
#define IO_MD0			0x00000400
#define IO_MD1			0x00000800
#define IO_MD2			0x00001000
#define IO_MD3			0x00002000
#define IO_MD4			0x00004000
#define IO_MD5			0x00008000
#define IO_MD6			0x00010000
#define IO_MD7			0x00020000
#define IO_SPI_CS1_N	0x00040000
#define IO_SPI_CS2_N	0x00080000
#define IO_SPI_CS3_N	0x00100000
#define IO_RTS0			0x00200000
#define IO_DBG_RS_OFF_N	0x00400000
#define IO_EXT_PWR		0x00800000
#define IO_PD24			0x01000000
#define IO_PD25			0x02000000
#define IO_FSTATUS		0x04000000
#define IO_PD27			0x08000000

// Pin Control Macros
#define external_power_on()		(AT91C_BASE_PIOD->PIO_SODR = IO_EXT_PWR)
#define external_power_off()	(AT91C_BASE_PIOD->PIO_CODR = IO_EXT_PWR)

#define debug_RS232_on()	(AT91C_BASE_PIOD->PIO_SODR = IO_DBG_RS_OFF_N)
#define debug_RS232_off()	(AT91C_BASE_PIOD->PIO_CODR = IO_DBG_RS_OFF_N)

#define COM_power_on()		(AT91C_BASE_PIOD->PIO_SODR = IO_COM_OFF_N)
#define COM_power_off()		(AT91C_BASE_PIOD->PIO_CODR = IO_COM_OFF_N)

#define CELL_power_on()		(AT91C_BASE_PIOA->PIO_CODR = IO_SPI_CS0_N)
#define CELL_power_off()	(AT91C_BASE_PIOA->PIO_SODR = IO_SPI_CS0_N)

/******************************************************************************
 * These two Macros control the high/low state of the cell phone board enable
 * IO line.  They are alias's for CELL_power_on() and CELL_power_off() Macros.
 *****************************************************************************/
#define CC864_power_ctrl_low() ( AT91C_BASE_PIOA->PIO_CODR = IO_CB_ON_OFF )
#define CC864_power_ctrl_high() ( AT91C_BASE_PIOA->PIO_SODR = IO_CB_ON_OFF )
#define CB_power_ctrl_pulse_activate() CC864_power_ctrl_high()
#define CB_power_ctrl_pulse_deactivate() CC864_power_ctrl_low()

#define COM_RS232_on()		(AT91C_BASE_PIOD->PIO_SODR = IO_COM_RS_OFF_N)
#define COM_RS232_off()		(AT91C_BASE_PIOD->PIO_CODR = IO_COM_RS_OFF_N)

#define COM_CHAN1_on()		(AT91C_BASE_PIOD->PIO_SODR = IO_RS_SEL1)
#define COM_CHAN1_off()		(AT91C_BASE_PIOD->PIO_CODR = IO_RS_SEL1)

#define COM_CHAN2_on()		(AT91C_BASE_PIOD->PIO_SODR = IO_RS_SEL2)
#define COM_CHAN2_off()		(AT91C_BASE_PIOD->PIO_CODR = IO_RS_SEL2)

#define GetModemType()		( (AT91C_BASE_PIOD->PIO_PDSR & (IO_MD0 | IO_MD1 | IO_MD2 | IO_MD3 | IO_MD4)) >> 10)
#define GetBoltOnType()		( (AT91C_BASE_PIOD->PIO_PDSR & (IO_MD5 | IO_MD6 | IO_MD7)) >> 15)

#endif  // _9975REVA

#ifndef BOOT_CODE

#define DCD_STAT()			(!(AT91C_BASE_MODEM->US_CSR & AT91C_US_DCD) )

#define READ_PHY_REG(X)		(0x60820000 | (((X)<<18) & AT91C_EMAC_REGA) )
#define WRITE_PHY_REG(X,Y)	(0x50820000 | (((X)<<18) & AT91C_EMAC_REGA) | ((Y) & AT91C_EMAC_DATA) )
#define NUM_PHY_REGS		0x20

// Exception Handler errors
typedef enum
{
	EXCPT_NONE		,
	EXCPT_UNDEF		,
	EXCPT_SWI		,
	EXCPT_PREFETCH	,
	EXCPT_DATA
} EXCEPTION_DEF;

// system interrupt priority levels
#define  UNUSED_INTERRUPT_LEVEL 	0	
#define  PIO_INTERRUPT_LEVEL 		3	
#define  MAC_INTERRUPT_LEVEL 		4	// Used to set MAC IRQ PRIO
#define	 SSC_INTERRUPT_LEVEL		5
#define  USART_INTERRUPT_LEVEL 		6
#define	 SYS_TICK_INTERRUPT_LEVEL	7

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
extern void		BSP_Init(void);
extern void		RebootNow(void);

extern void 	Undef_Handler(void);
extern void 	SWI_Handler(void);
extern void 	Prefetch_Handler(void);
extern void 	Data_Handler(void);
extern void 	SPUR_IRQ (void);
extern void 	CATCH_IRQ (void);

// Only call these from the low power exit and entry state machine in hComm.c
extern void 	SlowDownClocks(void);
extern void 	SpeedUpClocks(void);


/*
*********************************************************************************************************
*                                             TICK SERVICES
*********************************************************************************************************
*/
extern void		sys_interrupt_init(void);
extern void		sys_interrupt(void);
extern void		SetStackPointers(void);

/*
*********************************************************************************************************
*                                             PIO SERVICES
*********************************************************************************************************
*/
extern void		AIC_Init(void);
extern void		PIO_Init(void);
extern void		PIOA_Handler(void);
extern void		PIOB_Handler(void);
extern void		IRQ0_Handler(void);			// Transmitter/Receiver Interrupt

extern void		SSC0_Handler(void);
extern void		SSC1_Handler(void);
extern void		SSC2_Handler(void);
extern void		MAC_ISR(void);
extern void 	OS_CPU_FIQ_ISR_Handler(void);

/*
*********************************************************************************************************
*                                             USART SERVICES
*********************************************************************************************************
*/
extern void		USART_Init(void);
extern void    enable_9975G_port(char);
extern void		USART0_Handler(void);
extern void		USART1_Handler(void);
extern void		USART2_Handler(void);
extern void		USART3_Handler(void);

/*
*********************************************************************************************************
*                                              LED SERVICES
*********************************************************************************************************
*/
extern void		LED_On(void);
extern void		LED_Off(void);
extern int		LED_Toggle(void);

/*
*********************************************************************************************************
*                                             STACK SERVICES
*********************************************************************************************************
*/
extern void		bsp_putch(unsigned char ch);
extern void		bsp_putstr(unsigned char *buffer);
extern unsigned char    bsp_hasch(void);
extern unsigned char	bsp_getch(void);

/*
*********************************************************************************************************
*                             Hooks to Assembly functions
*********************************************************************************************************
*/
extern void ARMDisableInt(void);
extern void ARMEnableInt(void);
extern void ICacheDisable(void);
extern void ICacheEnable(void);
extern void DCacheDisable(void);
extern void DCacheEnable(void);
extern void MMU_Disable(void);
extern void MMU_Enable(void);
#endif // #ifndef BOOT_CODE

#endif
