/*
*********************************************************************************************************
* Company:  Hexagram, Inc.
* File:     flash.c
* OrigAuthor:   Keith Suhoza
* Date:     01/30/2006
* Portable:     No
*
* Copyright:    2006 Hexagram Inc. All rights reserved
*
* Decription:   Contains all code to program the flash memory
*               Code cannot be executed from flash
*
* Revisors:     Keith Suhoza   (KAS)
*
*
* Revision History:
* *****************
* 01/30/2006 -- KAS -- Original authoring
*
*********************************************************************************************************
*/
//#include "at91rm9200.h"
#include "bs_upgrade.h"
#include "flash.h"
// #include "bsp.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Local prototypes */
void fwu_get_msg( char *msg);
void fwu_send_msg( const char *msg );
unsigned char fwu_strcmp( char *str1, char *str2 );
unsigned char fwu_strlen( char *str );
void fwu_strcpy( char *str1, char *str2 );
char *fwu_strcat( char *str1, char *str2 );
unsigned long bs_read_flash_id(void);
int bs_check_flash_id(unsigned long flash_id);
unsigned long bs_rom_checksum(unsigned short *addr, unsigned long count);
long bs_clock(void);
void bs_blinkLED1(void);
void bs_blinkLED2(void);
static void flash_erase_block(unsigned long flash_id, volatile unsigned short *flash_addr);
static void flash_program_data(unsigned long flash_id, volatile unsigned short *flash_addr, unsigned short data);
static void flash_set_to_read_mode(unsigned long flash_id, unsigned short *flash_addr);
static void flash_lock_all_blocks(unsigned long flash_id);

/***************** Flash Local Constants ***************************/
// Firmware upgrade enable password
//#pragma location="BCONST"
const char flash_pswd[] = "5799MARGAXEH";

// Error Return Codes
//#pragma location="BCONST"
const char NO_ERROR[] =      "0";
//#pragma location="BCONST"
const char BAD_START[] =     "1";
//#pragma location="BCONST"
const char EARLY_END[] =     "2";
//#pragma location="BCONST"
const char BAD_SREC[] =      "3";
//#pragma location="BCONST"
const char SREC_SUMERR[] =   "4";
//#pragma location="BCONST"
const char FLASH_TIMEOUT[] = "5";
//#pragma location="BCONST"
const char UNKNOWN_FLASH[] = "6";
//#pragma location="BCONST"
const char ERASE_TIMEOUT[] = "7";
//#pragma location="BCONST"
const char BAD_PASSWORD[] =  "8";
//#pragma location="BCONST"
const char FINISHED[] =      "9";
//#pragma location="BCONST"
const char VERIFY_ERROR[] =  "A";
//#pragma location="BCONST"
const char LOWVCC_ERROR[] =  "B";

//#pragma location="BCODE"

// Test messages 
// create an array of messages


int message_id = 0;

 char *test[11] = {
   "S00600004844521B",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S7151010000018F09FE518F09FE518F09FE518F09FE59A",
   "S3151010001018F09FE5FFFFFFFF18F09FE518F09FE51A"};

void fwu_get_msg( char *msg )
{
   printf("\nlen %d %s", strlen(test[message_id]), test[message_id]);

   memset(msg, '\0', 20);


   int len = strlen(test[message_id]);

   memcpy(msg, test[message_id], strlen(test[message_id]));

//   msg[len+1] = '\0';
   //msg[len+1] = 0x00;

   message_id++;



    printf("\n%s", msg);
//    message_id++;
//    printf("\n%s", test[message_id]);
//    message_id++;

}



unsigned long __flash_upgrade(void)
{
    // Only allocate variables on the stack and pass them around!
    register int  i,j;
    s_record input_srec;
    unsigned long   blk_addr;
    unsigned char   not_done;
    unsigned char   high_nibble, low_nibble;
    unsigned short  byte_cnt, xmit_cksum, calc_cksum, flash_data;
    unsigned long   calc_cksum1;
    unsigned long   calc_cksum2;
    char            input_buffer[64];
    unsigned long   flash_id;
    int             flash_type;
    unsigned long   flask_mask;
    unsigned short *flash_addr;
    unsigned short *flash_src_addr;
    unsigned long  *src_addr;
    unsigned long  *dest_addr;
    char            copy_section2_flag;
    char            program_sector_flag;
    unsigned long   end_addr;
    char            blink_rate;
    char            num_sectors;
    char            mask_bits;
    unsigned long   main_sector_mask;

    flash_src_addr = (unsigned short *) CODE1_START_ADDR;


//    get_next_msg();


    // Get start up message.  if invalid or timeout then run main code
    fwu_get_msg(input_buffer);

    memcpy(input_buffer, flash_pswd, sizeof(flash_pswd));

    //AT91C_BASE_PIOA->PIO_SODR = IO_TEST_LED1 | IO_TEST_LED2; // LED OFF

    flash_id = bs_read_flash_id();

    if( input_buffer[0] && !fwu_strcmp((char *) flash_pswd, input_buffer) )
    {
        //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED1; // LED ON
        flash_type = bs_check_flash_id(flash_id);
        if(flash_type == INVALID_FLASH)
        {
            fwu_send_msg(UNKNOWN_FLASH);
            //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
            while(1);
        }
        else if( flash_type == J3_FLASH)
        {
            flask_mask = J3_FLASH_PAGE_MASK;
            num_sectors = NUM_MAIN_J3_SECTORS;
            mask_bits = NUM_J3_MASK_BITS;
        }
        else
        {
            flask_mask = C3_FLASH_PAGE_MASK;
            num_sectors = NUM_MAIN_C3_SECTORS;
            mask_bits = NUM_C3_MASK_BITS;
        }
        blink_rate = BS_BLINK_RECS;     // Blink Every x records
        main_sector_mask = 0;

        /*
        * Got start sequence, ready for first data record to determine
        * upgrade type.
        */





        printf("Waiting for S3 Record");
        do
        {
            fwu_send_msg(NO_ERROR);

            char test_msg[] = "S3151010000018F09FE518F09FE518F09FE518F09FE59A";
            memcpy(input_buffer, test_msg, sizeof(test_msg));
            fwu_get_msg(input_buffer);

           
            // Go get type
            input_srec.type = asc_to_hex(input_buffer[1]);
        } while ( input_srec.type != S3_RECORD );

        not_done = TRUE;
        do
        {
            /*
            * Parse byte count; includes address, data, and checksum.
            */
            byte_cnt = 0;
            for(i = BC_STRT; i < BC_END; i++)
            {
                byte_cnt <<= 4;
                byte_cnt |=  asc_to_hex( input_buffer[i] );
            } // for(i = BC_STRT; i < BC_END; i++)

            /*
            * Parse checksum transmitted with data.
            */
            xmit_cksum = 0;
            for(i = ADDR_STRT + (byte_cnt << 1) - CS_LEN;
                i < ADDR_STRT + (byte_cnt << 1); i++)
            {
                xmit_cksum <<= 4;
                xmit_cksum |=  asc_to_hex( input_buffer[i] );
            } // for(i = ADDR_STRT + (byte_cnt << 1) - CS_LEN; ...

            /*
            * Calculate checksum.
            */
            calc_cksum = 0;
            for( i = BC_STRT; i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2 )
            {
                high_nibble = asc_to_hex( input_buffer[i] );
                low_nibble  = asc_to_hex( input_buffer[i+1] );
                high_nibble <<= 4;
                calc_cksum += high_nibble | low_nibble;
            } // for( i = BC_STRT; i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2 )

            /*
            * Make sure no checksum errors, if error lock up box and wait for restart.
            */
            if( (xmit_cksum & 0x00FF) != (~calc_cksum & 0x00FF) )
            {
                printf("\nChecksum Error");
                fwu_send_msg(SREC_SUMERR);
                //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
                while(1);
            } // if( (xmit_cksum & 0x00FF) != (~calc_cksum & 0x00FF) )

            /*
            * Compute start address.
            */
            input_srec.address = 0;
            for( j = 0, i = ADDR_STRT; i < ADDR_STRT + ADDR_LEN; i++, j++ )
            {
                input_srec.address <<= 4;
                input_srec.address |= asc_to_hex( input_buffer[i] );
            } // for( j = 0, i = ADDR_STRT; i < ADDR_STRT + ADDR_LEN; i++, j++ )

            /*
            * Convert ASCII data in S-record to hex.
            */
            for( j = 0, i = ADDR_STRT + ADDR_LEN;
                 i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i += 2, j++ )
            {
                high_nibble  = asc_to_hex( input_buffer[i] );
                low_nibble   = asc_to_hex( input_buffer[i+1] );
                high_nibble <<= 4;
                input_srec.data[j] = high_nibble | low_nibble;
            } // for( j = 0, i = ADDR_STRT + ADDR_LEN; ...

            /*
            * Save number of actual program data bytes in S-record.
            */
            input_srec.data_bc = j;

            /*
            ** Got first S record in a new block, now determine which block.
            */
            blk_addr = input_srec.address & flask_mask;
            // Address depends on which microprocessor we are using
            if( blk_addr < FLASH_BASE_ADDR)
            {
                fwu_send_msg(BAD_START);
                //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
                while(1);
            } // if( blk_addr < BOOT_BASE_ADDR)
            // Skip Boot Sector
            else if(blk_addr >= FLASH_BASE_ADDR)    // Program entire flash for now.
            {
                flash_addr = (unsigned short *)blk_addr;

                flash_erase_block(flash_id, flash_addr);

                if( blk_addr >= MAIN_CODE_BASE_ADDR)
                {
                    main_sector_mask |= (1 << ((blk_addr - MAIN_CODE_BASE_ADDR) >> mask_bits) );
                }
                // Program all data bytes within current block as they come in
                while( ((input_srec.address & flask_mask) == blk_addr) &&
                      not_done)
                {
                    if( ! (blink_rate--))
                    {
                        blink_rate = BS_BLINK_RECS;
                        bs_blinkLED1();  // Toggle for every sector since programming is so fast
                    }

                    switch( input_srec.type )
                    {
                      case S0_RECORD:
                        break;

                      case S3_RECORD:
                        // Program Data words
                        flash_addr = (unsigned short *)input_srec.address;
                        for( j = 0; j < input_srec.data_bc; j += 2)
                        {
                            if( (input_srec.data[j] != 0xFF) || (input_srec.data[j+1] != 0xFF) )
                            {
                                flash_program_data(flash_id, flash_addr, (input_srec.data[j+1] << 8) | input_srec.data[j]);
                            } // if( input_srec.data[j] != 0xFF)
                            flash_addr++;
                        } // for( j = 0, i = input_srec.address; ...
                        flash_addr = (unsigned short *)input_srec.address;
                        flash_set_to_read_mode(flash_id, flash_addr);
#if 0
                        for( j = 0; j < input_srec.data_bc; j += 2)
                        {
                            if( *flash_addr != ((input_srec.data[j+1] << 8) |
                                               input_srec.data[j]))
                            {
                                fwu_send_msg(VERIFY_ERROR);
                                //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
                                while(1);
                            } // if( input_srec.data[j] != 0xFF)
                            flash_addr++;
                        } // for( j = 0, i = input_srec.address; ...
#endif
                        break;

                      case S7_RECORD:
                        /*
                        * Done, go restart box.
                        */
                        not_done = FALSE;
                        break;

                      default:
                        fwu_send_msg(BAD_SREC);
                        //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
                        while(1);
                    } // switch( input_srec.type )

                    /*
                    * Get next code record.
                    */
                    if( not_done )
                    {
                        fwu_send_msg(NO_ERROR);
                        fwu_get_msg(input_buffer);
                        // Go get type
                        input_srec.type = asc_to_hex(input_buffer[1]);
                        if( input_srec.type == S3_RECORD)
                        {
                            /*
                            * Parse byte count; includes address, data, and checksum.
                            */
                            byte_cnt = 0;
                            for(i = BC_STRT; i < BC_END; i++)
                            {
                                byte_cnt <<= 4;
                                byte_cnt |=  asc_to_hex( input_buffer[i] );
                            } // for(i = BC_STRT; i < BC_END; i++) ...

                            /*
                            * Parse checksum transmitted with data.
                            */
                            xmit_cksum = 0;
                            for(i = ADDR_STRT + (byte_cnt << 1) - CS_LEN;
                                i < ADDR_STRT + (byte_cnt << 1); i++)
                            {
                                xmit_cksum <<= 4;
                                xmit_cksum |=  asc_to_hex( input_buffer[i] );
                            } // for(i = ADDR_STRT + (byte_cnt << 1) - CS_LEN; ...

                            /*
                            * Calculate checksum.
                            */
                            calc_cksum = 0;
                            for( i = BC_STRT; i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2 )
                            {
                                high_nibble = asc_to_hex( input_buffer[i] );
                                low_nibble  = asc_to_hex( input_buffer[i+1] );
                                high_nibble <<= 4;
                                calc_cksum += high_nibble | low_nibble;
                            } // for( i = BC_STRT; i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2 )

                            /*
                            * Make sure no checksum errors, if error lock up box and wait for restart.
                            */
                            if( (xmit_cksum & 0x00FF) != (~calc_cksum & 0x00FF) )
                            {
                                fwu_send_msg(SREC_SUMERR);
                                //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
                                while(1);
                            } // if( (xmit_cksum & 0x00FF) != (~calc_cksum & 0x00FF) )

                            /*
                            * Compute start address.
                            */
                            input_srec.address = 0;
                            for( j = 0, i = ADDR_STRT; i < ADDR_STRT + ADDR_LEN; i++, j++ )
                            {
                                input_srec.address <<= 4;
                                input_srec.address |= asc_to_hex( input_buffer[i] );
                            } // for( j = 0, i = ADDR_STRT; i < ADDR_STRT + ADDR_LEN; i++, j++ )

                            /*
                            * Convert ASCII data in S-record to hex.
                            */
                            for( j = 0, i = ADDR_STRT + ADDR_LEN;
                                 i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2, j++ )
                            {
                                high_nibble  = asc_to_hex( input_buffer[i] );
                                low_nibble   = asc_to_hex( input_buffer[i+1] );
                                high_nibble <<= 4;
                                input_srec.data[j] = high_nibble | low_nibble;
                            } // for( j = 0, i = ADDR_STRT + ADDR_LEN; ...

                            /*
                            * Save number of actual program data bytes in S-record.
                            */
                            input_srec.data_bc = j;
                        } // if( input_srec.type == S1_RECORD)

                    } // if( not_done )
                } // while( ((input_srec.address & flask_mask) == blk_addr) &&
            }
            else
            {
                /*
                * Get next code record.
                */
                if( not_done )
                {
                    fwu_send_msg(NO_ERROR);
                    fwu_get_msg(input_buffer);
                    // Go get type
                    input_srec.type = asc_to_hex(input_buffer[1]);
                } // if( not_done )
            } // else if(blk_addr >= MAIN_BASE_ADDR)
        } while( not_done );
        fwu_send_msg(FINISHED);
        // Erase unused main sector blocks
        if( main_sector_mask)
        {
            for( i=0; i < num_sectors; i++)
            {
                if( !(main_sector_mask & (1 << i)) )
                {
                    blk_addr = MAIN_CODE_BASE_ADDR + (i << mask_bits);
                    flash_addr = (unsigned short *)blk_addr;
                    flash_erase_block(flash_id, flash_addr);
                } // if( !(main_sector_mask & (1 << i)) )
            } // for( i=0; i < num_sectors; i++)
        } // if( main_sector_mask)
        flash_addr = (unsigned short *) FLASH_BASE_ADDR;
        flash_set_to_read_mode(flash_id, flash_addr);
    } // if( input_buffer[0] && !fwu_strcmp((char *) flash_pswd, input_buffer) )

    // Need to do main code checksum calculation here!
#if 1
//    calc_cksum1 = bs_rom_checksum((unsigned short *) CODE1_START_ADDR, CODE1_END_ADDR - CODE1_START_ADDR - 3);
//    calc_cksum2 = bs_rom_checksum((unsigned short *) CODE2_START_ADDR, CODE2_END_ADDR - CODE2_START_ADDR - 3);

    calc_cksum1 = 0; // bs_rom_checksum((unsigned short*)CODE1_START_ADDR, CODE1_END_ADDR - CODE1_START_ADDR - 3);
    calc_cksum2 = 0; // bs_rom_checksum((unsigned short*)CODE2_START_ADDR, CODE2_END_ADDR - CODE2_START_ADDR - 3);

    copy_section2_flag = FALSE;
    program_sector_flag = FALSE;

#if 0
    // Try to boot from a valid partition.  If both are bad, crash and burn and wait for flash upgrade
    if( calc_cksum1 != *((unsigned long *) CODE1_CHKSUM_ADDR) )
    {
        // Both Checksum's bad - crash and burn here
        if( calc_cksum2 != *((unsigned long *) CODE2_CHKSUM_ADDR) )
        {
            //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED1 | IO_TEST_LED2; // LED's ON
            while(1);
        }
        else
        {
            // Good flash in Section 2, Section 1 corrupt
            // Always run from section 2 and attempt to re-program Section 1
            copy_section2_flag = TRUE;
            program_sector_flag = TRUE;
            blk_addr = CODE1_START_ADDR;
            end_addr = CODE1_END_ADDR;
            flash_src_addr = (unsigned short *) CODE2_START_ADDR;
        }
    }
    else if( (calc_cksum2 != *((unsigned long *) CODE2_CHKSUM_ADDR)) ||
             (calc_cksum2 != calc_cksum1) )
    {
        // Good sector 1, Bad sector 2 or mismatched Sector 2
        // Always run from section 1 and attempt to re-program Section 2
        program_sector_flag = TRUE;
        blk_addr = CODE2_START_ADDR;
        end_addr = CODE2_END_ADDR;
        flash_src_addr = (unsigned short *) CODE1_START_ADDR;
    }
#endif

#else
    program_sector_flag = FALSE;

    program_sector_flag = TRUE;
    blk_addr = CODE2_START_ADDR;
    end_addr = CODE2_END_ADDR;

#endif

#if 0
    // Reprogram invalid flash sector if necessary
    if( program_sector_flag)
    {
        //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED1; // LED ON
        flash_type = bs_check_flash_id(flash_id);
        if(flash_type == INVALID_FLASH)
        {
            //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
            while(1);
        }
        else if( flash_type == J3_FLASH)
        {
            flask_mask = J3_FLASH_PAGE_MASK;
        }
        else
        {
            flask_mask = C3_FLASH_PAGE_MASK;
        }
        input_srec.address = blk_addr;
        while( input_srec.address < end_addr)
        {
            /*
            ** Got first S record in a new block, now determine which block.
            */
            blk_addr = input_srec.address & flask_mask;

            flash_addr = (unsigned short *)blk_addr;

            flash_erase_block(flash_id, flash_addr);

            flash_set_to_read_mode(flash_id, flash_addr);

            // Program all data bytes within current block as they come in
            while( ((input_srec.address & flask_mask) == blk_addr) )
            {
                // Read Data word
                flash_data = *flash_src_addr++;
                if( flash_data != 0xFFFF)
                {
                    // Program Data word
                    flash_addr = (unsigned short *)input_srec.address;

                    flash_program_data(flash_id, flash_addr, flash_data);

                    flash_set_to_read_mode(flash_id, flash_addr);
                }
                input_srec.address += 2;
            } // while( ((input_srec.address & flask_mask) == blk_addr) )
        } // while( input_srec.address < end_addr)

        flash_addr = (unsigned short *) FLASH_BASE_ADDR;
        flash_set_to_read_mode(flash_id, flash_addr);
    } // if( program_sector_flag)

    flash_lock_all_blocks(flash_id);

    // Copy Main code to DRAM
    //AT91C_BASE_PIOA->PIO_SODR = IO_TEST_LED1 | IO_TEST_LED2; // LED OFF

    if( copy_section2_flag)
    {
        src_addr = (unsigned long *) CODE2_START_ADDR;
    }
    else
    {
        src_addr = (unsigned long *) CODE1_START_ADDR; // Load from section 1 by default
    }
    dest_addr = (unsigned long *) MAIN_BASE_ADDR;
    i = (CODE1_END_ADDR - CODE1_START_ADDR + 1) >> 2;
    do
    {
        *dest_addr++ = *src_addr++;
    } while( --i);

#endif

    // Return Address in R0 to start of main code
    return MAIN_BASE_ADDR;
} // flash_upgrade()

/****************************************************************************
FUNCTION: void fwu_get_msg( unsigned char *msg )

DESCRIPTION: This function will sit and read bytes off RS-232 until
the input data is LF (ASCII 10).  Will tack on 0x00 to
terminate string.

INPUTS:      Pointer to character array to place incomming data.

OUTPUTS:

EDIT HISTORY:

****************************************************************************/
//#pragma location="BCODE"
void xfwu_get_msg( char *msg )
{
    unsigned char done = FALSE;
#ifndef DEBUG
    long ticks;

    // Wait for at least 4 seconds
    ticks = bs_clock();
#endif
#if 1
    // Read From UART
    do
    {
        // Exit if timeout
#ifndef DEBUG
        if (bs_clock() - ticks > 4) {
            break;
        }
        break;
#endif
#if 0
        if( AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY)
        {
            // Grab data and save it
            *msg = AT91C_BASE_DBGU->DBGU_RHR;
            if( *msg == RS232_STOP_CHAR)
            {
                done = TRUE;
            }
            else
            {
                msg++;
            } // if( *msg != RS232_STOP_CHAR)
        } // if( U0RCTL_bit.RXWAKE)
#endif
    } while(  !done);   // LOOP UNTIL LF
    *msg = '\0';    // Replace LF with NULL or just put NULL in string if timeout
#endif
    printf("\n%s", msg);

} // fwu_get_msg()

/****************************************************************************
FUNCTION: void fwu_send_msg( char *msg )

DESCRIPTION: Sends a NULL terminated string out the GPIB or RS-232 port

INPUTS:      Pointer to character array to send

OUTPUTS:     none

EDIT HISTORY:

****************************************************************************/
//#pragma location="BCODE"
void fwu_send_msg( const char *msg )
{
#if 1
    printf("\n%s", msg);
#else
    // Send to UART1
    while( *msg)
    {
        //while( !(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
        //AT91C_BASE_DBGU->DBGU_THR = *msg++;
    } // while( *msg)
    // Send LF
    //while( !(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
    //AT91C_BASE_DBGU->DBGU_THR = RS232_STOP_CHAR;
    // Wait until Done!
    //while( !(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
#endif
} // fwu_send_msg()

/****************************************************************************
FUNCTION: unsigned char fwu_strcmp( char *str1, char *str2 )

DESCRIPTION: This function will compare argument strings(up to 0x00) and
return 0 if match, 1 otherwise. If str2 is not NULL terminated
behavior is undefined.

INPUTS:      Pointers to strings.

OUTPUTS:     0 if match, 1 otherwise.

EDIT HISTORY:

***************************************************************************/
//#pragma location="BCODE"
unsigned char fwu_strcmp( char *str1, char *str2 )
{
    while( *str2 && *str1 )
    {
        if( *str2 == *str1 )
        {
            str1++;
            str2++;
        }
        else
        {
            return( 1 );
        } // if( *str2 == *str1 )
    } // while( *str2 && *str1 )
    return( 0 );
} // fwu_strcmp

/****************************************************************************
FUNCTION: unisgned char fwu_strlen( char *str )

DESCRIPTION: This function will determine size of string pointed to by *str.
If str is not NULL terminated, operation is undefined. Length
does not include NULL.

INPUTS:      Pointer to string.

OUTPUTS:     length of string.

EDIT HISTORY:
***************************************************************************/
//#pragma location="BCODE"
unsigned char fwu_strlen( char *str )
{
    unsigned char len = 0;

    while( *str)
    {
        str++;
        len++;
    } // while( *str)
    return( len );
} // fwu_strlen()

/****************************************************************************
FUNCTION: void fwu_strcpy( char *str1, char *str2 )

DESCRIPTION: This function will copy str2 to str1 untill 0x00 is copied from
str2 to str1.   If str2 is not NULL terminated behavior is undefined.

INPUTS:      Pointers to strings.

OUTPUTS:     none

EDIT HISTORY:

****************************************************************************/
//#pragma location="BCODE"
void fwu_strcpy( char *str1, char *str2 )
{
    while( *str2)
    {
        *(str1++) = *(str2++);
    } // while( *str2)
    *str1 = 0x00;
} // fwu_strcpy()

/****************************************************************************
FUNCTION: char *fwu_strcat( char *str1, char *str2 )

DESCRIPTION: This function will concatenate str2 to str1 untill 0x00 is copied from
str2 to str1.   If str2 is not NULL terminated behavior is undefined.

INPUTS:      Pointers to strings.

OUTPUTS:     original value of str1

EDIT HISTORY:

****************************************************************************/
//#pragma location="BCODE"
char *fwu_strcat( char *str1, char *str2 )
{
    char *ret_val = str1;

    while( *str1) str1++;
    while( *str2)
    {
        *(str1++) = *(str2++);
    } // while( *str2)
    *str1 = 0x00;
    return ret_val;
} // fwu_strcat()

/*----------------------------------------------------------------------------
 Function Name       :  bs_read_flash_id
 Object              :  Queries Flash ID
 Input Params        :  None
 Output Params       :  Manufacturer ID | Device ID
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
unsigned long bs_read_flash_id(void)
{
    unsigned long flash_id;

    //flash_id = 0;
    //flash_id = J3_FLASH;
    flash_id = C3_FLASH;

#if 0
    unsigned short *flash_addr = (unsigned short *)FLASH_BASE_ADDR;

    /* enter software product identification mode */
    *((volatile unsigned short *)flash_addr + FLASH_SEQ_ADD1) = FLASH_CODE1;
    *((volatile unsigned short *)flash_addr + FLASH_SEQ_ADD2) = FLASH_CODE2;
    *((volatile unsigned short *)flash_addr + FLASH_SEQ_ADD1) = ID_IN_CODE;

    flash_id =  *(flash_addr + 0x00) << 16;
    flash_id |= *(flash_addr + 0x01);

    switch (flash_id)
    {
        case INTEL_28F320J3:
        case INTEL_28F640J3:
        case INTEL_28F128J3:
        case INTEL_28F256J3:
        case ST_M58LW032D:
        case ST_M58LW064D:
        case INTEL_28F320C3B:
        case INTEL_28F640C3B:
        case ST_M29W320DB:
        case ST_M29W640DB:
        case SHARP_LHF32FEZ:
        case ATMEL_AT49BV320D:
            *flash_addr = FLASH_READ_ARRAY;
            break;
        case MICROCHIP_SST39V3201C:
        case CYPRESS_S29GL032N:
            /* exit software product identification mode */
            *((volatile unsigned short *)flash_addr + FLASH_SEQ_ADD1) = FLASH_CODE1;
            *((volatile unsigned short *)flash_addr + FLASH_SEQ_ADD2) = FLASH_CODE2;
            *((volatile unsigned short *)flash_addr + FLASH_SEQ_ADD1) = ID_OUT_CODE;
            break;
        default:
            break;
    }
#endif

    return flash_id;
} // bs_read_flash_id()

/*----------------------------------------------------------------------------
 Function Name       :  bs_check_flash_id
 Object              :  Checks for valid Flash ID
 Input Params        :  flash_id
 Output Params       :  -1 if invalid, 0 if J3, 1 if C3
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
int bs_check_flash_id(unsigned long flash_id)
{
    int ret_val;

    switch( flash_id)
    {
        // J3 Styles
      case INTEL_28F320J3:
      case INTEL_28F640J3:
      case INTEL_28F128J3:
      case INTEL_28F256J3:
      case ST_M58LW032D:
      case ST_M58LW064D:
        ret_val = J3_FLASH;
        break;

        // C3 Style
      case INTEL_28F320C3B:
      case INTEL_28F640C3B:
      case ST_M29W320DB:
      case ST_M29W640DB:
      case SHARP_LHF32FEZ:
      case ATMEL_AT49BV320D:
      case MICROCHIP_SST39V3201C:
      case CYPRESS_S29GL032N:
        ret_val = C3_FLASH;
        break;

      default:
        ret_val = INVALID_FLASH;
    } // switch( flash_id)

    ret_val = C3_FLASH;
    return ret_val;
} // bs_check_flash_id()

/*F****************************************************************************

uint16 bs_rom_checksum(uint16 *addr, unsigned int count)

DESCRIPTION:    Calculates a 32 bit checksum by starting at 16 bit
                addr and adding consecutive words.

                NOTE:   This function returns the ones-complement of the
                        computed checksum.

INPUTS:         addr -  the beginning address
                count - the number of bytes to sum

OUTPUTS:        ones-complement of the computed checksum

GLOBAL VARIABLES USED:  None

EDIT HISTORY:
*****************************************************************************/
//#pragma location="BCODE"
unsigned long bs_rom_checksum(unsigned short *addr, unsigned long count)
{
    unsigned long sum = 0;

    while(count > 0)
    {
        sum += *(addr++);
        count -= 2;
    }
    return(~sum);
}

/*----------------------------------------------------------------------------
 Function Name       :  bs_clock
 Object              :  Return number of seconds since last reboot
                        Overrides time.h library function
 Input Params        :  None
 Output Params       :  seconds since last reboot
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
long bs_clock(void)
{
    
    long time1 = 0;
    long time2 = 0;

    do
    {
        //time1 = AT91C_BASE_ST->ST_CRTR & AT91C_ST_CRTV;
        //time2 = AT91C_BASE_ST->ST_CRTR & AT91C_ST_CRTV;
    } while( time1 != time2);
    return time1;
} // bs_clock()

/*----------------------------------------------------------------------------
 Function Name       :  bs_blinkLED1
 Object              :  Toggles LED1 during flash programming
 Input Params        :  None
 Output Params       :  None
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
void bs_blinkLED1(void)
{
#if 0
    if( AT91C_BASE_PIOA->PIO_ODSR & IO_TEST_LED1)
    {
        AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED1; // LED ON
    }
    else
    {
        AT91C_BASE_PIOA->PIO_SODR = IO_TEST_LED1; // LED OFF
    }
#endif
} // bs_blinkLED1()

/*----------------------------------------------------------------------------
 Function Name       :  bs_blinkLED2
 Object              :  Toggles LED2 during flash programming
 Input Params        :  None
 Output Params       :  None
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
void bs_blinkLED2(void)
{
#if 0
    if( AT91C_BASE_PIOA->PIO_ODSR & IO_TEST_LED2)
    {
        AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
    }
    else
    {
        AT91C_BASE_PIOA->PIO_SODR = IO_TEST_LED2; // LED OFF
    }
#endif
} // bs_blinkLED2()

/*----------------------------------------------------------------------------
 Function Name       :  flash_erase_block
 Object              :  selects an algorithm and erases a flash sector
 Input Params        :  flash_id - selects the erase algorithm
                        flash_addr - specifies which sector to erase
 Output Params       :  None
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
static void flash_erase_block(unsigned long flash_id, volatile unsigned short *flash_addr)
{
    unsigned short flash_value;

    switch (flash_id)
    {
        case INTEL_28F320J3:
        case INTEL_28F640J3:
        case INTEL_28F128J3:
        case INTEL_28F256J3:
        case ST_M58LW032D:
        case ST_M58LW064D:
        case INTEL_28F320C3B:
        case INTEL_28F640C3B:
        case ST_M29W320DB:
        case ST_M29W640DB:
        case SHARP_LHF32FEZ:
        case ATMEL_AT49BV320D:
            // Unlock Sector
            *flash_addr = FLASH_LOCK_BITS;
            *flash_addr = FLASH_CLEAR_LOCK;
            while( !(*flash_addr & DONE_BIT));
            // Check for extended Error Code Here
            // status = *sect_addr;

            // Erase Sector
            *flash_addr = FLASH_BLOCK_ERASE1;
            *flash_addr = FLASH_BLOCK_ERASE2;
            while( !(*flash_addr & DONE_BIT));
            break;
        case MICROCHIP_SST39V3201C:
        case CYPRESS_S29GL032N:
            // Erase Sector
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD1) = FLASH_CODE1;
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD2) = FLASH_CODE2;
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD1) = ERASE_SECTOR_CODE1;
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD1) = FLASH_CODE1;
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD2) = FLASH_CODE2;
            *flash_addr = ERASE_SECTOR_CODE2 ;

            /* use the toggle function for busy check */
            flash_value = *flash_addr;
            while (flash_value != *flash_addr)
            {
                flash_value = *flash_addr;
            }
            break;
        default:
            // a valid flash ID has already been verified
            // should not get here
            break;
    }
}

/*----------------------------------------------------------------------------
 Function Name       :  flash_program_data
 Object              :  selects an algorithm and programs data to flash
 Input Params        :  flash_id - selects the programming algorithm
                        flash_addr - specifies which location to program
                        data - specifies 16 bits of data to program
 Output Params       :  None
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
static void flash_program_data(unsigned long flash_id, volatile unsigned short *flash_addr, unsigned short data)
{
    unsigned short flash_value;

    printf("\nProgram data: %d addr:%08lx %02x,  ", flash_id, flash_addr, data);

    switch (flash_id)
    {
        case INTEL_28F320J3:
        case INTEL_28F640J3:
        case INTEL_28F128J3:
        case INTEL_28F256J3:
        case ST_M58LW032D:
        case ST_M58LW064D:
        case INTEL_28F320C3B:
        case INTEL_28F640C3B:
        case ST_M29W320DB:
        case ST_M29W640DB:
        case SHARP_LHF32FEZ:
        case ATMEL_AT49BV320D:
            *flash_addr = FLASH_WORD_PROGRAM;
            *flash_addr = data;
            while( !(*flash_addr & DONE_BIT));
            break;
        case MICROCHIP_SST39V3201C:
        case CYPRESS_S29GL032N:
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD1) = FLASH_CODE1;
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD2) = FLASH_CODE2;
            *((volatile unsigned short *)FLASH_BASE_ADDR + FLASH_SEQ_ADD1) = WRITE_CODE;
            *flash_addr = data;

            /* use the toggle function for busy check */
            flash_value = *flash_addr;
            while (flash_value != *flash_addr)
            {
                flash_value = *flash_addr;
            }
            break;
        default:
            // a valid flash ID has already been verified
            // should not get here
            break;
    }
}

/*----------------------------------------------------------------------------
 Function Name       :  flash_set_to_read_mode
 Object              :  selects an algorithm and returns the device to read mode
 Input Params        :  flash_id - selects the programming algorithm
                        flash_addr - can
 Output Params       :  None
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
static void flash_set_to_read_mode(unsigned long flash_id, unsigned short *flash_addr)
{
    switch (flash_id)
    {
        case INTEL_28F320J3:
        case INTEL_28F640J3:
        case INTEL_28F128J3:
        case INTEL_28F256J3:
        case ST_M58LW032D:
        case ST_M58LW064D:
        case INTEL_28F320C3B:
        case INTEL_28F640C3B:
        case ST_M29W320DB:
        case ST_M29W640DB:
        case SHARP_LHF32FEZ:
        case ATMEL_AT49BV320D:
            *(volatile unsigned short *)flash_addr = FLASH_READ_ARRAY;
            break;
        case MICROCHIP_SST39V3201C:
        case CYPRESS_S29GL032N:
            // not necessary but may recover a device behaving abnormally
            //*(volatile unsigned short *)FLASH_BASE_ADDR = ID_OUT_CODE;
            break;
        default:
            // a valid flash ID has already been verified
            // should not get here
            break;
    }
}

/*----------------------------------------------------------------------------
 Function Name       :  flash_lock_all_blocks
 Object              :  selects an algorithm and locks the device
 Input Params        :  flash_id - selects the locking algorithm and block
                                   configuration
 Output Params       :  None
*----------------------------------------------------------------------------*/
//#pragma location="BCODE"
static void flash_lock_all_blocks(unsigned long flash_id)
{
    unsigned int flash_addr;

    switch (flash_id)
    {
        case INTEL_28F320J3:
        case INTEL_28F640J3:
        case INTEL_28F128J3:
        case INTEL_28F256J3:
        case ST_M58LW032D:
        case ST_M58LW064D:
            for (flash_addr = FLASH_BASE_ADDR; flash_addr <= FLASH_TOP_ADDR; flash_addr += J3_FLASH_PAGE_SIZE)
            {
                // lock Sector
                *(volatile unsigned short *)flash_addr = FLASH_LOCK_BITS;
                *(volatile unsigned short *)flash_addr = FLASH_SET_LOCK;
                while( !(*(volatile unsigned short *)flash_addr & DONE_BIT));
             }
             *(volatile unsigned short *)flash_addr = FLASH_READ_ARRAY;
        case INTEL_28F320C3B:
        case INTEL_28F640C3B:
        case ST_M29W320DB:
        case ST_M29W640DB:
        case SHARP_LHF32FEZ:
        case ATMEL_AT49BV320D:
            for (flash_addr = FLASH_BASE_ADDR; flash_addr <= FLASH_TOP_ADDR; flash_addr += C3_FLASH_PAGE_SIZE)
            {
               // lock Sector
               *(volatile unsigned short *)flash_addr = FLASH_LOCK_BITS;
               *(volatile unsigned short *)flash_addr = FLASH_SET_LOCK;
               while( !(*(volatile unsigned short *)flash_addr & DONE_BIT));
            }
            *(volatile unsigned short *)flash_addr = FLASH_READ_ARRAY;
            break;
        case MICROCHIP_SST39V3201C:
        case CYPRESS_S29GL032N:
            // not necessary but may recover a device behaving abnormally
            *(volatile unsigned short *)FLASH_BASE_ADDR = ID_OUT_CODE;
            break;
        default:
            // invalid flash ID
            fwu_send_msg(UNKNOWN_FLASH);
            //AT91C_BASE_PIOA->PIO_CODR = IO_TEST_LED2; // LED ON
            while(1);
    }
}
