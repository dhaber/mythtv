#!/bin/bash

clear

echo "Running checks at: `date`"

#echo
#echo "-----------------------Bad Movies------------------------"
#./badMovies.pl

#echo
#echo "-----------------------Better Movies---------------------"
#./betterMovies.py


#echo
#echo "------------------------------Generic--------------------"
#./generic.pl

echo
echo "------------------------------Inactive-------------------"
./inactive.pl

echo
echo "--------------------------Movie Dupes---------------------"
./movieDupes.py

echo
echo "--------------------------Re-Record Dupes---------------------"
./rerecordDupes.py

echo
echo "---------------------------TV Dupes---------------------"
./dupes.py

echo
echo "------------------------------Late---------------------"
./late.py

#echo
#echo "----------------------------Not HD---------------------"
#./nothd.py

#echo
#echo "------------------------------Meta---------------------"
#./meta.pl

echo
echo "------------------------Default Shows---------------------"
./default.py

echo
echo "------------------------Bad Commercials---------------------"
./badComm.pl -short

echo
echo "------------------------Soccer ----------------------------"
./soccer.py
