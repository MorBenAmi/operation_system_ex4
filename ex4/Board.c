#define _CRT_SECURE_NO_WARNINGS
#include "Board.h"

void BasicBoardFill(int i, cell *data)
{
	char temp[SIZE_OF_INT];
	_itoa(i, temp, 10);
	data->destination_cell = i-1;
	strcpy(data->value, "  ");
	if(i<10)
	{	
		strcat(data->value, "0");
		strcat(data->value, temp);
		strcat(data->value, "  ");
	}
	else if(i>9 && i<100)
	{
		strcat(data->value,temp);
		strcat(data->value, "  ");
	}
	else if(i=100)
	{
		strcat(data->value,temp);
		strcat(data->value, " ");
	}
}
void UpdateSnakeHead(cell *curr, int dest)
{
	curr->value[POS_IN_STRING_OF_S]='v';
	curr->snake_head=1;
	curr->destination_cell=dest-1;
}
void UpdateSnaketail(cell *curr)
{
	curr->value[POS_IN_STRING_OF_S]='_';
	curr->snake_tail=1;
}
void UpdateLadderHead(cell *curr)
{
	curr->value[POS_IN_STRING_OF_L]='=';
	curr->ladder_head=1;
}
void UpdateLadderTail(cell *curr, int dest)
{
	curr->value[POS_IN_STRING_OF_L]='^';
	curr->ladder_tail=1;
	curr->destination_cell=dest-1;
}

void PrintLineSeperator() 
{
	int i;
	for(i=0;i<LINE_SIZE;i++)
		printf("-");
	printf("\n");
}
void PrintCell(cell cur_cell)
{
	int j;
	int numOfPlayers;
	char players[NUM_OF_PLAYERS + 1];
	char *play = "!@#$";

	memset(players, '\0', NUM_OF_PLAYERS + 1);
	numOfPlayers = 0;
	for (j = 0; j < NUM_OF_PLAYERS; j++) 
	{
		if (cur_cell.players_in_cell[j] == TRUE)
		{
			players[numOfPlayers] = play[j];
			numOfPlayers++;
		}
	}
	switch(numOfPlayers)
	{
		case 0:
			printf("%s",cur_cell.value);
			break;
		case 1:
			printf("   %s  ", players);
			break;
		case 2:
			printf("  %s  ", players);
			break;
		case 3:
			printf("  %s ", players);
			break;
		case 4:
			printf(" %s ", players);
			break;
	}
	printf("|");
}
void PrintBoardLine(cell board[], int line)
{
	int i;
	int start;
	int factor;
	int end;
	if (line % 2 == 0)
	{
		//asc
		start = line*NUM_OF_CELLS_IN_LINE;
		end = start + NUM_OF_CELLS_IN_LINE;
		factor = 1;
	}
	else
	{
		//dsc
		start = (line + 1)*NUM_OF_CELLS_IN_LINE - 1;
		end = start - NUM_OF_CELLS_IN_LINE;
		factor = -1;
	}
	printf("|");
	for (i=start; i!=end; i+=factor)
	{
		PrintCell(board[i]);		
	}
	printf("\n");
	PrintLineSeperator();
}

void PrintBoard(cell board[])
{
	int i;
	
	PrintLineSeperator();
	for(i=9; i>=0; i--)
	{
		PrintBoardLine(board, i);
	}
}
void BuildingBoard()
{
	int i;
	int up_ladder[]={14,31,38,84,59,67,81,91};
	int bottom_ladder[]={4,9,20,28,40,51,63,71};
	int snake_head[]={17,54,64,62,93,95,87,99};
	int snake_tail[] = {7,34,60,19,73,75,24,78};
	cell board[BOARD_SIZE];
	for(i=0;i<BOARD_SIZE;i++)
		BasicBoardFill(i+1, &(board[i]));
	for(i=0; i<NUM_OF_LADDERS_OR_SNAKES; i++)
	{
		UpdateLadderHead(&(board[up_ladder[i]-1]));
		UpdateLadderTail(&(board[bottom_ladder[i]-1]),up_ladder[i]);
		UpdateSnaketail(&(board[snake_tail[i]-1]));
		UpdateSnakeHead(&(board[snake_head[i]-1]),snake_head[i]);
	}
	PrintBoard(board);
	getchar();
}