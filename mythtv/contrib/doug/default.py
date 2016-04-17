#!/usr/bin/python

from MythTV import MythBE, Program, static
from datetime import timedelta, datetime, time

ignore = ["Brain Games"]

# we want shows that
# 1. are scheduled to record
# 2. are in the default reco group
# 3. not in ignore list
def willRec(x):
	return x.recstatus == Program.rsWillRecord \
		and x.recgroup == "Default" \
		and x.title not in ignore

myth = MythBE()
pending = filter(willRec, myth.getPendingRecordings())

print "%-20.20s %-35.35s %-20s %-20s %s" % ("title", "subtitle", "channel", "record start time", "record id")
for p in sorted(pending, key=lambda a: a.title):
	print "%-20.20s %-35.35s %-20s %-20s %s" % (p.title, p.subtitle, p.channum, p.starttime, p.recordid)

