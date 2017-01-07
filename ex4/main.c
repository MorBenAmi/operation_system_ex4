/*Limor Mendelzburg 308081389
	Mor Ben Ami 203607536
	Exercise 4*/
#include <Windows.h>
#include <stdio.h>
#define NumOfArguments 4

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int main(int argc, char* argv[])
{
	char* mode;
	//checks if we get all the arguments//
	if(argc!=NumOfArguments)
	{
		printf("ERROR - there is a wrong number of arguments\n");
		exit(ERROR_BAD_ARGUMENTS);
	}

	mode = argv[2]; //todo: change this to the correct number

	if(mode == "Server")
	{
		//todo: call server run
	}
	else if(mode == "Client")
	{
		//todo call client run
	}
	else
	{
		printf("ERROR - unknown running mode, closing app!\n");
		exit(ERROR_BAD_ARGUMENTS);
	}
}//main