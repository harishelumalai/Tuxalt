#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "core.h"
#include "mqmgr.h"

int create_mq(int key, int* mqid);      
int open_mq(int key, int *mqid);
int write_to_mq(int mqid, MQMSG *msg); 
int read_from_mq(int mqid, MQMSG *msg);
int close_mq(int mqid);                

int create_mq(int key, int *mqid)
{
	if(*mqid = msgget(key, 0666|IPC_CREAT) <0)
	{
		fprintf(OUTFILE, "Unable to create queue for key[%d]\n", key);
		return FAIL;
	}
	return *mqid;
}

int open_mq(int key, int *mqid)
{
	if(*mqid = msgget(key, 0666|IPC_CREAT|IPC_EXCL) <0)
	{
		fprintf(OUTFILE, "Unable to open queue for key[%d]\n", key);
		return FAIL;
	}
	return *mqid;
}

int write_to_mq(int mqid, MQMSG *msg)
{
	int 	type;
	if(0>mqid)
	{
		if(msgsnd(mqid, msg, sizeof(MQMSG), IPC_NOWAIT)<0)
		{
			fprintf(OUTFILE, "Unable to write to mq[%d]\n", mqid);
		}
	}
	else
		fprintf(OUTFILE, "WRITE: Invalid mqid [%d]\n", mqid);
}

int read_from_mq(int mqid, MQMSG *msg)
{
	int	type=GLOBAL_TYPE;
	if(0>mqid)
	{
		if(msgrcv(mqid, msg, sizeof(MQMSG), type, 0)<0)
		{
			fprintf(OUTFILE, "Unable to read from mq[%d]\n", mqid);
		}
	}
	else
		fprintf(OUTFILE, "READ: Invalid mqid [%d]\n", mqid);
}

int close_mq(int mqid)
{
	if(0>mqid)
	{
		msgctl(mqid, IPC_RMID, NULL);
	}
	else
		fprintf(OUTFILE, "CLOSE: Invalid mqid [%d]\n", mqid);
}
