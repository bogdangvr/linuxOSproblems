#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

void collatz (int *buffer, int x){
	buffer[0]=x;
	int last=1;
	while(x!=1){
		if (x%2==0){
			x=x/2;
		}
		else{
			x=x*3+1;
		}
		buffer[last]=x;
		last++;
	}
	buffer[last]=1;
	return;
}

void afisare(int *buffer){
	int i=0;
	printf("%d:",buffer[i]);
	while (buffer[i]!=1){
		printf("%d ",buffer[i]);	
		i++;
	}
	printf("1");
	printf("\n");
}

int main (int argc, const char *argv[]){
	char shm_name[] = "/collatz"; 
	int shm_fd; ///descriptorul folosit pentru manipularea obiectului
	///creem obiectul cu memorie partajata
	shm_fd = shm_open(shm_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	if (shm_fd<0){
		perror(NULL);
		return errno;
	}	
	///dimensiunea pe care dorim sa o aiba memowria partajata
	size_t shm_size = 1024 * argc;
	///definim dimensiunea
	if (ftruncate(shm_fd, shm_size) == -1){
		perror(NULL);
		///sterge obiectele create cu shm_open
		shm_unlink(shm_name);
		return errno;
	}

	printf("Starting parent %d\n",getpid());
	for (int i=1; i<argc; i++){
		int x = atoi(argv[i]);
		///creez proces
		pid_t pid=fork();
		if (pid<0){
			return errno;
		}
		else{
			if (pid==0){
				///setam un pointer catre adresa la care a fost incarcat obiectul
				int *addr = mmap(0, shm_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
				if (addr == MAP_FAILED){
					perror(NULL);
					shm_unlink(shm_name);
					return errno;
				}
				///mutam bufferul in functie de zona pe care vrem sa o accesam ca sa nu intram in alta serie collatz
				int *buffer = addr+(i-1)*1024/sizeof(int);
				collatz(buffer,x);
				printf("Done Parent %d Me %d\n", getppid(), getpid());
				return 0;
			}
		}
	}

	while(wait(NULL)>0);
	int *addr = mmap(0, shm_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	for (int i=1; i<argc; i++){
		afisare(addr+(i-1)*1024/sizeof(int));	
	}
	///eliberam zona de memorie incarcata anterior
	munmap(addr, shm_size);
	///stergem obiectul cread cu shm_open
	shm_unlink("/collatz");
	printf("Done Parent %d Me %d\n", getppid(), getpid());
	return 0;
}
