#!/usr/bin/python

from MythTV import MythBE, Program, static
from datetime import timedelta, datetime, time

chans = ["11.1","13.1","13_2","21.1", "7_1", "5_1", "4_1", "2_1", "21_1", "13_3","47_1"]
ignore = [973,2002,2115,2151,2156,1957,1761,1661,2162,1678,839,2172,1854,2176,1791,2047,2208,2209]

# we want shows that
# 1. are scheduled to record
# 2. are being recorded on non-HD channels ( < 500)
# 3. are not special HD channels (e.g. 11.1)
# 4. are not in our list of shows that we know are only on SD channels
def willRec(x):
	return x.recstatus == Program.rsWillRecord \
		and x.channum not in chans \
		and float(x.channum) < 500 \
		and x.recordid not in ignore

myth = MythBE()
pending = filter(willRec, myth.getPendingRecordings())

print "%-20.20s %-35.35s %-20s %-20s %s" % ("title", "subtitle", "channel", "record start time", "record id")
for p in sorted(pending, key=lambda a: a.title):
	if not static.VIDEO_PROPS.VID_HDTV in p.VideoProps:
		print "%-20.20s %-35.35s %-20s %-20s %s" % (p.title, p.subtitle, p.channum, p.starttime, p.recordid)

