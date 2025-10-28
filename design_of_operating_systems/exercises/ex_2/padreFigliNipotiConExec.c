#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define extension ".sort"
#define PERM 0644

int main(int argc, char **argv){
	int pid, fdw, status, ritorno;
	int pidFiglio;
	int number_file = argc - 1;
	char *file_name;
	char error[BUFSIZ];
	
	/* check on the number of parameters: at least one file name */
	if(argc < 2){
		sprintf(error, "Error: wrong number of parameters! Parameters passed to %s are only %d\n", argv[0], argc);
		perror(error);
		exit(1);
	}
	
	printf("I am the parent process with pid %d and I will create %d child processes, each generating a grandchild\n", getpid(), number_file);
	
	/* create children - each child will be associated with the corresponding file */
	for(int i = 0; i < number_file; i++){
		if((pid = fork()) < 0){
			sprintf(error, "Error during fork creating child %d\n", i);
			perror(error);
			exit(2);
		}
		else if(pid == 0){
			/* child code */
			printf("I am child %d with index %d\n", getpid(), i);
			
			/* in case of error (both in children and grandchildren) we decide to return -1 
			   which will be interpreted by the parent as 255 and therefore an invalid value! */
			
			/* children must create the specified file */
			file_name = (char *)malloc(strlen(argv[i+1]) + 6); /* allocate string: filename + '.' + "sort" (4 chars) + '\0' */
			if(file_name == NULL){
				sprintf(error, "Error in malloc\n");
				perror(error);
				exit(-1);
			}
			
			/* copy the filename associated with this child */
			strcpy(file_name, argv[i+1]);
			/* concatenate the extension specified in the requirements */
			strcat(file_name, extension);
			
			/* create the output file */
			fdw = creat(file_name, PERM);
			if(fdw < 0){
				sprintf(error, "Cannot create file %s\n", file_name);
				perror(error);
				exit(-1);
			}
			/* close the created file that the child doesn't use */
			close(fdw);
			
			/* each child creates a grandchild */
			if((pid = fork()) < 0){
				sprintf(error, "Error in fork creating grandchild\n");
				perror(error);
				exit(-1);
			}
			
			if(pid == 0){
				/* grandchild code */
				printf("I am the grandchild process of child with index %d and pid %d, about to execute sort command for file %s\n", i, getpid(), argv[i+1]);
				
				/* the specification says to use the sort FILTER, so we need to redirect both standard input and standard output */
				
				/* close standard input */
				close(0);
				/* open the associated file in read-only mode */
				if(open(argv[i+1], O_RDONLY) < 0){
					sprintf(error, "Error: FILE %s DOES NOT EXIST\n", argv[i+1]);
					perror(error);
					exit(-1);
				}
				
				/* close standard output */
				close(1);
				/* open the created file in write-only mode */
				if(open(file_name, O_WRONLY) < 0){
					sprintf(error, "Error: FILE %s cannot be opened for writing\n", file_name);
					perror(error);
					exit(-1);
				}
				
				/* The grandchild becomes the sort command: must use exec versions with 'p' at the end 
				   to use the PATH environment variable: NO parameters needed */
				execlp("sort", "sort", (char *)0);
				
				/* Should never return here!! */
				perror("Problems executing sort by grandchild");
				exit(-1);
			}
			
			/* the child must wait for the grandchild to return to the parent the value returned by the grandchild */
			pid = wait(&status);
			if(pid < 0){
				sprintf(error, "Error in wait\n");
				perror(error);
				exit(-1);
			}
			
			if((status & 0xFF) != 0){
				sprintf(error, "Grandchild with pid %d terminated abnormally\n", pid);
				perror(error);
				exit(-1);
			}
			else {
				ritorno = (int)((status >> 8) & 0xFF);
			}
			
			/* the child returns the value received from the grandchild */
			exit(ritorno);
		}
	} /* end for */
	
	/* parent code */
	/* The parent waits for the children */
	for(int i = 0; i < number_file; i++){
		pidFiglio = wait(&status);
		if(pidFiglio < 0){
			sprintf(error, "Error in wait\n");
			perror(error);
			exit(3);
		}
		
		if((status & 0xFF) != 0)
			printf("Child with pid %d terminated abnormally\n", pidFiglio);
		else {
			ritorno = (int)((status >> 8) & 0xFF);
			printf("Child with pid=%d returned %d (if 255 there are problems!)\n", pidFiglio, ritorno);
		}
	}
	
	exit(0);
}
