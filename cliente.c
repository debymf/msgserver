#include "cliente.h"

int main()
{
	int pid;
	int i;
		
	/* Somente o processo inicial cria outros 8 processos */
	for (i=0;i<8;i++){
		
		if (( pid = fork()) == 0) /* Se o processo for um filho, ele sai do loop */
		{
			break;
		}
		else{
			if (pid < 0){ /* Se ocorrer um erro na criacao do processo, encerra */
				printf("Erro na criacao dos processos");
				
			}
		}
	}
	
	if (pid == 0)
	printf("sou o processo filho\n");
	else
	{
	printf("sou o processo pai\n");
	sleep(20);
	}
	
	return 0;
}
     