/*----------------------------------------------------------------------------
 *      R T L   T C P N E T   E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    HTTP_DEMO.C
 *      Purpose: HTTP Server demo example
 *      Rev.:    V3.60
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <RTL.h>
#include <Net_Config.h>
#include <LPC17xx.h>                    /* LPC17xx definitions               */
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include "ADC.h"
#include "Serial.h"

BOOL LEDrun;
BOOL LCDupdate;
BOOL tick;
U32  dhcp_tout;
U8   lcd_text[2][20+1];                       /* Buffer for LCD text         */

extern LOCALM localm[];                       /* Local Machine Settings      */
#define MY_IP localm[NETIF_ETH].IpAdr
#define DHCP_TOUT   50                        /* DHCP timeout 5 seconds      */

static void init_io (void);
static void init_display (void);
#ifdef RTX_KERNEL
 U64 tcp_stack[800/8];                        /* A bigger stack for tcp_task */

 /* Forward references */
 __task void init       (void);
 __task void blink_led  (void);
 __task void timer_task (void);
 __task void tcp_task   (void);
#endif

/*--------------------------- init ------------------------------------------*/

#ifndef RTX_KERNEL

static void init () {
   /* Add System initialisation code here */ 

   init_io ();
   init_display ();
   init_TcpNet ();

   if (SysTick_Config(SystemFrequency / 10)) { /* Setup SysTick for 100 msec */
     while (1);                                /* Capture error */
   }
}

#else

__task void init (void) {
   /* Add System initialisation code here */ 

   init_io ();
   init_display ();
   init_TcpNet ();

   /* Initialize Tasks */
   os_tsk_prio_self (100);
   os_tsk_create (blink_led, 20);
   os_tsk_create (timer_task, 30);
   os_tsk_create_user (tcp_task, 0, &tcp_stack, sizeof(tcp_stack));
   os_tsk_delete_self();
}

#endif

/*--------------------------- timer_poll ------------------------------------*/

#ifndef RTX_KERNEL

volatile uint32_t msTicks;                          /* counts 100ms timeTicks */
void SysTick_Handler(void) {
   /* SysTick Handler */
   msTicks++;                                       /* increment tick counter */
}

static void timer_poll () {
   /* Timer Poll calls timer_tick every 100 ms */
   static uint32_t msTicksLast;

   if (msTicksLast != msTicks)  {                  /* Timer tick every 100 ms */
       msTicksLast = msTicks;
       timer_tick ();
       tick = __TRUE;
    }
}

#else

__task void timer_task (void) {
   /* System tick timer task */
   os_itv_set (10);
   while (1) {
      timer_tick ();
      tick = __TRUE;
      os_itv_wait ();
   }
}

#endif


/*--------------------------- init_io ---------------------------------------*/

static void init_io () {
  SER_init (1);                           /* initialize serial interface     */
  LED_init ();                            /* initialize LEDs                 */
  KBD_init ();                            /* initialize Push Button          */
  ADC_init ();                            /* initialize ADC                  */
}


/*--------------------------- fputc -----------------------------------------*/

int fputc(int ch, FILE *f)  {
   /* Debug output to serial port. */

   if (ch == '\n')  {
      SER_putChar (1, '\r');                 /* output CR                    */
   }
   return (SER_putChar (1, ch));
}


/*--------------------------- AD_in -----------------------------------------*/

U16 AD_in (void) {
   /* Read ARM Analog Input */
   U32 val = 0;

   ADC_startCnv();                                 /* start A/D conversion   */
   val = ADC_getCnv();                             /* read converted value   */
   ADC_stopCnv();                                  /* stop A/D conversion    */

   return (val);
}


/*--------------------------- get_button ------------------------------------*/

U8 get_button (void) {
   /* Read ARM Digital Input */
   U32 val = 0;

   val = KBD_get();                                /* read Joystick state    */
   val = (~val & KBD_MASK);                        /* key pressed is '0'     */

   return (val);
}


/*--------------------------- upd_display -----------------------------------*/

static void upd_display () {
   /* Update GLCD Module display text. */

   GLCD_clearLn (Line4);
   GLCD_displayStringLn(Line4, lcd_text[0]);
   GLCD_clearLn (Line5);
   GLCD_displayStringLn(Line5, lcd_text[1]);
   LCDupdate =__FALSE;
}


/*--------------------------- init_display ----------------------------------*/

static void init_display () {

  GLCD_init();                            /* initialize GLCD                 */

  GLCD_clear(White);
  GLCD_setBackColor(Blue);                        /* Set the Back Color    */
  GLCD_setTextColor(White);                       /* Set the Text Color    */
  GLCD_displayStringLn(Line0, "  MCB1700 RL-ARM    ");
  GLCD_displayStringLn(Line1, "   HTTP example     ");
  GLCD_displayStringLn(Line2, "    www.keil.com    ");
  GLCD_setBackColor(White);                        /* Set the Back Color    */
  GLCD_setTextColor(Blue);                       /* Set the Text Color    */
}


/*--------------------------- dhcp_check ------------------------------------*/

static void dhcp_check () {
   /* Monitor DHCP IP address assignment. */

   if (tick == __FALSE || dhcp_tout == 0) {
      return;
   }
#ifdef RTX_KERNEL
   tick = __FALSE;
#endif
   if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE && !(dhcp_tout & 0x80000000)) {
      /* Success, DHCP has already got the IP address. */
      dhcp_tout = 0;
      sprintf((char *)lcd_text[0]," IP address:");
      sprintf((char *)lcd_text[1]," %d.%d.%d.%d", MY_IP[0], MY_IP[1],MY_IP[2], MY_IP[3]);
      LCDupdate = __TRUE;
      return;
   }
   if (--dhcp_tout == 0) {
      /* A timeout, disable DHCP and use static IP address. */
      dhcp_disable ();
      sprintf((char *)lcd_text[1]," DHCP failed" );
      LCDupdate = __TRUE;
      dhcp_tout = 30 | 0x80000000;
      return;
   }
   if (dhcp_tout == 0x80000000) {
      dhcp_tout = 0;
      sprintf((char *)lcd_text[0]," IP address:");
      sprintf((char *)lcd_text[1]," %d.%d.%d.%d", MY_IP[0], MY_IP[1],MY_IP[2], MY_IP[3]);
      LCDupdate = __TRUE;
   }
}


/*--------------------------- blink_led -------------------------------------*/

#ifndef RTX_KERNEL

static void blink_led () {
   /* Blink the LEDs on MCB2300 board */
   const U8 led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                            0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
   static U32 cnt;

   if (tick == __TRUE) {
      /* Every 100 ms */
      tick = __FALSE;
      if (LEDrun == __TRUE) {
         LED_out (led_val[cnt]);
         if (++cnt >= sizeof(led_val)) {
            cnt = 0;
         }
      }
      if (LCDupdate == __TRUE) {
         upd_display ();
      }
   }
}

#else

__task void blink_led () {
   /* Blink the LEDs on MCB2300 board */
   const U8 led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                            0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
   static U32 cnt;

   LEDrun = __TRUE;
   while(1) {
      /* Every 100 ms */
      if (LEDrun == __TRUE) {
         LED_out (led_val[cnt]);
         if (++cnt >= sizeof(led_val)) {
            cnt = 0;
         }
      }
      if (LCDupdate == __TRUE) {
         upd_display ();
      }
      os_dly_wait(10);
   }
}

#endif


/*---------------------------------------------------------------------------*/

#ifndef RTX_KERNEL

int main (void) {
   /* Main Thread of the TcpNet */
   SystemInit();                                             /* setup clocks */

   init ();
   LEDrun = __TRUE;
   dhcp_tout = DHCP_TOUT;
   while (1) {
      timer_poll ();
      main_TcpNet ();
      dhcp_check ();
      blink_led ();
   }
}

#else

__task void tcp_task (void) {
   /* Main Thread of the TcpNet. This task should have */
   /* the lowest priority because it is always READY. */
   dhcp_tout = DHCP_TOUT;
   while (1) {
      main_TcpNet();
      dhcp_check ();
      os_tsk_pass();
   }
}


int main (void) {

   SystemInit();                                             /* setup clocks */

   /* Start with 'init' task. */
   os_sys_init(init);
   while(1);
}

#endif


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
