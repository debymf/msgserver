#include "cliente.h"
#include<errno.h>

int main()
{
	int pid;
	int i;
	int key1;
	
	/* Cada processo tera uma fila para receber e enviar as mensagens */
	for (i=0;i<9;i++){
		if ((idFila[i] = msgget(key1 = ftok("cliente.c", i), IPC_CREAT|0x1B6)) < 0)
		{
			printf("Erro na criacao de filas.\n");
			exit(1);
		}
	}
		
	/* Somente o processo inicial cria outros 8 processos */
	for (i=0;i<8;i++){
		if (( pid = fork()) == 0) /* Se o processo for um filho, ele sai do loop */
		{
			break;
		}
		else{
			if (pid < 0){ /* Se ocorrer um erro na criacao do processo, encerra */
				printf("Erro na criacao dos processos.");
				exit(1);
				
			}
		}
	}

	solicitarEntrada(getpid()%9);
	
	/*Remove as filas criadas*/
	
	
	if (getpid()%9==0){
		sleep(20);
		for (i=0;i<9;i++){
			msgctl(idFila[i], IPC_RMID, NULL);
		}
		
	}
	/*
	if (pid == 0)
	printf("sou o processo filho\n");
	else
	{
	printf("sou o processo pai\n");
	sleep(20);
	}
	*/
	return 0;
}

/*Funcao para solicitar entrada na pool para escrita, somente 5 processos conseguirao entrar */
int solicitarEntrada(int pid){
	Msg1Struct msg1_env;
	Msg1Struct  msg1_recv;

	int totalReceber=0;
	int processoDestino;
	int i;

	if (pid!=0){
		/*Calcula a quantidade que cada processo tera que receber e enviar dos processos mais abaixo e processos a direita*/
		if (pid-6<0){
			totalReceber=totalReceber+1;
			if (pid-3<0){
				totalReceber=totalReceber+1;
				if ((pid%3)==1){
					totalReceber = totalReceber +3;
				}
			}
		}
		
		/* Calcula o processo destino*/
		if (pid-3>=0){
			processoDestino = pid-3;
		}
		else{
			processoDestino = pid%6-1;
		}
		
		msg1_env.type = pid;
		
		/* 1 indica que o processo esta pedindo autorizacao */
		msg1_env.msg.autoriza=1;
		msg1_env.msg.pid=pid;
		/*Envia a msg para o processo Destino*/
		msgsnd(idFila[processoDestino], &msg1_env, sizeof(msg1_env.msg), 0);

		/* Cada processo deverá receber as msgs correspondentes e passar pra frente*/
		for (i=0;i<totalReceber;i++){
			msgrcv(idFila[pid], &msg1_recv, sizeof(msg1_recv.msg), 0, 0);
			msgsnd(idFila[processoDestino], &msg1_recv, sizeof(msg1_recv.msg), 0);
		}
	
	}
	else{
		for(i=0;i<8;i++){
			msgrcv(idFila[0], &msg1_env, sizeof(msg1_env.msg), 0, 0);
			printf("\nRecebo de: %d, Autorizacao: %d\n", msg1_env.msg.pid, msg1_env.msg.autoriza); 
		}

	}
	
}
     