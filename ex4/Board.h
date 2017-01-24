#ifndef BOARD_HEADER
#define BOARD_HEADER
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "GameConsts.h"

#define CELL_SIZE 7
#define BOARD_SIZE 100
#define SIZE_OF_INT 33
#define NUM_OF_LADDERS_OR_SNAKES 8
#define POS_IN_STRING_OF_S 4
#define POS_IN_STRING_OF_L 1
#define LINE_SIZE 71
#define NUM_OF_CELLS_IN_LINE 10

#define GAME_PIECES "!@#$"

typedef struct cell
{
	char value[CELL_SIZE];
	int destination_cell;
	BOOL players_in_cell[MAX_NUM_OF_PLAYERS];
}cell;

typedef struct game_board
{
	cell cells[BOARD_SIZE];
	int players_location[MAX_NUM_OF_PLAYERS];
}game_board;

void BasicBoardFill(int i, cell *data);
void BuildBoard(game_board *board);
void UpdateSnakeHead(cell *curr, int dest);
void UpdateSnaketail(cell *curr);
void UpdateLadderHead(cell *curr);
void UpdateLadderTail(cell *curr, int dest);
void PrintLineSeperator();
void PrintCell(cell cur_cell);
void PrintBoardLine(cell board[], int line);
void PrintBoard(game_board *board);

BOOL UpdateBoard(game_board *board, char game_piece, int dice_result);

#endif 