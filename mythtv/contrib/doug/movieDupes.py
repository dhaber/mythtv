#!/usr/bin/python

# finds currently recorded movies that have two or more copies

import operator
from MythTV import MythBE, Program, MythDB

allowedDupes = { 
	"A Tale of Two Cities" : 2,
	"Anna Karenina" : 3,
	"Arthur" : 2,
	"Death at a Funeral" : 2,
	"George Harrison: Living in the Material World" : 3,
	"Get Carter" : 2,
	"Gigi" : 2,
	"Julius Caesar" : 2,
	"Mutiny on the Bounty" : 2,
	"Richard III" : 2,
	"The Big Sleep" : 2,
	"Shaft" : 2,
	"The Wild One" : 2,
	"The Postman Always Rings Twice" : 2,
	"Witness for the Prosecution" : 2
	}


def findDupes(orig):  
    seen = {} 
    result = [] 
    for item in orig:
	seen[item.title] = seen.get(item.title,0) + 1 
    return dict((title,count) for title,count in seen.iteritems() if count > allowedDupes.get(title,1))

myth = MythBE()
mythDB = MythDB()

# get all recordings
recs = myth.getRecordings()

# limit to only movies that are currently recorded
recs = filter(lambda x: x.recgroup == "Movies", recs)

# limit to only ones that are recorded multiple times
recs = findDupes(recs)

# sort by title
recs = sorted(recs.iteritems(), key=operator.itemgetter(0))

for title,count in recs:
	print "%s %d" % (title,count)
