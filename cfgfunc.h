#ifndef	__CFGFUNC_H__
#define	__CFGFUNC_H__

#include "core.h"

#define	SERVERS_START	"SERVERS_START"
#define SERVERS_END	"SERVERS_END"
#define CFG_LINE_LEN	1024

extern int cf_next(char *line, char *word);
extern int cf_nextint(int *word);
extern int cf_get_servers(QUEUEROW_t *queuetable, int *count);
extern int cf_open(char *filename);
extern int cf_readline(char *buf);
extern int cf_locate(char* cfgstring);
extern int cf_rewind();


#endif
