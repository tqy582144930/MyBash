#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<dirent.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>

#define L 0
#define I 1
#define A 2

#define SETOPTION(option, flag) ((option) |= (1<<flag))
#define ISOPTION(option, flag) ((option) & (1<<(flag)))

int AnalyOption(int argc, char *argv[])
{
	int i = 1;
	int option = 0;
	for(; i < argc; i++)
	{
		if(strncmp(argv[i], "-", 1) == 0)
		{
			if(strstr(argv[i], "a") != NULL)
			{
				SETOPTION(option, A);
			}
			if(strstr(argv[i], "l") != NULL)
			{
				SETOPTION(option, L);
			}
			if(strstr(argv[i], "i") != NULL)
			{
				SETOPTION(option, I);
			}
		}
	}
	return option;
}

void PrintFileName(char *path, char *fileName)
{
	struct stat statbuf;
	char pathfile[128] = {0};
	//splice a complete path
	strcat(pathfile, path);
	strcat(pathfile, "/");
	strcat(pathfile, fileName);
	//return file status information
	lstat(pathfile, &statbuf);

	if(S_ISDIR(statbuf.st_mode))
	{
		printf("\033[1;34m%s\033[0m  ", fileName);
	}
	else if(S_ISFIFO(statbuf.st_mode))
	{
		printf("\033[40;33m%s\033[0m  ", fileName);
	}
	else if(S_ISLNK(statbuf.st_mode))
	{
		printf("\033[1;36m%s\033[0m  ", fileName);
	}
	else 
	{
		if(S_IXUSR & statbuf.st_mode ||
			S_IXGRP & statbuf.st_mode ||
			S_IXOTH & statbuf.st_mode)
		{
			printf("\033[1;32m%s\033[0m  ", fileName);
		}
		else 
		{
			printf("%s  ", fileName);
		}
		
	}
}

//printf file mode
void PrintfFileMode(char *path, char *filename)
{
	struct stat st;
	//file type
	char type[] = {'-', 'd', 'l', 'p', 'c', 'b', 's'};
	int type_index = 0;
	//splice a complete pathv
	char pathfile[128] = {0};
	strcat(pathfile, path);
	strcat(pathfile, "/");
	strcat(pathfile, filename);
	
	//return file status information
	lstat(pathfile, &st);
	if(S_ISDIR(st.st_mode))
	{
		type_index = 1;
	}
	else if(S_ISLNK(st.st_mode))
	{
		type_index = 2;
	}
	else if(S_ISFIFO(st.st_mode))
	{
		type_index = 3;
	}
	else if(S_ISCHR(st.st_mode))
	{
		type_index = 4;
	}
	else if(S_ISBLK(st.st_mode))
	{
		type_index = 5;
	}
	else if(S_ISSOCK(st.st_mode))
	{
		type_index = 6;
	}

	//recored mode
	char mode[] = "---------";
	if(st.st_mode & S_IRUSR) mode[0]='r';
	if(st.st_mode & S_IWUSR) mode[1]='w';
	if(st.st_mode & S_IXUSR) mode[2]='x';
	if(st.st_mode & S_IRGRP) mode[3]='r';
	if(st.st_mode & S_IWGRP) mode[4]='w';
	if(st.st_mode & S_IXGRP) mode[5]='x';
	if(st.st_mode & S_IROTH) mode[6]='r';
	if(st.st_mode & S_IWOTH) mode[7]='w';
	if(st.st_mode & S_IXOTH) mode[8]='x';

	struct passwd *pw = getpwuid(st.st_uid);
	struct group *gr = getgrgid(st.st_uid);
	char *time = ctime(&st.st_mtime);
	time[strlen(time) - 1] = 0;

	printf("%c%s %5.0d %s  %s %10.1d %s  ", type[type_index], mode, st.st_nlink, pw->pw_name, gr->gr_name, st.st_size, time );

}

//ptintf files in the path
void PrintPathFile(char *path, int option)
{
	//first we need to scan thr catalog
	 DIR *dir = opendir(path);
	 if(dir == NULL)
	 {
		perror(path);
		return;
	 }

	 struct dirent *dr = NULL;
	 while(NULL != (dr = readdir(dir)))
	 {
		int flag = 0;
		//if options do not have a, you do not printf . and ..
		if(!ISOPTION(option, A) && strncmp(dr->d_name, ".", 1) == 0)
		{
			continue;
		}
		if(ISOPTION(option, I))
		{
			//-i printf inode and fileName
			printf("%d   ", dr->d_ino);
		}
		if(ISOPTION(option, L))
		{
			//-l printf mode inode and fileName
			PrintfFileMode(path, dr->d_name);
			flag = 1;
		}

		//if options has a, you can printf . and ..
		PrintFileName(path, dr->d_name);
		if(flag)
		{
			printf("\n");
		}
	 }
	 closedir(dir);
	 printf("\n");
}

//The ls command displays the contents of the specified working directory
int main(int argc, char *argv[])
{
	//analy parameter
	int option = AnalyOption(argc, argv);
	
	//Loops to print files in the specified path or default path
	int i = 1;
	int flag = 0;
	for(; i < argc; i++)
	{
		//if argv has not "-",we need to print specifiled path files
		if(strncmp(argv[i], "-", 1) != 0)
		{
			flag = 1;
			printf("%s:\n", argv[i]);
			//pass in the specified path 
			PrintPathFile(argv[i], option);
			printf("\n");
		}
	}

	//we need to printf deafault path files
	if(!flag)
	{
		char path[128] = {0};
		//get current directory
		getcwd(path, 127);
		PrintPathFile(path, option);
	}
	return 0;
}
