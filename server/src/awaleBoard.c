#include <awaleBoard.h>

void initAwaleBoard(AwaleBoard* awaleBoard){
    awaleBoard->board = (int**)malloc(2*sizeof(int*));
    for(int i = 0; i < 2; i++){
        awaleBoard->board[i] = (int*)malloc(6*sizeof(int));
        for(int j = 0; j < 6; j++)
            awaleBoard->board[i][j] = 4;
    }
}

void displayAwaleBoard(const AwaleBoard* awaleBoard){
    printf("+----+----+----+----+----+----+\n");
    for (int i = 0; i < 2; i++) {
        printf("|");  
        for (int j = 0; j < 6; j++) {
            printf(" %2d |", awaleBoard->board[i][j]);  
        }
        printf("\n");
    }
    printf("+----+----+----+----+----+----+\n");
}

void freeAwaleBoard(AwaleBoard* awaleBoard) {
    for(int i = 0; i < 2; i++){
        free(awaleBoard->board[i]);
    }
    free(awaleBoard->board);
}

