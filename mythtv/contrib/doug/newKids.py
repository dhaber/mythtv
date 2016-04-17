#!/usr/bin/python

from MythTV import MythBE, Program, static
from datetime import timedelta, datetime, time

chans = ["11.1"]
ignore = [973,2002,2115,2151,2156,1957,1761,1661,2162,1678,839,2172]

# we want shows that
# 1. are scheduled to record
# 2. are being recorded on non-HD channels ( < 500)
# 3. are not special HD channels (e.g. 11.1)
# 4. are not in our list of shows that we know are only on SD channels
def willRec(x):
	return x.recstatus not in (Program.rsWillRecord, Program.rsEarlierShowing, Program.rsLaterShowing, Program.rsTooManyRecordings) \
		and x.recgroup == 'Kids'
#		and float(x.channum) < 500 \
#		and x.channum not in chans \
#		and x.recordid not in ignore

myth = MythBE()
pending = filter(willRec, myth.getPendingRecordings())

print "%-20.20s %-35.35s %-20s %-20s %s" % ("title", "subtitle", "channel", "record start time", "record id")
for p in sorted(pending, key=lambda a: a.starttime):
	print "%-20.20s %-35.35s %-20s %-20s %s" % (p.title, p.subtitle, p.channum, p.starttime, p.recstatus)

