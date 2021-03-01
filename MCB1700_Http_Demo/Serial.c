/*----------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: MCB1700 Low level serial functions
 * Version: V1.00
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#include <LPC17xx.H>                              /* LPC17xx definitions    */
#include "Serial.h"

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void SER_init (int uart) {
UART_TypeDef *pUart;
   
  if (uart == 0) {                               /* UART0 */
    PINCON->PINSEL0 |= (1 << 4);                 /* Pin P0.2 used as TXD0 (Com0) */
    PINCON->PINSEL0 |= (1 << 6);                 /* Pin P0.3 used as RXD0 (Com0) */

    pUart = (UART_TypeDef *)UART0;
  } else {                                       /* UART1 */
    PINCON->PINSEL4 |= (2 << 0);                 /* Pin P2.0 used as TXD1 (Com1) */
    PINCON->PINSEL4 |= (2 << 2);                 /* Pin P2.1 used as RXD1 (Com1) */

    pUart = (UART_TypeDef *)UART1;
  }

  pUart->LCR    = 0x83;                          /* 8 bits, no Parity, 1 Stop bit  */
  pUart->DLL    = 6;                             /* 115200 Baud Rate @ 18.0 MHZ PCLK */
  pUart->FDR    = 0x85;                          /* FR = 1,627 DIVADDVAL = 5, MULVAL = 8 */
  pUart->DLM    = 0;                             /* High divisor latch = 0         */
  pUart->LCR    = 0x03;                          /* DLAB = 0                       */
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_putChar (int uart, int c) {
  UART_TypeDef *pUart;

  pUart = (uart == 0) ? UART0 : (UART_TypeDef *)UART1;
  while (!(pUart->LSR & 0x20));
  return (pUart->THR = c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int SER_getChar (int uart) {
  UART_TypeDef *pUart;

  pUart = (uart == 0) ? UART0 : (UART_TypeDef *)UART1;
  while (!(pUart->LSR & 0x01));
  return (pUart->RBR);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (non blocking read)
 *----------------------------------------------------------------------------*/
int SER_getChar_nb (int uart) {
  UART_TypeDef *pUart;

  pUart = (uart == 0) ? UART0 : (UART_TypeDef *)UART1;
  if (pUart->LSR & 0x01)
    return (pUart->RBR);
  else
    return 0;
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
void SER_putString (int uart, unsigned char *s) {

  while (*s != 0) {
   SER_putChar(uart, *s++);
  }
}
