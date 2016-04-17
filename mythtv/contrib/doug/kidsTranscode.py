#!/usr/bin/python

# finds kids shows that need to be transcoded

import operator
from MythTV import MythBE, Program, MythDB

myth = MythBE()
mythDB = MythDB()

def filts(i):
	return i.title not in ["Super Why! Around the World Adventure", "Winnie the Pooh: Springtime With Roo", "Winnie the Pooh"]

def sorter(i):
	if i.subtitle:
		return i.title + " " + i.subtitle
	return i.title;

# get all recordings
recs = mythDB.searchRecorded(recgroup="Kids", transcoded=0)

# limit to only shows in the Kids group that are not transcoded
#recs = filter(lambda x: x.recgroup == "Kids" and x.title not in [''], recs)

# sort by title
recs = sorted(filter(filts,recs), key=sorter)

for i in recs:
	print "%-35.35s %-20.20s mythtranscode --queue --chanid %s --starttime \"%s\"" % (i.title,i.subtitle,i.chanid, i.starttime)
