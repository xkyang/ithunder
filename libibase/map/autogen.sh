#!/bin/sh

if [ $# -eq 0 ];then
   echo 'tcMap|testIMap|ximap' | tr '|' '\n' |
   while read line
   do
     if [ ! -e ${line}.c ];then
        cp ../../map/utils/${line}.c .
     fi
   done
   perl -i -p -e "s/%ND/%d/g" ximap.c
   if [ ! -e autogen.sh ];then
      ln -s ../../map/autogen.sh .
   fi
   
   gcc -O2 -o xTcMap tcMap.c imap.c -DTEST_IN -DHAVE_PTHREAD -lpthread
   gcc -O2 -o x2TcMap tcMap.c imap.c -DTEST_IN2 -DHAVE_PTHREAD -lpthread
   gcc -O2 -o x3TcMap tcMap.c imap.c -DTEST_IN3 -DHAVE_PTHREAD -lpthread
   gcc -O2 -o xRangeTcMap tcMap.c imap.c -DTEST_RANGE -DHAVE_PTHREAD -lpthread
   gcc -O2 -o xRangeMap testIMap.c ximap.c -DTEST_RANGE -DHAVE_PTHREAD -lpthread
   gcc -O2 -o xDelMap testIMap.c ximap.c -DTEST_DEL -DHAVE_PTHREAD -lpthread
   gcc -O2 -o xInMap testIMap.c ximap.c -DTEST_INS -DHAVE_PTHREAD -lpthread
   gcc -O2 -o x2InMap testIMap.c ximap.c -DTEST_INS2 -DHAVE_PTHREAD -lpthread
else
   echo 'tcMap|testIMap|ximap' | tr '|' '\n' |
   while read line
   do
     if [ -e ${line}.c ];then
        unlink ${line}.c
     fi
   done
   if [ -e autogen.sh ];then
      unlink autogen.sh
   fi
   rm -rf 1.idx*
   rm -rf xTcMap x3TcMap x2TcMap xRangeTcMap xDelMap xInMap x2InMap xRangeMap
fi
