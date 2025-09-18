#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char**argv){
    int fd=0; int n_line=10;
    char error[BUFSIZ];	
    int nread=0;
    char buffer[1];
    int count=0;

    if (argc > 3) 
    {	sprintf(error, "Errore nel numero di parametri dato che argc=%d\n", argc);
        perror(error);
            exit(1); 
    }

    if(argc==2){
        char *end;
        long tmp = strtol(argv[1], &end, 10);

        if (*end == '\0') {
            // tutta la stringa è stata convertita -> numero valido
            n_line = (int) tmp;
        } else {
            // c’è altro dopo le cifre -> probabilmente è un nome file
            if ((fd = open(argv[1], O_RDONLY)) < 0) {
                sprintf(error, "Errore nell'apertura del file\n");
                exit(1);
            }
        }
    }

    if(argc==3){
        char *end;
        long tmp = strtol(argv[2], &end, 10);

        if (*end == '\0') {
            // tutta la stringa è stata convertita -> numero valido
            n_line = (int) tmp;
        }else{
            sprintf(error,"Errore: il secondo parametro passato non è un numero\n");
            perror(error);
            exit(2);
        }
    

        if ((fd = open(argv[1], O_RDONLY)) < 0) {
                sprintf(error, "Errore nell'apertura del file\n");
                exit(1);
        }
    }

    while ((nread = read(fd, buffer, 1)) > 0 ){
            write(1, buffer, nread);
            if(buffer[0]=='\n'){count++;}
            if(count==n_line){break;}
    }

    exit(0);
    

}