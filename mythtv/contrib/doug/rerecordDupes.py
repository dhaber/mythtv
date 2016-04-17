#!/usr/bin/python

# finds currently recorded shows that have two or more copies

import os
import sys, getopt
import operator
from dateutil import parser
from MythTV import MythBE, Program, MythDB
from ConfigParser import ConfigParser
from StringIO import StringIO

parser = ConfigParser()
with open("data") as stream:
    stream = StringIO("[top]\n" + stream.read())
    parser.readfp(stream)

# returns a map from a unique key (programid) to a list of matching shows
def findDupes(orig):  
    seen = {} 
    result = [] 
    for item in orig:
	second = item.subtitle
	if second is None:
		second = item.programid
#	key = "%s - %s - %s" % (item.title, item.subtitle, item.programid)
	key = item.programid
	items =  seen.get(key,[])
	items.append(item)
	seen[key] = items
    return dict((title,items) for title,items in seen.iteritems() if len(items) > 1)

try:
    opts, args = getopt.getopt(sys.argv[1:],"e:",["episodes="])
except getopt.GetoptError:
    print 'rerecordDupes.py -e <episodes from bottom to run>'
    sys.exit(2)

episodes = 999
for o, a in opts:
    if o == "-e":
	episodes = int(a)

myth = MythBE()
mythDB = MythDB()

# get all recordings
recs = myth.getRecordings()

# kids or BBC
#recs = filter(lambda x: x.chanid == 4189 or x.chanid == 4689 or (x.recgroup == "Kids" and x.title not in ['The Berenstain Bears']), recs)
recs = filter(lambda x: x.title not in ['The Berenstain Bears', 'The Wild One', 'Bosom Buddies', 'Masterpiece Classic'] and x.recgroup <> 'Deleted', recs)

# limit to only ones that are recorded multiple times
recs = findDupes(recs)

# sort by title
#recs = sorted(recs.iteritems(), key=operator.itemgetter(1))
#recs = sorted(recs.iteritems(), key=lambda x: x[1][0].title)
recs = recs.iteritems()

# Find the oldest episode
eps = {}
for title,items in recs:
	oldest = None
	newest = None
	for item in items:
#		print "                 %-40.40s %-40.40s %-20.20s %s %s" % (item.title, item.subtitle, item.programid, item.starttime, item.filesize)
		if oldest is None:
			oldest = item
		elif item.starttime < oldest.starttime:
			oldest = item

		if newest is None:
			newest = item
		elif item.starttime > newest.starttime:
			newest = item

#	oldest.filesize = newest.filesize
	eps[`newest.starttime.timestamp()` + "_" + `newest.chanid`] = oldest


user= parser.get("top", "ApacheUser")
pw= parser.get("top", "ApachePassword")

i = 0
for key in sorted(eps.iterkeys()):
	oldest = eps[key]
	print "%-40.40s %-40.40s %-20.20s %s %s MB" % (oldest.title, oldest.subtitle, oldest.programid, oldest.starttime, oldest.filesize / 1024 / 1024)
	if i < (len(eps) - episodes):
		os.system("curl -s --data \"ajax=yes&delete=yes&chanid=%s&starttime=%d&forgetold=\" http://%s:%s@localhost/mythweb/tv/recorded" % (oldest.chanid, oldest.recstartts.timestamp(), user, pw))
		print "deleted"
	i = i+1
