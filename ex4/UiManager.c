#include "UiManager.h"
#include <stdio.h>
#include <stdlib.h>

void readInputFromUser(char *input)
{
	char current_char;
	int index = 0;

	printf("Enter command:\n");
	while ((current_char = getchar()) != '\n')
	{
		input[index++] = current_char;
	}
	input[index++] = '\n';
	input[index] = '\0';
}