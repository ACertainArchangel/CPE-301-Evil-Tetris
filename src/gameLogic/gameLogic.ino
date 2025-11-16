/// Global Vars
int gameBoard[25][10]; // standard 20x10 tetris board, extra 5 for holding pieces above board
int tickRate = 1 * 1000; // second to mS conversion, each tick to move a piece down 1 cell
// Each tetronimo will have a designated origin block to track on the board and for rotation
int originX = 0;
int originY = 0;
int nextPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = square, 5 = S, 6 = T, 7 = Z, to be spawned flat side facing down
// Temp block char though I dont think the new LCD will use this
byte blockChar[8] = {
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(9600);
}

void loop() {

}

void checkTetris(int* board[]) {
  // if the sum of the row is 10 we can assume the line is complete
  for(int row = 0; row < 25; row++) {
    int sum = 0;
    for(int col 0; col < 10; col++) {
      sum += board[row][col];
    }
    if (sum == 10) {
      // TETRIS
    }
  }
}

void placePiece(int* board[], int* piece) {
  int piece = 0; // Random value to pick next piece

}

void rotatePiece() {

}

void checkFail(int* board[]) {

}

void checkCollision(int* board[]) {

}