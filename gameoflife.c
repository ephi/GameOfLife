#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef short(*init_cb)();//callback for dead or alive status either from random or from player
typedef struct {
	int rows;
	int cols;
	short* board;
	short hasAllocatedboard;
} gameboard;
void printGameboard(gameboard);
short getDeadOrAliveFromPlayer();//in the initialization stage this can be used as callback in order to get dead or alive from player
short randomizeDeadOrAlive();//in the initialization stage this can be used as callback in order to randomize dead or alive state
void fillBoard(gameboard, init_cb);//fills the board in the initialization stage with random or player selection
gameboard initBoard();
gameboard cpyBoard(gameboard);
short cmprBoards(gameboard, gameboard);
void allocateBoard(gameboard*);
short* getRowFromBoard(int, gameboard);
short* getItemFromBoard(int, int, gameboard);
gameboard gameStage(gameboard);
void playGame();
void freeBoard(gameboard);
short areIndexsOfMatrixInBoundries(int i, int j, gameboard b);
void clearBoardFromScreen(gameboard b);


void main(){
	short restart;
	restart = 0;
	do
	{
		playGame();
		printf("Start a new game?\n1 -- Yes\n0 -- No\n");
		scanf("%hd", &restart);
	} while (restart == 1);

}
void playGame(){
	gameboard b, newBoard;
	short continueGame;
	b = initBoard();
	if (b.hasAllocatedboard == 0){
		printf("sorry not enough memory to run the game.");
		return;
	}
	printf("starting game...\nThe starting board is:\n");
	printGameboard(b);
	do{

		newBoard = gameStage(b);
		printf("The new board is:\n");
		printGameboard(newBoard);
		continueGame = cmprBoards(b, newBoard) == 1 ? 0 : 1;
		freeBoard(b);
		if (continueGame){
			printf("continue game?\n1 --- Yes\n0 --- No\n");
			scanf("%hd", &continueGame);
		}
		else
			printf("The board has not changed, Game Over\n");
		b = newBoard;
	} while (continueGame == 1);
	freeBoard(b);
	printf("Thanks for playing\n");
}

short getDeadOrAliveFromPlayer(){
	short isAlive;
	printf("please choose dead or alive\n1 --- alive\n0 --- dead\n");
	scanf("%hd", &isAlive);
	return isAlive;
}
short randomizeDeadOrAlive(){
	short isAlive = (short)(rand() % 2);
	printf("the game has selected by random ");
	isAlive == 1 ? printf("alive state") : printf("dead state");
	printf("\n");
	return isAlive;
}
void fillBoard(gameboard b, init_cb callback){
	int i,j;
	short* boardElement;
	for (i = 0; i < b.rows; i++)
	{
		for (j = 0; j < b.cols; j++)
		{
			boardElement = getItemFromBoard(i, j, b);
			printf("setting board at row: %d, col: %d\n", (i + 1), (j + 1));
			*boardElement = callback();
		}
	}

}
short cmprBoards(gameboard b1, gameboard b2){
	int i,j;
	if (b1.cols != b2.cols || b1.rows != b2.rows)
		return (short)0;
	for (i = 0; i < b1.rows; i++)
	{
		for (j = 0; j < b1.cols; j++)
		{
			if (*getItemFromBoard(i, j, b1) != *getItemFromBoard(i, j, b2))
				return (short)0;
		}
	}
	return (short)1;
}
gameboard cpyBoard(gameboard b){
	gameboard clonedBoard;
	int i, j;
	short* boardElement;
	clonedBoard.cols = b.cols;
	clonedBoard.rows = b.rows;
	clonedBoard.hasAllocatedboard = 0;
	allocateBoard(&clonedBoard);
	if (!clonedBoard.hasAllocatedboard){
		return clonedBoard;
	}
	for (i = 0; i < clonedBoard.rows; i++)
	{
		for (j = 0; j < clonedBoard.cols; j++)
		{
			boardElement = getItemFromBoard(i, j, clonedBoard);
			*boardElement = *getItemFromBoard(i, j, b);
		}
	}
	return clonedBoard;

}
void allocateBoard(gameboard *b){
	b->board = (short*)malloc(sizeof(short) * b->cols * b->rows);
	if (!b->board){
		b->hasAllocatedboard = 0;
		return;
	}
	b->hasAllocatedboard = 1;
}
gameboard initBoard(){
	gameboard b;
	int initMethod;
	init_cb callback;
	printf("please choose how many rows the game will have\n");
	scanf("%d", &b.rows);
	printf("please choose how many columns the game will have\n");
	scanf("%d", &b.cols);
	allocateBoard(&b);
	if (!b.hasAllocatedboard){
		return b;
	}
	printf("please select initialization method:\n1 --- randomize board\n2 --- choose board yourself\n");
	scanf("%d", &initMethod);
	switch (initMethod)
	{
	case 1:
		srand((unsigned int)time(NULL));
		callback = randomizeDeadOrAlive;//setting callback pointer as randomizeDeadOrAlive
		break;
	case 2:
		callback = getDeadOrAliveFromPlayer;//setting callback pointer as getDeadOrAliveFromPlayer
		break;
	default:
		callback = NULL;
		break;
	}
	fillBoard(b, callback);
	return b;
}

short areIndexsOfMatrixInBoundries(int i, int j, gameboard b){
	if (i < b.rows && i >= 0 && j < b.cols && j >= 0)
		return 1;//in boundry
	return 0;
}
gameboard gameStage(gameboard b){
	gameboard newBoard;
	int i, j;//indexs for running on matrix 
	int k, r;//index for running on neighbors(k is for neighnbor row, r is for neighbor col)
	int neighborRow, neighborCol;
	int aliveNeighbors;
	short *boardElement, *newBoardElement;
	aliveNeighbors = 0;
	newBoard = cpyBoard(b);
	for (i = 0; i < b.rows; i++)
	{
		for (j = 0; j < b.cols; j++)
		{
			for (r = -1; r <= 1; r++)//counting alive neighbors
			{
				neighborRow = i + r;
				for (k = -1; k <= 1; k++)
				{
					neighborCol = j + k;
					//makes sure that the indexes are in boundries and are not pointing to the same cell
					if (areIndexsOfMatrixInBoundries(neighborRow, neighborCol, b) && (i != neighborRow || j != neighborCol)){
						if (*getItemFromBoard(neighborRow, neighborCol, b) == (short)1) aliveNeighbors++;
					}
				}
			}
			boardElement = getItemFromBoard(i, j, b);
			newBoardElement = getItemFromBoard(i, j, newBoard);
			if (*boardElement == 1){//alive
				if (aliveNeighbors <= 1 || aliveNeighbors >= 4)
					*newBoardElement = (short)0;//set as dead

			}
			else//dead
			{
				if (aliveNeighbors == 3)
					*newBoardElement = (short)1;//set as alive
			}
			aliveNeighbors = 0;
		}
	}
	return newBoard;
}

short* getItemFromBoard(int i, int j, gameboard b){
	return (b.board + (i * b.cols + j));
}
void printGameboard(gameboard b){
	int i, j;
	for (j = 0; j < b.cols; j++)
	{
		printf("--");
	}
	printf("-");
	printf("\n");
	for (i = 0; i < b.rows; i++)
	{
		for (j = 0; j < b.cols; j++)
		{
			*getItemFromBoard(i, j, b) == (short)1 ? printf("|*") : printf("| ");
			if (j == b.cols - 1){
				printf("|");
			}
		}
		printf("\n");
		for (j = 0; j < b.cols; j++)
		{
			printf("--");
		}
		printf("-");
		printf("\n");
	}
}
void freeBoard(gameboard b){
	free(b.board);
