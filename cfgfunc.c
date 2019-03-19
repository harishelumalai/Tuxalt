#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "core.h"
#include "cfgfunc.h"

FILE	*cfgfd;

int cf_next(char *line, char *word);
int cf_nextint(int *word);
int cf_get_servers(QUEUEROW_t *queuetable, int *count);
int cf_open(char *filename);
int cf_readline(char *buf);
int cf_locate(char* cfgstring);
int cf_rewind();

int cf_next(char *line, char *word)
{
	fprintf(OUTFILE, "cf_next()\n");
	if(word != NULL)
	{
		strcpy(word , strtok(line, "\t "));
		if(word ==  NULL)
			return FAIL;
		return SUCCEED;
	}
	return FAIL;
}

int cf_nextint(int *word)
{
	fprintf(OUTFILE, "cf_nextint()\n");
	char tmp[CFG_LINE_LEN]="";
	if(SUCCEED==cf_next(NULL, tmp))
	{
		sscanf(tmp, "%d", word);
	}
}

int cf_get_servers(QUEUEROW_t *queuetable, int *count)
{
	fprintf(OUTFILE, "cf_get_servers()\n");
	char line[CFG_LINE_LEN]="";
	char tmp[CFG_LINE_LEN]="";
	QUEUEROW_t	*qt;

	qt=queuetable;

	if(queuetable == NULL)
	{	
		return FAIL;
	}

	if(SUCCEED==cf_open("/home/e3026114/samples/tuxalt/cfg/dpp.cfg"))
	{
		if(SUCCEED==cf_locate(SERVERS_START))
		{
			/* Located the search string. Now start parsing. */
			while(SUCCEED==cf_readline(line) && strcmp(line, SERVERS_END)!=0)
			{
				cf_next(line, tmp);
				if(tmp != NULL)
				{
					strcpy(qt->svrname, tmp);
					fprintf(OUTFILE, "srvname=[%s][%s]\n", qt->svrname, tmp);
				}
				cf_nextint(&(qt->qkey));
				fprintf(OUTFILE, "srvname=[%d]\n", qt->qkey);
				*count = (*count) + 1;
				qt++;
			}
		}
		else
		{
			fprintf(OUTFILE, "Unable to locate [%s] config file.", SERVERS_START);
			return FAIL;
		}
	}
}

int cf_open(char *filename)
{
	fprintf(OUTFILE, "cf_open() [%s]\n", filename);
	if(cfgfd == NULL)
	{
		cfgfd = fopen(filename, "r");
		if(cfgfd<=0)
		{
			fprintf(OUTFILE, "Unable to open config file\n");
			return FAIL;
		}
	}
	return SUCCEED;
}

int cf_strip_both(char *out_line, char *in_line)
{
	cf_strip_right(out_line,in_line);
	cf_strip_left(out_line,in_line);
}

int cf_strip_left(char *out_line, char *in_line)
{
	char	*head;
	int	i, len;
	len=strlen(in_line);
	head=in_line;
	for(i=0;i<len;i++)
	{
		if(*head == ' ' || *head == '\t' || *head == '\n')
			head++;
		else
		{
			memcpy(out_line, head, strlen(head));
			break;
		}
	}
	//fprintf(OUTFILE, "left [%s][%s]\n", out_line, head);
}

int cf_strip_right(char *out_line, char *in_line)
{
	char	*tail;
	int	i, len;
	len=strlen(in_line);
	tail=in_line+(len-1);
	for(i=len;i>=0;i++)
	{
		if(*tail == ' ' || *tail == '\t' || *tail == '\n')
		{
			*tail=0;
			tail--;
		}	
		else
		{
			if(out_line!=in_line)
				memcpy(out_line, in_line, strlen(in_line));
			break;
		}
	}
	//fprintf(OUTFILE, "right [%s][%s]\n", out_line, in_line);
}

int cf_readline(char *buf)
{
	fprintf(OUTFILE, "cf_readline()\n");
	if( buf != NULL && cfgfd )
	{
		do
		{
			memset(buf, 0x00, CFG_LINE_LEN);
			//fscanf(cfgfd, "%[^\n]", buf);
			fgets(buf, CFG_LINE_LEN, cfgfd);
			if(*buf==EOF)
				return FAIL;
			cf_strip_both(buf,buf);
			//fprintf(OUTFILE, "Now: [%ld]\n", ftell(cfgfd));
			//fprintf(OUTFILE, "Read [%s]\n", buf);
		}while(buf[0]=='#');
	}
	return SUCCEED;
}

int cf_locate(char* cfgstring)
{
	/* TODO: Remove loop */
	int loop=0;
	int ret=0;
	fprintf(OUTFILE, "cf_locate() [%s]\n", cfgstring);
	char	line[CFG_LINE_LEN]="";
	/* Move to top of file */
	cf_rewind();
	
	/* Start searching */
	do
	{
		memset(line, 0x00, sizeof(line));
		ret=cf_readline(line);
		if(ret<0)
			break;
		if(strncmp(line, cfgstring, strlen(cfgstring)) == 0)
		{
			fprintf(OUTFILE, "Located.\n");
			/* Now I've located search string. file cursor will now be in start of next line. */
			return SUCCEED;
		}
		loop++;
	}while(*line!=EOF && loop < 10);
	return FAIL;
}

int cf_rewind()
{
	fprintf(OUTFILE, "cf_rewind()\n");
	fprintf(OUTFILE, "Before: [%ld]\n", ftell(cfgfd));
	rewind(cfgfd);
	fprintf(OUTFILE, "After: [%ld]\n", ftell(cfgfd));
	return SUCCEED;
}
