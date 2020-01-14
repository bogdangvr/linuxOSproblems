#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

int main (int argc, const char *argv[]){
	/*struct stat sb;
	if (stat(argv[1], &sb)){
		perror (argv[1]);
		return errno;	
	}	*/

	if (argc != 3){
		perror("Introduceti caile catre doua fisiere");
		return -1;
	}
	int src = open (argv[1],O_RDONLY);
	int dst = open (argv[2],O_WRONLY|O_CREAT,S_IRWXU);
	if( src < 0 || dst < 0){ 
		perror("Eroare la deschidere");
		return errno;
	}
	size_t nbytes=1024;
	char *buffer = malloc (nbytes);
	int readB;
	while (readB=read(src,buffer,nbytes)){
		if (write(dst,buffer,readB)!=readB){
			perror("Eroare la scriere");
			return errno;
		}	
	}
	if (readB < 0) { 
		perror("Eroare la citire"); 
		return errno; 
	}
	if(close(src) == -1){
		perror("Eroare la inchiderea fis1"); 
		return errno;
	}
	if(close(dst) == -1){
		perror("Eroare la inchiderea fis1"); 
		return errno;
	}
	return 0;
}
