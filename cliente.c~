#include "cliente.h"
#include <errno.h>

int main()
{
	int pid;
	int pidAbstrato;
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

	pidAbstrato = getpid()%9;
	/* Se solicitarEntrada retornar 1, o processo foi cadastrado */
	if (solicitarEntrada(pidAbstrato)==1){
		printf("Processo %d foicadastrado.\n", pidAbstrato);
	}
	else{
		/* Se nao conseguir cadastrar, ele encerra*/
		printf("Processo %d nao conseguiu se cadastrar.", pidAbstrato);
		exit(1);
	}
	
	/*Remove as filas criadas*/
	if (getpid()%9==0){
		sleep(20);
		for (i=0;i<9;i++){
			msgctl(idFila[i], IPC_RMID, NULL);
		}
		
	}

	return 0;
}

/*Funcao para solicitar entrada na pool para escrita, somente 5 processos conseguirao entrar */
int solicitarEntrada(int pid){
	Msg1Struct msg1_env;
	Msg1Struct msg2_env;
	Msg1Struct  msg1_recv;
	Msg1Struct  msg2_recv;


	int totalReceber=0;
	int processoDestino;
	int processoDestInverso;
	int retorno;
	int i;

	if (pid!=0){
		
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
		
		processoDestInverso=0;
		
		
		/*Agora cada processo deve receber a resposta se conseguiu entrar na pool ou nao*/
		for (i=0;i<totalReceber+1;i++){
			msgrcv(idFila[pid], &msg1_recv, sizeof(msg1_recv.msg), 0, 0);
			
			//printf("\nMeu pid: %d Pid do recebido: %d\n", pid,msg1_recv.msg.pid);
			if (msg1_recv.msg.pid == pid){
				retorno = msg1_recv.msg.autoriza;
			}
			else{
				if ((pid%3) == (msg1_recv.msg.pid%3)){
					msgsnd(idFila[pid+3], &msg1_recv, sizeof(msg1_recv.msg), 0);
				}
				else{
					msgsnd(idFila[pid+1], &msg1_recv, sizeof(msg1_recv.msg), 0);
				}
			}
		}
		
	
	}
	else{
		msg2_env.type = pid;
		msg2_env.msg.autoriza=1;
		msg2_env.msg.pid=pid;
		/* TODO Tenta enviar processo 0*/
		
		retorno = msg2_env.msg.autoriza;
		for(i=0;i<8;i++){
			msgrcv(idFila[0], &msg2_recv, sizeof(msg2_recv.msg), 0, 0);
			/* TODO Tenta enviar os outros processos*/
			processoDestInverso = 0;
			
			if (msg2_recv.msg.pid%3==0){
				processoDestInverso = 3;
			}
			else{
				processoDestInverso = 1;
			}
			msgsnd(idFila[processoDestInverso], &msg2_recv, sizeof(msg2_recv.msg), 0);
		}
	}
	
	/* Retorna se o processo foi autorizado ou nao a entrar na pool */
	return retorno;
	
}
     
/* Funcao que permite que o processo envie uma mensagem para o servidor de impressao*/
void enviarMsg(int pid){
	Msg2Struct msg1_env;
	Msg2Struct msg2_env;
	Msg2Struct  msg1_recv;
	Msg2Struct  msg2_recv;


	int totalReceber= 0;
	int processoDestino;
	int processoDestInverso;
	int retorno;
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
		
		/* Grava a mensagem que sera enviada*/
		strcpy(msg1_env.msg.buf, "Ola! Sou o processo %d", pid);
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
		msg2_env.type = pid;
		strcpy(msg1_env.msg.buf, "Ola! Sou o processo %d", pid);
		msg2_env.msg.pid=pid;
		/* TODO Tenta enviar processo 0*/
		
		for(i=0;i<8;i++){
			msgrcv(idFila[0], &msg2_recv, sizeof(msg2_recv.msg), 0, 0);
			/* TODO Tenta enviar as msgs*/
	
		}
	}
	
	
}