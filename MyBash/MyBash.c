#include<stdio.h>
#include<string.h>
#include<pwd.h>
#include<sys/utsname.h>
#include<unistd.h>
#include<stdlib.h>

#define CMDNUM 10

//show PROMPT
void PrintfTag()
{
	//[userName+hostName directoryName /or~] $or#
	struct passwd *pw = getpwuid(getuid());
	struct utsname un;
	uname(&un);

	char pwd[128] = {0};
	getcwd(pwd, 127);//get directory path
	char *p = pwd + strlen(pwd);//p point pwd last location
	if(strcmp(pwd, pw->pw_dir) == 0)//this path is home path
	{
		p = "~";
	}
	else 
	{
		while(*p != '/')
		{
			p--;
		}
		if(strlen(pwd) != 1)
		{
			p++;
		}
	}

	char flag = '$';
	if(pw->pw_uid == 0)
	{
		flag = '#';
	}
	printf("[%s@%s %s]%c ", pw->pw_name, un.nodename, p, flag);
}

//recive PROMPT
int GetCmd(char *cmd)
{
	fgets(cmd, 127, stdin);//fgets will read the laster '\n'
	cmd[strlen(cmd) - 1] = 0;

	if(strlen(cmd) == 0)
	{
		return 0;
	}

	return 1;
}

//devison char string
void CutCmd(char *cmd, char *CMD[])
{
	int count = 0;
	char *p = strtok(cmd, " ");
	while(p != NULL)
	{
		CMD[count++] = p;
		if(count == CMDNUM)
		{
			break;
		}
		p = strtok(NULL, " ");
	}
}

//operator for cd
void OperatorCd(char *path)
{
	static char OLDPWD[128] = {0};//last locatian
	char nowpwd[128] = {0};
	getcwd(nowpwd, 127);

	if(path == NULL || strncmp(path, "~", 1) == 0)
	{
		struct passwd *pw = getpwuid(getuid());
		chdir(pw->pw_dir);//change now directory to home directory;
	}
	else if(strncmp(path , "-", 1) == 0)
	{
		if(strlen(OLDPWD) == 0)
		{
			printf("MyBash::OLDPWD not set\n");
			return;
		}
		else 
		{
			chdir(OLDPWD);//chang directory to last directory;	
		}
	}
	else 
	{
		if(-1 == (chdir(path)))
		{
			perror("MyBash: ");
			return;
		}
	}
	memset(OLDPWD, 0, 128);
	strcpy(OLDPWD, nowpwd);
}

//analysize CMD
int AnalyCmd(char *CMD[])
{
	if(strncmp(CMD[0], "cd", 2) == 0)
	{
		OperatorCd(CMD[1]);
		return 0;
	}
	else if(strncmp(CMD[0], "exit", 4) == 0)
	{
		exit(0);
	}
	return 1;
}

//come ture operation
void OperatorCmd(char *CMD[])
{
	
}

int main()
{
	while(1)
	{
		//show PROMPT
		PrintfTag();

		//recive PROMPT
		char cmd[128] = {0};
		//juduge stdin ture or false?
		if(!GetCmd(cmd))
		{
			continue;
		}

		char *CMD[CMDNUM] = {0};
		//devision char string
		CutCmd(cmd, CMD);

		//analysize CMD
		if(!AnalyCmd(CMD))
		{
			continue;
		}
		
		//come ture operation
//		OperatorCmd(CMD);
	}
	return 0;
}
