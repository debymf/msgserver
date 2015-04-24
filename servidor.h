#include <stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<string.h>


struct inServer
{
	long type;
	int pid;
	int auth;
};

struct msgServer
{
	long type;
	int pid;
	int auth;
	char buf[50];
};


int filaE1;
int filaE2;
int filaM;

int num_clientes=0;

int clientes_cadastrados[5]={0};

void	inserirClientes();
void	imprimirMsgs();
void	removerClientes();