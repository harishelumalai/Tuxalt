#ifndef __MQMGR_H__
#define	__MQMGR_H__

#define GLOBAL_TYPE	1;

typedef	struct
{
	char	name[32];	/* service name to be invoked */
	long	flags;		/* contains service attributes */ 
	char	*data;		/* request data */
	long	len;		/* request data length */
	int	cd; 		/* connection descriptor */
	int	appkey;		/* app auth client key (not used)*/
	long	cltid;		/* originating client id */
}TPSVCINFO;

typedef struct
{
	long		type;
	TPSVCINFO	msg;
}MQMSG;

extern int create_mq(int key, int *mqid);
extern int open_mq(int key, int *mqid);
extern int write_to_mq(int mqid, MQMSG *msg);
extern int read_from_mq(int mqid, MQMSG *msg);
extern int close_mq(int mqid);

#endif
