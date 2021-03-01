/******************************************************************
 *****                                                        *****
 *****  Name: easyweb.c                                       *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: implements a dynamic HTTP-server by using       *****
 *****        the easyWEB-API                                 *****
 *****                                                        *****
 ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LPC17xx.h"      // Keil: Register definition file for LPC17xx

#define extern            // Keil: Line added for modular project management

#include "easyweb.h"

#include "type.h"
#include "EMAC.h"         // Keil: *.c -> *.h    // ethernet packet driver
#include "tcpip.h"        // Keil: *.c -> *.h    // easyWEB TCP/IP stack

#include "webpage.h"                             // webside for our HTTP server (HTML)

unsigned int pagecounter = 100;
unsigned int adcValue    =   0;
unsigned char buff[MAX_TCP_RX_DATA_SIZE];

extern void TCPClockHandler(void);
void DoNetworkWork(void);

volatile DWORD TimeTick  = 0;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler (void) {
  TimeTick++;
  if (TimeTick >= 20) {
    TimeTick = 0;
    LPC_GPIO2->FIOPIN ^= 1 << 7;
    TCPClockHandler();  
  }  
}

//void main(void)
int main(void) {
	
	//InitOsc();              // Keil: No oscillator initialization necessary at this time.
	//InitPorts();			  // Keil: No port initialization necessary at this time.

  SystemInit();                          /* setup core clocks */
  SysTick_Config(0xf4240);               /* Generate interrupt every 10 ms: 100 MHz tick frequency, period 10 ns => interrupt each 1M ticks (each 10 ms)*/

  LPC_GPIO0->FIODIR   |= 1 << 21;					// ÉèÖÃLEDÓÐÐ§
  LPC_GPIO0->FIOPIN	  |= 1 << 21;

  LPC_GPIO2->FIODIR   |= (0xff << 0);                    /* P2.0..P2.7 defined as Output (LED) */

  LPC_PINCON->PINSEL3 |=  (3ul<<30);                   /* P1.31 is AD0.5 */
  LPC_SC->PCONP       |=  (1<<12);                   /* Enable power to ADC block */
  LPC_ADC->ADCR        =  (1<< 5) |                  /* select AD0.5 pin */
                          (4<< 8) |                  /* ADC clock is 25MHz/5 */
                          (1<<21);                   /* enable ADC */ 

  TCPLowLevelInit();


  /**(unsigned char *)RemoteIP = 24;               // uncomment those lines to get the
  *((unsigned char *)RemoteIP + 1) = 8;          // quote of the day from a real
  *((unsigned char *)RemoteIP + 2) = 69;         // internet server! (gateway must be
  *((unsigned char *)RemoteIP + 3) = 7;          // set to your LAN-router)

  TCPLocalPort = 2025;
  TCPRemotePort = TCP_PORT_QOTD;

  TCPActiveOpen();

  while (SocketStatus & SOCK_ACTIVE)             // read the quote from memory
  {                                              // by using the hardware-debugger
    DoNetworkStuff();
  }
*/

  TCPLocalPort = 6589;                  // set port we want to listen to
  
  while (1) {
    if (!(SocketStatus & SOCK_ACTIVE)) {
			TCPPassiveOpen();   				 // listen for incoming TCP-connection
    }
	
		DoNetworkStuff();              // handle network and easyWEB-stack events
    DoNetworkWork();
  }
	
}

void DoNetworkWork(void) {
	
  if (SocketStatus & SOCK_CONNECTED) {             // check if somebody has connected to our TCP 
    if (SocketStatus & SOCK_DATA_AVAILABLE) {      // check if remote TCP sent data
			memcpy(buff, TCP_RX_BUF, TCPRxDataCount);
			LPC_GPIO2->FIOPIN = (LPC_GPIO2->FIOPIN & 0x80) | 
													(*(uint8_t *)buff & 0x7f);
			
			TCPReleaseRxBuffer();                      // rx-buffer MUST be released periodically, else the other TCP get no ACKs for the data it sent
			return;
		}
	}
}

/*void DoNetworkWork(void) {
	unsigned short cnt;
	
  if (SocketStatus & SOCK_CONNECTED) {             // check if somebody has connected to our TCP 
    if (SocketStatus & SOCK_DATA_AVAILABLE) {      // check if remote TCP sent data
			cnt = TCPRxDataCount;
			memcpy(buff, TCP_RX_BUF, cnt);
			
			if (SocketStatus & SOCK_TX_BUF_RELEASED) {
				memcpy(TCP_TX_BUF, buff, cnt);						 // It uses TXFRAME1: TCPPreareFrame() is called automatically when TCPTransmitTxBuffer()
				TCPTxDataCount = cnt;
				TCPTransmitTxBuffer();									  
			}
			
			TCPReleaseRxBuffer();                      // rx-buffer MUST be released periodically, else the other TCP get no ACKs for the data it sent
			return;
		}
	}
}*/

// samples and returns the AD-converter value of channel 2

unsigned int GetAD7Val(void) {
// Keil: function replaced to handle LPC1768 A/D converter.
  unsigned int val;

  LPC_ADC->ADCR |=  (1<<24);                     /* start conversion */
  while (!(LPC_ADC->ADGDR & (1UL<<31)));         /* Wait for Conversion end */
  val = ((LPC_ADC->ADGDR >> 4) & 0xFFF);         /* read converted value */
  LPC_ADC->ADCR &= ~(7<<24);                     /* stop conversion */

  return(val);                                   /* result of A/D process */ 
}

// searches the TX-buffer for special strings and replaces them
// with dynamic values (AD-converter results)

void InsertDynamicValues(void)
{
  unsigned char *Key;
           char NewKey[5];
  unsigned int i;
  
  if (TCPTxDataCount < 4) return;                     // there can't be any special string
  
  Key = TCP_TX_BUF;
  
  for (i = 0; i < (TCPTxDataCount - 3); i++)
  {
    if (*Key == 'A')
     if (*(Key + 1) == 'D')
       if (*(Key + 3) == '%')
         switch (*(Key + 2))
         {
           case '8' :                                 // "AD8%"?
           {
             adcValue = GetAD7Val();                  // get AD value
             sprintf(NewKey, "0x%03X", adcValue);       // insert AD converter value
             memcpy(Key, NewKey, 5);                  
             break;
           }
           case '7' :                                 // "AD7%"?
           {
             sprintf(NewKey, "%3u", (adcValue*100)/4024);     // copy saved value from previous read
             memcpy(Key, NewKey, 3);                 
             break;
           }
		   case '1' :                                 // "AD1%"?
           {
 			 sprintf(NewKey, "%3u", ++pagecounter);    // increment and insert page counter
             memcpy(Key, NewKey, 3);  
			 *(Key + 3) = ' ';  
             break;
           }
         }
    Key++;
  }
}
