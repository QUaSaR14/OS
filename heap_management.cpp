#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

#define HEAP_SIZE 100//Memory Size
#define Null -1
#define MAGIC -2

/*
    header block has two elements :
    (1) Size of block
    (2) Address of next block or Null(-1) to indicate NULL in case of free block
                    OR
        Magic Number(-2) in case of allocated block
*/

typedef struct
{
    int addr;
    char name[20];
} pointer;

std::string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  getcwd( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

void intialize(int heap[],int heap_size,int &head,char hostname[],char username[],char dir[])
{
    head=0;
    heap[0]=heap_size-2;
    heap[1]=Null;
    gethostname(hostname,100);
    getlogin_r(username,100);
    char temp[100];
    std::string s(GetCurrentWorkingDir());
    strcpy(temp,s.c_str());
    std::string u(username);
    string substring=s.substr(0,6);
    substring=substring+u;
    int len=strlen(username);
    string part=s.substr(0,6+len);
    if(part==substring)
    {
	int l=strlen(temp);
	s="~"+s.substr(6+len,l-len-6);
	strcpy(dir,s.c_str());
    }
    else
    {
	strcpy(dir,temp);
    }
}

void shell(char *hostname,char *username,char* dir)
{
    cout << "\033[1;32m"<<username<<"@"<<hostname<<"\033[0m";
    cout<<":";
    cout << "\033[1;34m"<<dir<<"\033[0m";
    cout<<"$ ";
}

/*Returns   1 for malloc
            2 for free
            0 for invalid command
*/
int check_command(char* cmd,char* p_name,int &alloc_size)
{
    char subpart[3][15];
    int i=0,k=0,l=0;
    /*  i = index of subpart
	k = to tranverse through command
	l = length of subpart
    */
    while( cmd[k]!='\0' && i<3)//max 3 subparts
    {
        if(cmd[k]=='=' && i==0)//of beginning of 2nd subpart
        {
            subpart[0][l]='\0';
            i=2;
            l=0;
            subpart[1][0]='=';
            subpart[1][1]='\0';
        }
        else if(cmd[k]!=' ')
        {
            subpart[i][l]=cmd[k];
            l++;
        }
        k++;
    }
    subpart[i][l]='\0';
    if(i==2 && strcmp(subpart[1],"=")==0)//checking malloc command
    {
        int len=strlen(subpart[2]);
        std::string s(subpart[2]);
        string substring=s.substr(0,7);
        if( len>8 && strcmp(substring.c_str(),"malloc(")==0 && subpart[2][len-1]==')' )
        {
            alloc_size=0;
            int j=7;
            while(j<len-1 && subpart[2][j]<='9' && subpart[2][j]>='0')//getting malloc size
            {
                alloc_size*=10;
                alloc_size+=(subpart[2][j]-'0');
                j++;
            }
            if(j!=len-1 || alloc_size==0)//checking malloc argument
            {
                return 0;
            }
            else
            {
                strcpy(p_name,subpart[0]);//storing pointer name
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    else if( i==0 && k>6)//checking free command
    {
        std::string s(subpart[0]);
        string substring=s.substr(0,5);
        if(strcmp(substring.c_str(),"free(")==0 && subpart[0][l-1]==')')
        {
            int j=0;
            while(j+5<l-1)//storing pointer name
            {
                p_name[j]=subpart[0][j+5];
                j++;
            }
            p_name[j]='\0';
            return 2;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

/* returns address of allocated block else returns -1 */
int allocate(int heap[],int heap_size,int &head,int alloc_size)
{
    int min_space=heap_size;
    int curr_addr=head;
    int free_in=-1;
    int prev=-1;
    int prev_free=-1;
    while(curr_addr!=Null && min_space>0)//to find best fit free block
    {
        if(heap[curr_addr]>=alloc_size+2)
        {
            if(min_space>(heap[curr_addr]-alloc_size-2))
            {
                min_space=(heap[curr_addr]-alloc_size-2);
                free_in=curr_addr;
                prev_free=prev;
            }
        }
        prev=curr_addr;
        curr_addr=heap[curr_addr+1];
    }
    if(free_in==-1)//No block found
    {
        return -1;
    }

    else
    {
            if(prev_free!=-1)//not first free block
            {
                heap[prev_free+1]=free_in+alloc_size+2;
                heap[free_in+alloc_size+3]=heap[free_in+1];
                heap[free_in+alloc_size+2]=heap[free_in]-alloc_size-2;
                heap[free_in]=alloc_size;
                heap[free_in+1]=-2;
            }
            else// the first free block
            {
                head=free_in+alloc_size+2;
                heap[free_in+alloc_size+3]=heap[free_in+1];
                heap[free_in+alloc_size+2]=heap[free_in]-alloc_size-2;
                heap[free_in]=alloc_size;
                heap[free_in+1]=-2;
            }
            return free_in;

    }
}

void free_memory(int heap[],int heap_size,int &head,int free_addr)//make allocated block free block
{
    heap[free_addr+1]=head;
    head=free_addr;
}

void display_status(int heap[],int heap_size,int head,vector<pointer> Pointers)
{
    int total_free_space=0;
    int curr_block=head;
    cout<<"Free memory track: ";
    if(curr_block==-1)
    {
        cout<<"head=null.";
    }
    else
    {
        cout<<"head="<<head<<", ";
        while(curr_block!=Null)//to print free list
        {
            cout<<curr_block<<"("<<heap[curr_block]<<" units)->";
            total_free_space+=heap[curr_block];
            curr_block=heap[curr_block+1];//storing next free block
        }
        cout<<"null.";
    }
    cout<<"\nTotal free memory = "<<total_free_space<<" units." ;
    int vsize=Pointers.size();
    /*Printing user pointers*/
    if(vsize==0)
    {
        cout<<"\nNo user pointers at the moment.\n";
    }
    else
    {
        cout<<"\nUser pointers:";
        for(int i=0;i<vsize;i++)
        {
            cout<<Pointers[i].name<<"="<<Pointers[i].addr+2<<"("<<heap[Pointers[i].addr]<<" units)";
            if(i==vsize-1)
            {
                cout<<".\n";
            }
            else
            {
                cout<<", ";
            }
        }
    }
}

int coalesce_memory(int heap[],int heap_size,int &head)
{
    int i=0;
    int prev=-1;// pointer to previous free block
    int mem_saved=0;
    int found=0;
    while(i+1<heap_size)
    {
        if(heap[i+1]!=MAGIC)//if free block
        {
            if(prev!=-1)
            {
                heap[prev]=i;
            }
            prev=i+1;
            if(found==0)//first free block
            {
                found=1;
                head=i;
            }
            int j=i+heap[i]+2;//next block address
            while(heap[j+1]!=MAGIC && j<heap_size)//merging contiguos free blocks
            {
                heap[i]+=heap[j]+2;
                mem_saved+=2;
                j=j+heap[j]+2;
            }
            i=j;//next block address
        }
        else
        {
            i+=heap[i]+2;
        }
    }
    heap[prev]=Null;//free list terminated by null pointer
    if(mem_saved>0)
    {
        cout<<"Memory coalesced successfully, "<<mem_saved<<" units of memory saved.\nFree memory track: ";
        int curr_block=head,total_free_space=0;
        if(curr_block==Null)
        {
            cout<<"head=null.";
        }
        else
        {
            cout<<"head="<<head<<", ";
            while(curr_block!=Null)
            {
                cout<<curr_block<<"("<<heap[curr_block]<<" units)->";
                total_free_space+=heap[curr_block];
                curr_block=heap[curr_block+1];
            }
            cout<<"null.";
        }
        cout<<"\nTotal free memory: "<<total_free_space<<" units.\n" ;
    }
    else
    {
        cout<<"Memory not coalesced, no adjacent free memory chunks found.\n";
    }
    return mem_saved;
}

int main()
{
    int Heap[HEAP_SIZE];
    int head;
    char dir[100];
    char hostname[100];
    char username[100];
    gethostname(hostname,100);
    getlogin_r(username,100);
    intialize(Heap,HEAP_SIZE,head,hostname,username,dir);
    vector<pointer> Pointers;//in the stack
    int flag=0;
    string s;
    char command[30];
    pointer temp;
    int prev_head;
    system("clear");
    cout<< "Virtual Heap Manager is running ..."<<endl;
    do
    {
        shell(hostname,username,dir);
        getline(cin,s);
        strcpy(command,s.c_str());
        if(strcmp(command,"exit")==0)
        {
            flag=1;
	    cout<<"Program terminated."<<endl;
        }
        else if(strcmp(command,"display_status")==0)
        {
            display_status(Heap,HEAP_SIZE,head,Pointers);
        }
        else if(strcmp(command,"coalesce_memory")==0)
        {
            coalesce_memory(Heap,HEAP_SIZE,head);
        }
        else if(command[0]!='\0')
        {
            char pointer_name[20];
            int alloc_size;
            int f=check_command(command,pointer_name,alloc_size);

            if(f==1)
            {
                int found=0,addr,free_size;
                for(int i=0;i<Pointers.size() && found==0;i++)
                {
                    if(strcmp(Pointers[i].name,pointer_name)==0)
                    {
                        found=1;
                        addr=Pointers[i].addr;
                    }
                }
                if(found==0)
                {
                    prev_head=head;
                    int alloc_addr=allocate(Heap,HEAP_SIZE,head,alloc_size);
                    if(alloc_addr!=-1)
                    {
                        temp.addr=alloc_addr;
                        strcpy(temp.name,pointer_name);
                        Pointers.push_back(temp);
                        cout<<alloc_size<<" units of memory is allocated pointed by "<<pointer_name;
			cout<<". Changed pointers: "<<pointer_name<<"="<<alloc_addr+2;
                        if(prev_head!=head)
                        {
                            cout<<", head="<<head;
                        }
                        cout<<".\n";
                    }
                    else
                    {
                        cout<<"malloc operation failed.\n";
                    }
                }
                else
                {
                    cout<<"Pointer "<<pointer_name<<" already pointing to "<<addr+2<<"("<<Heap[addr]<<" units).\n";
                }
            }
            else if(f==2)
            {
                int found=0,free_addr,free_size;
                for(int i=0;i<Pointers.size() && found==0;i++)
                {
                    if(strcmp(Pointers[i].name,pointer_name)==0)
                    {
                        found=1;
                        free_addr=Pointers[i].addr;
                        Pointers.erase(Pointers.begin()+i);
                    }
                }
                if(found==1)
                {
                     prev_head=head;
                     free_size=Heap[free_addr];
                     free_memory(Heap,HEAP_SIZE,head,free_addr);
                     cout<<"Memory pointed by "<<pointer_name<<" ("<<free_size<<" units) is freed.";
                     cout<<" Changed pointers: ";
                     if(head!=prev_head)
                     {
                         cout<<" head="<<head;
                     }
                     else
                     {
                         cout<<"none";
                     }
                     cout<<".\n";
                }
                else
                {
                    cout<<"No such pointer.\n";
                }
            }
            else
            {
                cout<<"Invalid command.\n";
            }
        }
    }while(flag==0);

    return 0;
}

