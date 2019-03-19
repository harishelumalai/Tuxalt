#ifndef	__ENGINEMGR_H__
#define	__ENGINEMGR_H__

#include "core.h"

#define	MAX_PROCESS	10

extern	int	shmid;
extern	char	*shmaddr;
extern	char	*shmptr;
extern	pid_t	pid[10];

extern	QUEUEROW_t	queuelist[MAX_PROCESS];
//extern	PROCESSROW_t	processtable[MAX_PROCESS];


extern int create_shm(int shmkey, size_t shmsize);
extern int init_engine();
extern int write_to_shm(char *msg, int len);
extern int read_from_shm(char *buf, int *len);
extern int write_queues_to_shm(QUEUEROW_t *q, int count);
extern int read_queue_to_shm(QUEUEROW_t *q, int count);
extern int get_shmid();
extern void grace_exit(int signal);

#endif
