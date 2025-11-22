#include <Position.h>
#include <string.h>
/// TODO: Cleanup methods and positioning rework all loops to account to block ID's
/// Global Vars
int gameBoard[25][10]; // standard 20x10 tetris board, extra 5 for holding pieces above board
int BoundingBoxLarge[4][4]; // larger bounding box for line piece
int boundingBoxSmall[3][3];// smaller bounding box for other pieces
int tickRate = 1 * 1000; // second to mS conversion, each tick to move a piece down 1 cell
// Each tetronimo will have a designated origin block to track on the board and for rotation
int originX = 0;
int originY = 0;
int currentPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z
int nextPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z, to be spawned flat side facing down

// Position Trackers
int offSetRow = 0;
int offSetCol = 0;
int rotationState = 1;

bool fail = false;
/// Rotation States for each piece
/// line piece states (states 1-4)
Position stateLine1[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
Position stateLine2[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
Position stateLine3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
Position stateLine4[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};

/// J-Piece states (states 5-8)
Position stateJ1[4] = {Position(0,0), Position(1,0), Position(1,1), Position(1,2)};
Position stateJ2[4] = {Position(0,1), Position(0,2), Position(1,1), Position(2,1)};
Position stateJ3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(2,2)};
Position stateJ4[4] = {Position(0,1), Position(1,1), Position(2,0), Position(2,1)};

/// L-Piece states (states 9-12)
Position stateL1[4] = {Position(0,2), Position(1,0), Position(1,1), Position(1,2)};
Position stateL2[4] = {Position(0,1), Position(1,1), Position(2,1), Position(2,2)};
Position stateL3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(2,0)};
Position stateL4[4] = {Position(0,0), Position(0,1), Position(1,1), Position(2,1)};

/// Block piece state (state 13)
Position stateBlock1[4] = {Position(0,0), Position(0,1), Position(1,0), Position(1,1)};

/// S-Piece states (states 14-17)
Position stateS1[4] = {Position(0,1), Position(0,2), Position(1,0), Position(1,1)};
Position stateS2[4] = {Position(0,1), Position(1,1), Position(1,2), Position(2,2)};
Position stateS3[4] = {Position(1,1), Position(1,2), Position(2,0), Position(2,1)};
Position stateS4[4] = {Position(0,0), Position(1,0), Position(1,1), Position(2,1)};

/// T-Piece states (states 18-21)
Position stateT1[4] = {Position(0,1), Position(1,0), Position(1,1), Position(1,2)};
Position stateT2[4] = {Position(0,1), Position(1,1), Position(1,2), Position(2,1)};
Position stateT3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(2,1)};
Position stateT4[4] = {Position(0,1), Position(1,0), Position(1,1), Position(2,1)};

/// Z-Piece states (21-24)
Position stateZ1[4] = {Position(0,0), Position(0,1), Position(1,1), Position(1,2)};
Position stateZ2[4] = {Position(0,2), Position(1,1), Position(1,2), Position(2,1)};
Position stateZ3[4] = {Position(1,0), Position(1,1), Position(2,1), Position(2,2)};
Position stateZ4[4] = {Position(0,1), Position(1,0), Position(1,1), Position(2,0)};



void setup() {
  Serial.begin(9600);
  initBoard();
  initPiece();
}

void loop() {
  // Game loop
  while (!fail) {
    
    // Make new piece
    initPiece();
  }
  // Reset Game
  initPiece();
  initBoard();
}
/// TODO: fix for new system
void checkTetris() {
  // if the sum of the row is 10 we can assume the line is complete
  for(int row = 0; row < 25; row++) {
    int sum = 0;
    for(int col = 0; col < 10; col++) {
      sum += gameBoard[row][col];
    }
    if (sum == 10) {
      // TETRIS
    }
  }
  /// TODO: loop to clear lines?
}

void copyArry(int* source, int* destination, int len) {
  memcpy(destination, source, sizeof(src[0])*len);
}

void placePiece() {
  Position tempPos[4];
  int length = 4;
  switch (currentPiece) {
    case 1:
      if (rotationState == 1) {
        copyArray(stateLine1, tempPos, 4);
      }
      else if (rotationState == 2) {
        copyArray(stateLine2, tempPos, 4);
      }
      else if (rotationState == 3) {
        copyArray(stateLine3, tempPos, 4);
      }
      else {
        copyArray(stateLine4, tempPos, 4);
      }
      break;
    case 2:
      if (rotationState == 1) {
        copyArray(stateJ1, tempPos, 4);
      }
      else if (rotationState == 2) {
        copyArray(stateJ2, tempPos, 4);
      }
      else if (rotationState == 3) {
        copyArray(stateJ3, tempPos, 4);
      }
      else {
        copyArray(stateJ4, tempPos, 4);
      }
      break;
    case 3:
      if (rotationState == 1) {
        copyArray(stateL1, tempPos, 4);
      }
      else if (rotationState == 2) {
        copyArray(stateL2, tempPos, 4);
      }
      else if (rotationState == 3) {
        copyArray(stateL3, tempPos, 4);
      }
      else {
        copyArray(stateL4, tempPos, 4);
      }
      break;
    case 4:
      copyArray(stateBlock1, tempPos, length);
      break;
    case 5:
      if (rotationState == 1) {
        copyArray(stateS1, tempPos, 4);
      }
      else if (rotationState == 2) {
        copyArray(stateS2, tempPos, 4);
      }
      else if (rotationState == 3) {
        copyArray(stateS3, tempPos, 4);
      }
      else {
        copyArray(stateS4, tempPos, 4);
      }
      break;
    case 6:
      if (rotationState == 1) {
        copyArray(stateT1, tempPos, 4);
      }
      else if (rotationState == 2) {
        copyArray(stateT2, tempPos, 4);
      }
      else if (rotationState == 3) {
        copyArray(stateT3, tempPos, 4);
      }
      else {
        copyArray(stateT4, tempPos, 4);
      }
      break;
    case 7:
      if (rotationState == 1) {
        copyArray(stateZ1, tempPos, 4);
      }
      else if (rotationState == 2) {
        copyArray(stateZ2, tempPos, 4);
      }
      else if (rotationState == 3) {
        copyArray(stateZ3, tempPos, 4);
      }
      else {
        copyArray(stateZ4, tempPos, 4);
      }
      break;
  }

  // TODO: assign values to gameBoard
  for (int x = 0; x < 4; x++) {
    gameBoard[tempPos[x].getRow() + offSetRow][tempPos[x].getCol() + offSetCol] = currentPiece;
  }
}

void rotatePieceCW() {
  if (rotationState < 4) {
    rotationState++;
  }
  else {
    rotationState = 1;
  }
}

void rotatePieceCCW() {
  if (rotationState > 1) {
    rotationState--;
  }
  else {
    rotationState = 4;
  }
}

void movePiece(int row, int col) {
  /// TODO: Add bounds check here
  offSetRow += x;
  if (offSetCol + col <= 10) {
    offSetCol += y;
  }
}

bool checkEmpty(int col, int row) {
  bool empty = true;
  if (board[col][row] != 0) {
    empty = false;
  }
  return empty;
}

void checkFail() {
  int sum = 0;
  for (int x = 0; x < 10; x++) {
    if (gameBoard[0][x] >= 1) {
      sum++;
    }
  }
  if (sum == 10) {
    fail = true;
  }
}

void initBoard() {
  for(int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      gameBoard[x][y]= 0;
    }
  }
}

void initPiece() {
  originX = 0;
  originY = 0;
  offSetRow = 0;
  offSetCol = 0;
  rotationState = 1;
}

void checkCollision() {

}