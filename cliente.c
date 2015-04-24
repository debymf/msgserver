/**
 * Modulo cliente.
 * 
 * Deborah Mendes - 09/0042735
 * Jeovah Fialho - 09/97188
 */

#include "cliente.h"
#include <errno.h>


int main()
{
	int pid;
	int pidAbstrato;
	int i;
	int autorizacao;
	int saida;
	
	
	/* As filas seguintes sao utilizadas para comunicacao entre o servidor e o cliente*/
	if ((filaE1 = msgget(ftok("main.c", 1), IPC_CREAT|0x1B6)) < 0)
	{
		printf("Erro na criacao da fila de entrada.\n");
		exit(1);
	}
	if ((filaE2 = msgget(ftok("main.c", 2), IPC_CREAT|0x1B6)) < 0)
	{
		printf("Erro na criacao da fila de entrada.\n");
		exit(1);
	}
	if ((filaM = msgget(ftok("main.c", 3), IPC_CREAT|0x1B6)) < 0)
	{
		printf("Erro na criacao da fila de entrada.\n");
		exit(1);
	}
	
	/* Cada processo tera uma fila para receber e enviar as mensagens */
	for (i=0;i<9;i++){
		if ((idFilaE1[i] = msgget(ftok("cliente.c", i), IPC_CREAT|0x1B6)) < 0)
		{
			printf("Erro na criacao de filas dos processos.\n");
			exit(1);
		}
	}
	for (i=0;i<9;i++){
		if ((idFilaE2[i] = msgget(ftok("cliente.c", i+9), IPC_CREAT|0x1B6)) < 0)
		{
			printf("Erro na criacao de filas dos processos.\n");
			exit(1);
		}
	}
	
	for (i=0;i<9;i++){
		if ((idFilaM[i] = msgget(ftok("cliente.c", i+18), IPC_CREAT|0x1B6)) < 0)
		{
			printf("Erro na criacao de filas dos processos.\n");
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
	autorizacao = solicitarEntrada(pidAbstrato);
	
	if (autorizacao==0){
		printf("Processo %d nao conseguiu acessar o servidor.\n", pidAbstrato);
	}
	else{
		printf("Processo %d conseguiu acesso ao servidor.\n", pidAbstrato);
	}
	
	
	enviarMsg(pidAbstrato,autorizacao);
	
	
	saida = solicitarSaida(pidAbstrato, autorizacao);
	
	if(autorizacao==1){
		if (saida==2){
			printf("Processo %d conseguiu sair do servidor.\n", pidAbstrato);
		}
		else{
			printf("Processo %d não conseguiu sair do servidor.\n", pidAbstrato);
		}
	}

	/*Remove as filas criadas*/
	if (getpid()%9==0){
		sleep(5);
		for (i=0;i<9;i++){
			msgctl(idFilaE1[i], IPC_RMID, NULL);
			msgctl(idFilaE2[i], IPC_RMID, NULL);
		}
		for (i=0;i<9;i++){
			msgctl(idFilaM[i], IPC_RMID, NULL);
		}

		msgctl(filaE1, IPC_RMID, NULL);
		msgctl(filaE2, IPC_RMID, NULL);
		msgctl(filaM, IPC_RMID, NULL);
		
	}

	return 0;
}

/*Funcao para solicitar entrada na pool para escrita, somente 5 processos conseguirao entrar */
int solicitarEntrada(int pid){
	struct inServer msg1_env;
	struct inServer msg1_recv;
	struct inServer msg2_recv;

	int totalReceber=0;
	int processoDestino;
	int processoDestInverso;
	int retorno;
	int i;

	msg1_env.type = getpid();
	msg1_env.pid = pid;
	msg1_env.auth = 1;
	
	
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
		
		/*Envia a msg para o processo destino*/
		if(msgsnd(idFilaE1[processoDestino], &msg1_env, sizeof(msg1_env) - sizeof(long), 0)<0){
			printf("Erro ao enviar pedido de cadastro para processo superior.\n");
			perror("msgsnd");
			//exit(1);
		}

		/* Cada processo deverá receber as msgs correspondentes e passar pra frente*/
		for (i=0;i<totalReceber;i++){
			if(msgrcv(idFilaE1[pid], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0, 0)<0){
				printf("Erro ao receber pedido de cadastro do processo inferior.\n");
				perror("msgrcv");
				exit(1);
			}
			if(msgsnd(idFilaE1[processoDestino], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
				printf("Erro ao enviar pedido de cadastro para processo superior.\n");
				perror("msgsnd");
				//exit(1);
			}
		}	
		
		/* Agora cada processo deve receber a resposta se conseguiu entrar na pool ou nao */
		
		for (i=0;i<totalReceber+1;i++){
			if(msgrcv(idFilaE2[pid], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0, 0)<0){
				printf("Erro ao receber confirmacao de cadastro do processo superior.\n");
				perror("msgrcv");
				exit(1);
			}
		
			if (msg1_recv.pid == pid){
				retorno = msg1_recv.auth;
			}
			else{
				if ((pid%3) == ((msg1_recv.pid)%3)){
					if(msgsnd(idFilaE2[pid+3],&msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
						printf("Erro ao enviar confirmacao de cadastro para processo inferior.\n");
						perror("msgsnd");
						//exit(1);
					}
				}
				else{
					if(msgsnd(idFilaE2[pid+1], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
						printf("Erro ao enviar confirmacao de cadastro para processo inferior\n");
						perror("msgsnd");
						//exit(1);
					}
				}
			
			}

	
		}
	/* O processo 0 faz a comunicacao com o servidor de impressao*/
	}else{
		if(msgsnd(filaE1,&msg1_env, sizeof(msg1_env) - sizeof(long), 0)<0){
			printf("Erro ao enviar pedido de cadastro para o servidor de impressao.\n");
			perror("msgsnd");
			//exit(1);	
			
		}
		if(msgrcv(filaE2, &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
 				printf("Erro ao receber pedido de cadastro do processo inferior.\n");
 				perror("msgrcv");
 				//exit(1);
 		}
		retorno = msg2_recv.auth;
		for(i=0;i<8;i++){
			/* O processo 0 recebe todos os outros processos*/
			if(msgrcv(idFilaE1[0], &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
				printf("Erro ao receber pedido de cadastro do processo inferior.\n");
				perror("msgrcv");
				exit(1);
			}
			/* Envia para o servidor de impressao para pedir entrada no servidor */
			if(msgsnd(filaE1,&msg2_recv, sizeof(msg2_recv) - sizeof(long), 0)<0){
				printf("Erro ao enviar pedido de cadastro para o servidor de impressao.\n");
				perror("msgsnd");
				//exit(1);	
			}

			/* Retorna se o processo foi autorizado ou nao a entrar */
 			if(msgrcv(filaE2, &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
 				printf("Erro ao receber pedido de cadastro do processo inferior.\n");
 				perror("msgrcv");
 				//exit(1);
 			}

 			/* O processo 0 encaminha as respostas de volta*/
			if (msg2_recv.pid%3==0){
				processoDestInverso = 3;
			}
			else{
				processoDestInverso = 1;
			}

			if(msgsnd(idFilaE2[processoDestInverso], &msg2_recv, sizeof(msg2_recv) - sizeof(long), 0)<0){
				printf("Erro ao enviar confirmacao de cadastro para processo inferior.\n");
				perror("msgsnd");
				//exit(1);
			}
		}
	}

	/* Retorna se o processo foi autorizado ou nao a entrar na pool */
	return retorno;
} 
 
void enviarMsg(int pid, int autorizacao){
	struct msgServer msg1_env;
	struct msgServer msg1_recv;
	struct msgServer msg2_recv;

	int totalReceber=0;
	int processoDestino;
	int processoDestInverso;
	int retorno;
	int i;
	char msgEnviada[50];

	msg1_env.type = getpid();
	msg1_env.pid = pid;
	msg1_env.auth = autorizacao;
	sprintf(msgEnviada, "Ola! Sou o processo %d.", pid);
	strcpy(msg1_env.buf, msgEnviada);
	
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
		
		/*Envia a msg para o processo destino*/
		if(msgsnd(idFilaM[processoDestino], &msg1_env, sizeof(msg1_env) - sizeof(long), 0)<0){
			printf("Erro ao enviar msg para processo superior.\n");
			perror("msgsnd");
			//exit(1);
		}

		/* Cada processo deverá receber as msgs correspondentes e passar pra frente*/
		for (i=0;i<totalReceber;i++){
			if(msgrcv(idFilaM[pid], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0, 0)<0){
				printf("Erro ao receber msg do processo inferior.\n");
				perror("msgrcv");
				exit(1);
			}
			if(msgsnd(idFilaM[processoDestino], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
				printf("Erro ao enviar msg para processo superior.\n");
				perror("msgsnd");
				//exit(1);
			}
		}	
	}else{
		if(msg1_env.auth==1){
			if(msgsnd(filaM,&msg1_env, sizeof(msg1_env) - sizeof(long), 0)<0){
				printf("Erro ao enviar msg para o servidor de impressao.\n");
				perror("msgsnd");
				//exit(1);	
			}
		}

		for(i=0;i<8;i++){
			/* O processo 0 recebe todos os outros processos*/
			if(msgrcv(idFilaM[0], &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
				printf("Erro ao receber msg do processo inferior.\n");
				perror("msgrcv");
				exit(1);
			}
			/* Envia para o servidor de impressao para imprimir a msg servidor se ele tiver autorizacao*/
			if(msg2_recv.auth==1){
				if(msgsnd(filaM,&msg2_recv, sizeof(msg2_recv) - sizeof(long), 0)<0){
					printf("Erro ao enviar msg para o servidor de impressao.\n");
					perror("msgsnd");
					//exit(1);	
				}
			}
		}
	}	
}

int solicitarSaida(int pid, int autorizacao){
	struct inServer msg1_env;
	struct inServer msg1_recv;
	struct inServer msg2_recv;

	int totalReceber=0;
	int processoDestino;
	int processoDestInverso;
	int retorno;
	int i;

	msg1_env.type = getpid();
	msg1_env.pid = pid;
	msg1_env.auth = autorizacao;
	
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
		
		/*Envia a msg para o processo destino*/
		if(msgsnd(idFilaE1[processoDestino], &msg1_env, sizeof(msg1_env) - sizeof(long), 0)<0){
			printf("Erro ao enviar pedido de remocao para processo superior.\n");
			perror("msgsnd");
			//exit(1);
		}

		/* Cada processo deverá receber as msgs correspondentes e passar pra frente*/
		for (i=0;i<totalReceber;i++){
			if(msgrcv(idFilaE1[pid], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0, 0)<0){
				printf("Erro ao receber pedido de remocao ao processo inferior.\n");
				perror("msgrcv");
				exit(1);
			}
			if(msgsnd(idFilaE1[processoDestino], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
				printf("Erro ao enviar remocao para processo superior.\n");
				perror("msgsnd");
				//exit(1);
			}
		}	
		
		/* Agora cada processo deve receber a resposta se foi removido*/

		for (i=0;i<totalReceber;i++){
			if(msgrcv(idFilaE2[pid], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0, 0)<0){
				printf("Erro ao receber confirmacao de cadastro do processo superior.\n");
				perror("msgrcv");
				exit(1);
			}
			if (msg1_recv.pid == pid){
				retorno = msg1_recv.auth;
			}
			else{
				if ((pid%3) == ((msg1_recv.pid)%3)){
					if(msgsnd(idFilaE2[pid+3],&msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
						printf("Erro ao enviar confirmacao de cadastro para processo inferior\n");
						perror("msgsnd");
						//exit(1);
					}
				}
				else{
					if(msgsnd(idFilaE2[pid+1], &msg1_recv, sizeof(msg1_env) - sizeof(long), 0)<0){
						printf("Erro ao enviar confirmacao de remocao para processo inferior\n");
						perror("msgsnd");
						//exit(1);
					}
				}
			
			}

	
		}
	/* O processo 0 faz a comunicacao com o servidor de impressao*/
	}else{
		if(msg1_env.auth==1){
			if(msgsnd(filaE1,&msg1_env, sizeof(msg1_env) - sizeof(long), 0)<0){
				printf("Erro ao enviar pedido de cadastro para o servidor de impressao.\n", msg1_env.pid);
				perror("msgsnd");
				//exit(1);	
				
			}
			if(msgrcv(filaE2, &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
					printf(" Erro ao receber pedido de cadastro do processo inferior.\n");
					perror("msgrcv");
					//exit(1);
			}
		}
		retorno = msg2_recv.auth;
		for(i=0;i<8;i++){
			/* O processo 0 recebe todos os outros processos*/
			if(msgrcv(idFilaE1[0], &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
				printf("Erro ao receber remocao do processo inferior.\n");
				perror("msgrcv");
				exit(1);
			}
			if(msg2_recv.auth==1){
				/* Envia para o servidor de impressao para pedir entrada no servidor */
				if(msgsnd(filaE1,&msg2_recv, sizeof(msg2_recv) - sizeof(long), 0)<0){
					printf("Erro ao enviar pedido remocao para o servidor de impressao.\n", msg2_recv.pid);
					perror("msgsnd");
					//exit(1);	
				}

				/* Retorna se o processo foi autorizado ou nao a entrar */
				if(msgrcv(filaE2, &msg2_recv, sizeof(msg2_recv)- sizeof(long), 0, 0)<0){
					printf(" Erro ao receber pedido de remocao do processo inferior.\n");
					perror("msgrcv");
					//exit(1);
				}
			}
			/* O processo 0 encaminha as respostas de volta*/
			if (msg2_recv.pid%3==0){
				processoDestInverso = 3;
			}
			else{
				processoDestInverso = 1;
			}

			if(msgsnd(idFilaE2[processoDestInverso], &msg2_recv, sizeof(msg2_recv) - sizeof(long), 0)<0){
				printf("Erro ao enviar confirmacao de remocao para processo inferior.\n");
				perror("msgsnd");
				//exit(1);
			}
			
		}
	}

	/* Retorna se o processo conseguiu ou nao sair da pool */
	return retorno;
	
}
