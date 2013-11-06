#include <stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

/* Estrutura para pedir autorizacao ou retirada*/
typedef struct
{
	long type;
	struct _msg_struct1
	{
		int pid;
		int autoriza;
	} msg;
} Msg1Struct;

/* Estrutura para enviar a mensagem*/
typedef struct
{
	long type;
	struct _msg_struct2
	{
		int pid;
		char buf[50];
	} msg;
} Msg2Struct;

/* Variavel que ira guardar as filas para cada processo*/
int idFila[9];

int solicitarEntrada(int pid);
void enviarMsg(int pid);

