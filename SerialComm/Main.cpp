// SerialComm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <conio.h>

#if 0
uint8_t* hex_decode(char* in, size_t len, uint8_t* out)
{
   unsigned int i, t, hn, ln;

   for (t = 0, i = 0; i < len; i += 2, ++t)
   {

      hn = in[i] > '9' ? (in[i] | 32) - 'a' + 10 : in[i] - '0';
      ln = in[i + 1] > '9' ? (in[i + 1] | 32) - 'a' + 10 : in[i + 1] - '0';

      out[t] = (hn << 4) | ln;
      printf("%s", out[t]);
   }
   return out;
}

#endif

void atoh(char *ascii_ptr, char *hex_ptr, int len)
{
   int i;

   for (i = 0; i < (len / 2); i++)
   {

      *(hex_ptr + i) = (*(ascii_ptr + (2 * i)) <= '9') ? ((*(ascii_ptr + (2 * i)) - '0') * 16) : (((*(ascii_ptr + (2 * i)) - 'A') + 10) << 4);
      *(hex_ptr + i) |= (*(ascii_ptr + (2 * i) + 1) <= '9') ? (*(ascii_ptr + (2 * i) + 1) - '0') : (*(ascii_ptr + (2 * i) + 1) - 'A' + 10);

   }
}

#if 0
/*
                        * Got start sequence, ready for first data record to determine
                        * upgrade type.
                        */
do
{
   fwu_send_msg(NO_ERROR);
   fwu_get_msg(input_buffer);
   // Go get type
   input_srec.type = asc_to_hex(input_buffer[1]);
} while (input_srec.type != S1_RECORD);
#endif


/*
 * S-record field positions.
 */
#define		BC_STRT					2
#define		BC_LEN					2
#define		BC_END					BC_STRT + BC_LEN
#define		ADDR_STRT				BC_END
#define 	CS_LEN					2

#define 	S0_RECORD	0
#define		S1_RECORD	1
#define		S2_RECORD	2
#define		S3_RECORD	3
#define		S7_RECORD	7
#define		S8_RECORD	7
#define		S9_RECORD	9

/*
 * Parsed s-records go here.
 */
typedef struct
{
   unsigned char	type;           // Record type S0, 1, or 9 (only support S1S9 format)
   unsigned char	data_bc;        // Data byte count.
   unsigned long	address;        // Start address of record.
   unsigned char	data[32];       // Actual code data bytes.
} s_record;

#define asc_to_hex(a) ((a) & 0x40) ? (((a) & 0x0f) + 9) : ((a) & 0x0f)


// code from main board bs_upgrade

/****************************************************************************
FUNCTION: void fwu_get_msg( unsigned char *msg )

DESCRIPTION: This function will sit and read bytes off RS-232 until
the input data is LF (ASCII 10).  Will tack on 0x00 to
terminate string.

INPUTS:      Pointer to character array to place incomming data.

OUTPUTS:

EDIT HISTORY:

****************************************************************************/
void fwu_get_msg(char *msg)
{
   unsigned char done = FALSE;
#ifndef DEBUG
//	long ticks;

   // Wait for at least 4 seconds
//	ticks = bs_clock();
#endif

   // Read From UART
   do
   {
      // Exit if timeout
#ifndef DEBUG
//		if (bs_clock() - ticks > 4)
//			break;
#endif
//		if (AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY)
      {
         // Grab data and save it
//			*msg = AT91C_BASE_DBGU->DBGU_RHR;
//			if (*msg == RS232_STOP_CHAR)
         {
            done = TRUE;
         }
//			else
//			{
         msg++;
//			} // if( *msg != RS232_STOP_CHAR)
      } // if( U0RCTL_bit.RXWAKE)
   }
   while (!done);   // LOOP UNTIL LF
   *msg = '\0';    // Replace LF with NULL or just put NULL in string if timeout
} // fwu_get_msg()



/****************************************************************************
FUNCTION: void fwu_send_msg( char *msg )

DESCRIPTION: Sends a NULL terminated string out the GPIB or RS-232 port

INPUTS:      Pointer to character array to send

OUTPUTS:     none

EDIT HISTORY:

****************************************************************************/
void fwu_send_msg(const char *msg)
{
    printf("\n%s", msg);
#if 0
   // Send to UART1
   while (*msg)
   {
      //while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
      //AT91C_BASE_DBGU->DBGU_THR = *msg++;
   } // while( *msg)
   // Send LF
   //while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
   //AT91C_BASE_DBGU->DBGU_THR = RS232_STOP_CHAR;
   // Wait until Done!
   //while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
#endif
} // fwu_send_msg()

char input_buffer[100];

void test()
{
   s_record input_srec;
   /*
    * Got start sequence, ready for first data record to determine
    * upgrade type.
    */
   do
   {
      fwu_send_msg(NO_ERROR);
      fwu_get_msg(input_buffer);
      // Go get type
      input_srec.type = asc_to_hex(input_buffer[1]);
   }
   while (input_srec.type != S3_RECORD);
}



#define    BUFFERLENGTH 256


extern "C"
{

unsigned long __flash_upgrade(void);
}


void fwu_get_msg(HANDLE hComm, char* msg)
{
    BOOL  Read_Status;                      // Status of the various operations
    DWORD NoBytesRead;                     // Bytes read by ReadFile()
    int i = 0;
    do
    {
        //Read_Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
        char TempChar;
        Read_Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
        if (NoBytesRead > 0)
        {
            msg[i] = TempChar;
        }
        i += NoBytesRead;
    } while (NoBytesRead > 0);
    msg[i] = 0;
}
static 
unsigned char fwu_strcmp(char* str1, char* str2)
{
    while (*str2 && *str1)
    {
        if (*str2 == *str1)
        {
            str1++;
            str2++;
        }
        else
        {
            return(1);
        } // if( *str2 == *str1 )
    } // while( *str2 && *str1 )
    return(0);
} // fwu_strcmp

int main(int argc, char *argv[])
{
   HANDLE hComm;
   DWORD MORO;
   char pcCommPort[] = "COM4";
   BOOL Write_Status;
   DCB dcbSerialParams;                                    // Initializing DCB structure
   COMMTIMEOUTS timeouts = { 0 };
   BOOL  Read_Status;                      // Status of the various operations
   DWORD dwEventMask;                                              // Event mask to trigger
   char  TempChar[] = "";                        // Temperory Character
   char input_buffer[BUFFERLENGTH + 1];               // Buffer Containing Rxed Data
   DWORD NoBytesRead;                     // Bytes read by ReadFile()
   int i = 0;


   // __flash_upgrade();



   printf("\n\n +==========================================+");
   printf("\n |  Serial Transmission (Win32 API)         |");
   printf("\n +==========================================+\n");

   hComm = CreateFileA(pcCommPort,
                       GENERIC_READ | GENERIC_WRITE,
                       0,    // must be opened with exclusive-access
                       NULL, // no security attributes
                       OPEN_EXISTING, // must use OPEN_EXISTING
                       0,    // not overlapped I/O
                       NULL  // hTemplate must be NULL for comm devices
                      );

   if (hComm == INVALID_HANDLE_VALUE)
   {

      if (GetLastError() == ERROR_FILE_NOT_FOUND)
      {
         puts("cannot open port!");
         return -1;
      }

      puts("invalid handle value!");
      return -1;
   }
   else printf("opening serial port successful");

   dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

   Write_Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

   if (Write_Status == FALSE)
   {
      printf("\n   Error! in GetCommState()");
      CloseHandle(hComm);
      return 1;
   }


   //dcbSerialParams.BaudRate = CBR_57600;      // Setting BaudRate = 9600
   dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate = 9600
   //dcbSerialParams.BaudRate = CBR_9600;      // Setting BaudRate = 9600
   dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
                                             //dcbSerialParams.
   dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
   dcbSerialParams.Parity = ODDPARITY;      // Setting Parity = None

   Write_Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

   if (Write_Status == FALSE)
   {
      printf("\n   Error! in Setting DCB Structure");
      CloseHandle(hComm);
      return 1;
   }
   else
   {
      printf("\n   Setting DCB Structure Successful\n");
      printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
      printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
      printf("\n       StopBits = %d", dcbSerialParams.StopBits);
      printf("\n       Parity   = %d", dcbSerialParams.Parity);
   }

   // Set COM port timeout settings
   //timeouts.ReadIntervalTimeout = 50;
   timeouts.ReadIntervalTimeout = 5;
   timeouts.ReadTotalTimeoutConstant = 50;
   timeouts.ReadTotalTimeoutMultiplier = 10;
   timeouts.WriteTotalTimeoutConstant = 50;
   timeouts.WriteTotalTimeoutMultiplier = 10;
   if (SetCommTimeouts(hComm, &timeouts) == 0)
   {
      printf("Error setting timeouts\n");
      CloseHandle(hComm);
      return 1;
   }


   /*----------------------------- Writing a Character to Serial Port----------------------------------------*/
   char lp[] = "RDGFIELD? \r\n";           // lpBuffer should be  char or byte array, otherwise write wil fail

   DWORD  NumWritten;
   DWORD  dNoOFBytestoWrite;              // No of bytes to write into the port
   DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port

   dNoOFBytestoWrite = sizeof(lp); // Calculating the no of bytes to write into the port


   if (!WriteFile(hComm, lp, dNoOFBytestoWrite,
                  &dNoOfBytesWritten, NULL))
   {
      printf("Error writing text to %s\n", pcCommPort);
   }
   else
   {
      printf("\n %d bytes written to %s\n",
             dNoOfBytesWritten, pcCommPort);
   }

   if (Write_Status == TRUE) printf("\n\n    %s - Written to %s", lp, pcCommPort);
   else printf("\n\n   Error %d in Writing to Serial Port", GetLastError());

   /*------------------------------------ Setting Receive Mask ----------------------------------------------*/

   Read_Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

   if (Read_Status == FALSE) printf("\n\n    Error! in Setting CommMask");
   else printf("\n\n    Setting CommMask successfull");
   

   /*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/


   // open a log file
   FILE* fp;
   fopen_s(&fp, "c:\\dev\\log_file.txt", "a+");
   fprintf(fp, "Test");
   fclose(fp);

   // fclose(fp);


   int mode = 0;  // wait for the password

   while (1)
   {
       Read_Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

       if (Read_Status == FALSE)
       {
           printf("\n    Error! in Setting WaitCommEvent()");
       }
       else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
       {

           fwu_get_msg(hComm, input_buffer);
           printf("\nRX:%s", input_buffer);

           if (mode == 0)
           {
               if (!fwu_strcmp(input_buffer, (char*)"995799MARGAXEH"))
               {
                   printf("\nGot password");
                   mode = 1;
                   if (!WriteFile(hComm, "0\n", 2,
                       &dNoOfBytesWritten, NULL))
                   {
                   }
               }
           }
           else if (mode == 1) 
           {
               // waiting for first S3 records
               if (input_buffer[1] == '3')
               {
                   mode = 2;
               }
               if (!WriteFile(hComm, "0\n", 2,
                   &dNoOfBytesWritten, NULL))
               {
               }
           }
           else {
              if (input_buffer[1] == '7')
              {
                  if (!WriteFile(hComm, "9\n", 2,
                      &dNoOfBytesWritten, NULL))
                  {
                  }
              }
              else if (input_buffer[1] == '3')
              {
                  if (!WriteFile(hComm, "0\n", 2,
                      &dNoOfBytesWritten, NULL))
                  {
                  }
              }
              else
              {
                  if (!WriteFile(hComm, "0\n", 2,
                      &dNoOfBytesWritten, NULL))
                  {
                  }
              }
           }
       }
   }


   printf("\n");

   while (1)
   {

      //printf("\n\nWaiting for Data Reception");
      Read_Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

      /*-------------------------- Program will Wait here till a Character is received ------------------------*/

      if (Read_Status == FALSE)
      {
         printf("\n    Error! in Setting WaitCommEvent()");
      }
      else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
      {
         //printf("\nRX:");
          char hex_data[255];
          //while (1) {
              i = 0;
              do
              {
                  //Read_Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
                  char TempChar;
                  Read_Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
                  if (NoBytesRead > 0)
                  {
                      //printf("\n\n    BytesRead %d\n", NoBytesRead);
                      putc(TempChar,stdout);
                      input_buffer[i] = TempChar;
                  }

                  //if (!ReadFile(hComm, input_buffer, 255, &NoBytesRead, NULL))
                  //{
      //					printf("wrong character");
                  //}
                  //printf("\n\n    BytesRead %d\n", NoBytesRead);
                  //SerialBuffer[i] = TempChar;
                  //printf("%02x ", (int) (SerialBuffer[0] & 0xff ));
                  i += NoBytesRead;
              } while (NoBytesRead > 0);

              //if (i > 0) {
                //  fwu_send_msg("0\n");
              //}

              /*------------Printing the RXed Data to Console----------------------*/
#if 1              
              atoh(input_buffer, hex_data, i);

              fprintf(stdout, "\nRX:");
              fopen_s(&fp, "c:\\dev\\log_file.txt", "a+");

              fprintf(fp, "\nRX:");
              for (int j = 0; j < i - 1; j++)
              {
                  putc(input_buffer[j], fp);
                  putc(input_buffer[j], stdout);
              }
              fclose(fp);
#endif
          //}


         //// Check for the password
         //if (strcmp(input_buffer,"995799MARGAXEH"))
         //{

         //}


         s_record input_srec;
         input_srec.type = asc_to_hex(input_buffer[1]);

         do
         {
         //   fwu_send_msg(NO_ERROR);
            fwu_get_msg(input_buffer);
         // Go get type
            input_srec.type = asc_to_hex(input_buffer[1]);
            fwu_send_msg(NO_ERROR);
         } while (input_srec.type != S1_RECORD);

         if (input_srec.type != S1_RECORD)
         {
            unsigned short  byte_cnt, xmit_cksum, calc_cksum;
            unsigned char   high_nibble, low_nibble;
            //s_record input_srec;


            //	input_srec.type = asc_to_hex(input_buffer[1]);



            /*
             * Parse byte count; includes address, data, and checksum.
             */
            byte_cnt = 0;
            for (i = BC_STRT; i < BC_END; i++)
            {
               //byte_cnt <<= 2; // Don't call library function!!!
               //byte_cnt <<= 2;
               byte_cnt <<= 4;
               byte_cnt |= asc_to_hex(input_buffer[i]);
            }

            /*
            * Parse checksum transmitted with data.
            */
            xmit_cksum = 0;
            for (i = ADDR_STRT + (byte_cnt << 1) - CS_LEN;
                 i < ADDR_STRT + (byte_cnt << 1); i++)
            {
               //xmit_cksum <<= 2; // Don't call library function!!!
               //xmit_cksum <<= 2;
               xmit_cksum <<= 4;
               xmit_cksum |= asc_to_hex(input_buffer[i]);
            }

            /*
            * Calculate checksum.
            */
            calc_cksum = 0;
            for (i = BC_STRT; i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2)
            {
               int high_nibble = asc_to_hex(input_buffer[i]);
               int low_nibble = asc_to_hex(input_buffer[i + 1]);
               //high_nibble <<= 2;  // Don't call library function!!!
               calc_cksum += ((high_nibble << 4) | low_nibble);
            }

//       		printf("\n Byte_cnt:%d xmit_cksum:%02x calc_cksum:%02x ", byte_cnt, xmit_cksum, calc_cksum & 0xff);

            if ((xmit_cksum & 0x00FF) != (~calc_cksum & 0x00FF))
            {
               //		fwu_send_msg(SREC_SUMERR);
               //IO_STAT1 = 1;  // Turn off LED1&2 if Error
               //IO_STAT2 = 1;  // Turn off LED1&2 if Error
               //fwu_reboot();
               printf("\n Invalid checksum!!!");

            } // if( (xmit_cksum & 0x00FF) != (~calc_cksum & 0x00FF) )

            /*
             * Compute start address.
             */
            input_srec.address = 0;
            //for (int j = 0, i = ADDR_STRT; i < ADDR_STRT + 4; i++, j++)
            for (int j = 0, i = ADDR_STRT; i < ADDR_STRT + 8; i++, j++)
            {
               input_srec.address <<= 4;
               input_srec.address |= asc_to_hex(input_buffer[i]);
            }
#if 0
            printf("\n address:%08lx ", input_srec.address);

            /*
            * Convert ASCII data in S-record to hex.
            */
            //for (int j = 0, i = ADDR_STRT + 4;
            for (int j = 0, i = ADDR_STRT + 8;
                 i < ADDR_STRT + (byte_cnt << 1) - CS_LEN; i = i + 2, j++)
            {
               high_nibble = asc_to_hex(input_buffer[i]);
               low_nibble  = asc_to_hex(input_buffer[i + 1]);
//					high_nibble <<= 2;  // Don't call library function!!!
               input_srec.data[j] = (high_nibble << 4) | low_nibble;
               printf("%02x ", input_srec.data[j]);
            }
#endif
         }
         if ((hex_data[0] & 0xff) == 0xc7)
         //if (input_srec.type == S7_RECORD)
         {
            // Done
            printf("\nTx:Done");
            if (!WriteFile(hComm, "9\n", 2,
                           &dNoOfBytesWritten, NULL))
            {
               printf("Error writing text to %s\n", pcCommPort);
            }
         }
         else
         {
            printf("\nTx:Ok");
            if (!WriteFile(hComm, "0\n", 2,
                           &dNoOfBytesWritten, NULL))
            {
               printf("Error writing text to %s\n", pcCommPort);
            }
         }
      }
   }

   CloseHandle(hComm); //Closing the Serial Port
   printf("\n ==========================================\n");


   return 0;
}
#if 0

int main()
{
   std::cout << "Hello World!\n";
}
#endif

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
