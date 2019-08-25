#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<unistd.h>
#include<shadow.h>
#include<pwd.h>
#include<termios.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	char *user = "root";
	if(argc > 1)
	{
		user = argv[1];
	}

	printf("Password: ");
	//recored password
	char pwd[128] = {0};
	char c = 0;
	int j = 0;
	//first make the terminal does not echo
	struct termios oldterm, newterm;
	tcgetattr(0, &oldterm);
	newterm = oldterm;
	newterm.c_lflag &= ~ECHO;
	newterm.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &newterm);//change terminal control mode to unecho and non-standard mode
	while('\n' != (c = getchar()))
	{
		if(c == 127)//c is equal to BACKSPACE
		{
			if(j > 0)
			{
				pwd[--j] = 0;
				printf("\033[1D");
				printf("\033[K");
			}
			continue;
		}
		pwd[j++] = c;
		printf("*");
		fflush(stdout);
	}
	printf("\n");
	//change terminal control mode to dedault
	tcsetattr(0, TCSANOW, &oldterm);

	struct spwd *sp = getspnam(user);// /etc/shadow The normal user does not have permission to call this function
	assert(sp != NULL);

	//record $ID$key$ 
	char salt[128] = {0};//$ID$key$cipher
	int i = 0;
	int count = 0;
	while(sp->sp_pwdp[i])
	{
		salt[i] = sp->sp_pwdp[i];
		if(salt[i] == '$')
		{
			count++;
		}
		if(count == 3)
		{
			break;
		}
		i++;
	}

	//through password enterd by the user and the key we can generate complete ciphertext
	char *pw = (char *)crypt(pwd, salt);
	if(strcmp(pw, sp->sp_pwdp) != 0)
	{
		printf("password id error\n");
		exit(0);
	}

	pid_t pid = fork();
	assert(pid != -1);
	if(pid == 0)
	{
		struct passwd *pd = getpwnam(user); 
		assert(pd != NULL);
		//change user
		setuid(pd->pw_uid);
		execl(pd->pw_shell, pd->pw_shell, (char *)0);
		perror("exec error:");
		exit(0);
	}
	else 
	{
		wait(NULL);
	}
}
