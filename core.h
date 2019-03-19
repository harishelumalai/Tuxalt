#ifndef __CORE_H__
#define __CORE_H__

#define	SHMFILE		"shmfile"
#define	SHMID		65
#define SHMSIZE		1024

#define FAIL		-1
#define SUCCEED		0
#define OUTFILE         stdout

typedef struct
{
	char	svrname[40];
	key_t	qkey;
	int	mqid;
}QUEUEROW_t;

extern int tpinit();
extern int tpcall();
extern int tpreturn();
extern int tpsvrinit();
extern int tpterm();
extern int tpsvrdone();
extern int tpadvertise();
#endif
