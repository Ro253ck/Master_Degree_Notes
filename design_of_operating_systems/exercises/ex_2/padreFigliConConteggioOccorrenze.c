#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <string.h>


int main(int argc, char *argv[]) {

    int pid, status, ritorno,pidFiglio,fd;	
    size_t numero_figli;
    char error[200];	
    char buffer;

    /*Controllo se il numero di parametri passato è corretto*/
    if(argc < 4){
        sprintf(error,"Il numero di parametri passato è sbagliato\n");
        perror(error);
        exit(1);
    }

    /*Controllo se l'ultimpo parametro passato è un carattere*/
    if(strlen(argv[argc-1])>1){
        sprintf(error,"L'ultimo parametro passato non è un singolo carattere\n");
        perror(error);
        exit(2);
    }

    char carattere=argv[argc-1][0]; /*Prendo il carattere che devo contare*/

    /*Ciclo che genera N figli*/
    for(int i=0; i<(argc-2) ; i++){
        
        if((pid=fork()) < 0){
            sprintf(error,"Errore nella creazione del figlio numero %d",i);
            perror(error);
            exit(3);
        }else if(pid==0){

            /*Ho creato nella maniera corretta un figlio*/
            if((fd=open(argv[i+1],O_RDONLY))<0){
                sprintf(error,"Errore nell apertura del file\n");
                perror(error);
                exit(-1);
            }else{
                /*Leggo il file e confronto i caratteri*/
                int count=0;
                while((ritorno=read(fd,&buffer,1)) > 0){
                    if(carattere==buffer){
                        count++;
                    }
                }
                exit(count);
            }
        }
    }

    /*Ciclo padre che aspetta i figli*/
    for(int i=0;i<argc-2;i++){
        
        pidFiglio=wait(&status);
        if(pidFiglio < 0){
            sprintf(error, "Errore in wait\n");
		    perror(error);
                exit(4);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}