#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
	int fd, num_proc, num_block,block_size;
	int pid,status, ritorno;
	int expected,calculated;
	char error[BUFSIZ];	

	if(argc != 4){
		sprintf(error,"Number of parameter wrong");
		perror(error);
		exit(1);
	}else{
		/*Check if the passed parameters are integer*/
		char * e1, *e2, *e3;
		long tmp1=strtol(argv[1],&e1,10);
		long tmp2=strtol(argv[2],&e2,10);
		long tmp3=strtol(argv[3],&e3,10);

		if( *e1=='\0' && *e2=='\0' && *e3=='\0'){
			/*All the passed paremeter are integers, good!!*/
			num_proc= (int) tmp1;
			num_block= (int) tmp2;
			block_size= (int) tmp3;
			if(num_proc <= 0 || num_block <=0 || block_size <=0){
				sprintf(error,"The value of the parameters are negative or zero");
				exit(2);	
			}
		}else{
			sprintf(error,"The passed parameters are not all integers");
			perror(error);
			exit(3);
		}
	}

	char buffer[block_size];

	
	for(int i=0;i<num_proc;i++){
		if((pid=fork()) < 0 ){
			perror("fork");
			exit(4);
		}

		if(pid==0){
			/*We are in the sons process, each of them must write nblocks*blocksize byte on standard output*/
			for(int k=0; k<block_size; k++){
				buffer[k]= 'a' + getpid() % 26;
			}

			/*Write the buffer num_block time in the standard output*/
			for(int k=0; k<num_block; k++){
				if (write(1,buffer,block_size) != block_size){
					perror("write");
					exit(-1);
				}
			}
			exit(0);
		}
	}

	/*PARENT's CODE*/
	for(int j=0; j<num_proc;j++){
		pid=wait(&status);
		if(pid<0){
			perror("Errore in wait\n");
			exit(5);
		}
		if((status & 0xFF) != 0){
			sprintf(error,"Child with pid %d is tarminates in anamalous way",pid);
			perror(error);
		}else{
			ritorno=(int) ((status>>8) & 0xFF);
			sprintf(error,"The child with pid %d has returned %d\n",pid,ritorno);
			perror(error);
		}
	}

	/*Check the final lenght of the file respect to the lenght it should be*/
	expected=block_size*num_block*num_proc;
	sprintf(error, "Lunghezza che dovrebbe avere il file: %10lld\n", expected);
    perror(error);
    calculated=lseek(1, 0L, SEEK_END);
    sprintf(error, "Lunghezza attuale file: %10lld\n", (long long) calculated);
    perror(error);
    sprintf(error, "Differenza: %10lld\n", expected - calculated);
    perror(error);

	exit(0);
}
