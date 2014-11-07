#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "imap.h"
#include "timer.h"
#define MK 256
#define MASK 120000
#define MAX_DATA 4000000
//gcc -o xMap testIMap.c ximap.c -DTEST_INS -DHAVE_PTHREAD -lpthread
int main(int argc,char* argv[])
{
	if(argc != 2)
	{
	   fprintf(stdout,"usage: %s idx\n",argv[0]);
	   return -1;
	}
    IMAP *imap = NULL;
    int i = 0, j = 0, n = 0, no = 0, nfromto = 0, off = 0, from = 100, to = 10000;
    int32_t val = 0, docid = 0, *res = NULL;
    int32_t inputs[MK], nos[MK], last[MK], tall[MASK];

    if((imap = imap_init(argv[1])))
    {
        res = (int32_t *)calloc(60000000, sizeof(int32_t));
#ifdef TEST_DEL
		fprintf(stdout,"begin to test imap_del!\n");
        for(no = 0; no < MASK; no++)
        {
            tall[no] = 0;
        }
        for(i = 0; i < MAX_DATA; i++)
        {
            tall[imap->vmap[i].val]++;
        }
        for(i = 0; i < MAX_DATA; i++)
        {
            no = (rand()%MASK);
            imap_del(imap, i);
            --tall[no];
        }
        for(no = 0; no < MASK; no++)
        {
            tall[no] = tall[no] < 0 ? 0 : tall[no];
        }
        for(no = 0; no < MASK; no++)
        {
            n = imap_in(imap, no, NULL);
            if(n != tall[no])
                fprintf(stdout, "%d:[%d/%d]\n", no, n, tall[no]);
        }
		fprintf(stdout,"------------------------!\n");
#endif		
#ifdef TEST_INS		
		fprintf(stdout,"begin to test imap_in!\n");
		off = 0;
		no = scanf("%d",&val);
        for(i = 0; i < MAX_DATA; i++)
        {
           if(imap->vmap[i].val == val)
		   {
			  ++j;
		      fprintf(stdout,">>%d %d\n",i,val);
		   }
		}
        n = imap_in(imap, val, res);
		fprintf(stdout,"val:%d n:%d/%d\n",val,j,n);
		if(n > 0)
		{
	        while(off < n)
			{
		       fprintf(stdout,"<<%d %d\n",res[off++],val);
			}
		}
		fprintf(stdout,"------------------------!\n");
#endif		
#ifdef TEST_INS2		
		fprintf(stdout,"begin to test imap_in!\n");
		fprintf(stdout,"val:");
		no = scanf("%d",&val);
		fprintf(stdout,"docid:");
		no = scanf("%d",&docid);
		fprintf(stdout,"val:%d mval:%d\n",val,imap->vmap[docid].val);
        n = imap_in(imap, val, res);
		if(n > 0)
		{
		    fprintf(stdout,"val:%d n:%d\n",val,n);
	        while(off < n)
			{
			   if(res[off] == docid)
			   {
		          fprintf(stdout,"done---->off:%d\n",off);
				  break;
			   }
			   ++off;
			}
		}
		fprintf(stdout,"------------------------!\n");
#endif		
#ifdef TEST_RANGE
		fprintf(stdout,"begin to test imap_range!\n");
        for(i = 0; i < MAX_DATA; i++)
        {
		   if((imap->vmap[i].val >= from) && (imap->vmap[i].val <= to))
		   {
		      ++nfromto;
		      fprintf(stdout,"fromto:%d %d\n",i,imap->vmap[i].val);
		   }
		}
		off = 0;
        n = imap_range(imap, from, to, res);
		if(n > 0)
		{
		    fprintf(stdout,"from:%d to:%d n:%d/%d\n", from, to, nfromto,n);
	        while(off < n)
			{
			   docid = res[off++];
		       fprintf(stdout,"tofrom:%d %d\n",docid,imap->vmap[docid].val);
			}
		}
#endif	
        imap_close(imap);
        free(res);
    }
	return 0;
}
