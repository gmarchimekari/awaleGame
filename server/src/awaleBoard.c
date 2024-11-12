#include <awaleBoard.h>

void initAwaleBoard(AwaleBoard* awaleBoard){
    awaleBoard->board = (int*)malloc(12*sizeof(int));
    for(int i = 0; i < 12; i++){
        awaleBoard->board[i] = 4;
    }
}

void displayAwaleBoard(const AwaleBoard* awaleBoard){
    printf("+----+----+----+----+----+----+\n");
    for (int i = 0; i < 12; i++) {
        printf("| %2d ", awaleBoard->board[i]);
        if (i == 5 || i == 11) {
            printf("|\n");
        }
    }
    printf("+----+----+----+----+----+----+\n");
}

void freeAwaleBoard(AwaleBoard* awaleBoard) {
    free(awaleBoard->board);
}

