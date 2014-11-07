#!/bin/sh

echo "You must in the directory: libibase/src(y/n)"
read confirm
if [ x"$confirm" != x"y" ];then
   exit 0
fi

if [ $# -eq 0 ];then
   if [ -e query.c ];then
	 mv query.c ori.query.c
	 ln -s ../map/query.c .
   fi
   cd utils
   if [ -e imap.c ];then
	  mv imap.c ori.imap.c
      ln -s ../../map/utils/ximap.c imap.c
	  ./gen.sh
   fi
   cd ../
else
   if [ -e ori.query.c ];then
	 unlink query.c
	 mv ori.query.c query.c
   fi
   cd utils
   if [ -e ori.imap.c ];then
	  unlink imap.c
	  mv ori.imap.c imap.c
	  ./gen.sh
   fi
   cd ../
fi
