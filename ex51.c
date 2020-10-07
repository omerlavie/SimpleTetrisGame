#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <bits/signum.h>
#include <signal.h>

#define FD_SIZE         2
#define True            1
#define False           0
#define FD_READ_INDEX   0
#define FD_WRITE_INDEX  1
#define COMP_APP        "./draw.out"


/****************************************************
 * Function Name:       getch.
 * The Input:           NULL.
 * The Output:          NULL.
 * Function Operation:  The functions gets a pressed key without need to press enter afterward.
 ****************************************************/
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}


/****************************************************
 * Function Name:       isMovingChar.
 * The Input:           char c: pressed key.
 * The Output:          NULL.
 * Function Operation:  The functions check if the pressed key is in the array of moving chars.
 ****************************************************/
int isMovingChar(char c) {
    short int index;
    char movingChars[] = {'a', 'A', 's', 'S', 'd', 'D', 'w', 'W'};
    for(index = 0; index < sizeof(movingChars); index++) {
        if(c == movingChars[index]) {
            return True;
        }
    }
    return False;
}


/****************************************************
 * Function Name:       isQuitChar.
 * The Input:           char c: pressed key.
 * The Output:          NULL.
 * Function Operation:  The functions check if the pressed key is in the array of quitting chars.
 ****************************************************/
int isQuitChar(char c) {
    short int index;
    char quittingChars[] = {'q', 'Q'};
    for(index = 0; index < sizeof(quittingChars); index++) {
        if(c == quittingChars[index]) {
            return True;
        }
    }
    return False;
}


/****************************************************
 * Function Name:       writeError.
 * The Input:           NULL.
 * The Output:          NULL.
 * Function Operation:  Write error message to STDERR.
 ****************************************************/
void writeError() {
    char errorMessage[] = {"Failed to execute system call\n."};
    write(STDERR_FILENO, errorMessage, sizeof(errorMessage));
    exit(1);
}


/****************************************************
 * Function Name:       main.
 * The Input:           NULL.
 * The Output:          NULL.
 * Function Operation:  The program gets a char and send it singel.
 ****************************************************/
int main() {
    int     pipefd[FD_SIZE];
    pid_t   pid;
    if(pipe(pipefd) < 0) {
        writeError();
    }
    pid = fork();
    if(pid == -1) {
        // Failed to fork.
        writeError();
    }
    if(pid == 0) {
        // Child process.
        // Close unused read end.
        close(pipefd[FD_WRITE_INDEX]);
        if(dup2(pipefd[FD_READ_INDEX], STDIN_FILENO) == -1){
            writeError();

        }
        char* null = {NULL};
        execvp(COMP_APP, &null);
        exit(EXIT_FAILURE);
    } else {
        // Father process.
        char c;
        // Close unused write end.
        close(pipefd[FD_READ_INDEX]);
        c = getch();
        while(True) {
            if(isMovingChar(c) || isQuitChar(c)) {
                // Father process writes moving character c to pipe.
                if(write(pipefd[FD_WRITE_INDEX], &c, 1) == -1) {
                    writeError();

                }
                if(kill(pid, SIGUSR2) == -1) {
                    writeError();
                }
                if(isQuitChar(c)) {
                    // Exit if quit char was pressed.
                    return 0;
                }
            }
            if(isQuitChar(c)) {
                break;
            }
            // Get the next char.
            c = getch();
        }
        if(close(pipefd[FD_WRITE_INDEX] == -1)){
            writeError();
        }
    }
    return 0;
}