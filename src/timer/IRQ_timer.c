/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h"
#include "../quoridor.h"

extern int timeQuoridor;
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**                  Aggiorna la sezione della grafica relativa al timer e quando 
**									scattano i 20 secondi esegue la mossa di tipo Move che rappresenta 
**									il timeout del turno. Questa viene gestita da executeMove.
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	char t[6] = " ";
	if(timeQuoridor>0)timeQuoridor--;
	sprintf(t,"  %02ds",timeQuoridor);
	drawRectangleWithText(1, t, 1, Black);
	
	if(timeQuoridor==0){
		Move m;
		m.word32 = 0;
		m.bits.orientation=1;
		executeMove(m, true);
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
