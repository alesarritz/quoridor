#include "button.h"
#include "lpc17xx.h"
#include "../RIT/RIT.h"		 

extern int down_I0;
extern int down_K1;
extern int down_K2;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	down_I0=1;
	NVIC_DisableIRQ(EINT0_IRQn);	         	/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	down_K1=1;
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{	
	down_K2=1;
	NVIC_DisableIRQ(EINT2_IRQn);		          /* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */

  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


