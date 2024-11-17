#include <awaleBoard.h>
#include <string.h>
void initAwaleBoard(AwaleBoard* awaleBoard){
    awaleBoard->board = (int*)malloc(12*sizeof(int));
    for(int i = 0; i < 12; i++){
        awaleBoard->board[i] = 4;
    }
}

void freeAwaleBoard(AwaleBoard* awaleBoard) {
    free(awaleBoard->board);
}

void awaleBoard_sprint(char* buffer, void* data) {
    AwaleBoard* awaleBoard = (AwaleBoard*)data;
    strcat(buffer, "+--1-+--2-+--3-+--4-+--5-+--6-+\n");
    strcat(buffer, "-------------------------------\n");
    char temp[100];
    for (int i = 0; i < 6; i++) {
        sprintf(temp, "| %2d ", awaleBoard->board[i]);
        strcat(buffer, temp);
    }
    strcat(buffer, "|\n");
    for (int i = 11; i > 5 ; i--) {
        sprintf(temp, "| %2d ", awaleBoard->board[i]);
        strcat(buffer, temp);
    }
    strcat(buffer, "|\n");
    strcat(buffer, "-------------------------------\n");
    strcat(buffer, "+--6-+--5-+--4-+--3-+--2-+--1-+\n");
}
