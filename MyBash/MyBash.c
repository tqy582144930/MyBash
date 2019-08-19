#include<stdio.h>
#include<string.h>
#include<pwd.h>
#include<sys/utsname.h>

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
	//char *p = pwd + strlen(pwd);//p point pwd last location
	char *p = pwd;
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
	
}

//analysize CMD
int AnalyCmd(char *CMD[])
{

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
		if(!GetCmd(cmd))
		{
			continue;
		}

//		char *CMD[CMDNUM] = {0};
		//devision char string
//		CutCmd(cmd, CMD);

		//analysize CMD
//		if(!AnalyCmd(CMD))
//		{
//			continue;
//		}
		
		//come ture operation
//		OperatorCmd(CMD);
	}
	return 0;
}
