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
	curr->destination_cell=dest-1;
}
void UpdateSnaketail(cell *curr)
{
	curr->value[POS_IN_STRING_OF_S]='_';
}
void UpdateLadderHead(cell *curr)
{
	curr->value[POS_IN_STRING_OF_L]='=';
}
void UpdateLadderTail(cell *curr, int dest)
{
	curr->value[POS_IN_STRING_OF_L]='^';
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
	char players[MAX_NUM_OF_PLAYERS + 1];

	memset(players, '\0', MAX_NUM_OF_PLAYERS + 1);
	numOfPlayers = 0;
	for (j = 0; j < MAX_NUM_OF_PLAYERS; j++) 
	{
		if (cur_cell.players_in_cell[j] == TRUE)
		{
			players[numOfPlayers] = GAME_PIECES[j];
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

void PrintBoard(game_board *board)
{
	int i;
	
	PrintLineSeperator();
	for(i=9; i>=0; i--)
	{
		PrintBoardLine(board->cells, i);
	}
}

void BuildBoard(game_board *board)
{
	int i;
	int up_ladder[]={14,31,38,84,59,67,81,91};
	int bottom_ladder[]={4,9,20,28,40,51,63,71};
	int snake_head[]={17,54,64,62,93,95,87,99};
	int snake_tail[] = {7,34,60,19,73,75,24,78};
	for(i=0;i<BOARD_SIZE;i++)
		BasicBoardFill(i+1, &(board->cells[i]));
	for(i=0; i<NUM_OF_LADDERS_OR_SNAKES; i++)
	{
		UpdateLadderHead(&(board->cells[up_ladder[i]-1]));
		UpdateLadderTail(&(board->cells[bottom_ladder[i]-1]),up_ladder[i]);
		UpdateSnaketail(&(board->cells[snake_tail[i]-1]));
		UpdateSnakeHead(&(board->cells[snake_head[i]-1]),snake_head[i]);
	}
	for (i=0; i<MAX_NUM_OF_PLAYERS; i++)
	{
		board->players_location[i] = -1;
	}
}

void UpdateBoard(game_board *board, char game_piece, int dice_result) 
{
	int location;
	int player_index;
	player_index = (int)(strchr(GAME_PIECES, game_piece) - GAME_PIECES);
	location = board->players_location[player_index];
	if (location > 0) 
		board->cells[location].players_in_cell[player_index] = FALSE;
	location += dice_result;
	if (location >= BOARD_SIZE)
	{
		//game ended
	}
	//Get destination cell
	location = board->cells[location].destination_cell;

	//Update player location
	board->players_location[player_index] = location;
	board->cells[location].players_in_cell[player_index] = TRUE;
}