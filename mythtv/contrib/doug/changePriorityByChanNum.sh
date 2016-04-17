#!/bin/bash

if [ "$1" == "" ]
then
	echo "Need a channel"
	exit -1
fi

#. /etc/mythtv/mysql.txt.bak
. data

mysql -u $DBUserName -p$DBPassword $DBName <<ENDOFMYSQL
update channel set recpriority = -1 where channum = '$1';
select chanid, channum, callsign, name, recpriority from channel where channum = '$1';
ENDOFMYSQL

#mythbackend --resched
