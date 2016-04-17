#!/usr/bin/python

from datetime import timedelta, datetime, time
from MythTV import MythBE
from MythTV import Channel

def cmpTitle(a,b):
	return b.title - a.title

def filterPMovies(a):
	return a.recgroup == "Movies"

def filterRMovies(a):
	if a.channum == "#4925" or a.channum == "#4929":
		return False

	magic = datetime(2009,9,1,0,0,0,0,a.starttime.tzinfo)
#	return a.recgroup == "Movies" and (float(a.channum) < 500 or getCommMethod(a.chanid) <> -2 or a.starttime < magic)
	return a.recgroup == "Movies" and (getCommMethod(a.chanid) <> -2 or a.starttime < magic)

def getChannel(chanid):
	for c in channels:
		if c.chanid == chanid:
			return c

def getCommMethod(chanid):
	c = getChannel(chanid);
	return c.commmethod


myth = MythBE()
channels = list(Channel.getAllEntries())
pending = filter(filterPMovies, myth.getPendingRecordings())
all_movies = filter(filterPMovies, myth.getRecordings())
recorded = filter(filterRMovies, all_movies)

for r in sorted(recorded, key=lambda a: a.title):

	def r_filter(a):
		return a.title == r.title

	my_rec = filter(r_filter, all_movies)

	def p_filter(a):
		for b in my_rec:
			if a.title <> b.title:
				return False

#			print "%s %d" % (a.title, len(my_rec))

			if a.title == b.title \
			and a.description == b.description \
			and ( \
			  a.channum == b.channum \
                          or float(b.channum) == float(a.channum) + 500 \
                          # IFC moved so this hack is remove IFC if the original station was IFC
			  or (b.callsign == "IFCHD" and (a.callsign == "IFCHD" or a.callsign == "IFC")) \
                        ):
				return False

		return True


	my_pending = sorted(filter(p_filter, pending), key=lambda a:a.channum)
	my_pending_set = []
	last = 0
	for a in my_pending:
		if a.chanid <> last:
			my_pending_set.append(a)
			last = a.chanid
	
	if len(my_pending_set) > 0:
		print "%-20.20s %5s %-7.7s %s %.60s %s" % (r.title, r.channum, r.callsign, getCommMethod(r.chanid), r.description, r.starttime.strftime("%m/%d/%y"))
		for p in my_pending_set:
			print "%-20.20s %5s %-7.7s %s %.60s" % ("", p.channum, p.callsign, getCommMethod(p.chanid), p.description)


#	for a in my_pending:
#	print "------------"


