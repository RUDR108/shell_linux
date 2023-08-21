#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64


int pidT=0;                     //MAking global pid to kill foreground process.

void handler_sigint(){          //signal handler function kills only foreground process

    kill(pidT,SIGKILL);
    printf("\n");
  
}


/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

int main(int argc, char* argv[]) {


	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	char path[2000]="/";                     //String to keep path intact
	

    signal(SIGINT,handler_sigint);          // signal to kill processes 

	while(1) {			
		/* BEGIN: TAKING INPUT */
		
		strcat(path,"$");
		bzero(line, sizeof(line));
		printf("%s",path);
		scanf("%[^\n]", line);
		getchar();

        
   
		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
	
		if(tokens[0]==NULL){path[strlen(path)-1]='\0';continue;} 

        if(strcmp(tokens[0],"&")==0){
            path[strlen(path)-1]='\0'; 
            
            continue;
        }


		else if(strcmp(tokens[0],"cd")==0){          
			
			if(strcmp(tokens[1],"..")==0){   //if cd .. command given 
			if(strlen(path)>2){
				if(chdir("..")==0)

				{	
					path[strlen(path)-1]='\0'; 
					path[strlen(path)-1]='\0'; 
					int i=strlen(path);
					i--;
					while(path[i]!='/'){path[i]='\0';i--;}			
					continue;
				
				}
				else {printf("can't change directory!");continue;}

			}
			else{
				printf("can not go behind it!\n");
				path[strlen(path)-1]='\0'; 
				continue;
			}
			
			}else{
			int r=chdir(tokens[1]);  		// cd path to change path
			if(r==0)
			{path[strlen(path)-1]='\0';   	//-1 to remove /
			
			strcat(path,tokens[1]);
			strcat(path,"/");}
			else{
				printf("Wrong path\n");
				path[strlen(path)-1]='\0';
				continue;
			
			}
		}

		}

		else{

       int BG=0;                                   //checking if it is a background process
        while(tokens[i]!=NULL){
            if(strcmp(tokens[i],"&")==0){
                BG=1;
            }
            i++;
        }


        
        int len=0;                                  //Mantaining array of all child processes
        int pidArr[MAX_INPUT_SIZE];
        for(int i=0;i<(sizeof(pidArr)/sizeof(pidArr[0]));i++){
            if(pidArr[i]=='\0')break;
            else len++;
        }


      


        if(strcmp(tokens[0],"exit")==0){            //Killing all the processes on getting exit command
            for(int i=0;i<=len;i++){
                kill(pidArr[i],SIGKILL);               
            }
        }


        if(BG==1){
            tokens[i-1]='\0';
        }

        int status;

        int fk=fork();

        pidArr[len]=fk;            //Adding every process id to array,so that all can be killed by "exit" command

        
        
        if(BG==0){
            setpgid(fk,fk);
            
            pidT=fk;
            
        }

        if(BG==1){                        //making different group for each background process also
            setpgid(fk,fk);
         
        }

 


        int wc=0,pid=0;
   
        
	    if(fk<0){
			fprintf(stderr,"%s\n","No chid process created!\n");
		}      

        else if(fk==0){

            execvp(tokens[0],tokens);             //execvp take file and args,execv() other takes path and args 
			printf("Command execution falied!!\n");}
        else{
            if(BG==1)
                {
                
                waitpid(fk,&status,WNOHANG);
             
            
                    
                }
            else{
                int wc =wait(NULL);
                int status;
                int pid;
                do{
                    pid=waitpid(fk,&status,WNOHANG);
                    if(pid==0){continue;}
                    else{
                        if(pid>0)
                        printf("Shell: Background process finished\n");
                    }
                }while(pid==0);

                //path[strlen(path)-1]='\0';
                
            }
         }
        
            path[strlen(path)-1]='\0';
		
		}
       //do whatever you want with the commands, here we just print them

		// for(i=0;tokens[i]!=NULL;i++){
		// 	printf("found token %s (remove this debug output later)\n", tokens[i]);
		// }
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	
	}
	return 0;
}
