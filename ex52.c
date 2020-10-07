#include <unistd.h>
#include <stdio.h>
#include <bits/signum.h>
#include <signal.h>
#include "stdlib.h"

#define HEIGHT      20
#define WIDTH       20
#define TRUE        1
#define FALSE       0
#define HORIZONTAL  0
#define VERTICAL    1


typedef struct Point {
    int x;
    int y;
} Point;


typedef struct TetrisBoard {
    char    board[HEIGHT][WIDTH];
    Point   brickStart;
    int     brickIsVertical;
} TetrisBoard;


TetrisBoard* tetrisBoard;


/****************************************************
 * Function Name:       addBrick.
 * The Input:           TetrisBoard* b: Tetris board.
 *                      int x:          brick's starting point x value.
 *                      int y:          brick's starting point y value.
 *                      int isVertical: 0 - if the brick is horizontal, 1 - if the brick is vertical.
 * The Output:          NULL.
 * Function Operation:  The function adds .
 ****************************************************/
void addBrick(TetrisBoard* b, int x, int y, int isVertical) {
    if(y < 0) {
        /*
         * If the brick value is smaller than 0 (can happen only if the brick is being flipped at it's first apparence)
         * make it zero
         */
        y = 0;
    }
    b->brickStart.y = y;
    b->brickStart.x = x;
    if(isVertical) {
        b->brickIsVertical = TRUE;
        b->board[y][x]      = '-';
        b->board[y + 1][x]  = '-';
        b->board[y + 2][x]  = '-';

    } else {
        b->brickIsVertical = FALSE;
        b->board[y][x]      = '-';
        b->board[y][x + 1]  = '-';
        b->board[y][x + 2]  = '-';
    }
}


/****************************************************
 * Function Name:       initializeBoard.
 * The Input:           TetrisBoard* b: Tetris board.
 * The Output:          NULL.
 * Function Operation:  The function intalize all the cells in the Tetris board. it adds a space char to all cells,
 *                      the borders and a new brick at top.
 ****************************************************/
void initializeBoard(TetrisBoard* b) {
    int i, j;
    // Clear board.
    for(i = 0; i < HEIGHT; i++) {
        for(j = 0; j < WIDTH; j++) {
            b->board[i][j] = ' ';
        }
    }
    // Initialize board borders.
    // Set sides border.
    for(i = 0; i < HEIGHT - 1; i++) {
        b->board[i][0]          = '*';
        b->board[i][WIDTH - 1]  = '*';
    }
    // Set bottom border.
    for(i = 0; i < WIDTH; i++) {
        b->board[HEIGHT - 1][i] = '*';
    }
    // Add the first brick.
    addBrick(b, 9, 0, 0);
}


/****************************************************
 * Function Name:       removeBrick.
 * The Input:           TetrisBoard* b: Tetris board.
 * The Output:          NULL.
 * Function Operation:  The function remove the brick from the Tetris board.
 ****************************************************/
void removeBrick(TetrisBoard* b) {
    // Always remove the first pixel from the brick.
    b->board[b->brickStart.y][b->brickStart.x] = ' ';
    if(b->brickIsVertical) {
        // Brick is vertical.
        b->board[b->brickStart.y + 1][b->brickStart.x] = ' ';
        b->board[b->brickStart.y + 2][b->brickStart.x] = ' ';
    } else {
        // Brick is horizontal.
        b->board[b->brickStart.y][b->brickStart.x + 1] = ' ';
        b->board[b->brickStart.y][b->brickStart.x + 2] = ' ';
    }
}


/****************************************************
 * Function Name:       showBoard.
 * The Input:           TetrisBoard* b: Tetris board.
 * The Output:          NULL.
 * Function Operation:  The function prints the Tetris current state to screen.
 ****************************************************/
void showBoard(TetrisBoard* b) {
    int i, j;
    // Clear the screen.
    system("clear");
    // Print to screen all pixels of the board.
    for(i = 0; i < WIDTH; i++) {
        for(j = 0; j < WIDTH; j++) {
            write(STDOUT_FILENO, &b->board[i][j], 1);
        }
        write(STDOUT_FILENO, "\n", 1);
    }
}


/****************************************************
 * Function Name:       moveBrick.
 * The Input:           TetrisBoard* b: Tetris board.
 *                      char c:         The char that was pressed.
 * The Output:          NULL.
 * Function Operation:  The function moves the brick on the tetris board in the deisred direction.
 ****************************************************/
void moveBrick(TetrisBoard* b, char c) {
    // Save the old values of the current Tetris board.
    int oldX    = b->brickStart.x;
    int oldY    = b->brickStart.y;
    int oldVer  = b->brickIsVertical;
    // Remove old brick from screen.
    removeBrick(b);
    // Flip brick.
    if(c == 'w' || c == 'W') {
        if(b->brickIsVertical) {
            if(b->brickStart.x == 1) {
                addBrick(b, oldX, oldY + 1, HORIZONTAL);
            } else if (b->brickStart.x == WIDTH - 2) {
                addBrick(b, oldX - 2, oldY + 1, HORIZONTAL);
            } else {
                addBrick(b, oldX -1, oldY + 1, HORIZONTAL);
            }
        } else {
            // If horizontal.
            if(b->brickStart.y == HEIGHT - 2) {
                initializeBoard(b);
            } else {
                addBrick(b, oldX + 1, oldY - 1, VERTICAL);

            }
        }
    }
    // Move brick left.
    else if(c == 'a' || c == 'A') {
        if(b->brickStart.x == 1) {
            addBrick(b, oldX, oldY, oldVer);
        } else {
            addBrick(b, oldX - 1, oldY, oldVer);
        }
    }
    // Move brick right.
    else if(c == 'd' || c == 'D') {
        if(b->brickIsVertical) {
            if(b->brickStart.x == WIDTH - 2) {
                addBrick(b, oldX, oldY, oldVer);
            } else {
                addBrick(b, oldX + 1, oldY, oldVer);
            }
        } else {
            // If brick is horizontal.
            if(b->brickStart.x == WIDTH - 4) {
                addBrick(b, oldX, oldY, oldVer);
            } else {
                addBrick(b, oldX + 1, oldY, oldVer);
            }
        }
    }
    // Move brick down.
    if(c == 's' || c == 'S') {
        if(b->brickIsVertical) {
            if(b->brickStart.y >= HEIGHT - 4) {
                initializeBoard(b);
            } else {
                addBrick(b, oldX, oldY + 1, VERTICAL);
            }
        } else {
            if(b->brickStart.y == HEIGHT - 2) {
                initializeBoard(b);
            } else {
                addBrick(b, oldX, oldY + 1, HORIZONTAL);
            }
        }
    }
}


/****************************************************
 * Function Name:       getPressedKey.
 * The Input:           NULL.
 * The Output:          NULL.
 * Function Operation:  The function gets the key that was pressed and stop if the quit char was pressed.
 ****************************************************/
void getPressedKey() {
    signal(SIGUSR2, getPressedKey);
    char c = (char) getchar();
    // Stop if 'q' or 'Q' key was pressed.
    if(c == 'q' || c == 'Q') {
        exit(1);
    }
    // Move the brick in the direction of the key that was pressed.
    moveBrick(tetrisBoard, c);
    // Show the Tetris board on screen after the brick was moved.
    showBoard(tetrisBoard);
}


/****************************************************
 * Function Name:       brickFall.
 * The Input:           NULL.
 * The Output:          NULL.
 * Function Operation:  The function automatically makes the brick fall after each second.
 ****************************************************/
void brickFall() {
    signal(SIGALRM, brickFall);
    alarm(1);
    moveBrick(tetrisBoard, 's');
    showBoard(tetrisBoard);
}


/****************************************************
 * Function Name:       startGame.
 * The Input:           TetrisBoard* b: Tetris board.
 * The Output:          NULL.
 * Function Operation:  The function initallizes the board and starts the game.
 ****************************************************/
void startGame(TetrisBoard* b) {
    initializeBoard(b);
    showBoard(b);
    signal(SIGALRM, brickFall);
    alarm(1);
    signal(SIGUSR2, getPressedKey);
    while (TRUE) {
        pause();
    }
}


int main() {
    // Create a new Tetris game.
    tetrisBoard = (TetrisBoard*) malloc(sizeof(TetrisBoard));
    // Cheack if managed to allocate memory for the Tetris board.
    if(tetrisBoard == NULL) {
        char errorMessage[] = {"Failed to execute system call\n."};
        write(2, errorMessage, sizeof(errorMessage));
        exit(1);
    }
    startGame(tetrisBoard);
    free(tetrisBoard);
    return 0;
}