#include<stdio.h>

#define SETOPTION(option, flag) ((option) |= (1<<flag))
#define ISOPTION(option, flag) ((option) & (1<<(flag)))

int AnalyOption(int argc, char *argc[])
{
	int i = 1;
	int option = 0;
	for(; i < arcg; i++)
	{
		
	}
	return option;
}

//The ls command displays the contents of the specified working directory
int main(int argc, char *argv[])
{
	//analy parameter
	int option = AnalyOption(argc, argv);
	return 0;
}
