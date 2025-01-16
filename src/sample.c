#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "GLCD/GLCD.h"
#include "quoridor.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
** Function name:		main
**
** Descriptions:		Main Program
**                  Inizializza il sistema, i pulsanti, il joystick, il RIT (che viene anche abilitato), 
**                  il timer e lo schermo LCD. Dopodiché viene inizializzata la grafica per il gioco Quoridor 
**                  e si resta in attesa del comando d’inizio.
**
**
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	init_timer(0, 0x17D7840); 						/*  1s * 25MHz = 0x17D7840 */
	
	LCD_Initialization();
	LCD_Clear(Sand);
	
	initQuoridor();
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
