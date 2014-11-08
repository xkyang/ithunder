#!/bin/bash
#int
cp -f ximap.c imap.c
perl -i -p -e "s/%ND/%d/g" imap.c
#long
cp -f ximap.c lmap.c
perl -i -p -e "s/int32_t/int64_t/g" lmap.c
perl -i -p -e "s/IMM/LMM/g" lmap.c
perl -i -p -e "s/IMAP/LMAP/g" lmap.c
perl -i -p -e "s/imap/lmap/g" lmap.c
perl -i -p -e "s/typedef uint64_t u32_t/typedef uint32_t u32_t/g" lmap.c
perl -i -p -e "s/%ND/%ld/g" lmap.c
#double
cp -f ximap.c dmap.c
perl -i -p -e "s/int32_t/double/g" dmap.c
perl -i -p -e "s/IMM/DMM/g" dmap.c
perl -i -p -e "s/IMAP/DMAP/g" dmap.c
perl -i -p -e "s/imap/dmap/g" dmap.c
perl -i -p -e "s/typedef udouble u32_t/typedef uint32_t u32_t/g" dmap.c
perl -i -p -e "s/%ND/%f/g" dmap.c
