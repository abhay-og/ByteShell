#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include <sys/wait.h>
const int RL_BUFSZ=1024;
const char *TOKEN=" ";
char builtins[8][10]={"cd","help","exit","history","echo","export","math","get"};
#define MX_SZ 100

///////// MAP TO STORE THE VARIABLES USED IN THE SESSION AND ITS UTILITY FUNCTIONS 
typedef struct{
    char key[MX_SZ];
    int value;
}KeyValuePair;

KeyValuePair map[MX_SZ];
int mapSize=0;

int getValue(char* key){
    for(int i=0;i<mapSize;i++){
        if(strcmp(map[i].key,key)==0)return map[i].value;
    }
    return -1; 
}

void setValue(char* key,int value){
    for(int i=0;i<mapSize;i++){
        if(strcmp(map[i].key,key)==0){
            map[i].value = value;
            return;
        }
    }
    strcpy(map[mapSize].key,key);
    map[mapSize].value=value;
    mapSize++;
}


///////// STRUCTS TO STORE THE COMMANDS AND THEIR ARGUEMENTS AS A STACK SO THAT HISTORY CAN BE EASILY ACCESSED
typedef struct Command{
    char** args;
    int sz;
}Command;

typedef struct Stack{
    Command* elements[MX_SZ];
    int top;
} Stack;

Stack* Hist;

Stack* createStack(){
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->top = 0;
    return stack;
}

void push(Stack* stack,Command* command) {
    stack->top++;
    stack->elements[stack->top] = command;
}

void EmptyStack(Stack* stack){
    free(stack);
}

void Add_to_Hist(char **arggs){
    Command *C=(Command *)malloc(sizeof(Command));
    C->args=arggs;
    C->sz=0;
    push(Hist,C);
}


///////// HERE THE UTILITY FUNCTIONS OF THE BYTESHELL ARE DEFINED
char* Read_Line(){
    int BuffSize=1024;
    int Position=0;
    char* Buffer=malloc(sizeof(char)*BuffSize);
    int c;

    if(!Buffer){
        fprintf(stderr,"Allocation Error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        c=getchar();
        if(c=='\n'){
            Buffer[Position]='\0';
            return Buffer; 
        }
        else{
            Buffer[Position++]=c;
        }
        if(Position>=BuffSize){
            BuffSize+=RL_BUFSZ;
            Buffer=realloc(Buffer,BuffSize);
        }
        if(!Buffer){
            fprintf(stderr,"Allocation Error\n");
            exit(EXIT_FAILURE);
        } 
    }
}

char** Split_Line(char* Line){
    int BuffSize=1024,Position=0;
    char **Words=malloc(sizeof(char*)*BuffSize);
    char *Token;

    if(!Words){
        fprintf(stderr,"Allocation Error\n");
        exit(EXIT_FAILURE);
    }

    Token=strtok(Line,TOKEN);

    while(Token){
        Words[Position++]=Token;
        if(Position>=BuffSize){
            BuffSize+=RL_BUFSZ;
            Words=realloc(Words,BuffSize);
            if(!Words){
                fprintf(stderr,"Allocation Error\n");
                exit(EXIT_FAILURE);
            }
        }
        Token=strtok(NULL,TOKEN);
    }
    return Words;
}

int Launch(char **args){
    int status;
    pid_t pid=fork();
    if(pid==0){
        if(execvp(args[0],args)==-1){
            perror("Byteshell");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid>0){
        do{
            wait(&status);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } 
    else{
        perror("Fork failed");
    }
}


///////// HERE ARE THE BUILTIN FUNCTIONS IMPLEMENTED FOR THE BYTESHELL
int cd(char **args){
    if(args[1]==NULL){
        fprintf(stderr,"ByteShell expected arguement to cd\n");
    }
    else{
        if(chdir(args[1])!=0){
            perror("ByteShell");
        }
    }
    return 1;
}

int Help(char **args){
    printf("The following are the available builtins\n");
    for(int i=0;i<4;i++){
        printf("%s\t",builtins[i]);
    }
    printf("\n");
    return 1;
}

int Exit(char **args){
    exit(EXIT_SUCCESS);
    return 1;
}

char* concatenateStrings(char** strings) {
    size_t totalLength=0;
    int i=0;
    while(strings[i]!=NULL){
        totalLength+=strlen(strings[i]);
        i++;
    }
    totalLength+=i-1;
    char* result=(char*)malloc((totalLength+1)*sizeof(char));
    if(result==NULL){
        fprintf(stderr,"Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    result[0]='\0';
    for(i=0;strings[i]!=NULL;i++){
        strcat(result, strings[i]);
        if(strings[i+1]!=NULL){
            strcat(result, " ");
        }
    }
    return result;
}

int History(){
    if(Hist->top==0){
        return 1;
    }
    for(int i=Hist->top-1;i>0;i--){
        char *cc=concatenateStrings(Hist->elements[i]->args);
        if(cc!=NULL)printf("%s\n",cc);
    }
    return 1;
}

int Echo(char **args){
    char *cc=concatenateStrings(args);
    if(cc!=NULL){
        int len=strlen(cc);
        int flag=-1;
        for(int i=0;i<len;i++){
            if(cc[i]=='"' && flag==-1)flag=1;
            else if(cc[i]=='"' && flag==1)flag=-1;
            if(flag==1 && cc[i]!='"')printf("%c",cc[i]);
        }
        printf("\n");
    }
    return 1;
}

int Export(char **args){
    int val=atoi(args[3]);
    setValue(args[1],val);
    printf("The value of %s is set to %d\n",args[1],getValue(args[1]));
    return 1;
}

int Arithmetic(char **args){
    char *v1=args[1];
    char *v2=args[3];
    char *v3=args[5];
    int val1=getValue(v1);
    if(val1==-1){
        printf("Varaible %s not found\n",v1);
        return 1;
    }
    int val2=getValue(v2);
    if(val2==-1){
        printf("Varaible %s not found\n",v2);
        return 1;
    }
    int val3=getValue(v3);
    if(val3==-1){
        printf("Varaible %s not found\n",v3);
        return 1;
    }
    char *op=args[4];
    if(strcmp(op,"+")==0){
        setValue(v1,val2+val3);
    }
    else if(strcmp(op,"-")==0){
        setValue(v1,val2-val3);
    }
    else if(strcmp(op,"*")==0){
        setValue(v1,val2*val3);
    }
    else if(strcmp(op,"/")==0){
        if(val3==0){
            printf("Division by zero not allowed\n");
        }
        else setValue(v1,val2/val3);
    }
    else{
        printf("Invalid mathematical operation\n");
    }
    return 1;
}

int getv(char **args){
    int val=getValue(args[1]);
    if(val==-1)printf("Variable %s not found\n",args[1]);
    else printf("The value of the variable %s = %d\n",args[1],val);
    return 1;
}

///////// THE FUNCTION TO EXECUTE THE COMMANDS
int Execute(char **args){
    if(args[0]==NULL)return 1;
    for(int i=0;i<8;i++){
        if(strcmp(builtins[i],args[0])==0){
            if(i==0){
                return cd(args);
            }
            else if(i==1){
                return Help(args);
            }
            else if(i==2){
                return Exit(args);
            }
            else if(i==3){
                return History(args);
            }
            else if(i==4){
                return Echo(args);
            }
            else if(i==5){
                return Export(args);
            }
            else if(i==6){
                return Arithmetic(args);
            }
            else{
                return getv(args);
            }
        }
    }
    return Launch(args);
}

///////// FUNCTION TO RUN THE LOOP OF THE SHELL
void Shell_Loop(){
    char *Line;
    char **args;
    int status;
    do{
        printf("> ");
        Line=Read_Line();
        args=Split_Line(Line);
        Add_to_Hist(args);
        status=Execute(args);
    }while(status>0);
}

///////// MAIN FUNCTION
int main(int c,char **argv){
    Hist=(Stack *)malloc(sizeof(Stack));
    Shell_Loop();
    EmptyStack(Hist);
    return EXIT_SUCCESS;
}
