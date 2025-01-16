/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../quoridor.h"

volatile int down_I0=0, down_K1=0, down_K2=0;
extern int gameOn;
extern int next;
extern Player player[2];
extern uint8_t activePlayer;
extern Box board[7][7];

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**									Gestisce il debouncing e l’elaborazione dei dati di pulsanti e joystick. In particolare:
**										-	INT0: inizia il gioco (startGame)
**										-	KEY1: piazzamento muro in posizione centrale, stampa warning se nessun muro è disponibile. 
**										  Cliccarlo una seconda volta riporta il giocatore alla movimentazione del token.
**										-	KEY2: Se in modalità piazzamento muro, lo fa ruotare di 90°.
**										-	Joystick UP/DOWN/LEFT/RIGHT: verifica che il comando richiesto sia conforme alle direzioni accettabili 
**										  calcolate per il giocatore attivo (updatePlayerState) e invia l’esecuzione temporanea della mossa (executeMove).
**										-	Joystick SELECT: in caso di movimentazione token conferma l’ultima mossa (executeMove) eseguita con 
**										  successo tramite Joystick UP/DOWN/LEFT/RIGHT. In caso di piazzamento muro prima di confermare la mossa (executeMove) 
**											verifica che il suo inserimento non causi sovrapposizioni con altri muri (checkWallPosition) o trappole (checkTrap).
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void RIT_IRQHandler (void){					
	static int J_select=0;
	static int J_down = 0;
	static int J_left = 0;
	static int J_right = 0;
	static int J_up = 0;
	
	static Move jMove = {0};
	static int warning = 0;
	
	/* Current move management*/
	if(next==1){
		//Next turn -> reset jmove
		jMove.word32=0;
		jMove.bits.playerID=activePlayer;
		jMove.bits.x=player[activePlayer].x;
		jMove.bits.y=player[activePlayer].y;
		next=0;
	}
	
	/* Joystick management */
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick J_Select pressed p1.25*/
		J_select++;
		if(J_select==1 && gameOn==1){
				//Remove warning if present
			  if(warning==1) {
					drawMessageBox("   NO WALLS! Move token.", Sand); 
					warning=0;
				}
				
				//If inserting wall check acceptable wall position and trap
				if(jMove.bits.move==1 && 
  					(checkWallPosition(jMove.bits.x, jMove.bits.y, jMove.bits.orientation)==false ||
						 checkTrap(jMove.bits.x, jMove.bits.y, jMove.bits.orientation)==false)){
					return;
				}
		
				executeMove(jMove, true);
		}
	}
	else{J_select=0;}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick J_Down pressed p1.26 */
		J_down++;
		if(J_down==1 && gameOn==1){
				//Remove warning if present
			  if(warning==1) {
					drawMessageBox("   NO WALLS! Move token.", Sand); 
					warning=0;
				}
				
				//Handle
				if(jMove.bits.move==0 &&
					player[activePlayer].x==jMove.bits.x &&
					player[activePlayer].y==jMove.bits.y &&
					player[activePlayer].direction.bits.down>0){
					jMove.bits.x += player[activePlayer].direction.bits.down;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==0 &&
					(jMove.bits.x + player[activePlayer].direction.bits.down == player[activePlayer].x ||
					 jMove.bits.x + player[activePlayer].direction.bits.up == player[activePlayer].x)&&
					jMove.bits.y == player[activePlayer].y ){
					jMove.bits.x = player[activePlayer].x;
					jMove.bits.y = player[activePlayer].y;
					executeMove(jMove, false);
				}
			 else if(jMove.bits.move==1 && jMove.bits.x+1<7){
			    jMove.bits.x++;
					executeMove(jMove, false);
				}
		}
	}
	else{J_down=0;}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick J_Left pressed p1.27 */
		J_left++;
		if(J_left==1 && gameOn==1){
				//Remove warning if present
			  if(warning==1) {
					drawMessageBox("   NO WALLS! Move token.", Sand); 
					warning=0;
				}
			
				//Handle
				if(jMove.bits.move==0 &&
					player[activePlayer].x==jMove.bits.x &&
					player[activePlayer].y==jMove.bits.y &&
					player[activePlayer].direction.bits.left>0){
					jMove.bits.y -= player[activePlayer].direction.bits.left;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==0 &&
					jMove.bits.x == player[activePlayer].x &&
				 (jMove.bits.y - player[activePlayer].direction.bits.left == player[activePlayer].y ||
					jMove.bits.y - player[activePlayer].direction.bits.right == player[activePlayer].y)){
					jMove.bits.x = player[activePlayer].x;
					jMove.bits.y = player[activePlayer].y;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==1 && jMove.bits.y-1>0){
					jMove.bits.y--;
					executeMove(jMove, false);
				}
		}
	}
	else{J_left=0;}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick J_Right pressed p1.28 */
		J_right++;
		if(J_right==1 && gameOn==1){
				//Remove warning if present
			  if(warning==1) {
					drawMessageBox("   NO WALLS! Move token.", Sand); 
					warning=0;
				}
			
				//Handle
				if(jMove.bits.move==0 &&
					player[activePlayer].x==jMove.bits.x &&
					player[activePlayer].y==jMove.bits.y &&
					player[activePlayer].direction.bits.right>0){
					jMove.bits.y += player[activePlayer].direction.bits.right;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==0 &&
					(jMove.bits.y + player[activePlayer].direction.bits.right == player[activePlayer].y ||
				  jMove.bits.y + player[activePlayer].direction.bits.left == player[activePlayer].y) &&
					jMove.bits.x == player[activePlayer].x ){
					jMove.bits.x = player[activePlayer].x;
					jMove.bits.y = player[activePlayer].y;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==1 && jMove.bits.y+1<7){
					jMove.bits.y++;
					executeMove(jMove, false);
				}
		}
	}
	else{J_right=0;}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick J_up pressed p1.29 */
		J_up++;
		if(J_up==1 && gameOn==1){
				//Remove warning if present
			  if(warning==1) {
					drawMessageBox("   NO WALLS! Move token.", Sand); 
					warning=0;
				}
				//Player mode with player in starting position
				if(jMove.bits.move==0 &&
					player[activePlayer].x==jMove.bits.x &&
					player[activePlayer].y==jMove.bits.y &&
					player[activePlayer].direction.bits.up>0){
					jMove.bits.x -= player[activePlayer].direction.bits.up;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==0 &&
				 (jMove.bits.x - player[activePlayer].direction.bits.up == player[activePlayer].x ||
					jMove.bits.x - player[activePlayer].direction.bits.down == player[activePlayer].x) &&
					jMove.bits.y == player[activePlayer].y ){
					jMove.bits.x = player[activePlayer].x;
					jMove.bits.y = player[activePlayer].y;
					executeMove(jMove, false);
				}
				else if(jMove.bits.move==1 && jMove.bits.x-1>0){
					jMove.bits.x--;
					executeMove(jMove, false);
				}
		}
	}
	else{J_up=0;}
	
	/* button management */
	if(down_I0!=0){ 
		down_I0++;	
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){    	/* INT0 pressed */	
			if(down_I0==2 && gameOn<1){startGame();}
		}
		else {	/* button released */
			down_I0=0;	
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_K1!=0){ 
		down_K1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */			
			if(down_K1==2 && gameOn==1){
				//HANDLE KEY1  -- Wall Placement
				//Check wall availability
				if(player[activePlayer].walls==0 && jMove.bits.move==0) {
					warning = 1;
					drawMessageBox("   NO WALLS! Move token.", Magenta);  } 
				else if ( jMove.bits.move==0){
					//Place central wall
					jMove.bits.move=1;
					jMove.bits.orientation=1;
					jMove.bits.y=4;
					jMove.bits.x=3;
					executeMove(jMove, false);}
				else if( jMove.bits.move==1) {
					//Exit wall placement mode
					jMove.bits.move=0;
					jMove.bits.orientation=0;
					jMove.bits.x=player[activePlayer].x;
					jMove.bits.y=player[activePlayer].y;
					executeMove(jMove, false);}	
			}
		}
		else {	/* button released */
			down_K1=0;	
			NVIC_EnableIRQ(EINT1_IRQn);							
			LPC_PINCON->PINSEL4    |= (1 << 22);   
		}
	}
	if(down_K2!=0){ 
		down_K2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */
						
			if(down_K2==2 && gameOn==1 && player[activePlayer].walls!=0){
				//HANDLE KEY2 -- Wall rotation
				if(jMove.bits.move==1){
					jMove.bits.orientation=!jMove.bits.orientation;
					executeMove(jMove, false);
				}
			}
		}
		else {	/* button released */
			down_K2=0;	
			NVIC_EnableIRQ(EINT2_IRQn);							
			LPC_PINCON->PINSEL4    |= (1 << 24);    
		}
	}
	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
