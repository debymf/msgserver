#include <stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
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


int idFilaE1[9];
int idFilaE2[9];
int idFilaM[9];

int filaE1;
int filaE2;
int filaM;

int solicitarEntrada(int pid);
void enviarMsg(int pid, int autorizacao);
int solicitarSaida(int pid, int autorizacao);

