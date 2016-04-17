#!/usr/bin/python

import os
from MythTV import MythTV

d="/var/lib/mythtv/movies-group/rihanna/"

def cmpFileSize(a,b):
	return b.filesize - a.filesize

def filterMovies(a):
	return 	a.recgroup == "Movies" and a.storagegroup == "Default" and os.path.exists(d + a.filename.split("/")[3])
		

myth = MythTV()
progs = filter(filterMovies, myth.getRecordings())

for p in sorted(progs, cmpFileSize):
	print "%s %s %s" % (p.title, p.chanid, p.recstartts)
	p.setSG("Movies")
