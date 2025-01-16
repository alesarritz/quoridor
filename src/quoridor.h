/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           quoridor.h
** Last modified Date:  2023-12-28
** Last Version:        V1.00
** Descriptions:        Prototypes of functions of the Quoridor game
** Correlated files:    quoridor.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __QUORIDOR_H
#define __QUORIDOR_H

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/

/*******************************************************************************
**   Direction rappresenta le possibili mosse effettuabili da un token. 
**   Ogni direzione può assumere i seguenti valori: 
**   [0] bloccato, [1] accettabile, [2] accettabile con salto dell’altro token.
********************************************************************************/
typedef union{
	uint8_t word8;
	struct{
		uint8_t up:2;                 //0 blocked, 1 acceptable, 2 acceptable with jump over other player
		uint8_t down:2;
		uint8_t left:2;
		uint8_t right:2;
	}bits;
}Direction;


/*******************************************************************************
**   Player contiene tutti i dati che descrivono un giocatore quali: id univoco, 
**   colore, posizione, destinazione finale, numero di muri rimanenti e direzioni 
**   percorribili.
********************************************************************************/
typedef struct{
   uint8_t id;
   uint16_t color;
	 uint8_t x;
	 uint8_t y;
	 uint8_t finalX;
	 uint8_t walls;
	 Direction direction;
}Player;


/*******************************************************************************
**   Box descrive la singola casella della tavola da gioco. Ci dà informazioni 
**   sulla presenza/assenza di token e muri.
********************************************************************************/
typedef struct{
	 uint8_t player;
	 uint8_t wallH;   
	 uint8_t wallV;
}Box;


/*******************************************************************************
**   Wall descrive il singolo muro presente nella tavola da gioco. 
**   Ci dà informazioni sulla sua posizione orientamento.
********************************************************************************/
typedef struct{
	 uint8_t orientation;
	 uint8_t y;
	 uint8_t x;
}Wall;


/*******************************************************************************
**   Move descrive la mossa effettuata dal giocatore. In particolare:
**   -	playerID: identifica il giocatore [0/1]
**	 -	move: identifica il tipo di mossa [0] movimentazione token, [1] piazzamento muro
**   -	orientation: orientamento del muro [0] verticale, [1] orizzontale. Se move=0, default orientation=0. 
**   -	x,y: posizione
**   Se il giocatore fa scadere il tempo il suo turno viene saltato, ciò viene rappresentato da move=0 e orientation=1.

********************************************************************************/
typedef union{
	uint32_t word32;
	struct{
		uint32_t playerID:8;       // 0/1 to identify the player
		uint32_t move: 4;          // 0: moving player, 1: placing wall
		uint32_t orientation: 4;   // 0: vertical, 1: horizontal  -> Default to 0 if moving the player.
		uint32_t y:8;
		uint32_t x:8;
	}bits;
}Move;



/* Private variables ---------------------------------------------------------*/
extern Box board[7][7];
extern Player player[2];
extern Direction direction; //used by joystick to set valid move, updated after move
extern uint8_t activePlayer;

/* Private function prototypes -----------------------------------------------*/	

void drawBoard(void);
void drawSquare(uint16_t squareX, uint16_t squareY, uint16_t color);
void drawCircle(uint16_t x, uint16_t y, uint16_t color);
void drawRectangleWithText(uint16_t rectNumber, const char* text, uint8_t lineNumber, uint16_t color);
void drawMessageBox(const char* text, uint16_t color);
void drawWall(int startX, int startY, int orientation,  uint16_t color);

void updatePlayerState(uint8_t id, uint16_t newX, uint16_t newY, bool wall);
void highlightPlayerMoves(uint8_t id, uint16_t color);

void initQuoridor(void);
void startGame(void);
void stopGame(void);

void nextTurn(void);
void deleteLastMove(void);
void executeMove(Move m, bool confirm);
void checkWinner(void);
bool checkWallPosition(uint8_t x, uint8_t y, uint8_t orientation);
bool checkTrap(uint8_t x, uint8_t y, uint8_t orientation);
bool search(uint8_t x, uint8_t y, bool visited[7][7]);

#endif /* end __QUORIDOR_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
