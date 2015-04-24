/**
 * Modulo servidor de impressao.
 * 
 * Déborah Mendes - 09/0042735
 * Jeovah Fialho - 09/97188
 */

#include "servidor.h"

int main(){
	int key1;
	
	
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

	printf("Inicia servidor de impressao\n");
	inserirClientes();
	imprimirMsgs();
	removerClientes();
	
	sleep(10);
	msgctl(filaE1, IPC_RMID, NULL);
	msgctl(filaE2, IPC_RMID, NULL);
	msgctl(filaM, IPC_RMID, NULL);
	
	return 0;
	
}

void inserirClientes(){
	struct inServer msg1_recv;
	int i;
	
	for(i=0;i<9;i++){
		if(msgrcv(filaE1, &msg1_recv, sizeof(struct inServer) - sizeof(long), 0, 0)<0){
			printf("Erro ao receber cliente para cadastro.\n");
			perror("msgrcv");
			exit(1);
		}
		
		/*Se o numero de clientes cadastrados for maior que 5, significa que lotou, e os restantes nao serao autorizados*/
		if (i>4){
			msg1_recv.auth=0;
		}
		else{
			/*Cadastra o cliente na lista*/
			clientes_cadastrados[msg1_recv.pid]=1;
		}
;
		if(msgsnd(filaE2,&msg1_recv, sizeof(struct inServer) - sizeof(long), 0)<0){
			printf("Erro ao enviar resposta de cadastro ao cliente.\n");
			perror("msgsnd");
			//exit(1);	
		}

	}

}

void imprimirMsgs(){
	struct msgServer msg1_recv;
	int i;
	for(i=0;i<5;i++){
		if(msgrcv(filaM, &msg1_recv, sizeof(struct msgServer) - sizeof(long), 0, 0)<0){
			printf(" Erro ao receber cliente para imprimir msg.\n");
			perror("msgrcv");
			exit(1);
		}
		/*Se o cliente estiver cadastrado, imprime sua mensagem*/
		if (clientes_cadastrados[msg1_recv.pid]==1){
			printf("Mensagem: %s\n", msg1_recv.buf);
		}

	}

}

void removerClientes(){
	struct inServer msg1_recv;
	int i;
	
	for(i=0;i<5;i++){
		if(msgrcv(filaE1, &msg1_recv, sizeof(struct inServer) - sizeof(long), 0, 0)<0){
			printf("Erro ao receber cliente para retirada.\n");
			perror("msgrcv");
			exit(1);
		}

		/*Remove o cliente da lista e confirma que ele foi retirado*/
		clientes_cadastrados[msg1_recv.pid]=0;
		msg1_recv.auth=2;

		if(msgsnd(filaE2,&msg1_recv, sizeof(struct inServer) - sizeof(long), 0)<0){
			printf("Erro ao enviar resposta de remocao ao cliente.\n");
			perror("msgsnd");
			//exit(1);	
		}

	}

}
