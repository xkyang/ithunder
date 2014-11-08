#!/bin/sh

echo "You must in the directory: libibase/src(y/n)"
read confirm
if [ x"$confirm" != x"y" ];then
   exit 0
fi

WORKDIR=`pwd`

if [ $# -eq 0 ];then
   if [ -e query.c ];then
	 mv query.c ori.query.c
	 ln -s ../map/query.c .
   fi
   cd ../map/utils/
   ./gen.sh
   cd $WORKDIR
   cd utils
   echo 'i|l|d' | tr '|' '\n' |
   while read flag
   do
      MAP=${flag}map.c
      if [ -e $MAP ];then
         mv $MAP ori.$MAP
         ln -s ../../map/utils/$MAP .
      fi
   done
   cd $WORKDIR
else
   if [ -e ori.query.c ];then
      unlink query.c
      mv ori.query.c query.c
   fi
   cd utils
   echo 'i|l|d' | tr '|' '\n' |
   while read flag
   do
      MAP=${flag}map.c
      if [ -e ori.$MAP ];then
         unlink $MAP
         mv ori.$MAP $MAP
      fi
   done
   cd $WORKDIR
   cd ../map/utils/
   rm -rf [dil]map.c
fi
