#!/usr/bin/python

import os
import time
from MythTV import MythTV
from MythTV import MythDB

d="/var/lib/mythtv/movies-group/rihanna/"



def cmpTime(a,b):
	atime = int(time.mktime(a.recstartts.timetuple()))
	btime = int(time.mktime(b.recstartts.timetuple()))
	return btime - atime

def filterMovies(a):
	return a.stars > 0 and a.recstatus == -1;
		

myth = MythTV()
db = MythDB()
c = db.cursor()
#                c.execute("""UPDATE recorded SET %s = %%s
#                                WHERE chanid=%%s and starttime=%%s""" % field,
#                                (value,self.chanid,self.starttime))
#                c.close()
progs = filter(filterMovies, myth.getPendingRecordings())

for p in sorted(progs, cmpTime):
	print "%s %s %-8s %-8s %-20s" % (p.recstartts, p.chanid, p.recgroup, p.storagegroup, p.title)
	if p.recgroup <> "Movies" or p.storagegroup <> "Movies":		
		c.execute("UPDATE record SET recgroup='Movies', storagegroup='Movies' WHERE recordid='%s'" % (p.recordid))
		print "UPDATE: %s!" % p.recordid

c.close()
