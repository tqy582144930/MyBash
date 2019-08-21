#include<stdio.h>
#include<pwd.h>
#include<unistd.h>

int main()
{
	char path[128] = {0};
	getcwd(path, 127);
	printf("%s\n", path);
}
