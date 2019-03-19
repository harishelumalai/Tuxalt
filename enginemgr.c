#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "core.h"
#include "cfgfunc.h"
#include "enginemgr.h"
#include "mqmgr.h"


#define	MAX_PROCESS	10

int	shmid;
char	*shmaddr;
char	*shmptr;
pid_t	pid[10];
int	status[10];

/* should use linked list or array? */
QUEUEROW_t	queuelist[MAX_PROCESS];
//PROCESSROW_t	processtable[MAX_PROCESS];

int create_shm(key_t shmkey, size_t shmsize);
int init_engine();
int write_to_shm(char *msg, int len);
int read_from_shm(char *buf, int *len);
int write_queues_to_shm(QUEUEROW_t *q, int count);
int read_queue_to_shm(QUEUEROW_t *q, int count);
int get_shmid();
void grace_exit(int signal);

int create_shm(int shmkey, size_t shmsize)
{
	fprintf(OUTFILE, "Creating shared memory\n");
	shmid = shmget(shmkey, shmsize, 0666|IPC_CREAT);

	if(shmid <= 0)
	{
		return FAIL;
	}
	return SUCCEED;
}

int init_engine()
{
	fprintf(OUTFILE, "Initializing engine\n");
	int 	ret=FAIL;
	int	server_count = 0;
	int	i = 0;

	/* Set Signal handler */
	signal(SIGINT, grace_exit);

	/* Read list of servers and queue keys from config */
	cf_get_servers(&queuelist[0], &server_count);

	fprintf(OUTFILE, "server count [%d]\n", server_count);
	/* Create Message queues for each server */
	for(; i<server_count; i++)
	{
		queuelist[i].mqid=create_mq(queuelist[i].qkey, &queuelist[i].mqid);
		if(queuelist[i].mqid <= 0)
			fprintf(OUTFILE, "Unable to create mq\n");
		else
			fprintf(OUTFILE, "Message Queue[%d] created for [%s]\n", queuelist[i].mqid, queuelist[i].svrname); 
	}

	/* Swarm server processes */
	for(; i<server_count; i++)
	{
		//fprintf(OUTFILE, "Fork and Exec [%s]\n",queuelist[i].svrname);
		pid[i]=fork();
		if(pid[i] == -1)
		{
			fprintf(OUTFILE, "Unable to fork.\n");
			grace_exit(FAIL);
		}
		else if(pid[i] == 0)
		{
			/* What Child process should do */
			char tmp[100]="";
			strcpy(tmp, "/home/e3026114/samples/tuxalt/bin/");
			strcat(tmp, queuelist[i].svrname);
			fprintf(OUTFILE, "From[%d] Running Execvp(%s)\n", getpid(), tmp);
			//execl(tmp, queuelist[i].svrname, NULL);
			execl(tmp, queuelist[i].svrname, "service1", NULL);
			fprintf(OUTFILE, "From[%d] errno[%d]\n", getpid(), errno);
			exit(errno);
		}
		else
		{
			fprintf(OUTFILE, "Spawn [%s] successful\n", queuelist[i].svrname);
			/*  Continue creating servers */
			continue;
		}
	}

	/* Create SHM */
	ret=create_shm(12345, sizeof(QUEUEROW_t)*10);

	/* Print Process table */
	fprintf(OUTFILE, "%50s\t%s\n", "SERVER NAME","Processid");
	for(i=0;i<server_count;i++)
	{
		fprintf(OUTFILE, "%50s\t%d\n", queuelist[i].svrname, pid[i]);
	}

	/* Print Queue table(Bulletin Board of Tuxedo) */
	fprintf(OUTFILE, "%50s\t%s\t\n", "SERVER NAME","QUEUEkey","QUEUEid");
	for(i=0;i<server_count;i++)
	{
		fprintf(OUTFILE, "%50s\t%d\t%d\n", queuelist[i].svrname, queuelist[i].qkey, queuelist[i].mqid);
	}


	/* Destroy all message queues */
	for(i=0;i<server_count;i++)
	{
		close_mq(queuelist[i].mqid);
	}

	/*
	fprintf(OUTFILE, "Waiting for processes to end\n");
	for(i=0;i<server_count;i++)
	{
		if(waitpid(pid[i], &status[i], 0)>0)
			fprintf(OUTFILE, "Terminated\t%s\t[%d]\n", queuelist[i].svrname, status[i]);
		else
			fprintf(OUTFILE, "Wait Failed\t%s\n", queuelist[i].svrname);
	}
	*/

	return ret;
}

void close_engine()
{
	fprintf(OUTFILE,"Closing Engine\n");
	if(shmid > 0)
	{
		shmctl(shmid, IPC_RMID, NULL);
	}
}

int write_to_shm(char *msg, int len)
{
	int	ret;
	/* Attach yourself to shared memory */
	if(SUCCEED == ret)
	{
		shmptr=(char*) shmat(shmid, (void*)0, 0);
	}
	if(shmptr)
	{
		memcpy(shmptr, msg, len);
	}
	else
	{
		fprintf(stdout, "Unable to write to shm\n");
		return FAIL;
	}
	/* Detach from shared memory */
	shmdt(shmptr);

	return SUCCEED;
}

int read_from_shm(char *buf, int *len)
{
	int	ret=FAIL;
	/* Attach yourself to shared memory */
	if(SUCCEED == ret)
	{
		shmptr=(char*) shmat(shmid, (void*)0, 0);
	}
	if(shmptr)
	{
		/* Copy data */
		strcpy(buf, shmptr);
	}
	else
	{
		fprintf(stdout, "Unable to read from shm\n");
		return FAIL;
	}
	/* Detach from shared memory */
	shmdt(shmptr);

	return SUCCEED;
}

int write_queues_to_shm(QUEUEROW_t *q, int count)
{
	fprintf(OUTFILE,"Writing Queue details to shm\n");
	int	ret=FAIL;
	int	i=0;
	if(SUCCEED == ret)
	{
		shmptr=(char*) shmat(shmid, (void*)0, 0);
	}
	if(shmptr)
	{
		for(;i<count;i++)
		{
			/* Copy data */
			memcpy(shmptr+(sizeof(QUEUEROW_t)*i), q+i, sizeof(QUEUEROW_t));
		}
	}
	else
	{
		fprintf(stdout, "Unable to read from shm\n");
		return FAIL;
	}
	/* Detach from shared memory */
	shmdt(shmptr);

	return SUCCEED;
}

int read_queue_to_shm(QUEUEROW_t *q, int count)
{
	fprintf(OUTFILE, "Reading QUEUE from shm\n");
	int	ret=FAIL;
	int	i=0;
	if(SUCCEED == ret)
	{
		shmptr=(char*) shmat(shmid, (void*)0, 0);
	}
	if(shmptr)
	{
		for(;i<count;i++)
		{
			/* Copy data */
			memcpy(q+i, shmptr+(sizeof(QUEUEROW_t)*i), sizeof(QUEUEROW_t));
		}
	}
	else
	{
		fprintf(stdout, "Unable to read from shm\n");
		return FAIL;
	}
	/* Detach from shared memory */
	shmdt(shmptr);

	return SUCCEED;
}

int get_shmid()
{
	return shmid;
}

void grace_exit(int signal)
{
	fprintf(stdout, "Signal Received: [%d]\n", signal);

	/* Destory shared memory */
	if(shmid)
	{
		shmctl(shmid, IPC_RMID, NULL);
	}
	exit(signal);
}





int main()
{
	//Create all shm and msg q and processes
	init_engine();


	close_engine();
	fprintf(OUTFILE, "Exiting[%d]\n", getpid());
	return 0;
}

