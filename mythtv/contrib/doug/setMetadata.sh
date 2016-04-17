#!/bin/bash

if [ "$1" == "" ]
then
	echo "Need Chan ID"
	exit -1
fi
CHANID=$1

if [ "$2" == "" ]
then
	echo "Need Starttime"
	exit -1
fi
STARTTIME=$2

if [ "$3" == "" ]
then
	3="null"
fi
INETREF=$3
SEASON=$4
EPISODE=$5
if [ "$SEASON" == "" ]
then
	SEASON=0
fi

if [ "$EPISODE" == "" ]
then
EPISODE=0
fi

. /etc/mythtv/mysql.txt.bak

mysql -u $DBUserName -p$DBPassword $DBName <<ENDOFMYSQL
update recorded set inetref=$INETREF, season=$SEASON, episode=$EPISODE where chanid = $CHANID and starttime = "$STARTTIME";
select title,subtitle,inetref,season,episode from recorded where chanid = $CHANID and starttime = "$STARTTIME";
ENDOFMYSQL
#update channel set recpriority = -1 where channum = $1;
#select chanid, channum, callsign, name, recpriority from channel where channum = $1;

#mythbackend --resched
