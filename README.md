## Quoridor Game

This document briefly illustrates the main functions and data structures used for developing the Quoridor board game on the LandTiger board.

**NOTE:** The Quoridor board game has been developed during my first year of MSc in Computer Engineering at **Politecnico di Torino**, as part of the course of _Architetture dei Sistemi di Elaborazione_.

---

### FUNCTIONS

#### **sample.c**

**`int main(void)`**

Initializes the system, buttons, joystick, RIT (which is also enabled), timer, and LCD screen. Then it initializes the graphics for the Quoridor game and waits for the start command.

#### **quoridor.c**

**`void drawBoard(void)`**

Draws the game board and initializes the 7x7 board matrix of type `Box` that represents it.

**`void drawCircle(uint16_t squareX, uint16_t squareY, uint16_t color)`**

Given the position `[X(row), Y(column)]` on the board and the color, draws a circle representing a player.

**`void drawSquare(uint16_t squareX, uint16_t squareY, uint16_t color)`**

Given the position `[X(row), Y(column)]` on the board and the color, draws a square. This is used to highlight possible moves for a player or to remove past moves.

**`void drawRectangleWithText(uint16_t rectNumber, const char *text, uint8_t lineNumber, uint16_t color)`**

Given the rectangle index (0=left, 1=center, 2=right), text, line number (1st or 2nd), and color, draws the sections with information about the timer and players' walls.

**`void drawMessageBox(const char *text, uint16_t color)`**

Given the text and color, draws a message box below the game board containing the specified text. It is used to show warnings about wall usage or declare the game winner.

**`void drawWall(int startX, int startY, int orientation, uint16_t color)`**

Draws a wall at the position `[X(row), Y(column)]` on the board with the specified orientation (horizontal or vertical) and color. The wall’s color indicates its state:
- **Blue**: Placed wall.
- **Cyan**: Acceptable position but not yet placed.
- **Magenta**: Unacceptable position due to overlap.
- **Red**: Unacceptable position because it traps the opponent.

**`void updatePlayerState(uint8_t id, uint16_t newX, uint16_t newY, bool wall)`**

Updates the game state for the specified player ID with their new position and wall usage (if applicable). It updates the board matrix and:
- Player position.
- Remaining walls.
- Possible directions for the player based on the current game state.

**`void highlightPlayerMoves(uint8_t id, uint16_t color)`**

Colors the squares of the possible moves for the specified player using the calculated directions. The colors are:
- **Gray**: Highlights possible moves.
- **Black**: Removes highlighted moves due to turn change or wall placement.

**`void initQuoridor(void)`**

Initializes the game, draws the board, and sets up players along with their timer and wall sections.

**`void startGame(void)`**

Starts the game. Calculates (`updatePlayerState`) and highlights (`highlightPlayerMoves`) the possible moves for the first player, and starts the 20-second timer for their turn.

**`void stopGame(void)`**

Ends the game and disables the timer.

**`void nextTurn(void)`**

Updates the players' states (`updatePlayerState`), switches the active player, highlights their possible moves (`highlightPlayerMoves`), and restarts the 20-second timer.

**`void deleteLastMove(void)`**

Removes the last move made by drawing the player or wall in its previous position. This is done during a turn change or before confirming a move.

**`void executeMove(Move m, bool confirm)`**

Executes a move and confirms it if `confirm` is `True`. Handles:
- Timeout.
- Player movement.
- Wall placement.

If the move is valid, it updates the game board, checks for victory (`checkWinner`), and moves to the next turn if no victory is detected.

---

### STRUCT & UNION

#### **`typedef union Direction`**
```c
typedef union {
    uint8_t word8;
    struct {
        uint8_t up : 2;
        uint8_t down : 2;
        uint8_t left : 2;
        uint8_t right : 2;
    } bits;
} Direction;
```
Represents the possible moves for a token:
- **0**: Blocked.
- **1**: Valid move.
- **2**: Valid move involving a jump over another token.

#### **`typedef struct Player`**
```c
typedef struct {
    uint8_t id;
    uint16_t color;
    uint8_t x;
    uint8_t y;
    uint8_t finalX;
    uint8_t walls;
    Direction direction;
} Player;
```
Describes a player with:
- Unique ID.
- Color.
- Position (x, y).
- Final destination.
- Remaining walls.
- Possible directions for movement.

#### **`typedef struct Wall`**
```c
typedef struct {
    uint8_t orientation;
    uint8_t y;
    uint8_t x;
} Wall;
```
Represents a wall on the board with:
- Orientation (horizontal or vertical).
- Position (x, y).

---

### GAMEPLAY CONTROLS

#### **Joystick**
- **UP/DOWN/LEFT/RIGHT**: Moves the token or wall.
- **SELECT**: Confirms the current move or wall placement.

#### **KEY1**
- Initiates wall placement mode.
- Cancels wall placement mode when pressed again.

#### **KEY2**
- Rotates a wall by 90 degrees during wall placement mode.

#### **INT0**
- Starts the game.

---

### GAMEPLAY FLOW
1. Initialize the board and players.
2. Start the game with `INT0`.
3. Players alternate turns to:
   - Move tokens.
   - Place walls.
4. Each turn is limited to 20 seconds.
5. The game ends when a player reaches the opponent's side.
