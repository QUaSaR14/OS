#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>
#include<signal.h>
#include<iostream>
#include<string.h>
#include<unistd.h> 
#include<sys/types.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define MAXLIST 100

using namespace std;

void sigintHandler(int sig_num)
{
	//printf("Signal %d caught...\n",sig_num);
	signal(SIGINT, sigintHandler); 
}

void sighandler(int sig_num) 
{
    signal(SIGTSTP, sighandler); 
} 

void parseSpace(char* str, char** parsed) 
{ 
    int i; 
  
    for (i = 0; i < MAXLIST; i++) { 
        parsed[i] = strsep(&str, " "); 
  
        if (parsed[i] == NULL) 
            break; 
        if (strlen(parsed[i]) == 0) 
            i--; 
    } 
} 

int parselt(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "<"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
} 

int parsegt(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, ">"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
} 

void run_command_sym(char *str,char *str2,int symbol)//string str)
{
	int fd0,fd1;
	int rc = fork();
	char *command[MAXLIST];
	if(rc<0)
	{
		printf("Process Creation failed\n");
		exit(0);
	}
	else if (rc == 0)
	{
			if(symbol==0)
			{
				if((fd0 = open(str2, O_RDONLY) )< 0){
				    perror("cant open file");
				    //exit(0);                    
				}
				dup2(fd0, 0);
				close(fd0);
				
			}
			else if(symbol==1)
			{
				//if((fd1 = open(str2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR) )< 0){
				if((fd1 = open(str2, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU) )< 0){
				
				    perror("cant open file");
				    //exit(0);                    
				}
				dup2(fd1, 1);
				close(fd1);
			}
			parseSpace(str,command);
			if (strcmp(command[0], "cd") != 0)
			{
				if(execvp(command[0],command)==-1)
				{
					cout<<"Shell: Incorrect command\n";
				}
				//exit(errno);
			}
			exit(errno);
	}
	else 
	{
		int status;
		waitpid(rc,&status,0);
		if (strcmp(command[0], "cd") == 0)
		{
			if (chdir(command[1])) 
			{
				printf("Error:\n");
			}
		}      /*you made a exit call in child you need to wait on exit status of child*/
		WIFEXITED(status);
	}	
}

void run_command2(char *str)//string str)
{
	int rc = fork();
	char *command[MAXLIST];
	parseSpace(str,command);
	if(rc<0)
	{
		printf("Process Creation failed\n");
		exit(0);
	}
	else if (rc == 0)
	{
			if (strcmp(command[0], "cd") != 0)
			{
				if(execvp(command[0],command)==-1)
				{
					cout<<"Shell: Incorrect command\n";
				}
				//exit(errno);
			}
			exit(errno);
			
	}
	else 
	{
		int status;
		waitpid(rc,&status,0);
		if (strcmp(command[0], "cd") == 0)
		{
			if (chdir(command[1])) 
			{
				printf("Error:\n");
			}
		}       /*you made a exit call in child you need to wait on exit status of child*/
		WIFEXITED(status);
	}	
}

void run_command(char *str)//string str)
{
	char *strsplit[2];
	int multi=parselt(str,strsplit);
	if(multi)
	{
		run_command_sym(strsplit[0],strsplit[1],0);
	}
	else
	{
		int multi=parsegt(str,strsplit);
		if(multi)
		{
			run_command_sym(strsplit[0],strsplit[1],1);
		}
		else
		{
			run_command2(str);
		}
	}	
}

int parsedand(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < MAXLIST; i++) { 
        strpiped[i] = strsep(&str, "&&"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
} 

int parsetand(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < MAXLIST; i++) { 
        strpiped[i] = strsep(&str, "&&&"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
}

void executedand(char** strpiped)
{
	char *command[MAXLIST];
	int rc = 1;
	for (int i = 0; strpiped[i]!=NULL && rc!=0; i++) 
	{
		//rc=1;
		rc=fork();
		//cout<<strpiped[i]<<" ";
		//parseSpace(strpiped[i],command);
		if(rc<0)
		{
			printf("Process Creation failed\n");
			exit(0);
		}
		else if (rc == 0)
		{
				run_command(strpiped[i]);
				exit(0);					
		}
		else 
		{
			/*int status;
			waitpid(rc, &status, 0);*/
			wait(NULL);
		}	
	} 	
}

void executetand(char** strpiped)
{
	char *command[MAXLIST];
	int pid[MAXLIST];
	int rc = 1;
	int i;
	for (i = 0; strpiped[i]!=NULL && rc!=0; i++) 
	{
		//cout<<strpiped[i]<<" ";
		rc=pid[i]=fork();
		//cout<<strpiped[i]<<" ";
		
		if(pid[i]<0)
		{
			printf("Process Creation failed\n");
			exit(0);
		}
		else if (pid[i] == 0)
		{
				run_command(strpiped[i]);
				exit(0);					
		}
	}
	int k=i-1;
	for (i = 0; i < k; i++) 
	{ 
		if (pid[i] > 0) 
		{
			int status;
			waitpid(pid[i], &status, 0);
			if (status > 0) {
			// handle a process sent exit status error
			}
		}
		else
		{
		// handle a proccess was not started
		}     
	}	
}
int execute(string str)
{
	char* strpiped[MAXLIST]; 
	int multi = 0; 
	char cstr[str.size()+1];
	strcpy(cstr,str.c_str());
	char *command[MAXLIST];  
	multi = parsetand(cstr, strpiped);
	if(multi)
	{
		executetand(strpiped);
	}
	else 
	{
		multi = parsedand(cstr, strpiped);
		if(multi)
		{
			executedand(strpiped);
		}
		else
		{
			run_command(cstr);
		}
	}
	return 1;
}
void start_code()
{
	const char* userr = getenv("user");
	string user = userr;
	const char* home = getenv("hostname");
	string host = home;
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	string output=user + "@" + host;
	cout << "\033[1;32m"<<output<<"\033[0m"<<":";
	cout<<"\033[1;34m"<<cwd<<"\033[0m";
	cout<<"$";
}

int main()
{
	signal(SIGTSTP, sighandler); 
	signal(SIGINT, sigintHandler); 
	system("clear");
	cout<<"\n//**************************************************************************//\n";
	cout<<"			Welcome To My Custom Shell Prompt\n";
	cout<<"//**************************************************************************//\n";
	int rc = fork();
	if(rc<0)
	{
		printf("Process Creation failed\n");
		exit(0);
	}
	else if (rc == 0){
		
		string str="";
		string ext="exit";
		while(str!=ext)
		{
			start_code();
			getline(cin,str);
			if(str!="")
			{
				if(str==ext)
				{
					return 0;
				}
				execute(str);
			}
		}
	}
	else 
	{

		int rc=wait(NULL);
		cout<<"\n//**************************************************************************//\n";
		cout<<"			Hope to see you again, Bye!\n";
		cout<<"//**************************************************************************//\n";
	}
	return 0;	
}
