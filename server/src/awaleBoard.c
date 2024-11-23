#include <awaleBoard.h>
#include <string.h>
void awaleBoard_init(AwaleBoard* awaleBoard){
    awaleBoard->board = (int*)malloc(12*sizeof(int));
    for(int i = 0; i < 12; i++){
        awaleBoard->board[i] = 4;
    }
}

void awaleBoard_free(AwaleBoard* awaleBoard) {
    free(awaleBoard->board);
}
