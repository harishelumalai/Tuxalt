#include <stdio.h>
#include <stdlib.h>

#include "core.h"

int main(int argc, char **argv)
{
	int i=0;
	char	svcname[32]="";
	if(argc!=2)
	{
		fprintf(OUTFILE, "usage: ./server1 <servicename>\n");
		exit(-1);
	}
	strcpy(svcname, argv[1]);
	while(i<10)
	{
		fprintf(OUTFILE, "SERVER 1 SERVICE[%s] RUNNING\n", svcname);
		i++;
		sleep(3);
	}
	return 0;
}
