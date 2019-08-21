#include<stdio.h>
#include<string.h>
#include<pwd.h>
#include<sys/utsname.h>
#include<unistd.h>
#include<stdlib.h>
#include<assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMDNUM 10

//show PROMPT
void PrintfTag()
{
	//[userName+hostName directoryName /or~] $or#
	struct passwd *pw = getpwuid(getuid());
	struct utsname un;
	uname(&un);
	char nodeName[128] = {0};
	strcpy(nodeName, un.nodename);
	char *name = strtok(nodeName, ".");//we need to cut nodename from .


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
		if(strlen(pwd) != 1)// if pwd=/ we do not need move p;
		{
			p++;//now p is point / ,we need to move next char
		}
	}

	char flag = '$';
	if(pw->pw_uid == 0)//if path is equal to home diretory,we need to change $ to #
	{
		flag = '#';
	}
	printf("MyBash[%s@%s %s]%c ", pw->pw_name, name, p, flag);
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
	if(path == NULL)
	{
		return;//if cd+' ', we do not change our directory
	}
	static char OLDPWD[128] = {0};//last locatian
	char nowpwd[128] = {0};
	getcwd(nowpwd, 127);

	if(strncmp(path, "~", 1) == 0)
	{
		struct passwd *pw = getpwuid(getuid());
		chdir(pw->pw_dir);//change now directory to home directory;
	}
	else if(strncmp(path , "-", 1) == 0)
	{
		if(strlen(OLDPWD) == 0)
		{
			printf("MyBash: cd: OLDPWD not set\n");
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
		OperatorCd(CMD[1]);//cd cmd only need one parameter, if you send patameters we can only get first parameter
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
	pid_t pid = fork();
	assert(-1 != pid);

	//child progress
	if(0 ==pid)
	{
		char path[128] = "/home/siMeng/Desktop/MyBash/MyBin/";
		if(strstr(CMD[0], "/") == NULL)//if CMD has /,user give path,so we do not search in our given path,we can use user given path to do
		{
			strcat(path, CMD[0]);
		}
		else
		{
			memset(path, 0, 128);
			strcpy(path, CMD[0]);
		}

		execv(path, CMD);
		printf("%s:Command Not Found\n", CMD[0]);
		exit(0);//protect child progress still run
	}
	else 
	{
		//father progress
		wait(NULL);
	}
	
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
		//only cd and exit those cmd need to come ture in MyBash,other cmds we need to creat child progress to come ture;
		if(!AnalyCmd(CMD))
		{
			continue;
		}
		
		//come ture operation
		OperatorCmd(CMD);
	}
	return 0;
}
