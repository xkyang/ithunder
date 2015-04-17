#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "ibase.h"

#ifdef MAP_LOCKED
#define MMAP_SHARED MAP_SHARED|MAP_LOCKED
#else
#define MMAP_SHARED MAP_SHARED
#endif

int main(int argc,char* argv[])
{
	if(argc != 2)
	{
	   fprintf(stdout,"usage:%s path\n",argv[0]);
	   return -1;
	}
	char* path = argv[1];
	int k = 0, x = 0;
    int fd = open(path, O_CREAT|O_RDWR, 0644);
	if(fd > 0)
    {
       IBSTATE* state = (IBSTATE *)mmap(NULL, sizeof(IBSTATE), PROT_READ|PROT_WRITE, MMAP_SHARED, fd, 0);
	   if((state == NULL) || (state == (void *)-1))
       {
           fprintf(stdout, "mmap state file (%s) failed, %s\n", path, strerror(errno));
            _exit(-1);
       }
       for(k = 0; k < state->nsecs; k++)
       {
           x = state->secs[k];
		   fprintf(stdout,"k:%d x:%d id:%d\n",k,x,state->ids[x]);
	   }
    }
    else
    {
        fprintf(stdout, "open state file (%s) failed, %s\n", path, strerror(errno));
        _exit(-1);
    }
	return 0;
}

//gcc -o xState state.c -libase
