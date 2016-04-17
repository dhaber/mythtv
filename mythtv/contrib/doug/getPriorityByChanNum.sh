#!/bin/bash

if [ "$1" == "" ]
then
	echo "Need a channel"
	exit -1
fi

. /etc/mythtv/mysql.txt.bak

mysql -u $DBUserName -p$DBPassword $DBName <<ENDOFMYSQL
select chanid, channum, callsign, name, recpriority from channel where channum = '$1';
ENDOFMYSQL

#mythbackend --resched
