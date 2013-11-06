#include <stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

typedef struct
{
	long type;
	struct _msg_struct
	{
		int pid;
		int autoriza;
	} msg;
} Msg1Struct;



int idFila[9];

int solicitarEntrada(int pid);

