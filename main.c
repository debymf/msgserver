/**
 * Modulo de execucao.
 * 
 * Déborah Mendes - 09/0042735
 * Jeovah Fialho - 09/97188
 */

#include "main.h"

int main(){
	int i;
	int pid;

	if (( pid = fork()) < 0)
	{
		printf("Erro no fork.\n");
		exit(1);
	}
	if (pid == 0)
	{
		if (execl("./servidor", "servidor",  (char *) 0) < 0)
			printf("Erro no Execl = %d\n", errno);
			exit(1);
	}
	else
	{
		if (( pid = fork()) < 0)
		{
			printf("Erro no fork.\n");
			exit(1);
		}
		if (pid == 0){
			sleep(1);
			if (execl("./cliente", "cliente",  (char *) 0) < 0)
				printf("Erro no Execl = %d\n", errno);
				exit(1);
		}
	}
	
	wait(1);
	
	/*Cria o processo servidor*/
	
	return 0;
}