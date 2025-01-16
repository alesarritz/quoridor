/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           quoridor.c
** Last modified Date:  2023-12-28
** Last Version:        V1.00
** Descriptions:        functions to handle the Quoridor game
** Correlated files:    quoridor.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "GLCD/GLCD.h"
#include "quoridor.h"
#include "timer/timer.h"
#include <string.h>

#define BOARD_SIZE 7 
#define SQUARE_SIZE 26 
#define SPACE_BETWEEN 6 
#define RECTANGLE_WIDTH 70
#define RECTANGLE_HEIGHT 50
#define MBOX_WIDTH 224
#define MBOX_HEIGHT 25

Player player[2];
Box board[7][7];
Wall wallsArray[16];
uint8_t nWalls=0;
Move lastMove;
uint8_t activePlayer;
volatile int gameOn=0;
volatile int next=0;
volatile int wallOk=0;
extern int timeQuoridor;

/******************************************************************************
** Function name:		drawBoard
**
** Descriptions:		Disegna la tavola da gioco e inizializza la matrice 
**                  board 7x7 di tipo Box  che la rappresenta.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void drawBoard(){
	uint16_t i, j, k,l;
	uint16_t startX = 10; 
	uint16_t startY = 10; 
	uint16_t x, y;

	for (i = 0; i < BOARD_SIZE; ++i) {
			for (j = 0; j < BOARD_SIZE; ++j) {
					//Init board matrix
					board[i][j].wallH = 0;
				  board[i][j].wallV = 0;
				  board[i][j].player = 0;
				
					x = startX + (i * (SQUARE_SIZE + SPACE_BETWEEN));
					y = startY + (j * (SQUARE_SIZE + SPACE_BETWEEN));

					// Draw squares
					for (k = 0; k < SQUARE_SIZE; ++k) {
							for (l = 0; l < SQUARE_SIZE; ++l) {
									LCD_SetPoint(x + k, y + l, Black); 
							}
					}
			}
	}
}

/******************************************************************************
** Function name:		drawSquare
**
** Descriptions:		Data la posizione [X(riga), Y(colonna)] nella tavola e il colore 
**                  disegna una casella della tavola. Utilizzata per evidenziare le 
**                  possibili mosse di un giocatore o per eliminare quelle passate.
**
** parameters:			coordinata x, coordinata y, colore
** Returned value:	None
**
******************************************************************************/
void drawSquare(uint16_t squareX, uint16_t squareY, uint16_t color){
    // Calculate the screen position based on square coordinates
    uint16_t x = 10 + (squareY * (SQUARE_SIZE + SPACE_BETWEEN));
		uint16_t y = 10 + (squareX * (SQUARE_SIZE + SPACE_BETWEEN));

		int16_t k,l;
		for (k = 0; k < SQUARE_SIZE; ++k) {
				for (l = 0; l < SQUARE_SIZE; ++l) {
						LCD_SetPoint(x + k, y + l, color); 
				}
		}
}

/******************************************************************************
** Function name:		drawCircle
**
** Descriptions:		Data la posizione [X(riga), Y(colonna)] nella tavola e il 
**                  colore, disegna un cerchio volto a rappresentare un giocatore.
**
** parameters:			coordinata x, coordinata y, colore
** Returned value:	None
**
******************************************************************************/
void drawCircle(uint16_t squareX, uint16_t squareY, uint16_t color) {
    // Calculate the screen position 
    uint16_t screenX = squareX * (SQUARE_SIZE + SPACE_BETWEEN) + 10;
    uint16_t screenY = squareY * (SQUARE_SIZE + SPACE_BETWEEN) + 10;

    // Calculate the center of the square
    uint16_t centerX = screenX + SQUARE_SIZE / 2;
    uint16_t centerY = screenY + SQUARE_SIZE / 2;

    // Calculate the radius of the circle 
    uint16_t radius = (SQUARE_SIZE - SPACE_BETWEEN) / 2 - 1;
		int16_t x,y;
    // Draw the circle 
    for (x = -radius; x <= radius; x++) {
        for (y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                LCD_SetPoint(centerY + y, centerX + x, color);
            }
        }
    } 
}

/******************************************************************************
** Function name:		drawRectangleWithText
**
** Descriptions:		Dato l’indice del rettangolo (0=sinistra,1=centro,2=destra), 
**                  il testo, il numero della riga (1^ o 2^) e il colore disegna le 
**                  sezioni con le informazioni relative a timer e muri dei giocatori.
**
** parameters:			codice rettangolo, testo, numero della riga, colore
** Returned value:	None
**
******************************************************************************/
void drawRectangleWithText(uint16_t rectNumber, const char *text, 
	                         uint8_t lineNumber, uint16_t color) {
    // Calculate the starting position 
    uint16_t startX = 10; 
    uint16_t startY = (BOARD_SIZE * (SQUARE_SIZE + SPACE_BETWEEN)) + 40; 
		
    // Calculate positions for the specified rectangle
    uint16_t rectX = startX + (rectNumber * (RECTANGLE_WIDTH + SPACE_BETWEEN));
    uint16_t rectY = startY;
		uint16_t x,y;
    // Draw rectangle border
    for (x = rectX; x < rectX + RECTANGLE_WIDTH; x++) {
        LCD_SetPoint(x, rectY, Black); // Top border
        LCD_SetPoint(x, rectY + RECTANGLE_HEIGHT - 1, Black); // Bottom border
    }
    for (y = rectY; y < rectY + RECTANGLE_HEIGHT; y++) {
        LCD_SetPoint(rectX, y, Black); // Left border
        LCD_SetPoint(rectX + RECTANGLE_WIDTH - 1, y, Black); // Right border
    }

    // Put string inside the rectangle
		if(lineNumber) rectY+=20;
    GUI_Text(rectX + 5, rectY + 5, (uint8_t *)text, color, Sand);
}

/******************************************************************************
** Function name:		drawMessageBox
**
** Descriptions:		Dato il testo e il colore, disegna sotto la tavola da gioco un 
**                  riquadro con testo all’interno. Viene utilizzato per mostrare i warning 
**                  relativi alla terminazione dei muri disponibili e per dichiarare il 
**                  vincitore della partita. 
**
** parameters:			testo da stampare, colore
** Returned value:	None
**
******************************************************************************/
void drawMessageBox( const char *text, uint16_t color){
	// Calculate the starting position 
	uint16_t startX = 10; 
	uint16_t startY = (BOARD_SIZE * (SQUARE_SIZE + SPACE_BETWEEN))+10;

	uint16_t x,y;
	
	// Draw rectangle border
	for (x = startX; x < startX + MBOX_WIDTH; x++) {
			LCD_SetPoint(x, startY, color); // Top border
			LCD_SetPoint(x, startY + MBOX_HEIGHT - 1, color); // Bottom border
	}
	for (y = startY; y < startY + MBOX_HEIGHT; y++) {
			LCD_SetPoint(startX, y, color); // Left border
			LCD_SetPoint(startX + MBOX_WIDTH - 1, y, color); // Right border
	}

	// Put string inside the rectangle
	GUI_Text(startX + 5, startY + 5, (uint8_t *)text, color, Sand);
}

/******************************************************************************
** Function name:		drawWall
**
** Descriptions:		Data la posizione [X(riga), Y(colonna)] nella tavola, l’orientamento 
**                  (orizzontale o verticale) e il colore disegna un muro nella tavola da gioco. 
**                  Il muro a seconda del colore assume un significato differente:
**                  -	Blu: muro inserito 
**                  -	Ciano: muro in posizione accettabile ma non ancora inserito.
**                  -	Magenta: muro in posizione non accettabile per sovrapposizione con altri muri
**                  -	Rosso: muro in posizione non accettabile perché intrappola il giocatore avversario
**
** parameters:			coordinata x, coordinata y, orientamento, colore
** Returned value:	None
**
******************************************************************************/
void drawWall(int startX, int startY, int orientation, uint16_t color) {
    uint16_t x0, y0, x1, y1;
    x0 = (startY) * (SQUARE_SIZE + SPACE_BETWEEN) + 10;
    y0 = (startX) * (SQUARE_SIZE + SPACE_BETWEEN) + 10;
	
    if (orientation == 0) {
        // Vertical orientation
				x0-=4; x1 = x0;
        y1 = y0 + SQUARE_SIZE;  
				y0 = y0 - SQUARE_SIZE - 5;
			  LCD_DrawLine(x0,y0,x1,y1,color);
				x0++; x1++;
				LCD_DrawLine(x0,y0,x1,y1,color);
				if (color == Blue){
					board[startX][startY].wallV =1;
					board[startX-1][startY].wallV =1;
				}

    } else if (orientation == 1) {
        // Horizontal orientation
        x1 = x0 + SQUARE_SIZE; 
			  x0 = x0 - SQUARE_SIZE -5; 
				y0-=4; y1 = y0;
				LCD_DrawLine(x0,y0,x1,y1,color);
				y0++; y1++;
				LCD_DrawLine(x0,y0,x1,y1,color);
				if (color == Blue){
					board[startX][startY].wallH = 1;
					board[startX][startY-1].wallH = 1;
				}	
    } 
}

/******************************************************************************
** Function name:		updatePlayerState
**
** Descriptions:		Dato l’id del giocatore, la sua nuova posizione e l’inserimento (o meno) 
**									di un muro aggiorna la matrice board col nuovo stato del giocatore e aggiorna 
**                  i seguenti dati relativi a quest’ultimo:
**									-	posizione
**									-	numero muri restanti
**									-	direzioni percorribili dato il presente stato del gioco 
**
** parameters:			id del giocatore, nuova coordinata X, nuova coordinata Y, inserimento muro (vero/falso)
** Returned value:	None
**
******************************************************************************/
void updatePlayerState(uint8_t id, uint16_t newX, uint16_t newY, bool wall){
	char walls[9] = "";
	//Remove old player[id] position from board
	board[player[id].x][player[id].y].player=0;
	
	//Update player[id] position and wall number
	player[id].x = newX; player[id].y = newY;
	if(wall) {
		player[id].walls--;
		sprintf(walls,"Walls: %d",player[id].walls);
		drawRectangleWithText(id*2, walls,1, Black);
	} 
	
	//Update player[id] position on board
	board[player[id].x][player[id].y].player=1;
	
	//Update acceptable directions
	player[id].direction.word8 = 0;
	
	if(newY>0 && board[newX][newY].wallV==0 && board[newX][newY-1].player==0) player[id].direction.bits.left=1;
	else if(newY>1 && board[newX][newY-1].wallV==0 && board[newX][newY-1].player==1) player[id].direction.bits.left=2;
		
	if(newY<6 && board[newX][newY+1].wallV==0 && board[newX][newY+1].player==0) player[id].direction.bits.right=1;
	else if(newY<5 && board[newX][newY+2].wallV==0 && board[newX][newY+1].player==1) player[id].direction.bits.right=2;
	
	if(newX>0 && board[newX][newY].wallH==0 && board[newX-1][newY].player==0) player[id].direction.bits.up=1;
	else if(newX>1 && board[newX-1][newY].wallH==0 && board[newX-1][newY].player==1) player[id].direction.bits.up=2;
		
	if(newX<6 && board[newX+1][newY].wallH==0 && board[newX+1][newY].player==0) player[id].direction.bits.down  = 1;
	else if(newX<5 && board[newX+2][newY].wallH==0 && board[newX+1][newY].player==1) player[id].direction.bits.down  = 2;
}

/******************************************************************************
** Function name:		highlightPlayerMoves
**
** Descriptions:		Dato l’id del giocatore e il colore da utilizzare, colora le 
**									caselle relative alle possibili mosse del giocatore. Utilizza le 
**									direzioni calcolate dalla funzione updatePlayerState e colore le caselle di:
**									-	GRIGIO: per evidenziare le possibili mosse
**									-	NERO: per cancellare le possibili mosse a causa del passaggio al turno seguente 
**													o del piazzamento di un muro
**
** parameters:			id del giocatore, colore
** Returned value:	None
**
******************************************************************************/
void highlightPlayerMoves(uint8_t id, uint16_t color){
		if(player[id].direction.bits.left==1) drawSquare(player[id].x, player[id].y-1,color);
		if(player[id].direction.bits.left==2) drawSquare(player[id].x, player[id].y-2,color);
	
		if(player[id].direction.bits.right==1) drawSquare(player[id].x, player[id].y+1,color);
		if(player[id].direction.bits.right==2) drawSquare(player[id].x, player[id].y+2,color);
	
		if(player[id].direction.bits.up==1) drawSquare(player[id].x-1, player[id].y,color);
		if(player[id].direction.bits.up==2) drawSquare(player[id].x-2, player[id].y,color);
	
		if(player[id].direction.bits.down==1) drawSquare(player[id].x+1, player[id].y,color);
		if(player[id].direction.bits.down==2) drawSquare(player[id].x+2, player[id].y,color);
}

/******************************************************************************
** Function name:		initQuoridor
**
** Descriptions:		Inizializza il gioco, disegna la tavola, i giocatori e le 
**									sezioni relative a timer e muri dei giocatori.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void initQuoridor(void){
	Player p;
	//Draw board 
	drawBoard();
	
	//Init player 1
	drawCircle(6, 3, White);
	p.id=0; p.color=White; p.x=6; p.y=3; p.walls=8; 
	p.direction.word8=0; p.finalX=0;
	player[0] = p;
	board[6][3].player = 1;
	
	//Init player 2
	drawCircle(0, 3,Red);
	p.id=1;	p.color = Red; p.x=0; p.y=3; p.finalX=6;
	player[1]=p;
	board[0][3].player = 1;

	//Draw game data
	drawRectangleWithText(0, "Player 1",0, White);
	drawRectangleWithText(1, " Timer",0, Black);
	drawRectangleWithText(2, "Player 2",0, Red);
	
	drawRectangleWithText(0, "Walls: 8",1, Black);
	drawRectangleWithText(1, "  20s",1, Black);
	drawRectangleWithText(2, "Walls: 8",1, Black);
	
	//Init last Move
	lastMove.word32 = 0;
	lastMove.bits.playerID = 255;
}

/******************************************************************************
** Function name:		startGame
**
** Descriptions:		Inizia il gioco. Calcola (updatePlayerState) ed evidenzia (highlightPlayerMoves) 
**									le possibili mosse del primo giocatore e avvia il timer di 20 secondi per il primo turno.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void startGame(){
	//If starting new game (not the first since power on)
	if(gameOn==-1){
		LCD_Clear(Sand);
		initQuoridor();
	}
	
	updatePlayerState(0, player[0].x, player[0].y, false);
	updatePlayerState(1, player[1].x, player[1].y, false);
	activePlayer=0;
	
	//Highlight player 0 moves
	highlightPlayerMoves(0, DarkGrey);
	//Reset/Start timer
	timeQuoridor=21;
	reset_timer(0);
	enable_timer(0);
	
	gameOn=1;
	next=1;
	wallOk=0;
}

/******************************************************************************
** Function name:		stopGame
**
** Descriptions:		Termina il gioco e disabilita il timer.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void stopGame(){
	disable_timer(0);
	gameOn=-1;
	next=0;
}

/******************************************************************************
** Function name:		nextTurn
**
** Descriptions:		Aggiorna lo stato dei giocatori (updatePlayerState), seleziona 
**									come attivo il giocatore in attesa, ne evidenzia le possibili mosse 
**                  (highlightPlayerMoves) e riavvia il timer di 20 secondi.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void nextTurn(){
	activePlayer = !activePlayer;
	updatePlayerState(0, player[0].x, player[0].y, false);
	updatePlayerState(1, player[1].x, player[1].y, false);
	highlightPlayerMoves(activePlayer, DarkGrey);
	//Reset/Start timer
	timeQuoridor=21;
	reset_timer(0);
	enable_timer(0);
	next=1;
	wallOk=0;
}

/******************************************************************************
** Function name:		deleteLastMove
**
** Descriptions:		Elimina l’ultima mossa effettuata disegnando il giocatore o il 
**									muro nella precedente posizione. Operazione effettuata quando si 
**                  cambia posizione durante il turno, prima di confermare la mossa o 
**									prima del timeout.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void deleteLastMove(){
	int i;
	//Redraw last move's Token
	if(lastMove.bits.move==0){
		if(lastMove.bits.x==player[lastMove.bits.playerID].x && lastMove.bits.y==player[lastMove.bits.playerID].y){
			drawCircle(player[lastMove.bits.playerID].x, player[lastMove.bits.playerID].y, Black);
		}
		else{
		drawCircle(lastMove.bits.x, lastMove.bits.y, DarkGrey);
		drawCircle(player[lastMove.bits.playerID].x, player[lastMove.bits.playerID].y, player[lastMove.bits.playerID].color);}
	}
	else{
		drawWall(lastMove.bits.x, lastMove.bits.y, lastMove.bits.orientation, Sand);
		for(i=0; i<nWalls; i++){
				drawWall(wallsArray[i].x, wallsArray[i].y,wallsArray[i].orientation, Blue);
		}
	
	}
}

/******************************************************************************
** Function name:		executeMove
**
** Descriptions:		Riceve in input la mossa da effettuare e la conferma (o la 
**									temporaneità di quest’ultima). Gestisce i seguenti casi:
**									-	Timeout
**									-	Movimentazione giocatore
**									-	Piazzamento muro
**									Se la variabile confirm vale True la mossa è valida (i controlli 
**                  vengono eseguiti quando si gestisce l’interazione col pulsante SELECT 
**                  del joystick) e il giocatore ha confermato la sua scelta. La tavola da 
**                  gioco viene aggiornata, si verifica se la mossa abbia portato alla vittoria 
**                  (checkWinner) e in caso negativo si passa al turno seguente (nextTurn).
**
** parameters:			mossa, conferma
** Returned value:	None
**
******************************************************************************/
void executeMove(Move m, bool confirm){
	int i; bool trap;
	//Time expired
	if(m.bits.move==0 && m.bits.orientation==1){
		if(lastMove.bits.playerID!=255)deleteLastMove();
		drawCircle(player[lastMove.bits.playerID].x, 
		           player[lastMove.bits.playerID].y, 
		           player[lastMove.bits.playerID].color);
		
		//Remove possible moves highlight 
		highlightPlayerMoves(activePlayer, Black);
		
		lastMove.word32 = 0; 
		lastMove.bits.playerID = 255;
		nextTurn();
	}
	
	//Move Player
	else if(m.bits.move==0){
		if(confirm){
			highlightPlayerMoves(m.bits.playerID, Black);
			
			//Save new move
			updatePlayerState(m.bits.playerID, m.bits.x, m.bits.y, false);
			drawCircle(m.bits.x, m.bits.y, player[m.bits.playerID].color);
			lastMove.word32 = 0; 
			lastMove.bits.playerID = 255;
			checkWinner();
			if(gameOn==1) nextTurn();
		}else{
			if(lastMove.bits.playerID!=255) deleteLastMove();
			else drawCircle(player[m.bits.playerID].x, player[m.bits.playerID].y, Black);
			
			if(lastMove.bits.move==1) highlightPlayerMoves(m.bits.playerID, DarkGrey);
			drawCircle(m.bits.x, m.bits.y, player[m.bits.playerID].color);
			lastMove = m;
		}
	}
	
	//Place Wall
	else{
		if(confirm) {
			drawWall(m.bits.x, m.bits.y, m.bits.orientation, Blue); 
			highlightPlayerMoves(m.bits.playerID, Black);
			//Save new move
			updatePlayerState(m.bits.playerID, player[m.bits.playerID].x, player[m.bits.playerID].y, true);
			
			//Save Wall
			wallsArray[nWalls].x = m.bits.x;
			wallsArray[nWalls].y = m.bits.y;
			wallsArray[nWalls].orientation = m.bits.orientation;
			nWalls++;
			
			lastMove.word32 = 0; 
			lastMove.bits.playerID = 255;
			nextTurn();
		}
		else {
			if(lastMove.bits.playerID!=255 
			  && !(lastMove.bits.move==0 && (lastMove.bits.x==player[lastMove.bits.playerID].x 
			  && lastMove.bits.y==player[lastMove.bits.playerID].y))
			){
					deleteLastMove();
			}
			if(lastMove.bits.move==0) highlightPlayerMoves(m.bits.playerID, Black);
			
			//Redraw all walls 
			if(wallOk==0){
				for(i=0; i<nWalls; i++){
					drawWall(wallsArray[i].x, wallsArray[i].y,wallsArray[i].orientation, Blue);
				}
			}
			
			wallOk = checkWallPosition(m.bits.x, m.bits.y, m.bits.orientation) ? 1 : 0;
			if(wallOk==1) drawWall(m.bits.x, m.bits.y, m.bits.orientation, Cyan);
			else drawWall(m.bits.x, m.bits.y, m.bits.orientation, Magenta);
			
			trap = checkTrap(m.bits.x, m.bits.y, m.bits.orientation);
			if(!trap)drawWall(m.bits.x, m.bits.y, m.bits.orientation, Red);
			
			lastMove = m;
		}
	}
}

/******************************************************************************
** Function name:		checkWallPosition
**
** Descriptions:		Data la posizione [X(riga), Y(colonna)] nella tavola, l’orientamento 
**                  (orizzontale o verticale) verifica se la posizione del muro impostata 
**                  dal giocatore è accettabile (il muro non si sovrappone agli altri già 
**                  presenti) oppure no. Restituisce True se il posizionamento è accettabile.
**
** parameters:			coordinata x, coordinata y, orientamento
** Returned value:	None
**
******************************************************************************/
bool checkWallPosition(uint8_t x, uint8_t y, uint8_t orientation){
	//Check HORIZONTAL
 	if(orientation==1 && (board[x][y].wallH==1 || board[x][y-1].wallH==1 || (board[x-1][y].wallV==1 && board[x][y].wallV==1)))
		return false;
	//Check VERTICAL
	if(orientation==0 && (board[x][y].wallV==1 || board[x-1][y].wallV==1 || (board[x][y-1].wallH==1 && board[x][y].wallH==1)))
		return false;
	return true;
}

/******************************************************************************
** Function name:		checkTrap
**
** Descriptions:		Data la posizione [X(riga), Y(colonna)] nella tavola, l’orientamento 
**                  (orizzontale o verticale) verifica se la posizione del muro impostata 
**                  dal giocatore intrappola il giocatore avversario impedendogli permanentemente 
**                  di raggiungere la vittoria. Inserisce temporaneamente il muro nella matrice board 
**                  e chiama la funzione search dandogli in input la posizione corrente del giocatore 
**                  avversario e una matrice booleana inizializzata a False. Restituisce True se è stato 
**                  trovato almeno un percorso che porta l’avversario alla vittoria.
**
** parameters:			coordinata x del muro, coordinata y del muro, orientamento
** Returned value:	Percorso trovato dalla posizione iniziale alla meta (vero/falso)
**
******************************************************************************/
bool checkTrap(uint8_t x, uint8_t y, uint8_t orientation) {
    bool visited[7][7] = {false};
		bool pathFound=false;
		
		//Temporarily insert the wall
		if (orientation == 0) {
				board[x][y].wallV = 1;
				board[x - 1][y].wallV = 1;
		} else {
				board[x][y].wallH = 1;
				board[x][y - 1].wallH = 1;
		}

    pathFound = search(player[!activePlayer].x, player[!activePlayer].y, visited);

		//Remove the temporary wall
		if (orientation == 0) {
				board[x][y].wallV = 0;
				board[x - 1][y].wallV = 0;
		} else {
				board[x][y].wallH = 0;
  				board[x][y - 1].wallH = 0;
		}

    return pathFound;
}

/******************************************************************************
** Function name:		search
**
** Descriptions:		Funzione ricorsiva che riceve in input la posizione visitata dal 
**                  giocatore avversario e una matrice booleana rappresentante le caselle 
**                  visitabili dal giocatore. Il valore di ritorno della funzione indica se 
**                  sia possibile per il giocatore avversario raggiungere la sua destinazione 
**									finale partendo dalle coordinate date.
**
** parameters:			coordinata x del giocatore, coordinata y del giocatore, matrice delle posizioni visitate
** Returned value:	Percorso trovato dalla posizione iniziale alla meta (vero/falso)
**
******************************************************************************/
bool search(uint8_t x, uint8_t y, bool visited[7][7]){
		bool res=false;
	  if (x == player[!activePlayer].finalX )return true;
    visited[x][y] = true;

    // Check in all possible directions
    if (x > 0 && board[x][y].wallH == 0 && !visited[x - 1][y])     {res |= search(x - 1, y, visited);}
		if(res) return true;
    if (x < 6 && board[x + 1][y].wallH == 0 && !visited[x + 1][y]) {res |= search(x + 1, y, visited);}
		if(res) return true;
    if (y > 0 && board[x][y].wallV == 0 && !visited[x][y - 1])     {res |= search(x, y - 1, visited);}
		if(res) return true;
    if (y < 6 && board[x][y + 1].wallV == 0 && !visited[x][y + 1]) {res |= search(x, y + 1, visited);}
		
		return res;
}

/******************************************************************************
** Function name:		checkWinner
**
** Descriptions:		Verifica il raggiungimento della destinazione finale del giocatore 
**                  che ha appena effettuato una mossa. Il caso di esito positivo stampa 
**									l’avviso di vittoria (drawMessageBox) e termina il gioco.
**
** parameters:			None
** Returned value:	None
**
******************************************************************************/
void checkWinner(){
	char str[25] = "";
	if(player[activePlayer].x == player[activePlayer].finalX){
		sprintf(str,"     WINNER: Player %d   ", activePlayer+1);
		drawMessageBox(str, player[activePlayer].color);
		stopGame();
	}
}

/******************************************************************************
**                            End Of File
******************************************************************************/
