#include <awaleBoard.h>

void initAwaleBoard(AwaleBoard* awaleBoard){
    awaleBoard->board = (int*)malloc(12*sizeof(int));
    for(int i = 0; i < 12; i++){
        awaleBoard->board[i] = 1;
    }
}


void freeAwaleBoard(AwaleBoard* awaleBoard) {
    free(awaleBoard->board);
}

