#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include "imap.h"
#include "timer.h"
#define MK 256
#define MASK  120000
#define MAX_DATA 4000000
//rm -rf /tmp/1.idx* && gcc -O2 -o imap imap.c -DIMAP_TEST -DTEST_IN -DHAVE_PTHREAD -lpthread && ./imap
int main()
{
    IMAP *imap = NULL;
    int i = 0, j = 0, n = 0, total = 0, no = 0, nofrom = 0, noto = 0, nofromto = 0, stat[MASK], stat2[MASK];
    int32_t val = 0, from = 100, to = 10000, *res = NULL, half = MASK / 2 , down = half, up = half;
    int32_t inputs[MK], nos[MK], last[MK], tall[MASK];
    int32_t all = 0;
    time_t stime = 0, etime = 0;
    void *timer = NULL;

    if((imap = imap_init("./1.idx")))
    {
        res = (int32_t *)calloc(60000000, sizeof(int32_t));
        TIMER_INIT(timer);
#ifdef TEST_DEB
            n = imap_in(imap, 169, NULL);
            fprintf(stdout, "169:%d\n", n);
#endif
#ifdef TEST_IN
        for(no = 0; no < MASK; no++)
        {
            tall[no] = 0;
        }
        for(i = 0; i < MAX_DATA; i++)
        {
            no = (rand()%MASK);
            imap_set(imap, i, no);
            tall[no]++;
        }
        for(no = 0; no < MASK; no++)
        {
            n = imap_in(imap, no, NULL);
            if(n != tall[no])
                fprintf(stdout, "%d:[%d/%d]\n", i, n, tall[no]);
        }
#endif
#ifdef TEST_IN2
        for(no = 0; no < MASK; no++)
        {
            tall[no] = 0;
        }
        for(i = MAX_DATA;i > 0; --i)
        {
            no = (rand()%MASK);
            imap_set(imap, i, no);
            tall[no]++;
        }
        for(no = 0; no < MASK; no++)
        {
            n = imap_in(imap, no, NULL);
            if(n != tall[no])
                fprintf(stdout, "%d:[%d/%d]\n", i, n, tall[no]);
        }
#endif
#ifdef TEST_IN3
        for(no = 0; no < MASK; no++)
        {
            tall[no] = 0;
        }
        for(i = MAX_DATA;i > 0; --i)
        {
			if((i % 2) == 0)
			{
			   no = down--;
			   if(down < 0)
			   {
			     down = half;
			   }
			}
			else
			{
			   no = up++;
			   if(up >= MASK)
			   {
			     up = half;
			   }
			}
            imap_set(imap, i, no);
            tall[no]++;
        }
        for(no = 0; no < MASK; no++)
        {
            n = imap_in(imap, no, NULL);
            if(n != tall[no])
                fprintf(stdout, "%d:[%d/%d]\n", i, n, tall[no]);
        }
#endif
#ifdef TEST_RANGE
        for(i = 0; i < MAX_DATA; i++)
        {
            no = (rand()%MASK);
            imap_set(imap, i, no);
			if(no >= from)
			{
			   ++nofrom;
			   if(no <= to)
			   {
			     ++nofromto;
			   }
			}
			if(no <= to)
			{
			   ++noto;
			}
        }
		n = imap_range(imap,from,to,NULL);
        fprintf(stdout, "from/to:[%d/%d] => [%d:%d]\n", from, to, nofromto, n);
		n = imap_rangefrom(imap,from,NULL);
        fprintf(stdout, "from:%d => [%d:%d]\n", from, nofrom, n);
		n = imap_rangeto(imap,to,NULL);
        fprintf(stdout, "to:%d => [%d:%d]\n", to, noto, n);
#endif
#ifdef TEST_INS
        for(i = 0; i < MAX_DATA; i++)
        {
            no = (rand()%MASK);
            imap_set(imap, i, no);

        }
		n = imap_range(imap,from,to,NULL);
        fprintf(stdout, "from/to:[%d/%d] => [%d:%d]\n", from, to, nofromto, n);
		n = imap_rangefrom(imap,from,NULL);
        fprintf(stdout, "from:%d => [%d:%d]\n", from, nofrom, n);
		n = imap_rangeto(imap,to,NULL);
        fprintf(stdout, "to:%d => [%d:%d]\n", to, noto, n);
#endif
#ifdef TEST_INS2
        //fprintf(stdout, "sizeof(stat):%d\n", sizeof(stat));
        memset(stat, 0, sizeof(stat));
        memset(stat2, 0, sizeof(stat2));
        srand(time(NULL));
        n = MK;
        for(i = 0; i < n; i++)
        {
            no = (rand()%MASK);
            nos[i] = no;
            if((i % 3) == 0)
                inputs[i] = no * -1;
            else
                inputs[i] = no;
        }
        TIMER_RESET(timer);
        for(i = 1; i < MAX_DATA; i++)
        {
           j = (rand()%n);
           val = inputs[j];
           no = nos[j];
           stat[no]++;
           imap_set(imap, i, val);
           last[j] = i;
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "set() %d data, time used:%lld\n", MAX_DATA, PT_LU_USEC(timer));
        TIMER_RESET(timer);
        for(i = 0; i < n; i++)
        {
            imap_del(imap, last[i]);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "del() time used:%lld\n", PT_LU_USEC(timer));
        TIMER_RESET(timer);
        for(i = 0; i < n; i++)
        {
            val = inputs[i];
            no = nos[i];
            stat2[no] = imap_in(imap, val, res);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "in() time used:%lld\n", PT_LU_USEC(timer));
        TIMER_RESET(timer);
        total = imap_ins(imap, inputs, n, NULL);
        TIMER_SAMPLE(timer);
        fprintf(stdout, "ins(keys, NULL) total:%d time used:%lld\n", total, PT_LU_USEC(timer));
        TIMER_RESET(timer);
        total = imap_ins(imap, inputs, n, res);
        TIMER_SAMPLE(timer);
        fprintf(stdout, "ins(keys, res:%p) total:%d time used:%lld\n", res, total, PT_LU_USEC(timer));
        for(i = 0; i < n; i++)
        {
            j = nos[i];
            if(stat[j] != stat2[j])
                fprintf(stdout, "%d:%d/%d::%d\n", j, stat[j], stat2[j], inputs[i]);
        }
#ifdef OUT_ALL
        for(i = 0; i < total; i++)
        {
            fprintf(stdout, "%d:%d\n", i, res[i]);
        }
#endif
#endif
        /*
        for(i = 0; i < imap->state->count; i++)
        {
            fprintf(stdout, "%d:{min:%d max:%d}(%d)\n", i, imap->slots[i].min, imap->slots[i].max, imap->slots[i].count);
        }
        */
#ifdef TEST_RANGEFILTER
            imap_set(imap, 1, 1234567);
            imap_set(imap, 2, 1567890);
            fprintf(stdout, "rangefrom():%d\n", imap_rangefrom(imap, 1569000, NULL));
            fprintf(stdout, "rangeto():%d\n", imap_rangeto(imap, 1111111, NULL));
            fprintf(stdout, "range():%d\n", imap_range(imap, 1111111, 1400000, NULL));
#endif
#ifdef TEST_RANGE22
        srand(time(NULL));
        TIMER_RESET(timer);
        for(i = 1; i < MAX_DATA; i++)
        {
            val = 1356969600 + (rand()%31536000);
            imap_set(imap, i, val);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "set() %d timestamps,  time used:%lld\n", MAX_DATA, PT_LU_USEC(timer));
        srand(time(NULL));
        TIMER_RESET(timer);
        all = 0;
        for(i = 0; i < 1000; i++)
        {
            val = 1356969600 + (rand()%31536000);
            all += imap_rangefrom(imap, val, res);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "rangefrom() 1000 times total:%lld, time used:%lld\n", (long long int)all, PT_LU_USEC(timer));
        srand(time(NULL));
        TIMER_RESET(timer);
        all = 0;
        for(i = 0; i < 1000; i++)
        {
            val = 1356969600 + (rand()%31536000);
            all += imap_rangeto(imap, val, res);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "rangeto() 1000 times total:%lld, time used:%lld\n", (long long int)all, PT_LU_USEC(timer));
        srand(time(NULL));
        TIMER_RESET(timer);
        all = 0;
        for(i = 0; i < 1000; i++)
        {
            from = 1356969600 + (rand()%31536000);
            to = from + rand()%31536000;
            all += imap_range(imap, from, to, res);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "range(%p) 1000 times total:%lld, time used:%lld\n", res, (long long int)all, PT_LU_USEC(timer));
        srand(time(NULL));
        TIMER_RESET(timer);
        all = 0;
        for(i = 0; i < 1000; i++)
        {
            from = 1356969600 + (rand()%31536000);
            to = from + rand()%31536000;
            all += imap_range(imap, from, to, NULL);
        }
        TIMER_SAMPLE(timer);
        fprintf(stdout, "range(null) 1000 times total:%lld, time used:%lld\n", (long long int)all, PT_LU_USEC(timer));

#endif
        imap_close(imap);
        TIMER_CLEAN(timer);
        free(res);
    }
}
